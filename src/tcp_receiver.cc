#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{

  if ( message.RST ) {
    reassembler_.reader().set_error();
    return;
  }

  // reassembler expects indices to start at 0, need to unwrap seqno of TCPSenderMessage

  if ( message.SYN ) {
    ISN_ = message.seqno;
  }

  if ( !ISN_ ) { // can't do anything until ISN is set
    return;
  }

  uint64_t unwrapped = message.seqno.unwrap( ISN_.value(), checkpoint_ ); // could be 0 if beginning of sequence
  uint64_t index = unwrapped + message.SYN - 1;

  reassembler_.insert( index, message.payload, message.FIN );

  checkpoint_ = unwrapped + message.payload.size() + message.SYN + message.FIN;
}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage message;

  message.window_size = min( writer().available_capacity(), static_cast<uint64_t>( UINT16_MAX ) );

  message.RST = writer().has_error();

  if ( ISN_ ) {

    uint64_t ackno = writer().bytes_pushed() + 1 + ( writer().is_closed() ? 1 : 0 );

    message.ackno = Wrap32::wrap( ackno, ISN_.value() );
  }

  return message;
}
