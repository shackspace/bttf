BTTF Clock
==========

A triple wall clock modeled after the Time Circuits from a well-known SF movie of the 80s.

Designed by [Timm][1] and [Jocki][2] of [shackspace][3], together with and for our friends at [PCB-Pool][4].

Technology
==========

The clock is driven by an Arduino or a compatible uC board. It consists of two components:

 1. A shield for the Arduino which houses a battery-backed real-time clock (RTC) and combines Arduino pins and power into a bus.
 1. A display board representing a single clock, including numeric and alphanumeric LED displays, LEDs, LED drivers and brightness control.
    The display boards can be daisy-chained to an almost arbitrary amount. For a full triple clock, you'd use three of these, with differently colored LEDs.

Display Bus
-----------

The display bus consists of ground, two power rails and five logic signals. The two power rails are logic power and LED power -- both run at 5V, but the shield allows to separate them and power the LEDs from an external power supply while the logic including the Arduino is powered through USB. This is to prevent the external power supply from feeding into the USB or vice versa. Powering the whole clock from a PC USB port is usually not possible because the LEDs draw up to one Ampere and most hosts will disable the port as soon as the displays turn on.

The logic pins are what's needed to control a multiplexed display driven by a TLC59282 constant-current shift register towards ground, and a 74HC164 non-latching shift register power p-FETs towards 5V:

 * The TLC is controlled by the LATCH and BLANK signals, data is shifted in using the SDATA\_IN and SCLK signals. The SDATA\_OUT signal of one display connects to SDATA\_IN of the next display in the chain.
 * The shift register is clocked whenever the TLC is latched, so the LATCH signal can serve as its clock. In addition, we need a serial input !COL0 that is set to 0 (turning the p-FET on) on the first multiplex step and 1 (turning the p-FET off) on all other steps, so that we have a single "p-FET on" signal clocking through the shift register.

All signals but the SDATA_IN/OUT signals are connected in parallel across all display modules, the SDATA signals are daisy-chained from one module to the next -- as a result of this, each module has a distinct input and output connector.

Subdirectories
==============

[controller](controller) - The Arduino shield, designed in [Eagle][5], complete with BOM in CSV format.  
[display](display) - The display module, designed using [KiCad][6] because it exceeds Eagle's size limits, also with BOM.  
[firmware](firmware) - The full firmware, ready for upload using the Arduino IDE.

Setting up
==========

To complete the clock into a working thing (after soldering), you will need a battery, an Arduino (or compatible) and a 5V 1A power supply with the usual barrel plug (center positive). Put the battery into the holder, put the shield on the Arduino, connect it to the first display using a ribbon cable. The top connector on each display is the input, the bottom one is the output. Daisy-chain as many displays as you like - or have ;)

The power supply goes into the shield, not the Arduino. There's a jumper on the shield that connects the LED driver's power supply (fed through the barrel jack) and the logic power supply (including the Arduino) – leave that open if you have both the Arduino connected to the PC and the power supply hooked up to the shield, otherwise close it.

Upload the firmware to the Arduino and connect to it using the Serial Monitor at 9600 Baud. Send a "?" to the Arduino to get some help on how to set up the clock. Each display line can be set to display either a fixed value (any alphanumerical+numerical string, not just a valid time) or the current time as delivered by the battery-backed RTC. To make the RTC start ticking, you will have to set its time once. You can set each display's brightness using the trimmers on the back – you might even be able to power the thing via USB on lowest brightness, but no guarantees.

[1]: http://twitter.com/timmedia
[2]: http://twitter.com/dop3j0e
[3]: http://shackspace.de/
[4]: http://pcb-pool.com/
[5]: http://www.cadsoft.de/eagle-pcb-design-software/
[6]: http://www.kicad-pcb.org/