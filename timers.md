Most applications where mcu's are employed are real time

- the processing of data has strict timing requirements
- we must not process data too late (e.g. ABS)

This means that we need a precise notion of time

**Timers keep time in MCU systems**

- they may be driven from an internal or external clock.
- they can generate interrupts

There's also another use of timers: **mcu's are duty cycled**

- if there's no work to do the mcu can enter some sort of shutdown mode
- the timer can periodically wake up the MCU when there is work to do

``` Duty cycling
Duty cycling is a power-saving technique where a device alternates between an active (on) state and a sleep (off) state, rather than running continuously.
The duty cycle is the fraction of time the device spends active:

Duty Cycle = Active Time / Total Period

For example, if a sensor wakes up for 10ms every 1 second, its duty cycle is 1% — meaning it's asleep 99% of the time, consuming very little power.
Why timing matters: The device needs a precise clock or timer to know when to wake up, how long to stay active, and when to go back to sleep. Without accurate timing, the device might miss its communication window or stay awake too long, wasting the power savings entirely.

Common examples:

- IoT sensors that wake up periodically to take a reading and transmit data
- Microcontrollers that sleep between tasks and wake on a timer interrupt

The tradeoff is latency vs. power: a lower duty cycle saves more energy but means the device responds less frequently.
```

**Timers from the SoC perspective look like a peripheral**

- it has some inputs
  - external clock
  - the core can configure the timer with mmapped writes

the timer has also some outputs

- tipycally timers generate preciselly timed events (interrupts) to wake up the dma or the core
- it can also generate some signal (shaped with some timing)

## Basic counting functionality

the fundamental building block of a timer is a **counter**

the clock can be internal to the timer, or an external input

when something relevant happens to the count based on the configuration of the timer (e.g. reaches zero), an interrupt/event is generated

...

la configurazione ci permette, ad esempio, di regolare il periodo

- basta resettare il count a 1, 2, ... invece che a zero
- la risoluzione ...

...

high resolution needs a fast clock, but this has some tradeoffs

- more power
- bigger counts (that need more logic to be handled)

## Compare register

generalisation of the behaviour of the timer

- il timer può generare un evento tramite confronto del count con un valore configurato nel compar register
- come accennato prima, non generiamo un event solo quando il count arriva a zero

Ad esempio possiamo configurare il timer scrivendo un valore nel compare register e generare un duty-cycle confrontando il count con quel valore

- il reset del counter causerà la periodicità del duty-cycle

## Capture register

External input (sensor, software trigger, ...) can generate additional compare actions

For example

- Capture time (i.e. measure time == count value) when Capture Input signal occurs
- when capture is triggered, count value is placed in the capture register and an interrupt is generated
- the core then can look up when that event happened

# STM32

...

A programmable prescaler is a way to slow down the clock

- if you have a fast clock, in order to measure a long time (1s) you need a huge count
- this means that you need many count bits
- the prescaler can divide the input clock by 2/4/..., this allows to measure long periods

...

having an internal clock in addition to the external clock is useful because of the different constraints that the two clocks have

- we already have the external clock at our disposal always (it needs to drive the mcu), this is typically fast
  - uses a lot of power and can't measure long periods
- the internal clock is typically slower
  - this clock is very useful when entering a low-power mode
  - we want to shut-down all of the mcu (also the power-hungry external clock)
  - the internal clock is used to exit the low-power mode

# PWM (Pulse Width Modulation)

Pulse Width Modulation (PWM) is a simple method of **using a digital waveform to control an analog variable**

- this is a use of a timer that produces an output
- one of the most common uses of timers

un circuito RC è un low-pass filter -> mi produce un segnale costante in output che è l'avg value del segnale digitale in input

- the input signal is digital
- the output signal is analog
  - tipically a mechanical signal (e.g. a motor that spins the wheel; the higher the duty cycle, the higher the speed of the motor)

we say that the input digital signal is duty-cycled because of how we control for how much time it stays high or low

We could also generate a duty cycled wave without a timer using the core

- use a while loop that keeps writing 0/1 to a mmapped address
- **very wasteful**

We can configure a timer that generates our waveform for us, keeping the core free

- also the core can't generate a very precise waveform because it needs to handle interrupts
- the timer is way more precise
