#include <iostream>

#include "arp_message.hh"
#include "debug.hh"
#include "ethernet_frame.hh"
#include "exception.hh"
#include "helpers.hh"
#include "network_interface.hh"

using namespace std;

static size_t ARP_RETRY_TIMEOUT_ = 5000;
static EthernetAddress ETHERNET_BROADCAST_ { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static size_t ARP_ENTRY_TTL_ = 30000;


//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( InternetDatagram dgram, const Address& next_hop )
{
  const uint32_t next_hop_ip = next_hop.ipv4_numeric();
  
  if ( arp_cache_.count(next_hop_ip) == 1) { // if already known
    
    EthernetFrame frame;

    frame.header.type = EthernetHeader::TYPE_IPv4;
    frame.header.dst = arp_cache_[next_hop_ip].addr;
    frame.header.src = ethernet_address_;
    frame.payload = serialize( dgram );
    transmit( frame );
    return;
  }

  // only reaches here if address not known and need to queue
  queued[next_hop_ip].push_back( move( dgram ) );


  bool send_arp = false;
  if ( not arp_req_times_.count( next_hop_ip ) ) {
    send_arp = true;
  } else if ( time_ - arp_req_times_[next_hop_ip] >= ARP_RETRY_TIMEOUT_ ) {
    send_arp = true;
  }

  if ( send_arp ) {
    ARPMessage arp_req;

    arp_req.opcode = ARPMessage::OPCODE_REQUEST;

    arp_req.sender_ethernet_address = ethernet_address_;
    arp_req.target_ethernet_address = {};

    arp_req.sender_ip_address = ip_address_.ipv4_numeric();
    arp_req.target_ip_address = next_hop_ip;

    EthernetFrame arp_frame;
    arp_frame.header.type = EthernetHeader::TYPE_ARP;

    arp_frame.header.dst = ETHERNET_BROADCAST;
    arp_frame.header.src = ethernet_address_;

    arp_frame.payload = serialize(arp_req);
    transmit(arp_frame);

    arp_req_times_[next_hop_ip] = time_;
  }




}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  // if unintended:
  if (frame.header.dst != ethernet_address_ and frame.header.dst != ETHERNET_BROADCAST_) {
    return;
  }
  
  if ( frame.header.type == EthernetHeader::TYPE_IPv4 ) {
      
    InternetDatagram datagram;
    if ( parse( datagram, frame.payload ) ) {

      datagrams_received_.push( move(datagram) );

    }

    return;
  }



  // ARP messages

  if (frame.header.type == EthernetHeader::TYPE_ARP) {
    ARPMessage arp_msg;
    if ( not parse(arp_msg, frame.payload) ) {
      return;
    }

    // learn sender mapping
    const uint32_t ip_sender = arp_msg.sender_ip_address;
    const EthernetAddress eth_sender = arp_msg.sender_ethernet_address;
    arp_cache_[ip_sender] = {eth_sender, time_};


    // if alr queued:
    if ( queued.count( ip_sender ) ) {
      for ( auto& queued_dgram : queued[ip_sender] ) {
        EthernetFrame out_frame;
        out_frame.header.dst = eth_sender;
        out_frame.header.src = ethernet_address_;
        out_frame.header.type = EthernetHeader::TYPE_IPv4;
        out_frame.payload = serialize(queued_dgram);
        transmit(out_frame);
      }
      queued.erase( ip_sender );
    }

    // need to send reply
    if ( arp_msg.opcode == ARPMessage::OPCODE_REQUEST
      and arp_msg.target_ip_address == ip_address_.ipv4_numeric() ) {
      
        ARPMessage reply;
      reply.opcode = ARPMessage::OPCODE_REPLY;

      reply.sender_ethernet_address = ethernet_address_;
      reply.target_ethernet_address = eth_sender;

      reply.sender_ip_address = ip_address_.ipv4_numeric();
      reply.target_ip_address = ip_sender;

      EthernetFrame reply_frame;
      reply_frame.header.type = EthernetHeader::TYPE_ARP;

      reply_frame.header.dst = eth_sender;

      reply_frame.header.src = ethernet_address_;

      reply_frame.payload = serialize(reply);
      
      transmit(reply_frame);
    }

  }

}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  time_ += ms_since_last_tick;

  //get rid of expired
  for ( auto it = arp_cache_.begin(); it != arp_cache_.end(); ) {
    if ( time_ - it->second.timestamp >= ARP_ENTRY_TTL_ ) {
      it = arp_cache_.erase( it );
    } else {
      ++it;
    }
  }

  // drop after some timeout (as specified in spec FAQs)
  for ( auto it = queued.begin(); it != queued.end(); ) {

    const uint32_t ip = it->first;
    if ( arp_req_times_.count(ip) and time_ - arp_req_times_[ip] >= ARP_RETRY_TIMEOUT_ ) {

      //drop
      it = queued.erase(it);
      arp_req_times_.erase(ip);
      
    } else {
      ++it;
    }
  }



}
