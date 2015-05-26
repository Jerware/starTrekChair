This code adds sound to Adam Savage's Star Trek captain's chair
featured in the following Tested video:

https://youtu.be/jaVi06DaTk0

It compiles on Teensy 3.1, requires a Teensy Audio Board, and is designed to 
work in parallel with the LED lamp code. Both sources are available in this
GITHUB repository:

https://github.com/Jerware/

Jeremy Williams wrote the code and designed the sound/LED hardware. Please
note that it is still very much in the prototype phase, and you will find room for 
improvement. For instance, the two code bases are independent and unaware
of each other. (Each switch is wired to both boards.) A revision might employ
i2c and a master/slave relationship between the boards.

As always, thanks to the amazing library developers for keeping Arduino coding
from becoming a chore. You are all giants. I'll monitor the feedback on Github
from time to time but this code should be considered unsupported. #LLAP.