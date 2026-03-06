pulpissimo is the microcontroller that we're going to work with

- it has a riscv-core that manages a bunch of peripherals (fabric controller)
- interessante notare le memory bank multiple che possono gestire richieste concorrenti da più master
  - abilita trasferimenti concorrenti da parte del DMA (copy machine)
  - we have concurrent access to the memory
- HWPE is an accelerator
  - this block also has access to the memory
  - we have three actors that access memory: CPU, the DMA and the accelerator
- the memory banks are shared memory, the three actors also have some internal private memory

---

padframe and mux is a physical block that is used to connect the physical pads (striscia di metallo logicamente simile ad un pin) of the microcontroller to the pads of the chip

- microcontroller is connected to the padframe
- the padframe is connected to the chip

- this is the installation of the microcontroller
- thanks to the padfram box we don't have to change the microcontroller when we instantiate it in a different chip, we only have to change the padframe box that implements the specific connections of the target chip

---

two major interconnects: APC and SOC

- very tipycal for microcontrollers
- APB is used for low-speed connections (peripherals that don't have high speed requirements for data production/consumption)
- SOX is used for fast connections

...

in order to get data from i/o peripherals we just access a memory address specified in the memory map (memory-mapped i/o)

- quite a convoluted path to access the gpio
- a load that goes to the memory is fast, a load that goes to the i/o peripherals is slow because of this slow path

the set of i/o peripherals we have access to is specific to the chip that the microcontroller is installed in

```
chiedi meglio che cosa è chip-specific e cosa non lo è
```

- the memory map is chip-specific, the pads may be mapped to different peripherals in different chips
- from the pov of the microcontroller it doesn't matter, it just accesses memory like it always does

An i/o peripherals may be

- a led or button -> slow peripheral
- a video display -> fast peripheral
  - we form the image in memory and we stream the data of the image to the display

---

interleaved memory

- memory that is composed of interleaved addresses from all the memory banks
- interleaving is useful because when multiple actors access concurrently neighbouring addresses, these accesses get distributed to different memory banks

we also have core-privileged memory banks

- not interleaved
- other actors may access these memory addresses but they will have to wait if the core want to access at the same time the same address (core-privileged)
  - il core è privilegiato nella logica di arbitraggio

# Interconnects

crossmatrix represents the connections

- the columns are the initiators of transactions
- the rows represent are the targets of transactions
- on the rows we also have the name of the bus that carries the data of these transactions

```
Slide 29 importante
```

an initiator starts a request and waits for an answer, a target can only wait for a request and respond

a bus is not only a bunch of wires

- it also has a lot of logic to route the data to the correct target and arbitration logic to deal with congestion coming from multiple initiators

there are 3 functions that the bus does

- the connection
- the rounting
- the control (arbitraggio)

## Interconnect Protocols

In practice there are many initiators and many targets: to ensure that the communication is valid (e.g., no conflicts with other communications), we use communication protocols

**How is a protocol implemented?**

- we divide the bus wires into groups
  - data wires
    - addresses
    - write data
    - read data
    - parallel wires (eg. 32)
  - control wires
    - ...
    - single wires
  - **NB: on the chip we don't have bidirectional wires!** the wires only go from the initiator to the target or viceversa
  - We group control and data wires into request channels (from initiator to target) and response channels (from target to initiator)
    - at minimum we have 2 channels (one for requests and one for responses) but we may have more (for example for different kinds of traffic: fast, slow)

- the control wires perform a **handshake**: a rule to distinguish valid transactions (syncs the initiator and the target)

How do we measure the performance of our interconnect?

- the two main metrics are latency and bandwidth
- there is a tradeoff between the two
  - things that increase the bandwidth also tend to increase the latency

formula utile (**che useremo negli esercizi**) in slide 40 che usa un modello utile (nel caso medio)

- assumiamo che i nostri byte vengono emessi sul bus tutti insieme in un unico grande blocco sequenziale (largo B)
- il tempo (latenza) per tutto il blocco è pari a: il tempo per il primo blocco/word + (N-1)/B
  - dopo che arriva il primo blocco, arrivano il resto dei byte a blocchi grandi B

# Standard Buses

## AMBA

usa due protocolli diversi per i due tipi diversi di bus (high/low speed)

- AHB
  - faster (high bandwidth and frequency), more complex and thus more power consuming
- APB
  - the opposite

### APB

synchronous and not pipelined

every transfer takes (at least) two cycles

- also, every transfer has to fully complete before beginning the next one (not pipelined)
- only 50% of the wires are used at a given time

```
Guarda le slide per capire il protocollo
```

- 2 phases

**NB: in APB the assumption is that there is a single initiator**. This is why the bus is very simple, we can have single shared bus (there is no concurrency)

in a bus there is only one communication that can happen at any given time

- this is slow if i have many initiator, bacause i have to serialize the communications

Crossbar:

- colleghiamo ogni initiator con ogni target
  - ogni initiator ha i suoi fili e quindi abbiamo piena concorrenza
- abbiamo un sacco di fili (scala con I*T)
  - ricorda che i singoli "fili logici" nei grafici hanno anche parallelismo alto
- we can afford a crossbar only when the number of initators is low

We have to find a middle ground between a bus (full serialization) and a crossbar (no serialization but way too many wires)

Usually the middle ground is achieved by going through some hops (giusto per avere un'idea non c'è da impararlo bene)

- riduciamo i fili ma paghiamo con un pochino di latenza aggiuntiva

## Outstanding transactions and pipelined buses

you start a read/write (could be a different initiator) while the previous one has yet to finish

...

the APB bus doesn't do this

we can even use out-of-order multiple outstanding transactions where a next responde could come before a previous slow response from a slow target

- we avoid blocking faster responses because of slower targets
