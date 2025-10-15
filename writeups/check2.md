Checkpoint 2 Writeup
====================

My name: Ina Chun

My SUNet ID: inachun

I collaborated with: N/A

I would like to thank/reward these classmates for their help: N/A

This lab took me about 3 hours to do. I did not attend the lab session.

Describe Wrap32 and TCPReceiver structure and design. [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

In my wrap function, I used a stateless approach that doesn't actually modify the object and simply uses static implementation. I utilize a mask that keeps only the lowest 32 bits, and therefore wraps the value. The unwrap function uses the upper 32 bits (along with an offset -- the didfference between the wrapped value and the zero point), then adjusts for wraparound. The checkpoint is critical to makign sure that I consider wrapped values within the correct context, since we want to track them around this wraparound design.

This design was quite straightforward, given the clear framework that was given for the Wrap32 class. Maintaining an internal state would add unnecessary complexity and would also need to ensure that concurrent access wouldn't cause issues. With this design, the checkpoint parameter is sufficient and allows us to control the unwrapping "window" while still producing optimal performance.

Some risks therefore are that the checkpoints need to be maintained correctly by the caller, which I needed to sanity-check during development. There are also no validation steps for the unwrapped values, and I could add more error logs to sanity-check the unwrapped values before I return.

Implementation Challenges:
I needed to be careful with boundary issues, as I initially failed some tests that covered the edge of boundaries (whether with window size maximums or wraparound edge-cases).

Remaining Bugs:
I am not entirely sure about integer underflow cases, because I use teh guard of base >= mod guard, since I later subtrat base -= mod but if the base is too small, this could potentially cause underflow.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: some edge cases and realized that I 
was starting to run the tests after I had an initial implementation, instead of running
my mind through the edge cases. A good reminder for myself to not just
rely on the given test cases! :)

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [submit as GitHub PR
  and include URL here]
