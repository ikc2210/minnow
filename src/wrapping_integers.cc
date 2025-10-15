#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  uint64_t rel = n + static_cast<uint64_t>( zero_point.raw_value_ );
  return Wrap32 { static_cast<uint32_t>( rel & 0xFFFFFFFFull ) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  const uint64_t mod = 1ULL << 32;
  const uint64_t half = 1ULL << 31;
  const uint64_t MASK = ~0xFFFFFFFFULL;

  const uint32_t offset = raw_value_ - zero_point.raw_value_;
  uint64_t base = ( checkpoint & MASK ) + static_cast<uint64_t>( offset );

  if ( base + half < checkpoint ) {
    base += mod;
  } else if ( checkpoint + half < base && base >= mod ) {
    base -= mod;
  }
  return base;
}
