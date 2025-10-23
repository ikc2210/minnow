Checkpoint 3 Writeup
====================

My name: Ina Chun

My SUNet ID: inachun

I collaborated with: czhang7

I would like to thank/reward these classmates for their help: czhang7

This checkpoint took me about 5 hours to do. I did not attend the lab session.

Program Structure and Design of the TCPSender [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: 

This TCPSender was an interesting assignment to pair our last implementation of the Receiver.
While the previous checkpoints focused on translatin segments to a byte stream, this 
checkpoint implemented a translation from a byte stream to segments. Multiple features were 
supported in this TCPSender implementation, including a retransmission timer, payload segmentation, 
and processing of cumulative ACKs.

I utilized a queue (outstanding_messages_) to keep track of TCPSender messages that had been
sent but were yet to be acknowledged. The queue allowed for easier tracking of cumulative order, 
as we would need to pop from the front of said queue. This would give a O(1) way to access
the front element with ease, and ACKS were expected to resolve segments in the order they were 
sent. Other design options, such as a vector, would replicate this functionally but make it more 
difficult to keep order, and we also did not need the extra features (such as indices or any key-value 
pairings). If our sender was not expected to be cumulative and needed to retransmit
messsges out of order, our queue would have been insufficient.

I also utilized other variables (such as the bools sent_SYN_, sent_FIN_) to ensure that the appropriate 
flags (SYN, FIN) were only sent once. I also stored other information to implement
the timer, such as the retransmission timeout (rto_) and the retrainsmission_cnt_ for easy access.
The total_time_passed_ also kept a uniform state of the amount of the time that passed since the 
beginning of the timer. 

My implementation on sequence_numbers_in_flight() could likely have been improved, as I 
currently compute the total number of unacknowledged seqnos by linear iteration, which cause O(n). 
However, I could make this function O(1) by ultimately maintaining a running counter that
I update whenever it is modified. For the sake of this checkpoint, however, I found it sufficiently
efficient to use a linear approach.

Hands-on Activity (with Labmate):

First began with 500,000 bytes (half a megabyte), then iterated to 1 megabyte, then 5 megabytes, then 10 megbaytes, 20 megabytes, and then 50 megabytes. We stopped at 50 megabytes due to limited time spent working
on this assignment. Catherine was the server, and I was the client. This took about 5 minutes.

For 50 MB:
- Her SHA256: fc13129df98586708e0d4aa00f2be6f4be9e442dfa8f4463c8ef4c86c697b420  /tmp/big.txt
- My SHA256: fc13129df98586708e0d4aa00f2be6f4be9e442dfa8f4463c8ef4c86c697b420  /tmp/big-received.txt

- Her file size (ls output): -rw-rw-r-- 1 catherinemnzhang catherinemnzhang 50000000 Oct 21 18:28 /tmp/big.txt
- My file size (ls output): -rw-rw-r-- 1 inachun inachun 50000000 Oct 21 18:32 /tmp/big-received.txt


Implementation Challenges:
I had quite a bit of debugging to get through for this checkpoint, as I often forgot that 
FIN and SYN would each require a seqno. Furthermore, I often had to remind myself of the documentation
of previous checkpoints, since I would forget about some components until their relevant tests failed 
(for instance, RST and error-checking). In addition, I initially overcomplicated the timer design (as I was 
unsure of how re-defining the rto_ would affect outstanding messages) and needed to redesign the way I was
storing the passage of time.

Remaining Bugs:
Given the linear approach for my sequence_numbers_in_flight(), I'm not 100% confident how 
well my code would run on stress tests. I would also like to add further congestion control 
if I had more time.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: how much longer it took communicate with my labmate over the 
Internet, with one of us running tcp_native as a server and the other running tcp_ipv4 as the
client. Though I expected the time delay to be much larger, I was not prepared for how much 
longer it would take to transmit even 1 MB to the labmate. This part of the checkpoint was definitely
a test of patience.

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [submit as GitHub PR
  and include URL here]
