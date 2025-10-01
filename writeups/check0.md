Checkpoint 0 Writeup
====================

My name: Ina Chun

My SUNet ID: inachun

I collaborated with: czhang7

I would like to credit/thank these classmates for their help: czhang7

This lab took me about 3 hours to do. I did not attend the lab session.

My secret code from section 2.1 was: 631552

I was surprised by or edified to learn that: an error should not be fatal. My first implementation failed one of the three tests because I would assume that an error should interrupt the read/write process, but we
wanted it to remain informational.

Describe ByteStream implementation. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

I used a string to maintain a buffer and implemented an offset that 
represented the beginning of the ByteStream. Peek() would return a string_view that 
skipped the unneeded first segment in buf_.data(). The prefix would only be erased
if the skipped offset became too large (aka more than half the size of the buffer). Alternative
implementations could have involved using a circular type of queue, but this would have made it harder to 
implement peek() as we would need to return a continuous string_view. However, a queue would have made pop 
much more simpler. Ultimately, I ended up going with this implementation as it was a simple 
way to provide contiguous peeks and created O91) pops. However, there could be 
better heuristics by which I should erase the offset, as opposed to the current implementation of an arbitrary half-point. My byte-stream test speed is the following (which is faster than the threshold needed for the assignment):

  Start 39: byte_stream_speed_test
    ByteStream throughput (pop length 4096):  7.81 Gbit/s
    ByteStream throughput (pop length 128):   5.90 Gbit/s
    ByteStream throughput (pop length 32):    3.48 Gbit/s

Implementation Challenges:
I debated using another heuristic to trigger the erasure of the offset. I also had to think for a bit about how we could
allow arbitrarily-long streams while maintaining the capacity constraint.

Remaining Bugs:
None -- I would ideally like to test with extremely large inputs.

- Optional: I had unexpected difficulty with: re-adjusting to writing c++ code and type-casting, as I've been coding in Java and Python all summer.

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I contributed a new test case that catches a plausible bug
  not otherwise caught: [provide Pull Request URL]

