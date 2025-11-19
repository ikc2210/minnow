#include "router.hh"
#include "debug.hh"

#include <iostream>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  routing_table_.push_back( { route_prefix, prefix_length, next_hop, interface_num } );
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{

  for ( auto& interface : interfaces_ ) {

    auto& datagrams_in = interface->datagrams_received();

    while ( !datagrams_in.empty() ) {
      auto dg = datagrams_in.front();
      datagrams_in.pop();

      if ( dg.header.ttl < 2 ) {

        // if TTL not sufficient
        continue;
      }

      // reudce ttl
      dg.header.ttl--;

      dg.header.compute_checksum();

      // longest-prefix match

      const RouteEntry* best_route = nullptr;
      uint8_t max_len = 0;

      for ( const RouteEntry& route : routing_table_ ) {

        // viable route match
        bool route_matched = route_matches( dg.header.dst, route.route_prefix, route.prefix_length );

        if ( route_matched ) {

          if ( route.prefix_length >= max_len ) {

            max_len = route.prefix_length;
            best_route = &route;
          }
        }
      }

      // no route matched
      if ( best_route == nullptr ) {
        continue;
      }

      // forward datagram
      Address next_hop_address = best_route->next_hop.value_or( Address::from_ipv4_numeric( dg.header.dst ) );
      interfaces_[best_route->interface_num]->send_datagram( dg, next_hop_address );
    }
  }
}

bool Router::route_matches( uint32_t dst_ip, uint32_t route_prefix, uint8_t prefix_length ) const
{

  if ( prefix_length == 0 ) {

    // default
    return true;
  }

  uint32_t shift = 32 - prefix_length;
  return ( dst_ip >> shift ) == ( route_prefix >> shift );
}