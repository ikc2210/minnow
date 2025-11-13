Checkpoint 5 Writeup
====================

My name: Ina Chun

My SUNet ID: inachun

I collaborated with: cba1, czhang7

I would like to thank/reward these classmates for their help: cba1, czhang7

This checkpoint took me about 3 hours to do. I did not attend the lab session.

Program Structure and Design of the NetworkInterface [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]:

I primarily used mpas in my design of the network interface. 
I first used a map that mapped from IP addresses to ARP Entry structs, each of which
holds an address and a timestamp. This map allows us to have O(logN) lookup in order to see if
we already know a destination address before sending. There is also another map that maps IP addresses to timestamps of when an ARP request was last sent. This allows us to implement essentially a cooldownp eriod in order to prevent netwrok flooding. Finally, there is a third map that maps IP addreses sto the lists of datagrams that are waiting for ARP resolution. Datagrams are queueud here and we send them once we receive the ARP reply in order to know a destination's MAC address.

In impelmenting the functions, we utilize these maps to be quite efficient and straightforward. For instnace, in send_datagram, we check the cache and if a MAC address 
is known, we immediately send the datagram. If not, we then queues the datagram and sends an ARP request, with the timeout in place. Recv_frame hanls IPv4 datagrams and ARP messages differently. The last funciton (tick) manages an expiration for cache entries and also a cleanup for datagrams that were queued but were never sent out because they never received 
ARP responses. 

Another design alternative could have been to not drop old queued datagrams, which would make
implementation simpler but would have wasted memory. Furthermroe, storing one datagram per IP 
could also be similarly simpler but would not be able to preserve all queued datagrams and packets could be dropped.


Implementation Challenges:
I had to re-reference some details of ARPMessage, as I kept facing minor bugs. I also
had to remember to learn the mappings from both ARP replies and requests.

Remaining Bugs:
None known, although I would like to stress test my code to make sure we don't flood the network. I'd also want to try rapid retries and make sure the 5 second cooldown window is well respected even if tried many times simultnaeously.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
