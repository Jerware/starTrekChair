This code adds sound & light to Adam Savage's Star Trek captain's chair
featured in the following Tested video:

https://youtu.be/jaVi06DaTk0

It compiles on Teensy 3.1, requires a Teensy Audio Board, and the two
code bases are designed to work in parallel. Both are available in this
GITHUB repository:

https://github.com/Jerware/

Jeremy Williams wrote the code and designed the sound/LED hardware.

Update April 2017:
This is the second revision of the code & hardware. The sound board now acts
as a master and the LED board is a slave. All buttons are routed solely
to the sound board, and it sends signals to the LED board via hardware
serial. There is also a pin expander IC incorporated instead of the pins
on the underside of the Teensy 3.1.

A PCB is now available from OSH Park:
https://oshpark.com/shared_projects/sdLJDqlW

As always, thanks to the amazing library developers for keeping Arduino coding
from becoming a chore. You are all giants. I'll monitor the feedback on Github
from time to time but this code should be considered unsupported. #LLAP.
