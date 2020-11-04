# SCV_dumper
 A simple card dumper for Yeno / Epoch Super Cassette Vision using an Arduino Uno.
 
This is a work in progress, the only game dump was Astro Wars 2, so you can see the rom size is hard-defined in the arduino sketch.

The sketch workflow is:
- Initialize
- Send "READY:" to the serial port
- Wait for "READ ALL" from the serial
- Send "START:" to the serial
- Read the rom, send hexadecimals to the serail port
- when the maximum size is reach, send ":END" to the serial
- loop

optional command:
- GETSIZE : return the actual read range
- SETSIZE xxxxx : set xxxxx as read range (in bits)

You can use the serial monitor of the arduino IDE to comunicate with the dumper, and a hexadecimal editor to put the result in a binary.

Todo :
- [x] allow possibility to change the rom size from the serial
- [ ] create a tool to send serial command / build the binary
- [ ] allow to send binary to write on a flashcard
- [ ] create a flashcard :)
- [ ] save/read saves form games with ram

 
This project was possible thanks the followings links:
- [Arduino: ColecoVision Cartridge Reader by MatthewH](https://www.instructables.com/id/Arduino-ColecoVision-Cartridge-Reader/) for the general concept.
- [Enri's Super Cassette Vision page](http://www43.tok2.com/home/cmpslv/Scv/EnrScv.htm) for the hardware documentation
- [Furrtek Gameboy dumper](http://furrtek.free.fr/?a=gbflash) for the use of 74LS164 instead of 74HC595

**DISCLAMER** : I'm not reponsible if you brun / broke your own hardware. **To use at your own risk**.