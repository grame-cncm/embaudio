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
- Lecture 8: Faust Language tutorial [https://faust.grame.fr/](https://faust.grame.fr/)

#### Part 2: Embedded Audio System Architecture
- Lecture 7: RTone comference on embedded systems in industry [https://rtone.fr/](https://rtone.fr/)
- Lecture 9: Embedded System Peripherals
- Lecture 10: embedded OS, free-RTOS, embedded Audio linux devices

#### Part 3: LyraT programming
- Lecture 11-14: mini project
- Lecture 15-16: demonstrations

#Introduction to AUD2020 and ESP3
<figure>
<p>
<img src="img/esp32-small.jpg"  width="30%"> 
<img src="img/esp32-functional-bock.png"  width="30%"> 
<img src="img/esp32-pin-layout.png"  width="30%"> 
</p><figcaption><center>ESP32 die shot, taken from [zeptobars](https://zeptobars.com/en/read/Espressif-ESP32-Wi-Fi-Bluetooth-2.4Ghz-ISM),  its functional block and its pin layout, taken from esp32 datasheet </center></figcaption>
</figure>

The development in AUD are performed on [LyraT](https://www.espressif.com/en/products/hardware/development-boards) which is developed by the espressif company.
The programming environment used is <b>esp-idf</b> and <b>esp-adf</b> (esp-adf is a wrapper around esp-idf so as to enable easy audio configuration).

[Espressif Systems](https://www.espressif.com/) is a fabless IC design company, founded in 2008 in Shanghai, China (~200 employees in 2018). Espressif is  designing and manufacturing low power wireless sensor chips.

Their main product today is the ESP32 microcontroler, based on an old Xtensa processor architecture, that is sold for less the 5€ and that is strongly oriented toward Internet of Things (IoT).  Before that was ESP8089 and ESP8266 wifi chip

#ESP32 and LyraT
The [ESP32](https://fr.wikipedia.org/wiki/ESP32) is a Single 2.4 GHz Wi-Fi and Bluetooth combo chip (i.e. radio basebands and processor integrated into the same chip). The processor is  and Xtensa Single or Dual-core 32-bit LX6
microprocessor that can reach 600 MIPS. It comes with a ROM of 448 KB, and a SRAM of 520KB and, as any micro-controller, with a bunch of peripherals (ADC, DAC, Touch pad, SPI, I2S, I2C, PWM, SDIO, Ethernet, UART, etc.), see its [functionnal blocks](lecture1/img/esp32-functional-bock.png).

The two CPUs are named “PRO\_CPU” and
“APP\_CPU” (for “protocol” and “application”), however, for most
purposes the two CPUs are interchangeable.

Espressif proposes three  documentations for the ESP32:
<ul>
<li> [esp32_datasheet_en.pdf](lecture1/img/esp32_datasheet_en.pdf): brief presentation (30 pages) of the whole ESP32 architecture, including the  {\em Memory Map} and {\em Peripheral Pin Configurations}.
<li>   [esp32_technical_reference_manual_en.pdf](lecture1/img/esp32_technical_reference_manual_en.pdf): Complete reference guide of the chip, 600 pages, useful to understand the configuration of a particular component such as a peripheral for instance.
<li> [esp32_hardware_design_guidelines_en.pdf](lecture1/img/esp32_hardware_design_guidelines_en.pdf) which presents in details the interface to ESP32 chip and is  mostly useful to integrate ESP32 in a new board. 
</ul>

We use ESP32-LyraT v4.3 evaluation board, all available documentation is present on  [espressif web site](https://docs.espressif.com/projects/esp-adf/en/latest/get-started/get-started-esp32-lyrat.html#). You will need to access to the [hardware reference](https://docs.espressif.com/projects/esp-adf/en/latest/design-guide/board-esp32-lyrat-v4.3.html) and the [schematics of the board](https://dl.espressif.com/dl/schematics/esp32-lyrat-v4.3-schematic.pdf).

<figure>
<center>
<img  src="img/lyrat-diag1.png" width="40%">
<img  src="img/lyrat-photo2.png" width="40%">
</center>
<center>
<caption> LyraT simple diagram and board layout (from  espressive webite)</caption>
</center>
</figure>


The ESP32-LyraT v3.4 is a hardware platform designed for the dual-core ESP32 audio applications, e.g., Wi-Fi or Bluetooth audio speakers, speech-based remote controllers, connected smart-home appliances with one or more audio functionality, etc.

The components are quite clearly shown on Figure above,  here are some precision:
<ul>
<li> Output socket to connect headphones use  a 3.5 mm stereo jack. The socket may be used with mobile phone headsets and is compatible with OMPT standard headsets only. It does not work with CTIA headsets.
<li> <b> When programming (i.e. flashing) the board </b>, the following actions must be performed: hold down the Boot button and simultaneously momentarily press the Reset button. This  initiates the firmware upload mode. Then user can upload firmware through the serial port (using the flash program on the host computer).
<li> <b>once the board is  programmed (i.e. flashed)</b>, pressing the Reset button is necessary for the new program to start.
<li> The audio chip used is the [``ES8388`` from Everest](lecture1/img/ES8388-EverestSemiconductor.pdf). It is quite important because performance and properties of audio codec vary a lot from one to another. It is connected to I2C and I2S busses of the ESP32.
<li> The <b>USB-UART</b> port is used to have a serial communication between the ESP32 and the host computer as well as for flashing/programming the ESP32 with JTAG protocol using  ``openocd`` tool.
<li> The Green 'Standby/Charging' LED indicates that the board is powered from USB. The red 'Power On' LED indicates that the board is on (there is a switch to cut it off). The 'Green' LED can be used by the user program.
</ul>

#ESP32 developpement framework: ESP32 IDF
IDF stands for  IOT Development Framework, it is relatively straightforward to install it on your computer. 
It has been installed on TC dept machines, in ``/opt/esp-adf/esp-idf``. In order to use it, you have to run the file ``export.sh`` located in the directory where you have installed IDF.

###Installing ESP32 IDF on your computer
Note that IDF installation uses more than 3GB of disk space. 
Note also you will need to have Python3 (and not Python2.7), you can handle different version of Python on Linux using [update-alternatives](https://linuxconfig.org/how-to-change-from-default-to-alternative-python-version-on-debian-linux)

Follow the instruction on the [espressive IDF getting started page](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) and install IDF on your computer (The installation is quite long 10-20 minutes depending on the quality of your connection).

Although it is not mandatory, it is recommended to add the ``IDF_PATH`` (which is the location where you installed ESP32-IDF) in your environment. 
When you have install ESP32-IDF in directory ''ESP32-IDF'', you have to source the ``export.sh`` file before building a project:

```source  ESP32-IDF/export.sh```


it is not recommended to perform the source of ''export.sh'' in the profile script because it might invalidate other tool using python. Instead you can define a command for performing the source:

```alias get_idf='. $HOME/esp/esp-idf/export.sh'```

###Connecting LyraT and Running an example project
Follow [espressive tutorial (final steps)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) to run the `` get-started/hello_world/`` example.

You should be able to have a LED blinking and to interact with the UART connection.

### Different compilation tools used

ESP32-IDF projects supports several build/compilation tools:
<ul>
<li> ``make``, using a generic and quite complex ``ID_PATH/make/project.mk`` Makefile for all existing project. This was the original only tool used IDF, however it is being progressively replaced by the ``cmake`` compilation tools. 
<li> ``cmake`` which is the recommended toolchain as ``make`` might not be supported anymore in further version. Here is an example of project compilation with ``cmake``:

```
mkdir build;
cd build;
cmake ../
make 
make flash 
make monitor
```

<li> ``idf.py``  is a top-level python config/build command line tool for ESP-IDF provided by espressive build. Here is an example of project compilation with ``idf.py``:
```
idf.py all
idf.py flash 
idf.py monitor
```
<li> we recommend the use of ``cmake`` tool.
</ul>




