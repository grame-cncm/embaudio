# Makefile for Compiling Audio without arduino tool
# inspired from Teensyduino Core Library (http://www.pjrc.com/teensy/)
# Tanguy Risset, Romain Michon with help from Guillaume Salagnac
# Arduino file needed but not arduino IDE.

# Use these lines for Teensy 4.0
MCU = IMXRT1062
MCU_LD = imxrt1062.ld
MCU_DEF = ARDUINO_TEENSY40

# Use these lines for Teensy 4.1
#MCU = IMXRT1062
#MCU_LD = imxrt1062_t41.ld
#MCU_DEF = ARDUINO_TEENSY41

# The name of your project (used to name the compiled .hex file)
TARGET = main

# configurable options
OPTIONS = -DF_CPU=600000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DUSING_MAKEFILE
#

# options needed by many Arduino libraries to configure for Teensy model
OPTIONS += -D__$(MCU)__ -DARDUINO=10813 -DTEENSYDUINO=154 -D$(MCU_DEF)

# for Cortex M7 with single & double precision FPU
CPUOPTIONS = -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb



BUILD=./build

ifdef ARDUINOPATH

# path location for Teensy Loader,  teensy_reboot
TOOLSPATH = $(abspath $(ARDUINOPATH)/hardware/tools)

# path location for Arduino libraries 
LIBRARYPATH = $(abspath $(ARDUINOPATH)/libraries)

# path location for the arm-none-eabi compiler
COMPILERPATH = $(abspath $(ARDUINOPATH)/hardware/tools/arm/bin)

###################################################
# PATH added for external compilation
##################################################
KERNEL_SOURCES = $(ARDUINOPATH)/hardware/teensy/avr/cores/teensy4
AUDIO_SOURCES = $(ARDUINOPATH)/hardware/teensy/avr/libraries/Audio
SPI_SOURCES = $(ARDUINOPATH)/hardware/teensy/avr/libraries/SPI
SD_SOURCES = $(ARDUINOPATH)/libraries/SD/src
SERIALFLASH_SOURCES = $(ARDUINOPATH)/hardware/teensy/avr/libraries/SerialFlash
WIRE_SOURCES = $(ARDUINOPATH)/hardware/teensy/avr/libraries/Wire
MYDSPPATH = /home/trisset/cours/2022/AUD/embaudio/examples/teensy/libraries/mydsp/src

DIRS = $(KERNEL_SOURCES) $(KERNEL_SOURCES)/util $(AUDIO_SOURCES) $(AUDIO_SOURCES)/utility  $(SPI_SOURCES) $(SD_SOURCES) $(SD_SOURCES)/utility $(SERIALFLASH_SOURCES) $(WIRE_SOURCES) $(WIRE_SOURCES)/utility $(MYDSPPATH)

INCLUDE_DIRS = -I$(KERNEL_SOURCES) -I$(KERNEL_SOURCES)/utils -I$(AUDIO_SOURCES) -I$(AUDIO_SOURCES)/utility  -I$(SPI_SOURCES) -I$(SD_SOURCES) -I$(SD_SOURCES)/utility -I$(SERIALFLASH_SOURCES) -I$(WIRE_SOURCES) -I$(WIRE_SOURCES)/utility -I$(MYDSPPATH)

INCLUDE_FLAGS = $(INCLUDE_DIRS)

LIBPATH = $(ARDUINOPATH)/hardware/teensy/avr/cores/teensy4
MCU_LD = $(LIBPATH)/imxrt1062.ld

###################################################
# Rules to find files in these PATH
# list all .c .c++ and .S file in DIRS, prune used to ignore subdirs
###################################################

CFILES = $(foreach   dir,${DIRS},\
  $(shell find ${dir} -path '${dir}/*' -prune -name '*.c' ! -name "main.c"  ))
CPPFILES=$(foreach dir,${DIRS}, \
  $(shell find ${dir} -path '${dir}/*' -prune -name '*.cpp' ! -name "main.cpp" ))
SFILES=$(foreach dir,${DIRS}, $(shell find ${dir} -name '*.S'))

COBJS=$(addprefix build/,$(CFILES:.c=.o))
CPPOBJS=$(addprefix build/,$(CPPFILES:.cpp=.o))
SOBJS=$(addprefix build/,$(SFILES:.S=.o))

OBJS:= ${COBJS} ${CPPOBJS} ${SOBJS}

else
# Default to the normal GNU/Linux compiler path if NO_ARDUINO
# and ARDUINOPATH was not set.
COMPILERPATH ?= /usr/bin
#PLEASE SET ARDUINOPATH
endif


# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -O2 $(CPUOPTIONS) -MMD $(OPTIONS) -I. $(INCLUDE_FLAGS) -ffunction-sections -fdata-sections 
 
# compiler options for C++ only
CXXFLAGS =  $(CPPFLAGS) -std=gnu++14 -felide-constructors -fno-exceptions -fpermissive -fno-rtti #-Wno-error=narrowing   

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -L$(LIBPATH) -Os -Wl,--gc-sections,--relax $(SPECS) $(CPUOPTIONS) -T$(MCU_LD)

# additional libraries to link
LIBS = -larm_cortexM7lfsp_math -lm -lstdc++


# names for the compiler programs
CC = $(COMPILERPATH)/arm-none-eabi-gcc
CXX = $(COMPILERPATH)/arm-none-eabi-g++
OBJCOPY = $(COMPILERPATH)/arm-none-eabi-objcopy
SIZE = $(COMPILERPATH)/arm-none-eabi-size

# the actual makefile rules (all .o files built by GNU make's default implicit rules)

all: build $(TARGET).hex

dd:
	ls ${DIRS}/bootdata.c

# Makefile debug use only 
debug:
#	@echo MCU_LD: ${MCU_LD}
	@echo DIRS: ${DIRS}
# 	@echo CFILES: ${CFILES}
#	@echo CPPFILES: ${CPPFILES}
#	@echo COBJS: ${COBJS}
#	@echo CPPOBJS: ${CPPOBJS}
#	@echo OBJS: ${OBJS}

build:
	echo $(OBJ)
	mkdir -p build

#################################################
# ADD your own files here  needed by
#  addind obj name to $OBJ
#################################################




#################################################
# DO not modify the other rules 
#################################################

$(BUILD)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: %.S
	mkdir -p $(shell dirname $@)
	$(CC) $(CPPFLAGS) -c -o $@  $^

build/%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) $(CPPFLAGS) -c -o $@  $^

build/%.o: %.cpp
	mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $^ 

$(TARGET).elf: $(BUILD)/main.o $(OBJS) 
	$(CC) $(LDFLAGS) $^ -o $@  $(LIBS)

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@
ifneq (,$(wildcard $(TOOLSPATH)))
	$(TOOLSPATH)/teensy_post_compile -file=$(basename $@) -path=$(shell pwd) -tools=$(TOOLSPATH)
	-$(TOOLSPATH)/teensy_reboot
endif

boot: main.hex
	$(TOOLSPATH)/teensy_post_compile -file=$(basename main.hex) -path=$(shell pwd) -tools=$(TOOLSPATH)
	-$(TOOLSPATH)/teensy_reboot

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean: 
	rm -rf  $(TARGET).elf $(TARGET).hex *~ *.o *.d

distclean: clean
	rm -rf $(BUILD)
