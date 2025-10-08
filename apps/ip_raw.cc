#include "socket.hh"

using namespace std;

// int main()
// {

//   RawSocket sock;
//   string datagram;
//   datagram += 0b0100'0101;
//   datagram += string(7,0);
//   datagram += 64;
//   datagram += 5;
//   datagram += string( 6, 0 );

//   // destination address
//   datagram += char(10);  
//   datagram += char(144);
//   datagram += char(0);
//   datagram += char(159);

//   // message
//   datagram += "Hiiiiii!";

//   sock.connect(Address("10.144.0.159"));
//   sock.write(datagram);  

//   return 0;
// }

int main() { // UDP
  const string msg = "HEYYYY! Ina is sending this from 10.144.0.158";
  const Address dst{"10.144.0.159", 4242};

  UDPSocket sock;
  sock.connect(dst);
  sock.write(msg);
  return 0;
}
