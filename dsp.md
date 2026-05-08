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

... (big O notation) ...

# Algorithm precision

**Floating point ...**

it's expensive to support

- need many internal bits inside the functional units to align the exponents of operands
- not always possible (or needed) to implement in embedded devices

**Fixed point**

this representation implicitly defines the number of fractionary bits q, hence a fixed point number is represented as $r = i * 2^{-q}$

in dsp processing we don't need to change dynamically the range of numbers to represent

- hence fixed point is appropriate in this domain and we can leverage its cheapness

## reducing the numerical precision of our data

good idea because it:

- reduces the memory footprint of the program
- reduces the energy consumption of the program

NB: need to evaluate with a metric how much our quantization is affecting the precision of the results of our algorithm (QSNR)

**Recipe for effectively reducing bit precision (picking the right number of bits)**:

1. Define the **quantization range** (intervallo dei valori $[min, max]$ da rappresentare) of all the variables of the f^ function
    - for static coefficients (like filter weights) its easy (the range is known)
    - for dynamic variables (input signal) it's not
        - you may consider running simulations to get some statistical info like: max, min, mean and variance
    - Quantization range may be smaller than the value range of the coefficients
        - your coefficients might technically span from $-1.0$ to $1.0$, but maybe $99\%$ of them are between $-0.1$ and $0.1$
        - you might choose a smaller Quantization Range to get more "resolution" (granularity) for those common small numbers, and just apply something like saturation for the outliars

2. Define the fixed-point word-length and fractionary bits that fits the quantization range

3. Check the evaluation metric(s). Relaxing the above constraints 2) and 3) if the requirements on the evaluation metric are not matched

# Roofline model

...

## double buffering

...

# Architectures for DSP

all p.x instructions are custom pulp instructions

...

## RISC-V Custom instructions and speeding up computation

very regular pointer increments

very regular branches

hw loops and post increment loops are cheap modifications to hardware

...

notice that we're using 'quantized' data types (not full words) but we're not exploiting them to our full advantage

- we are getting smaller memory footprint
- but we're not computing faster
  - when we load we're sign-extending the data into the registers
  - we're using half the register length to do our computation
  - instead we can load and compute two data elements at a time

attenzione però:

- doing this all in software isn't a very smart idea
  - loading 2 data elements at a time works
  - but when computing we would need to reshuffle the data inside the registers and the cost would be more than the gain
- we can use a dedicated instruction and dedicated hardware to compute the macs inside a single register coherently without reshuffling (actually it also does the dot-product reduction)

this is loop stripmining (vectorize loops and compute the residual iterations scalarly)

...

unrolling

- needs a lot of registers that i might not have
