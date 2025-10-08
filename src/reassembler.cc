#include "reassembler.hh"
#include "debug.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, std::string data, bool is_last_substring )
{
  auto& w = output_.writer();

  // end of file
  if ( is_last_substring ) {
    uint64_t end = first_index + data.size();
    if ( !eof_is_known_ || end < eof_index_ ) {
      eof_is_known_ = true;
      eof_index_ = end;
    }
  }

  // if entirely outside of acceptable area
  uint64_t unacceptable = first_unassembled_idx_ + w.available_capacity();
  if ( first_index + data.size() <= first_unassembled_idx_ || first_index >= unacceptable ) {

    if ( eof_is_known_ && first_unassembled_idx_ == eof_index_ ) {
      w.close();
    }
    return;
  }

  // erase unacceptable portions
  if ( first_index < first_unassembled_idx_ ) {
    data.erase( 0, first_unassembled_idx_ - first_index );
    first_index = first_unassembled_idx_;
  }
  if ( first_index + data.size() > unacceptable ) {
    data.resize( unacceptable - first_index );
  }

  if ( data.empty() ) {
    if ( eof_is_known_ && first_unassembled_idx_ == eof_index_ )
      w.close();
    return;
  }

  // merge overlapping segments
  uint64_t left = first_index, right = first_index + data.size();
  std::string combined = data;

  auto it = segments.lower_bound( left );
  if ( it != segments.begin() ) {
    auto prev = std::prev( it );
    uint64_t prev_left = prev->first;
    uint64_t prev_right = prev_left + prev->second.size();

    // if prev segment overlaps with left edge
    if ( prev_right >= left ) {

      // etend on left side
      if ( prev_left < left ) {

        combined.insert( 0, prev->second.substr( 0, left - prev_left ) );
        left = prev_left;
      }

      // extend on right side
      if ( prev_right > right ) {

        combined += prev->second.substr( right - prev_left );
        right = prev_right;
      }

      pending_ = pending_ - prev->second.size();

      segments.erase( prev );
    }
  }

  for ( it = segments.lower_bound( left ); it != segments.end() && it->first <= right; ) {
    uint64_t curr_left = it->first;
    uint64_t curr_right = curr_left + it->second.size();

    if ( curr_right > right ) {

      combined += it->second.substr( right - curr_left );
      right = curr_right;
    }

    pending_ -= it->second.size();
    it = segments.erase( it );
  }

  pending_ += combined.size();
  segments[left] = combined;

  // push continuous portions to output
  while ( true ) {

    auto h = segments.find( first_unassembled_idx_ );
    if ( h == segments.end() || w.available_capacity() == 0 ) {
      break;
    }

    size_t n = std::min( h->second.size(), (size_t)w.available_capacity() );
    w.push( h->second.substr( 0, n ) );

    pending_ -= n;
    first_unassembled_idx_ += n;

    if ( n == h->second.size() ) {
      segments.erase( h );
    } else {
      std::string tail = h->second.substr( n );
      segments.erase( h );
      segments[first_unassembled_idx_] = tail;
      break;
    }
  }

  if ( eof_is_known_ && first_unassembled_idx_ == eof_index_ ) {
    w.close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  return pending_;
}
