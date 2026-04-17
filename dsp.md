processing on the edge device is useful because you save the power you need to transmit the data

- es compression: a smart camera can transmit that a person in at your door instead of trasmitting a (large) raw image

processed data is much smaller in size than raw data

...

design goal:

- the time it takes to do the processing needs to be smaller than the period
- while minimizing energy consumption

**Typical DSP algorithms**

we always end up doing MACs

- non linear functions are expanded into a series that gives us a polynomial
- with integrals we use the trapezoid method
- ...

following our design goal, we want to minimize the time to do the computation; **this is our focus**

- by doing this i can maximize the time i can go to sleep
- and/or run slower (reducing voltage, frequency, etc.)

# How do you optimize DSP algorithms for embedded?

how do i know if my initial implementation is good or bad? I need a methodology
