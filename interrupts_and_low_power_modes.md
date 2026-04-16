# infarinatura generale riguardante power consumption

power consumption is one of the main constraints of ES

peak power and average power

- will my chip burn?
- how much my battery will last?

we focus mostly in average power since we're working with mcus

...

batteries are energy stores

- unfortunately, l'unità di misura non è i joule ma Amper per hour

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

- abbiamo leakage power sempre e comunque (l'unico modo per non avere leakage power è staccare il circuito dall'alimentazione)

slide 5

la frequenza con cui possiamo operare dipende dalla tensione (linea rossa vs blu)

- non possiamo abbassare la tensione senza una caduta esponenziale della frequenza

## Reducing Dynamic Power in a MCU

1) Drop operating Voltage and Frequency -> more than quadratic effect
2) Reduce switching activity -> turn off unused modules
3) Lower total capacitance -> make a smaller chip

1 and 2 are “knobs” exposed to the embedded software developer!

---

looking at the graph for dynamic power we notice that

- reducing voltage has a quadratic effect on power (lo sapevamo già)
- has an exponential effect on frequency (and thus on performance)

this means that we can reduce voltage only when the exponential of the frequency doesn't kill us

looking at the graph for static power we notice something different

when you lower voltage it's good, but the leakage power will dominate the dynamic power

- since the frequency decreases exponentially, faster of the rate at which the leakage power decreases
- at a certain point, the total energy spent ends up being higher, because **we take that much longer to execute the program while the leakage power hasn't deacreased enough**

the takeaway is that:

- lowering voltage in the region where dynamic power dominates is good
- lowering voltage in the region where static power dominates is very bad
- **there is a threshold voltage that gives us maximum efficiency**
  - however, maybe we can't achieve this maximum efficiency point because we need higher frequency

## achieving low power in an mcu

let's look at three techniques

**Clock gating**

let's take as an example an MCU with an adder and a multiplier unit

when we're doing an add, we don't need the multiplier, and we don't want it to consume energy

we can gate the clock of the multiplier (o meglio il clock dei flip-flop che mantengono gli operandi del multiplier) when doing an add so it doesn't do a useless multiply that wastes energy

**Frequency scaling**

**Multiple-clock domains**

## Power Gating

con clock gating abbiamo comunque la presenza di leakage power (il circuito è comunque alimentato anche se non riceve un clock)

possiamo fare **power gating**: spegnere sezioni del chip

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
