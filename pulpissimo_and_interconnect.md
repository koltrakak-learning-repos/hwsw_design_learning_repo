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

two major interconnects: APB and AHB

- very tipycal for microcontrollers
- APB is used for low-speed connections (peripherals that don't have high speed requirements for data production/consumption)
- AHB is used for fast connections

...

**OSS**: in order to get data from i/o peripherals we just access a memory address specified in the memory map (memory-mapped i/o)

- quite a convoluted path to access the gpio
- a load that goes to the memory is fast, a load that goes to the i/o peripherals is slow because of this slow path

the set of i/o peripherals we have access to is specific to the chip that the microcontroller is installed in

```
chiedi meglio che cosa è chip-specific e cosa non lo è (oppure lascia stare che non è così importante)
```

- the memory map is chip-specific, the pads may be mapped to different peripherals in different chips
- from the pov of the microcontroller it doesn't matter, it just accesses memory like it always does

An i/o peripherals may be:

- a led or button -> slow peripheral
- a video display -> fast peripheral
  - we form the image in memory and we stream the data of the image to the display

---

Interleaved memory

- memory that is composed of interleaved addresses from all the memory banks
- interleaving is useful because when multiple actors access concurrently neighbouring addresses, these accesses get distributed to different memory banks

we also have core-privileged memory banks

- not interleaved
- other actors may access these memory addresses but they will have to wait if the core want to access at the same time the same address (core-privileged)
  - il core è privilegiato nella logica di arbitraggio

# Interconnects

Crossmatrix represents the connections:

- the **columns are the initiators** of transactions
- the **rows are the targets** of transactions
- on the rows we also have the name of the bus that carries the data of these transactions

```
Slide 29 importante
```

an initiator starts a request and waits for an answer, a target can only wait for a request and respond

**NB**: A bus is not only a bunch of wires

- it also has a lot of logic to route the data to the correct target and arbitration logic to deal with congestion coming from multiple initiators
- there are 3 functions that the bus does
  - the connection
  - the routing
    - defines the memory map accessible by the core (masters)
  - the control (arbitraggio)

## Interconnect Protocols

In practice there are many initiators and many targets: to ensure that the communication is valid (e.g., no conflicts with other communications), we use communication protocols

**How is a protocol implemented?**

- we distinguish the bus wires into:
  - data wires
    - addresses
    - write data
    - read data
    - tipically they are parallel wires (eg. 32)
  - control wires
    - Read Enable, Write Enable, ...
    - single wires
  - **NB: on the chip we don't have bidirectional wires!** the wires only go from the initiator to the target or viceversa
  - We also group control and data wires into **request channels** (from initiator to target) and **response channels** (from target to initiator)
    - at minimum we have 2 channels (one for requests and one for responses)
    - but we may have more (for example for different kinds of traffic: fast, slow)

- the control wires perform a **handshake**: a rule to distinguish valid transactions (syncs the initiator and the target)

**How do we measure the performance of our interconnect?**

- the two main metrics are latency and bandwidth
- there is a tradeoff between the two: **things that increase the bandwidth also tend to increase the latency**
  - eg: multiple outstanding transactions, out-of-order access, ... -> complicano la logica del bus e quindi aumentano la latenza, il beneficio è la bandwidth

Formula utile (**che useremo negli esercizi**) in slide 40 che usa un modello utile (nel caso medio)

- assumiamo che i nostri byte vengono emessi sul bus tutti insieme in un unico grande payload
- il payload è diviso in vari blocchi larghi quanto permette la largezza di banda B
- il tempo (latenza) per tutto il payload è pari a:
  - il tempo per il primo blocco/word + (N-1)/B
  - dopo che arriva il primo blocco, arrivano il resto dei byte a blocchi grandi B ogni unità di tempo

For complex cases, we can still use this simplified model if we consider average latency and average bandwidth

---

# Standard Buses

## AMBA

lo standard che definisce come funzionano varie tipologie di bus

usa due protocolli diversi per i due tipi diversi di bus (high/low speed)

- AHB
  - faster (high bandwidth and frequency), more complex and thus more power consuming
  - una sua evoluzione è AXI
- APB
  - the opposite

### APB

synchronous and not pipelined

every transfer takes (at least) two cycles

- also, every transfer has to fully complete before beginning the next one (not pipelined)
- only 50% of the wires are used at a given time

2 phases: SETUP and ACCESS

Guarda le slide per capire il protocollo

- PENABLE
  - segnale del Master
  - il segnale PENABLE indica il passaggio dalla fase di Setup alla fase di Access.
  - cosa dice: "Ho già impostato l'indirizzo (PADDR) e i dati (PWDATA), ora i segnali della transazione sono attivi e validi."
  - tempismo: Viene alzato esattamente un ciclo di clock dopo PSEL. Rimane alto per tutta la durata della fase di accesso.
- PREADY
  - segnale dello Slave
  - il segnale PREADY serve allo Slave per prendersi il tempo necessario.
  - cosa dice: "Ho finito di elaborare la tua richiesta. Puoi chiudere la transazione."
  - tempismo: Se la periferica è velocissima, PREADY può essere alto fin dall'inizio. Se la periferica è lenta, tiene PREADY a 0 finché non è pronta.

**NB: in APB the assumption is that there is a single initiator**.

- This is why the bus is very simple, we can have single shared bus (there is no concurrency)

## Bus vs interconnects

In a bus there is only one communication that can happen at any given time

- this is slow if i have many initiators bacause i have to serialize the communications

Crossbar (exmple of more advanced interconnect):

- colleghiamo ogni initiator con ogni target
  - ogni initiator ha i suoi fili e quindi abbiamo piena concorrenza
- abbiamo un sacco di fili (scala con I*T)
  - ricorda che i singoli "fili logici" nei grafici hanno anche parallelismo alto
- we can afford a crossbar only when the number of initators is low

We have to find a middle ground between a bus (full serialization) and a crossbar (no serialization but way too many wires)

Usually the middle ground is achieved by going through some hops (giusto per avere un'idea non c'è da impararlo bene)

- riduciamo i fili ma paghiamo con un pochino di latenza aggiuntiva

### Outstanding transactions and pipelined buses

Outstanding transactions make it possible to perform new LD/ST operations while older ones are still pending.

- can perform a new operation only if there's a free independent channel
- APB bus doesn't do this

We can even use out-of-order multiple outstanding transactions where a next responde could come before a previous slow response from a slow target

- we avoid blocking faster responses because of slower targets

clearly we need to consider the complexity and latency cost of these techniques and not only the performance (bandwidth) gain

---

routers route based on the address

...

---

### Burst transfers

burst transfers deal with requests that need a consecutive bunch of data

- a burst is an advance type of transaction that is composed of many reads and writes
- **with a burst you can minimize the usage of the request channel**

```
I'm guessin APB also doesn't do this
```
