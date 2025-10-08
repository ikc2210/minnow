Checkpoint 1 Writeup
====================

My name: Ina Chun

My SUNet ID: inachun

I collaborated with: cba1

I would like to thank/reward these classmates for their help: 

This lab took me about [n] hours to do. I did not attend the lab session.

I was surprised by or edified to learn that: [describe]

Report from the hands-on component of the lab checkpoint: [include
information from 2.1(4), and report on your experience in 2.2]


2.1(4): The average round-trip delay between my VM sending an "echo request" and receiving an "echo reply" is about 62.262 ms. The delivery rate is 100% and the loss rate is 0%.

Datagrams were not duplicated.

Yes, I was able to examine an ICMP echo request in Wireshark and noted that all IPv4 header fields were as expected. For instance, Version = 4, IHL = 5, time to live = 63, protocol = 1, header checksum = 0x4024, source address = 10.144.0.159, destination address = 10.144.0.158.

When comparing between the sender VM and the receiver VM, most fields were identical. However, the TTL was reduced (changed from 64 to 63) and the IP header checksum changed. 

----

Describe Reassembler structure and design. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

Implementation Challenges:
[]

Remaining Bugs:
[]

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]
