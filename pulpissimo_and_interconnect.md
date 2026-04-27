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

Interconnects are important because all communication between the processor and mamory/peripherals happens through the interconnect

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
    - Read Enable, Write Enable
    - valid, ready
    - single wires

The control wires perform a **handshake**: a rule to distinguish valid transactions (syncs the initiator and the target)

- Interconnect operation is divided in phases of activity (es: request and response phases of a read/write transaction)
  - states of a Finite State Machine governing the interconnect traffic

**NB: on the chip we don't have bidirectional wires!** the wires only go from the initiator to the target or viceversa

**NB**: We also group control and data wires into **request channels** (from initiator to target) and **response channels** (from target to initiator)

- at minimum we have 2 channels (one for requests and one for responses)
- but we may have more (for example for different kinds of traffic: fast, slow)

## How do we measure the performance of our interconnect?

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

```
Usually the middle ground is achieved by going through some hops (giusto per avere un'idea non c'è da impararlo bene)

- riduciamo i fili ma paghiamo con un pochino di latenza aggiuntiva
```

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

### Burst transfers

burst transfers deal with requests that need a consecutive bunch of data

- a burst is an advance type of transaction that is composed of many reads and writes
- **with a burst you can minimize the usage of the request channel**

```
I'm guessin APB also doesn't do this
```

ogni blocco di dati di risposta in un burst viene chiamato beat

without burst transfers, we can only utilize half of the bandwidth available from a bus channel (if we don't pipeline)

- because we have to wait for the other channel for every message
- **burst transfers allow us to have a much better utilization of the bandwidth of the response channel**

**NB**: we can also apply pipelining with burst transfers

- this allows us to use the freed bandwidth of the request channel by sending request while the response data is still coming (multiple burst-based outstanding transactions)
- like a mail system, we don't have to wait for an answer before sending another email (asynchronous)

## AXI

multiple burst-based outstanding transactions (which can complete out-of-order)

5 channels / phases

- we have separated the request and response channels for reads and writes
- sicuramente abbiamo bisogno di un canale per indirizzo e dati
  - ma il canale non racchiudeve proprio questi fili? Si, ma adesso abbiamo un control flow più complicato dato che siamo in un network interconnect
  - quando un initiator emette un indirizzo, la rete risponde dicendo se può gestire la richiesta o meno
- in generale, **valid/ready protocol** per ogni canale
  - initiator raises valid
  - network raises ready when it's ready
- the reads need 2 channels
  - address read (request)
  - data read/response read (response)
- the writes need 3 channels: address write, data write, but i also need an acknowledgement
  - now i can send many writes without waiting for an answer
  - prima o poi il master deve sapere se i suoi messaggi sono arrivati

Infine, Per-channel IDs (dei fili in più per canale) to differentiate multiple, ooo, outstanding transactions

- ad esempio id per data-read (da chi vengono questi dati?), id per response write (chi è che ha ricevuto i miei dati?)
- i bit di id sono limitati e quindi le multiple outstanding transactions sono altrettanto limitate

### Esempi AXI

si parte sempre alzando valid dell'address, data always comes later

---

# Other actors on the interconnect

each core in a multicore system can be an initiator

in an MCU we have only a single core, nevertheless we still have multiple actors

DMA

- **the core configures the DMA to transfer data between memory and peripherals, and/or memory and memory without disturbing the core**
- processor is good at computation, data transfers are a distraction

the copies happen concurrently with computation

- if the DMA is balanced the processor can keep computing all the time

## Double buffering

we need two buffers

- one for the DMA to copy data into
- the other for the processor to do computation

it's important that they are not overlapped, otherwise the dma would overwrite the data the processor is working on

we have many sync points:

- after a transfer is finished by the DMA, this is a synchronization point
- another sinchronization point happens when the dma is finished, there are no other free buffers, and the processor is still computing
  - the cpu will tell the dma to wait
- lastly, when the processor finishes computing we have another synchronization point where the cpu tells the DMA to start copying again

## Circular buffers

double buffering is the least memory hungry technique, we could use even more buffers appoggiandoci ad un circular queue

noi siamo felici quando la cpu non deve mai aspettare il dma, in altre parole quando la cpu continua a computare sempre e comunque

quando il write pointer e il read pointer puntano a buffer diversi siamo a posto e nessuno deve aspettare

usare più buffer e una circular queue ci da un po' più di gioco quando i tempi di trasferimento e computazione non sono costanti

## DMAs are not autonomous

we want to configure the DMA as less frequently as possible (we don't want the cpu to spend it's time programming the dma)

this is why advanced dmas have more complex semantics that reduce the number of configuration needed

**Advanced DMAs can also handle multiple peripherals by using multiple streams**

- Independence: Streams can work simultaneously.
  - While Stream 0 is moving data from the SPI to RAM, Stream 4 could be moving data from RAM to the UART.
- The Arbiter: Since all streams share the same physical bus (es AXB), the Arbiter decides which stream gets to use the bus if two streams want to move data at the exact same microsecond.
- es:
  - stream 1 is the dma copying raw camera data to memory
  - the processor does some image processing
  - stream2 is the dma copying the processed image to a wireless peripheral so you can see the processed image in your phone

This means that the DMA can have multiple initiator / target ports

- initiator ports are used by the dma to do the copying
- target ports are used by the processor to configure the dma

dma data streams sono gestiti con un valid/ready handshake classico

...

fundamentally there are 3 ways to configure a dma

- mmapped registers
- dedicated ISA instructions
- descriptor based interface: prepare a pile of work and put it in memory, the dma will read the descriptors and do its work until it finishes. When that happens the dma will ask for another pile of work

most used ones are the 1st and the last

skip slide 96

esercizio lo facciamo dopo
