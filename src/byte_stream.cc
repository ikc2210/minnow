#include "byte_stream.hh"
#include "debug.hh"
#include <cstdint>
#include <algorithm>


using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

// Push data to stream, but only as much as available capacity allows.
void Writer::push( string data )

{
  // edge case
  if (is_closed()) { return; }

  const uint64_t len = min<uint64_t>( available_capacity(), static_cast<uint64_t>(data.size()) );
  if (len == 0) { return; }

  buf_.append(data.data(), static_cast<size_t>(len)); 
  total_pushed_ += len;
}

// Signal that the stream has reached its ending. Nothing more will be written.
void Writer::close()
{
  isclosed_ = true;
}

// Has the stream been closed?
bool Writer::is_closed() const
{
  return isclosed_;
}

// How many bytes can be pushed to the stream right now?
uint64_t Writer::available_capacity() const
{

  const uint64_t remaining = static_cast<uint64_t>(buf_.size()) - offset_;
  if (remaining <= capacity_) { 
    return capacity_ - remaining; 
  } else {
    return 0;
  }
}

// Total number of bytes cumulatively pushed to the stream
uint64_t Writer::bytes_pushed() const
{
  return total_pushed_; // all pushed
}

// Peek at the next bytes in the buffer -- ideally as many as possible.
// It's not required to return a string_view of the *whole* buffer, but
// if the peeked string_view is only one byte at a time, it will probably force
// the caller to do a lot of extra work.
string_view Reader::peek() const
{
  const uint64_t remaining = static_cast<uint64_t>(buf_.size()) - offset_;
  return string_view( buf_.data() + offset_, remaining);
}

// Remove `len` bytes from the buffer.
void Reader::pop( uint64_t len )
{
  const uint64_t remaining = static_cast<uint64_t>(buf_.size()) - offset_;
  const uint64_t num = min<uint64_t>(len, remaining);
  offset_ += num;
  total_popped_ += num;

  if (offset_ > buf_.size() / 2) {
    buf_.erase(0, offset_);
    offset_ = 0;
  }
}

// Is the stream finished (closed and fully popped)?
bool Reader::is_finished() const
{
  return ( buf_.size() - offset_ == 0) && isclosed_;
}

// Number of bytes currently buffered (pushed and not popped)
uint64_t Reader::bytes_buffered() const
{
  return buf_.size() - offset_;
}

// Total number of bytes cumulatively popped from stream
uint64_t Reader::bytes_popped() const
{
  return total_popped_;
}
