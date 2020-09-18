# Lecture 8: Embedded System Peripherals

This course will explain in more details the principles of embedded programming,  peripheral programming, an interrupt handling.

# Slides

It is (temporarily) available through sildes [here](lecture8/img/cours1-embedded.pdf)

#Exercice: use of timers
## Read the doc...
Have 10 minutes looking at chapter 19 (p498) of [esp32_datasheet_en.pdf](lecture1/img/esp32_datasheet_en.pdf): the chapter dedicated to timers.
<ol>
<li> How many timers do we have ?</li>
<!-- 4 timers: two group of timers 0 and 1 with two timers 0 and 1 -->
<li> Where are presented the registers configuring timee 0 (group 0)?</li>
<!-- p499 is the list, the details are in section 19.4: register 19.1 -> 19.9, some. The interrupt register (from reg 19.19, p 506) are important two -->
<li> More difficult: Where are defined the macros to access these register in IDF suite?  </li>
<!-- Plou difficile: il faut y aller a coup de grep, c'est dans  esp-idf/components/soc/esp32/include/soc/timer_group_reg.h -->
</ol>
## Run a timer simple app
go in the ``example/blink-timer`` directory of the course github, run the example and look at the code.
<ol>
<li> Identify the timer ISR </li>
<li> Identify the timer initialisation </li>
<li> change the blink rythm is some way </li>
<li> Print the address and the value of TIMG_T0CONFIG_REG(0) </li>
</ol>
## Raw Programming of timer register
Here again, the API proposed by IDF is quite complex. In embedded programming, simple is beautiful, we can program all the timer register directly.
<ol>
<li> Comment the line that starts the timer: ``timer_start(TIMER_GROUP_0, 0);``, does it work now ?
<li> Replace this command by a direct activation of the timer: setting the bit ``TIMG_T0_EN`` in register `` TIMG_T0CONFIG_REG(0)`` (See documentation page 501: register ``TIMGn_TxCONFIG_REG``).
<!--
	int config_reg=read_reg(TIMG_T0CONFIG_REG(0));
    config_reg |= 1<<31 ;  
    printf("config_reg 0x%x\n", config_reg);
    write_reg(TIMG_T0CONFIG_REG(0),config_reg);
-->