vector processing is a generalization of simd processing

- operands of instructions are vectors
- **number of elements of the vectors is configurable at run-time**
  - we have a configuration instruction that specifies the VL and the type of each element
  - this can be thought as configurable SIMD -> we don't have different instructions for different number of elements

Vector Processing Advantages:

- No dependencies within a vector
  - Can have very deep pipelines, no dependencies!
- Each instruction generates a lot of work
  - Reduces instruction fetch bandwidth requirements
- Highly regular memory access pattern
- No need to explicitly code loops
  - Fewer branches in the instruction sequence

Disadvantages:

- only works if we have data-parallelism
  - however, ML, DSP, ... and many other areas do have data-parallelism, and so vector processors are a good fit there
  - not everywhere though

# Vector processor concepts

- wide registers
  - notably, non è necessario specificare esattamente il numero di bit
  - ci interessa solamente che siano vettori
  - implementazioni diverse possono avere dimensioni diverse
- functional units are also wide (not necessarily as wide as the registers)
  - parallel compute
- the bus to memory is also wider since we need to fetch more data

un vector processor continua ad avere i registri scalari

- un vector processor è pensabile come un processor normale ma con wide registers and FUs

## vector programming model

VLMAX = max number of elements
VL = number of elements actually used

- stored in VL register a CSR configurable by the programmer
- VL <= VLMAX

**NB**: specificando un VL minore, il numero logico di registri vettoriali può incrementare

- se uso VL = VLMAX/2 ho il doppio dei registri vettoriali dato che la seconda metà di ogni registro vettoriale può essere usata come registro vettoriale separato

`A vector processor allows you to decide how wide your vector registers are and how many vector registers you have`

Vector memory operations are also interesting

- the simplest case accesses contiguous chunks of memory
- we can also specify a stride and an offset
- (and also more complex stuff that we don't cover)

what if we have data-dependent conditional execution?

- we use masking
  - with dedicated instructions that create the masks
  - and variations of regular instructions that consider the mask
- we may have a dedicated mask register (1 bit per vector element)

...

### Vector stripmining

What if # data elements > # elements in a vector register?

Idea: Break loops so that each iteration operates on # elements in a vector register (VL)

il programmatore non conosce VLMAX (dipende dalla specifica implementazione hardware)

- non vogliamo fare una ricerca e ricompilare il nostro programma per ogni vector processor diverso

vector processor permettono di specificare un AVL = numero di elementi del vettore nella mia applicazione

- l'architettura può essere interrogata per il valore del VL da usare
- proviamo a settare VL a AVL e sarà il vector processor stesso a settare e restituire il VL che può gestire

vector code is agnostic to the vector length ONLY IF YOUR CODE SUPPORTS STRIPMINING (possiamo interrogare l'architettura provando a settare vl a avl)!

- Proper vector code can work on different architectures with different VRF sizes
- **Vector code is portable**

## vector execution model

le unità funzionali sono fortemente pipelined in maniera tale da ignorare operazioni ad alta latenza come divisioni floating point

- non ci sono problemi di alee dato che stiamo processando vettori (embarassingly parallel)

non c'è bisogno di avere lo stesso numero di unità funzionali rispetto al numero di elementi nei vettori

- es: se VL=32 è il parallelismo nel mio datapath=4 (ho 4 FU) allora dovrò fare semplicemente 8 iterazioni per completare la singola operazione sul vettore
- con parametri diversi tutto continua a funzionare

vector chaining:

- analogia con bypassing del caso scalare: non aspetto la scrittura nel rf del dato, lo uso appena finita la computazione
- Without chaining, we must wait for last element of result vector to be written before starting dependent instruction
- With chaining, we can start dependent instruction as soon as the elemement that we need gets computed
  - remember that elements of a result vector get computed in "waves" (num_lanes alla volta)

NB: vector chaining allows easier overlapping execution of multiple vector instructions

- in altre parole incrementiamo **vector ILP**
- di nuovo, analogia con pipelined forwarding nel caso scalare
