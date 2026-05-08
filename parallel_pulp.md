even in ES we use multiple cores and accelerators since it's more energy efficient

- we want to be near threshold and to use multiple cores to recover the lost frequency

# Architecture

firstly, we need multiple cores

- but we don't want very general purpose cores if we want to be very energy efficient
- no OOO, no superscalar, no speculation, ...
- we want simple cores

TCDM is a scratchpad memory

- memory mapped in the address space of the host fabric controller
- implemented with sram
- used directly by the cores (L1) but it's not a cache since it's not automatic
  - you need to transfer memory into it
- the cores share this memory, and thus can communicate with it
- we use banking and a crossbar (not really) interconnect to make all the cores access memory indipendently and concurrently
  - this is important because, since the cores are simple, they cannot hide the latencies of memory access (we need them to be a single cycle long)
  - remember that banking means interleaving words of memory across the memory banks (word goes to bank 1, second word to ...)
    - at the start all cores are probably going to access the same bank
    - this causes different access latencies for different cores
    - after this disalignment, different cores are going to be acessing different banks concurrently (until the next barrier)

DMA inside the cluster

- used to move data between the TCDM of the cluster and the outside world

Hardware synchronizer

- we don't want to spend more time during syncronization than during the parallel work
- we use a special kind of load that goes to the hw synchronizer (instead of the interconnect)
  - if the core doesn't immediatly receive from the synchronizer a signal, the core gets clock-gated (to reduce energy)
  - this block has a mask with one bit for each core
  - only when all cores emitted the load (and the mask gets full) the synchronizer emits the wakeup signal to the cores to make them proceed

Instruction Caches for the cores

- reads from L2 external memory
- not a scratchpad because i$ are read only, thus we don't have the complexity of coherency
- some cache space is shared between cores since it's likely that they're going to be using the same region of code

FPU units were skipped

- these are shared between cores aswell since they are expensive

Esempio di DMA transfer

- the goal with multiple cores is using them all of the time
- we want to hide data transfer, synchs, etc with techniques like double buffering

# Execution model

initially, only the host core is active and the cluster is clock-gated

then, the host thread activates the cluster through an api and instructs core0 to execute a function

(All code is in L2 memory (cached for cluster threads))

(Stacks are in L2 for the FC, and in L1 TCDM for the cluster cores (no caches!))

---

takehome interessante:

- stranamente, codice non ottimizzato produce uno speedup minore quando parallelizzato
- per capire quanto la parallelizzazione è risultata efficace bisogna usare un codice di partenza migliore possibile (O3)
  - non vogliamo avere a che fare con più effetti ortogonali alla volta
