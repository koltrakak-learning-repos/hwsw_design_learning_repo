# infarinatura generale riguardante power consumption

power consumption is one of the main constraints of ES

peak power and average power

- will my chip burn?
- how much my battery will last?

we focus mostly in average power since we're working with mcus (not enough power to burn the chip)

...

batteries are energy stores

- unfortunately, l'unità di misura non è in joule ma Ampere per hour

## Where do we spend power in an MCU?

we focus on dynamic power (switching), and leakage power

dynamic power

- f, power is proportional to the frequency
- V^2, power is proportional to the square of the voltage
  - the most important variable to reduce when wanting lower power consumption (it's squared)
- C, roughly proportional to the size of the chip or logic block (bigger areas consume more power)
- A, activity (how much switching is happening on average): probability of switching
- **NB**: se il chip non sta facendo niente -> non c'è switching -> non c'è dynamic power -> **dynamic power è presente solo se il chip sta facendo qualcosa**

leakage power

- **abbiamo leakage power sempre e comunque**
- l'unico modo per non avere leakage power è staccare il circuito dall'alimentazione

slide 5

- la frequenza con cui possiamo operare dipende dalla tensione
- non possiamo abbassare la tensione senza una caduta esponenziale della frequenza

### Reducing Dynamic Power in a MCU

1) Drop operating Voltage and Frequency -> more than quadratic effect
2) Reduce switching activity -> turn off unused modules
3) Lower total capacitance -> make a smaller chip

1 and 2 are “knobs” exposed to the embedded software developer!

Looking at the graph for dynamic power we notice that

- reducing voltage has a quadratic effect on power (lo sapevamo già)
- has an exponential effect on frequency (and thus on performance)

this means that we can reduce voltage only when the exponential of the frequency doesn't kill us

Looking at the graph for static power we notice something different

when you lower voltage it's good, but the leakage power will dominate the dynamic power

- since the frequency decreases exponentially, faster of the rate at which the leakage power decreases
- at a certain point, the total energy spent ends up being higher, because **we take that much longer to execute the program while the leakage power hasn't deacreased enough**

the takeaway is that:

- lowering voltage in the region where dynamic power dominates is good
- lowering voltage in the region where static power dominates is very bad
- **there is a threshold voltage that gives us maximum efficiency**
  - however, maybe we can't achieve this maximum efficiency point because we need higher frequency

# Achieving low power in an mcu

let's look at three techniques

**Clock gating**:

let's take as an example an MCU with an adder and a multiplier unit

when we're doing an add, we don't need the multiplier, and we don't want it to consume energy

we can gate the clock of the multiplier (o meglio il clock dei flip-flop che mantengono gli operandi del multiplier) when doing an add so it doesn't do a useless multiply that wastes energy

**Frequency scaling**:

reduce the frequency of the parts of the system that don't need high speed at a particular time

**Multiple-clock domains**:

faster clocks consume more power, thus, it's a good a idea to use slower clocks wherever possible

**Voltage scaling**:

coupled with frequency scaling -> reducing the frequency means that we can reduce the voltage (obtaing the benefit of the square scaling)

- this also reduces leakage power

**Power gating**:

con clock gating abbiamo comunque la presenza di leakage power (il circuito è comunque alimentato anche se non riceve un clock)

possiamo fare power gating: spegnere sezioni del chip

**NON sempre applicabile**

- sezioni del chip che hanno uno stato (ie memorie) vengono resettate quando tolte dall'alimentazione
  - bisogna salvare lo stato e fare un reboot quando si riaccende la sezione -> overhead
- abbiamo poi un time-to-wakeup: non possiamo portare la tensione da zero a vdd istantaneamente
- waking up implies a cost in time and energy
  - need to restore the state
- power gating is useful only if i can ammortize the cost of sleeping
  - **need to sleep long enough**

# MCU application phases

we have active and inactive phases (mcu is duty cycled)

- we can have a timer that periodicly wakes us up
  - periodic and regular active phases
  - es: sample the smoke level of a room every second
- we can have an interrupt that wakes us up
  - irregular active phases
  - es: a data packet arrives

...

during inactive periods we use all the previous techniques to lower the power consumption

**however we don't use zero power**

when i go into the off state i can't immediatly wake up and do a task, i need to spend some time initializing

```
un buon esercizio d'esame e capire se mi conviene andare nell'off state o meno
```

# Interrupts

...

possiamo avere un terzo stato in qui abbiamo applicato power gating a parte del chip (deep sleep state)

- timers e clock continuano ad essere presenti come nel "shallow sleep state"

## Events

events are like interrupts but managed completely in software (non c'è dell'hardware che blocca il mio control flow e mi fa saltare verso un interrupt vector)

an event is something to react to only if you were explicitly waiting for it

- interrupts arrive and are managed at any time ; events arrive at any time, are managed only when explicitly waited

---

# practical example | STM32L1 MCU

## Clock Distribution

many clock sources

- some run very fast, some run very slow
- we also see that some clock sources have their speed configurable
- however, clocks that are able to run very fast consume more power even if configured to be slow because of their circuitry
- **this is why we need multiple clock sources, using a slow clock allows us to be low power**
- we also have some external clock sources
  - external clocks sources are needed because they produce very stable and precise frequencies (physical properties of materials needed like quarts oscillators)
  - takeaway: if we need a precise clock, we use an external one, if not we use an internal one

...

the system clock doesn't go everywhere in the chip

we have a lot of control logic (prescalers) that produce many different clocks from the system clock for different parts of the chip

- we want to feed different parts of the chip with the lowest frequency clock that we can manage for power reasons

...

we use the super slow clocks (LSE, LSI) when we are in a sleep state just to be able to wake up

- again this is for power reasons

RTC (Real Time Clock)

## Voltage domains

vabe, abbiamo parlato solo di power gating come prima

## Power Modes

we're not given directly the power

instead we're given A/Hz, because we have multiple operating frequencies and voltages

- to get the power we multiply by the operating frequency and then by the voltage

...

LPRUN/LPSLEEP modes are states in which we're not doing work so we do stuff like clock gating and use timers (with slow clocks) to wakeup

STOP/STANDBY modes are states in which we're applying power gating

### RUN MODE

everything is on and we use high speed clock

to reduce power we can

- clock gate peripherals if we're not using them
- frequency scale down if we don't need to go fast (but still using the high-speed source)
- reduce the voltage (if the power supply is configurable), thus lowering the maximum frequency, if we don't need to go fast
  - a rough approximation of the redution you get is A/Hz * V_new^2 / V_old^2
  - this is an approximation because we have leakage power that we're not considering, this approximation is optimistic (the real A/Hz would be a bit more)

### LPRUN MODE

some peripherals are clock gated

reduce the voltage

use the medium speed clock source

- clearly we go slower by using less power

### SLEEP

core is clock gated (sleep mode), thus stops executing instructions and waits for an interrupt/event

- ricorda che clock gating elimina la switching activity, e quindi dynamic power, ma non leakage power

**peripherals are kept running**; here's why

1. Autonomous Data Collection (DMA)
    - Peripherals can move data without the CPU. For example, a DMA controller can move data from an ADC (Analog-to-Digital Converter) directly into RAM while the core sleeps.
    - Why? If you need to sample a sensor 1,000 times a second, waking the CPU for every single sample is a massive waste of power.
    - You let the peripherals send interrupts to the DMA that copies the data to memory and only wake the CPU once the buffer is full.

2. Communication Buffering
    - If your device is connected to a computer via UART or SPI, data might arrive at any time. If the peripheral was off, you would miss the incoming data bits.
    - By keeping the peripheral active, it can receive the byte, store it in a register, and then send an interrupt to "wake up" the core to process the message.

3. Monitoring for "Events" (Not just Timers)
    - It isn't always about a timer waking the core. Sometimes the peripherals are monitoring the physical world and wake up the core themselves with interrupts

### STANDBY

power gating everything except wakeup sources

the time to wakeup is significant

### Conclusioni

These power modes allow us

- to trade-off performance for power if we're doing domething
- to trade-off time-to-wakeup for power if we're  not doing anything

**NB**: the more deep is the sleep state you go in the more the number of actors that can wake you up is reduced

- es: in light-sleep peripherals can wake you up; whereas in deep sleep only the timer can wake you up
