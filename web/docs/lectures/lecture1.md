# Lecture 1: Course Introduction and Programming Environment Setup

This lecture is devoted to the software suite install so that everybody can follow the other lectures from Insa or from his home if it needs to be done in distant work.

## Course outline
All Lecture (2h on a computer) are labs on the LyraT board

####Part 1 : Board introduction and Audio Signal Processing Basics </b>

- Lecture 1 (2h): Course Introduction, programming env setup
- Lecture 2: Basic signal processing,
- Lecture 3: Audio systems architecture, audio callback
- Lecture 4: Hardware Audio codec configuration
- Lecture 5 & 6: Audio signal processing basics synthesis
- Lecture 7: Faust Language tutorial [https://faust.grame.fr/](https://faust.grame.fr/)

#### Part 2: Embedded Audio System Architecture
- Lecture 8: RTone comference on embedded systems in industry [https://rtone.fr/](https://rtone.fr/)
- Lecture 9: TBD
- Lecture 10: TDB 

#### Part 3: LyraT programming
- Lecture 11-14: mini project
- Lecture 15-16: demonstrations

#Introduction to AUD2020 and Teensy
<figure>
<p>
<img src="img/teensy40_front.jpg"  width="30%"> 
<img src="img/teensy3_audio.jpg"  width="30%"> 
<img src="img/teensy3_audio_2.jpg"  width="30%"> 
</p><figcaption><center>Teensy 4.0  from [PRJC](https://www.pjrc.com/store/teensy40.html),  and the associated audio adaptor board </center></figcaption>
</figure>

The development in AUD are performed on [teensy](https://www.pjrc.com/store/teensy40.html) which is developped by PJRC. It is a microcontroller that offers many I/O pins and a USB interface. It is programmed using  specialization of the arduino programming environnement ([teensyduino](https://www.pjrc.com/teensy/teensyduino.html)).

 Teensy is a brand of microcontroller development boards  designed by the co-owner of PJRC, [Paul Stoffregen](https://github.com/PaulStoffregen). The first 
 Teensy 2.0, Teensy++ 2.0 (and discontinued predecessors) use an 8-bit AVR microcontrollers. Teensy 3.0 (and up) have instead Freescale microcontrollers, running ARM Cortex-M CPUs. The technical characteritics of all Teensy can be compared [here](https://www.pjrc.com/teensy/techspecs.html)

In AUD, we use [Teensy 4.0](https://www.pjrc.com/store/teensy40.html) which contain an ARM Cortex-M7 at 600 MHz with a Floating point unit, hence it can handle non trivial audio treatment.
<figure>
<p>
<img src="img/teensy40_front.jpg"  width="30%"> 
<img src="img/teensy3_audio.jpg"  width="30%"> 
<img src="img/teensy3_audio_2.jpg"  width="30%"> 
</p><figcaption><center>Teensy 4.0  from [PRJC](https://www.pjrc.com/store/teensy40.html),  and the associated audio adaptor board </center></figcaption>
</figure>


#Teensy 4.0 and Audio shield (from PJRC website).
Teensy 4.0 uses many powerful CPU features useful for true real-time microcontroller platform. The CPU is an ARM Cortex-M7 dual-issue superscaler clocked at at 600 MHz.  CPU performance is many times faster than typical 32 bit microcontrollers. The Floating Point Unit performs 32 bit float and 64 bit double precision math in hardware. DSP extension instructions accelerate signal processing, filters and Fourier transform. The Audio library automatically makes uses of these DSP instructions.
<p align="center">
 <img src="img/coremark_barchart_t40_small.png" /> <br>
 Teensy performance from Core Benchmarks
</p>



This pinout reference card comes with Teensy 4.0  (<em >do not loose it! </em>). The pins are not 5V tolerant. Do not drive any digital pin higher than 3.3V.


<figure>
<center>
<img  src="img/teensy40_card10a_rev2.png" width="40%">&nbsp  &nbsp  &nbsp   <img  src="img/teensy40_card10b_rev2.png" width="40%">
</center>
<center>
<caption> Teensy 4.0 pin map (from  PJRC webite)</caption>
</center>
</figure>

Teensy 4.0 has a total of 40 input/output signal pins. 24 are easily accessible when used with a solderless breadboard. The available pins include general purpose IO (GPIO, digital or analog, i.e. ADC), as well as integrated serial protocols (I2C, I2S, CAN,  SPI and UART protocols) that are used to connect to other devices. 

In AUD, we use the [audio adaptor board](https://www.pjrc.com/store/teensy3_audio.html) provided by PJRC that integrates a low power stereo codex (NXP Semiconductors SGTL5000 codec) and a SD card reader.


<figure>
<center>
<p>
<img src="img/teensy4_audio_front.jpg"  width="30%"> &nbsp &nbsp &nbsp 
<img src="img/teensy4_audio_back.jpg"  width="30%"> 
</p><figcaption><center>Teensy audio adaptor (rev. D)  from [PRJC](https://www.pjrc.com/store/teensy40.html),   </center></figcaption>
</center>
</figure>

The audio codec connects to Teensy using 7 signals (Yellow signal in pin map above) which are used by two protocol: I2C and I2S. This is a traditionnal configuration for audio codec: the I2C (or I²C: Inter-Integrated Circuit) protocol   is used to configure the codec (sample rate, input and output pins etc.) and the I2S (or I²S: Inter-IC Sound) is used to transfer samples bit by bit in both direction (i.e. from and to the teensy). The I2C pins SDA and SCL are used to control the chip and adjust parameters. Audio data uses I2S signals, TX (to headphones and/or line out) and RX (from line in or mic), and 3 clocks, LRCLK (44.1 kHz), BCLK (1.41 MHz) and MCLK (11.29 MHz). All 3 clocks are created by Teensy which means that the SGTL5000 operates in "slave mode".

The schematics of the audio shield board, rev. D,  can bee seen [here](https://www.pjrc.com/store/teensy3_audio.html) and the schématic of the Teensy 4.0 can be seen at the end of the page [here](https://www.pjrc.com/store/teensy40.html). Of course, as they are both made by PJRC, they are designed to be compatible. We (the teachers!) have soldered the connectors so that the audio shiel can be easily connected to the tennsy.

The USB connector of the Teensy can support many serial communication from the host computer to the Teensy: (JTAG for flashing/programming, Serial UART, midi, mouse etc. see `Tools -> USB Type` menu in arduino IDE). In AUD it is mainly used to program the device (i.e. download binary code into flash memory) and textual communication between the host and the Teensy using UART communicatino protocol (Serial). In linux machine, the serial port will appear as `/dev/ttyACM0`

Teensy 4.0 has 2 Mbyte of flash memory intended for storing your code. The flash memory can also store read-only variables and arrays.1024K of memory is available for variables and data. Half of this memory (RAM1) is accessed as tightly coupled memory for maximum performance. The other half (RAM2) is optimized for access by DMA. Normally large arrays & data buffers are placed in RAM2, to save the ultra-fast RAM1 for normal variables.  The memory map is the following:
<figure>
<center>
<img  src="img/teensy4_memory.png" width="50%">
</center>
<center>
<caption> Teensy 4.0 pin map (from  PJRC webite)</caption>
</center>
</figure>

When the compiler builds your program, all global variables, static variables, and compiled code is assigned to dedicated locations in memory. This is called static allocation, because the memory addresses are fixed. By default, allocation tries to use the ultra-fast DTCM & ITCM memory. The following keywords allow control over where the compiler will place your variables and code within the memory.
{TODO: shorten that:}


DMAMEM - Variables defined with DMAMEM are placed at the beginning of RAM2. Normally buffers and large arrays are placed here. These variables can not be initialized, your program must write their initial values, if needed.
PROGMEM & F() - Variables defined with PROGMEM, and strings surrounded by F() are placed only in the flash memory. They can be accessed normally, special functions normally used on 8 bit boards are not required to read PROGMEM variables.
    FASTRUN - Functions defined with "FASTRUN" are allocated in the beginning of RAM1. A copy is also stored in Flash and copied to RAM1 at startup. These functions are accessed by the Cortex-M7 ITCM bus, for the fastest possible performance. By default, functions without any memory type defined are treated as FASTRUN. A small amount of memory is typically unused, because the ITCM bus must access a memory region which is a multiple of 32K.
    FLASHMEM - Functions defined with "FLASHMEM" executed directly from Flash. If the Cortex-M7 cache is not already holding a copy of the function, a delay results while the Flash memory is read into the M7's cache. FLASHMEM should be used on startup code and other functions where speed is not important. 

As your program runs, it may use all of the RAM which was not reserved by static allocation. Because the specific memory address for each variable is computed as your program runs, this is called dynamic memory allocation.

Local Variables - Local variables, and also return addresses from function calls and the saved state from interrupts are placed on a stack which starts from the top of RAM1 and grown downward. The amount of space for local variable is the portion of RAM1 not used by FASTRUN code and the initialized and zeroed variables.
Heap - Memory allocated by C++ "new" and C malloc(), and Arduino String variables are placed in RAM2, starting immediately after the DMAMEM variables. 
	
#Teensy  developpement framework: teensyduino
	
[Arduino's IDE](https://www.pjrc.com/teensy/td_download.html) software with the Teensyduino add-on is the primary programming environment for Teensy. Other environment can be used: [Visual Micro](https://www.visualmicro.com/), [PlatformIO](https://platformio.org/platformio-ide) or traditionnal command line Makefile (type `make` in directory `$(arduino)/{Arduino}/hardware/teensy/avr/cores/teensy4/`). 

TEENSYDUINO
 
PROGRAMMING 



###Installing teensyduino on your computer

#Getting Started on TC Machines

### Flashing the LED.

# Known Problems
