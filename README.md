# DB540-STM
![A picture of the display that this is about displaying the text DB540-STM in large lettering, spanning all five lines.](/Images/IMG_20181003_220326.jpg?raw=true)
This is an STM32 (and Raspberry Pi) implementation for driving DB540 (and possibly many others) scrolling LED signs.
I wrote it for my Daktronics (formerly Data Display) DB054005 sign, which has 5 lines of text, each one 240 pixels wide and 7 pixels tall. The hardware is extremely simple, it's basically just a giant chain of shift registers, not even latched, and some transistors to drive the pixel rows. Connector pinouts can be found in the Hardware/ subdirectory.

## What can it do?
Currently, this software can:

* Display static graphics
* Display scrolling graphics
* Scroll separately in each line
* Scroll at different speeds per line
* Scroll in different directions per line
* Display static graphics in arbitrary patterns overlaid on top of the scrolling layer

## What can't it do?
The software can not currently:

* Render text on its own
* Scroll vertically
* Make sandwiches, toasts or bagels (the STM32 lacks a BAGEL input pin)

## How does it work?
The STM32 receives bitmap data via an SPI interface and control commands via an USART. It is designed to be interconnected with a Raspberry Pi, for which some sample Python code is included. The Raspberry Pi handles all the text and graphics rendering and sends the finished bitmap to the STM32; the STM32 then uses this bitmap and can scroll parts of it using "scroll windows" which move across the bitmap.
## License
Just use it. I don't care. The code contains some CubeMX generated stuff, so pay attention to the license of these files.