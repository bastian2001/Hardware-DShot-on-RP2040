# Hardware-DShot-on-RP2040

This is an example project to keep the motor at a constant speed of 2500RPM using bidirectional dshot and a rudimentary PID controller. The motor will start turning after 10s after turning the RP2040 on.

> [!NOTE]
> March 2025
> 
> This project has been superseded by [my new library](https://github.com/bastian2001/pico-bidir-dshot)

> [!NOTE]
> April 2024
> 
> Two things about the state of this repo:
> 1. I have some newer implementations of bidirectional DShot that are more resilient towards clock deviations between RP2040 and ESC and that won't require manual clock adjustments.
> 2. I am using the wizio-pico core here, that was deprecated and put offline a month or so after I put this project live. The code in this repo, as it is, will likely not compile. I'd recommend that you check out the newer, more complete and still maintained [RP2040 Arduino Core by earlephilhower](https://github.com/earlephilhower/arduino-pico). Make sure that when you install it, you follow [these instructions](https://arduino-pico.readthedocs.io/en/latest/platformio.html#important-steps-for-windows-users-before-installing) to enable long file paths on Windows and Git.
>
> **DON'T. SEE THE UPPER NOTE**
> 
> ~You can find the newer code in my Kolibri-FC repo, or more detailed:~
> - ~[The PIO program](https://github.com/bastian2001/Kolibri-FC/blob/cf9afae8129266d358f9593f183405e2dec888e8/Firmware/src/pioasm/bidir_dshot_x1.pio)~
> - ~[Starting a DShot transmission](https://github.com/bastian2001/Kolibri-FC/blob/cf9afae8129266d358f9593f183405e2dec888e8/Firmware/src/drivers/esc.cpp#L72-L85)~
> - ~[Reading the eRPM packet and preparing the buffer for the next transmission](https://github.com/bastian2001/Kolibri-FC/blob/cf9afae8129266d358f9593f183405e2dec888e8/Firmware/src/pid.cpp#L81-L136)~
> - ~[required platform.ini parameters](https://github.com/bastian2001/Kolibri-FC/blob/645737a893532741ce766df3081f4849e4fdcfa0/Firmware/platformio.ini#L12-L18), though I don't recommend 264MHz, rather go with 132MHz, as I am facing some issues with the chip stability right now, and the 264MHz is only required because of the slow PID loop on my end 🙈~

- `outpin` is the DShot pin
- `pwmpin` is an arbitrary pin (has to be outpin + 1) that does the 10kHz PID loop frequency (used as a quick and dirty timer only, not useful for actual output)
- The processor runs at 132kHz, and is currently set to DShot 1200.
- Uses the PIO State Machines for correct timing of the DShot signal
- `appendChecksum()` is used to append a checksum to the outgoing DShot packet
- `calcRPM()` calculates the RPM based on the received packet
- `calcThrottle()` is the PID loop with the control constants kP, kI, and kD
- the pio code is somewhat documented, so not gonna go into detail here
- if you want to switch the dshot speed (300 - 2400 are possible), do the following:
  1. in bidir_dshot.pio: set `DELAY_SHIFT_COUNT` to the according value
  2. in line 60, main.cpp (`pio_sm_set_clkdiv_int_frac`) set the appropriate clock dividers (integer and fractional). There are global defines for DShot 300 up to DShot 2400.
  3. recompile the pio code using pioasm. It might be necessary to delete the old `bidir_dshot.pio.h` beforehand.
  4. potentially adjust the pwm frequency of pwmpin to a lower value. 10kHz is quite aggressive and even DShot 2400 can only go ever so slightly faster.
- the loop is actually limited in its speed even without any explicit delay due to `pio_sm_get_blocking` to the actual pwmpin frequency.
  - Keep in mind: When no ESC is attached (and thus no bidirectional data is received) the loop runs significantly slower because the state machine has to meet its timeout for waiting for data
- use `pio_sm_put(pio, sm, appendChecksum((throttle + 48) << 1));` to set the throttle to your desired value, where throttle is the throttle value from 0 to 2000
  - remove the + 48 if you want to send a specific "stop" signal. This is for example necessary to initialize the ESC
  
Sorry for the very dirty documentation, it was not meant to go online like this yet ^^. Please post an issue if you have any questions.
