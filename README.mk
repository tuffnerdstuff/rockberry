# Setup (TODO)
* Install sooperlooper (version?)
* Compile jack
* Install python osc library (which?)
* Make settings in /boot/config.txt (composite & correct overscan)
* Autorun startup script after desktop started

# Problems
I had quite some troubles avoiding false positives when reading the hardware button states.
to investigate the problem I wrote a small button test script (test_buttons.py) which compares
different methods. What I found outis, that the event detection method is much more sensitive
to short voltage spikes and raising the debounce time did not work. When I checked the button
state in a loop though, the detection was much more accurate. I found an interesting blogpost
on raspberrypi.org titled "Avoiding false hits with RPi.GPIO edge detection" which explains
this in detail. So either I go fo polling, or I use the method described in the post, where
I have to add another check for the current button state after 5ms.
