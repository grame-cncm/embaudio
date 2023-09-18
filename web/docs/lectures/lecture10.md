# Embedded Operating Systems

This course will present the important notions of embedded operating systems and show how a primitive operating system (protothreads) can be deployed on teensy.

## Slides

It is (temporarily) available through sildes [here](lecture10/img/cours_2_os.pdf)

## A teensy program with protothreads 
Download the [protothreads_blink.tar](lecture10/img/protothreads_blink.tar) archive and uncompress it. 

Have a look at the ``protothread_blink.ino`` program. This programs defines two protothreads:`thread_red_led` that switch the LED state every  seconds, and `thread_sine` that changes the sine tune every 0.25 seconds. Both these thread are unlock by a dedicated timer. 

Can you understand how it works? 

This example, of course, is a bit trivial because the changes in the LED state and the sine tune could have been done in timer callbacks. But, first,  we try to avoid writing to much code in the call back and, second, one has to imagine other conditions for unlocking the threads such as internal variable states or input on UART... 

 What happens if one ``PT_WAIT_UNTIL`` is commented out? For instance in ``thread_led_ref``:
 
 ``//PT_WAIT_UNTIL(pt, TIMER_LED_RED_ON == 1);``

Note finaly, note that many variables have been set as global variables as local protothread variables are not remanent.


## Add a UART printing 
copy this ``protothreads_blink`` directory to a new ``protothreads_count`` and modify the program to print on UART serial port, every second,   the number of blinking of the LED since the beginning of the program.  This imply the following tasks (see [UART teensy Documentation](https://www.pjrc.com/teensy/td_uart.html)):

- Initialize serial communication at 9600 bauds using `Serial.begin(9600);`
-  Declare a global variable `blink_count`
- Declare a new timer counting each second (up to four interval timers can be used in teensy 4.0)
- Increment `blink_count` at each blink
- Print (using `Serial.print`) the value of `blink_count` in new timer callback.
 
Use the command ``minicom -D /dev/ttyACM0`` to visualize what is send on serial port by the teensy. 

## Control LED and sound with keyboard
copy this ``protothreads_count`` directory to a new ``protothreads_control``

declare a new protothread ``static PT_THREAD(thread_receive(struct pt *pt))` that will receive characters on UART serial port for the host computer. the condition in the `PT_WAIT_UNTIL` will be `Serial.available()>0` (indicating that the receiving buffer contains some characters, see [UART teensy Documentation](https://www.pjrc.com/teensy/td_uart.html)). As soon as a character is typed, echo its ascii code. 

Use that mecanism to control the sound and LED:

- Typing 's' will switch off/on the sound (i.e. setting ``myDsp.setFreq(0)`) 
- Typing 'o' will switch off/on the LED blinking.

Hint: you can switch the value of a variable `var` between 0 and 1 using the Xor operator: `var = var ^ 1;`

** Solution **

```

#include <Arduino.h>
#include <Audio.h>

#include "MyDsp.h"
#include "protothreads/pt.h"

static struct pt pt[3];


// Teensy 3.x and 4.x  have the LED on pin 13
const int ledPin = LED_BUILTIN;
volatile int ledState = LOW;
volatile unsigned int  TIMER_LED_RED_ON = 0;
volatile unsigned int  TIMER_SINE_ON = 0;
volatile unsigned int  SOUND_ON = 1;
volatile unsigned int  LED_ON = 1;
//blink count
volatile unsigned long blink_count = 0; // use volatile for shared variables

IntervalTimer TimerLed;
IntervalTimer TimerSine;
IntervalTimer TimerCount;

MyDsp myDsp;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(myDsp,0,out,0);
AudioConnection patchCord1(myDsp,0,out,1);


static PT_THREAD(thread_led_red(struct pt *pt))
{
    PT_BEGIN(pt);

    while(1)
    {
      if (ledState == HIGH)
	ledState = LOW;
      else 
	if (LED_ON == 1)
	  ledState = HIGH;
        else
	  ledState = LOW;
      digitalWrite(ledPin, ledState);
      TIMER_LED_RED_ON = 0;
      PT_WAIT_UNTIL(pt, TIMER_LED_RED_ON == 1);
    }

    PT_END(pt);
}

static PT_THREAD(thread_sine(struct pt *pt))
{
    PT_BEGIN(pt);

    while(1)
    {
      TIMER_SINE_ON = 0;
      if (SOUND_ON == 1)
	myDsp.setFreq(random(50,1000));
      else
	myDsp.setFreq(0);
      	
      PT_WAIT_UNTIL(pt, TIMER_SINE_ON == 1);
    }
    PT_END(pt);
}

static PT_THREAD(thread_receive(struct pt *pt))
{
    PT_BEGIN(pt);

    while(1)
    {

      PT_WAIT_UNTIL(pt, Serial.available() > 0);
      while (Serial.available() > 0) {
      	int incomingByte = Serial.read();
	      Serial.print("UART received: ");
	      Serial.println(incomingByte, DEC);
	      if (incomingByte == 's')
	        {
	        SOUND_ON = SOUND_ON ^ 1;
	        Serial.print("SOUND_ON is now: ");
	        Serial.println(SOUND_ON, DEC);
	        }
	      if (incomingByte == 'l') {
	        LED_ON = LED_ON ^ 1;
	        Serial.print("LED_ON is now: ");
	        Serial.println(LED_ON, DEC);
	      }
      }
    }

    PT_END(pt);
}

void timer_interrupt_led() {
  TIMER_LED_RED_ON = 1;
  //blink countxs
  blink_count++;
}

//blink count
void timer_interrupt_count() {
  Serial.print("number of blink=");
  Serial.println(blink_count);
}

void timer_interrupt_sine() {
  TIMER_SINE_ON = 1;
}



void setup(void)
{
 
  pinMode(ledPin, OUTPUT);

  //blink count
  Serial.begin(9600);

  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.5);
  
  TimerLed.begin(timer_interrupt_led, 500000);  // blinkLED to run every 0.5 seconds
  TimerSine.begin(timer_interrupt_sine, 100000);  // Sine  to run every 0.1 seconds
  TimerCount.begin(timer_interrupt_count, 2000000);  // Count blink every 2 seconds

  PT_INIT(&pt[0]);
  PT_INIT(&pt[1]);

  

}


void loop(void)
{
 
    thread_led_red(&pt[0]);
    thread_sine(&pt[1]);
    thread_receive(&pt[2]);

}

```
