...

NB:

- with the parallel pragma we're just declaring an amount of parallelism to the compiler
- the actual number that are going to be executing in parallel depends on the hardware capability of the hardware
- OMP astrae il parallelismo della macchina e permette di scrivere parallel programs
  - se poi la macchina supporta un grado di parallelismo minore rispetto a quello dichiarato, il runtime sequenzializzerà i thread

...

It's the compiler's job to support OMP

- traduce le direttive in chiamate a threading libraries come pthread

Direttive, funzioni di libreria (omp.h) ed env variables formano l'api di OMP

# Direttive importanti

## omp parallel

leggi le slide che sono molto chiare ...

### data-scopes

defaults:

- variables outside the parallel region are shared by default
- variables inside the parallel region are private by default

a private variable is implemented by the runtime system by reserving private regions (one for each thread) in the shared memory, where some space gets allocated for each copy of the private variable

- the private regions can be logically private as well as actually private depending on the sofistication of the runtime implementation

private variables are used to differentiate the behaviour of each thread in the parallel region thanks to the different data that they operate on

firstprivate

- basically private but with initialization value defined by the outside scope

### overheads

importante considerare gli overhead delle parallel regions (fork instructions, join instructions)

`negli esercizi viene spesso chiesto se lo speedup che si guadagna con la parallelizzazione è maggiore dello slowdown causato dalla creazione dei thread e dalla barriera`

## work-sharing-constructs
