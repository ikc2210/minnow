#include "tcp_sender.hh"
#include "debug.hh"
#include "tcp_config.hh"
#include <queue>

using namespace std;

// How many sequence numbers are outstanding?
uint64_t TCPSender::sequence_numbers_in_flight() const
{

  // runtime considerations here, could technically store internally?

  uint64_t total = 0;
  std::queue<TCPSenderMessage> outstanding_copy = outstanding_messages_;

  while ( !outstanding_copy.empty() ) {
    total += outstanding_copy.front().sequence_length();
    outstanding_copy.pop();
  }

  return total;
}

// How many consecutive retransmissions have happened?
uint64_t TCPSender::consecutive_retransmissions() const
{
  return retransmission_cnt_;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  uint64_t window_sz = window_sz_ ? window_sz_ : 1;

  uint64_t window_end;
  if ( ack_received_ ) {
    window_end = ack_seqno_ + window_sz;
  } else {
    window_end = window_sz; // starts at seqno 0
  }

  // check if full:
  if ( window_end <= next_seqno_ ) {
    return;
  }

  uint64_t avail = window_end - next_seqno_;

  while ( avail > 0 ) { // keep trying until no more space left
    TCPSenderMessage message;
    message.seqno = Wrap32::wrap( next_seqno_, isn_ ); // need to wrap

    if ( !sent_SYN_ ) { // first segment
      message.SYN = true;
      sent_SYN_ = true;
      avail--; // SYN counts as one seqno
      next_seqno_++;
    }

    // read payload
    if ( reader().bytes_buffered() > 0 && avail > 0 ) {
      uint64_t size_of_payload = min( { avail, TCPConfig::MAX_PAYLOAD_SIZE, reader().bytes_buffered() } );

      read( reader(), size_of_payload, message.payload );

      avail = avail - size_of_payload;
      next_seqno_ = next_seqno_ + size_of_payload;
    }

    // check FIN
    if ( reader().is_finished() && !sent_FIN_
         && avail >= 1 ) { // needed to add avail >=1 here, since FIN requires seqno

      message.FIN = true;
      sent_FIN_ = true;
      // FIN also takes up seqno
      avail--;
      next_seqno_++;
    }

    if ( reader().has_error() ) {
      message.RST = true;
    }

    if ( message.sequence_length() == 0 ) {
      break;
    } // prevent sending empty message
    transmit( message );
    outstanding_messages_.push( message ); // message sent out

    // start timer if needed
    if ( !timer_is_running_ ) {

      timer_is_running_ = true;
      total_time_passed_ = 0; // reset
    }

    // break out of loop:
    if ( message.FIN ) {
      break;
    }

    // need to wait for more data:
    if ( !reader().is_finished() && reader().bytes_buffered() == 0 ) {
      // stream if curr empty, can't send yet
      break;
    }
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  TCPSenderMessage empty_msg;
  empty_msg.seqno = Wrap32::wrap( next_seqno_, isn_ );
  empty_msg.RST = reader().has_error();
  return empty_msg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  if ( msg.RST ) {
    reader().set_error();
    return;
  }

  window_sz_ = msg.window_size; // window size can be updated

  if ( !msg.ackno.has_value() ) {
    return;
  }

  uint64_t ack_no = msg.ackno->unwrap( isn_, next_seqno_ );

  // invalid acknos - maybe should indicate error?
  if ( ack_no > next_seqno_ ) {
    return;
  }

  ack_received_ = true;
  ack_seqno_ = ack_no;

  bool need_rto_reset = false;
  while ( !outstanding_messages_.empty() ) {

    TCPSenderMessage oldest = outstanding_messages_.front();
    uint64_t oldest_start = oldest.seqno.unwrap( isn_, next_seqno_ );
    uint64_t oldest_end = oldest_start + oldest.sequence_length();

    if ( ack_no >= oldest_end ) { // if acknowledged
      outstanding_messages_.pop();
      need_rto_reset = true;

    } else { // oldest is still oustanding
      break;
    }
  }

  if ( need_rto_reset ) { // point 7 in spec
    rto_ = initial_RTO_ms_;
    retransmission_cnt_ = 0;

    if ( !outstanding_messages_.empty() ) {

      total_time_passed_ = 0;
      timer_is_running_ = true;
    } else {
      timer_is_running_ = false;
    }
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  total_time_passed_ += ms_since_last_tick;

  if ( !timer_is_running_ ) {
    return; // or error?
  }

  if ( total_time_passed_ >= rto_ ) { // expired
    if ( !outstanding_messages_.empty() ) {
      TCPSenderMessage earliest = outstanding_messages_.front();
      transmit( earliest );
    }

    if ( window_sz_ > 0 ) {
      retransmission_cnt_++;
      rto_ *= 2;
    }

    total_time_passed_ = 0; // reseet
  }
}
