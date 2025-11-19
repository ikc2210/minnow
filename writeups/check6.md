Checkpoint 6 Writeup
====================

My name: Ina Chun

My SUNet ID: inachun

I collaborated with: czhang7, cba1

I would like to thank/reward these classmates for their help: czhang7, cba1

This checkpoint took me about 3 hours to do. I did not attend the lab session.

Program Structure and Design of the Router [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: 

I implemented the routing table using a simple vector that stored a custom struct called RouteEntry.
The RouteEntry struct contains the route perfix, prefix length, optional next-hop-address and hte interface number.
This is a pretty simple appproach, with low risk of bugs and is quite easy to intuitively understnad. However, this 
causes an O(n) lookup time, which is not ideal but is specified in the assignment spec as acceptable. Another design
I considered was a sorted vector, which would be ordered by the prefix lenght. However, this would require further maintenance
as insertion would require us to find the appropriate place to insert in the vector. For the purposes of this lab,
I concluded that a simple vector would be sufficient.

I similarly use a simle approach to incorporate the longest-prefix algorithm, by iterating through all the routes
in the routing table and check if each route is a match with the destination address (with bit-shifting). I maintain a record of hte
longest-prefix-length route that has been seen so far, and update it accordingly when necessary.

Implementation Challenges:
This implementation was pretty straight-forward, I just needed to make sure the bit-shifting was implemented correctly. This was used 
as opposed to bit-masking, just for its ease of implementation. Another issue that I had was making sure to use const references in the 
routing table in order to avoid dangling pointers to temporary copies, and also making these "const" changes throughout my work.

Remaining Bugs:
Passes known tests, but I would like to observe the runtime (which I know is O(n)) for very large inputs.
I would also like to see how it mgiht change if I use a trie or a sorted vector.


- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]