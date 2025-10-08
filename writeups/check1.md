Checkpoint 1 Writeup
====================

My name: Ina Chun

My SUNet ID: inachun

I collaborated with: cba1

I would like to thank/reward these classmates for their help: 

This lab took me about 10 hours to do. I did not attend the lab session.

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

I primarily utilized a map with the key being the start index of each segment, and the value being the data itself. Because the segments were going to be processed in disjoint order, I thought it would be most efficient to utilize a map. I considered using a linked list or some similara data structure, but soon realized that would require O(n) intsertion/lookup, instead of O(logn) like a map. A map also allowed lookup to be more efficient because of sorting by index.

I utilized a pending_ variable to track the number of bytes stored in the map, which was used by the helper function. I also store the index of the first unassembled segment, along with eof_is_known_ and eof_index in order to know whether we've received the last substring (and if so, where it ends). 

I realize that the last two variables could have been combined into an optional one of uint64_t type, but I chose to separate them to make the logic clear for myself when coding.

The insert function first cnonsiders whether the data can fit inside the allowed space, and clips edges that will not fit in the allowed space. The function then checks if the previous segment (to the left of this segment) overlaps or touches the current; if so, we combine them together. The function then looks to the right of the current segment as well.

Implementation Challenges:
I had to spend a lot of time debugging the merging segment of the code, as there were many scenarios of overlapping left/right segments. For instance, if the currently-processed segment overlapped on both the left and right edge, the function needed to avoid certain edge cases. Furthermore, I had to debug for a while to figure out some errors where I wasn't updating pending_ properly, which caused some test cases to fail at first.

Remaining Bugs:
I would want to further test large segments, or inputs where there are many tiny segments with a lot of complicated overlaps. Although my code passes all test cases, I would want to be more confident that all the edge cases of overlaps are handled correctly.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]
