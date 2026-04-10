most applications where mcu's are employed are real time

this means that we need a precise notion of time

there's also another use of timers: mcu's are duty cycled

- if there's no work to do the mcu can enter some sort of shutdown mode

timers from the SoC perspective look like a peripheral

- the core can configure timers with mmapped writes
- inputs

the timer has also some outputs

- tipycally timers generate preciselly timed events (interrupts) to wake up the dma or the core
- it can also generate some signal (shaped with some timing)

# basic functionality

the fundamental building block of a timer is a counter

the clock can be internal to the timer, or an external input

when something relevant happens to the count (based on the configuration of the timer), an interrupt/event is generated

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

- come accennato prima, non generiamo un event solo quando il count arriva a zero

## capture register

external input can generate additional compare actions

- we store the count of when an event happens in the capture register
- the core then can look up when that event happened

## STM32

a programmable prescaler is a way to slow down the clock

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

# PWM

this is an use of a timer that produces an output

- one of the most common uses of timers

un circuito RC è un low-pass filter -> mi produce un segnale costante in output che è l'avg value del segnale digitale in input

- the input signal is digital
- the output signal is analog
  - tipically a mechanical signal (e.g. a motor that spins the wheel; the higher the duty cycle, the higher the speed of the motor)

the duty cycle is the manipulation of the regular input digital signal

without a timer we could generate a duty cycled wave with a core

- use a while loop that keeps writing 0/1 to a mmapped address
- very wasteful

we can configure a timer that generates our waveform for us, keeping the core free

- also the core can't generate a very precise waveform because it needs to handle interrupts
- the timer is way more precise
