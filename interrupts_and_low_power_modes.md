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

Reducing Dynamic Power in a MCU:

1) Drop operating Voltage and Frequency -> more than quadratic effect
2) Reduce switching activity -> turn off unused modules
3) Lower total capacitance -> make a smaller chip

1 and 2 are “knobs” exposed to the embedded software developer!

---

when you lower voltage is good (), but the leakage power will dominate the dynamic power

- since the frequency decreases exponentially, faster of the rate at which the leakage power decreases
- at a certain point, the total energy spent ends up being higher, because we take that much longer to execute the program while the leakage power hasn't deacreased enough
