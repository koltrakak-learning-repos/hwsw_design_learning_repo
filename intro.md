# class organization

helper functions

- peripherals
- dma
- timers

acceleratin for microcontrollers

- DSP
- neural networks

modulo 2

- parallel programming on multicore microcontrollers

---

buona occasione per ripassare digital design

- prova a guardare un po' dall'insegnamento richiesto
- anche il libro di hennessy and patterson

---

calcolatrice da portare all'esame

puoi portare 10 pagine di appunti,

- non c'è bisogno di ricordarsi a memoria roba, epico

mi raccomando, non fare dei pocci quando risolvi gli esercizi

# intro topics

embedded system is a system that does information processing (computation) in a loop with the physical world

communication (compression, encoding/decoding, ...) and AI are pushing more and more capable embedded systems

main components are

- processors
- accelerators
- memory
- peripherals
- and the interconnect

we will study all of them

interessante distinguere tra

- ES che interagiscono con umani
- ES che interagiscono con l'ambiente (obstacle avoidance)
- ES che interagiscono con altre macchine

ES can be very different depending on the requirements the system they are embedded in have

- un wearable è molto diverso da una centralina

ES have many constraints

- size and weight
- power is very important because of batteries and thermal constraints
  - non abbiamo spazio per del cooling
  - anche se non abbiamo una batteria, consumare troppa potenza scalda troppo e rompe il sistema

Reliability

- temperature
- shock
- vibration

Safety (different from reliability)

- if there's something wrong, the system enters a state that will not damage people
  - a car breaks automatically if there's a fault
- reliability means that the systems keeps working even in harsh conditions

## We could have dedicated chips for embedded systems

this way we could have the smallest and most power efficient implementation

- but actually, most implementation typically use a programmable processor even with strict requirements because we need the flexibility to change our algorithms, ...
- very few applications don't require flexibility
- we still need efficiency though, this is achieved by specializing the processor with specific instruction that accelerate the workload

furthermore, ASIC cost a lot

## SoC

it's a chip that contains a lot of functionality (flexibility) and are very cheap

- maybe we don't need all the functionality
  - we choose the proper subfunctions for our goals
- but they are so cheap and efficient in their integration that we don't care

When implementing an ES we usually go for a SoC

### What is inside a SoC?

the boss is a programmable processor that manages many auxiliary functions

auxiliary functions

timer

- emits a periodic event that gives the system a notion of time

watchdog

- sort of like a timer, but check on the processor
- triggers an event to the processor and waits for the processor response
- this is like an heartbeat to check if something is wrong
- if something is wrong it probably will shut down the system -> safety

buses have different requirements in terms of performance

- we use different buses for different data speeds
  - highway vs local road
- we use a bridge when we need communication between high performance peripherals and low performance peripherals
