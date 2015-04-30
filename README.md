# ATTiny84BinaryClock

This project's goal is to create a binary clock using an ATTiny84 along with a DS1307 RTC.

Here's a quick video of the final product: https://www.youtube.com/watch?v=mD4HjlXqTbA

Disclaimer: I'm awful at choosing the proper location of stuff on a board :)

## Parts

- ATTiny84
- 74HC595N 8-bit Shift Register
- 6x 2N2222A PNP Transistors
- 5V Power Supply
- Bunch of wires
- 20 blue LEDs
- A perfboard big enough to fit everything but small enough to fit in the enclosure.
- A plastic box.
- 6x 1k Ω resistors for the transistors (larger values can be used).
- 4x 220 Ω resistors to limit the power provided to a LED.
- DS1307 Real Time Clock kit (I bought one from Adafruit).

## Things to know

Using the real time clock is kinda painful with the ATTiny84 because that microcontroller does not support the I2C protocol. Because of this, I've used the I2CMaster v2 which can be found here: https://forums.adafruit.com/viewtopic.php?f=25&t=13722&start=0 . I modified it to remove support for the real I2C implementation which did not compile if the board was set to ATTiny.

Make sure you verify your pin numbers in the code, I changed them at some point because my schematic was initially wrong. The pen and paper version is the most accurate of the final result.

I also used the Time library found on Arduino.cc. It is useful because it provides a cache of the time, so it doesn't have to be retrieved off the RTC every loop.
