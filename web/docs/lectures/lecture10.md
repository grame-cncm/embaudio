# Lecture 10: Embedded OS, freeRTOS

This course will present the important notions of embedded operating systems and  explain in more details the FreeRtos operating system used on ESP32.
## Slides

It is (temporarily) available through sildes [here](lecture10/img/cours2-os.pdf)

##Exercices
### Creating tasks
Create an IDF project that  creates two FreeTtos Tasks: one is generating number each second, send via a FIFO of length 10 these numbers to the other task which is  printing them on uart. The two task will have the same priority: 10.

One can use the function ``esp_random`` for generating random numbers, ``xTaskCreate`` to create task and ``xQueueCreate``, ``xQueueSend``, ``xQueueReceive`` to communicate between the tasks.
### Experimenting priority and starving
Try to saturate the FIFO (i.e. send faster than receiving by using ``vTaskDelay``. Change the priority of the receiver task, set the priority to 9, does it starve the sender? Wath happens it the receiver does not receive anymore but executes a stupid loop such as this on:
```
for(;;) {
    cpt++;
    }
```