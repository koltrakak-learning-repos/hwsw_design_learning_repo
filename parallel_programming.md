interessante il fatto che, in sistemi reali (eterogenei), modelli a memoria condivisa e a scambio di messaggi vengono mischiati

- shared-memory -> comunicazione facile, sincronizzazione difficile
- message-passing -> il contrario

...

interessante anche considerare il fatto che ILP è un performance boost gratuito (dal punto di vista del programmatore)

- è l'hardware che si occupa di riconoscere istruzioni indipendenti e di eseguirle in parallelo (superscalar) o di non aspettare il risultato di un'istruzione ad alta latenza (OOO)

...

The PULP DSP extension includes a packed-SIMD instruction set

- still SIMD but a bit different
- ad esempio il register file è quello normale in cui i registri sono da 32 bit

...

divergence makes me execute twice (or more depending on the number of conditions)

...

## Modelli di programmazione parallela

pthread come esempio di threading library:

- con pthread, il modo per esprimere parallelismo è fare **outlining**:
  - devo racchiudere il mio workload in una funzione in modo da poter lanciare un nuovo thread che esegue la funzione
- we won't use pthread since it's verbose

OpenMP directives

# Bare-metal parallelism in PULP

On MCUs like pulp we can't use a threading library like pthread because it's too expensive (non chiediamoci il perchè)

we have a parallel cluster that must be activated

- we use a standard open/use/close idiom
- we just need to change a few parameters, the rest it's boilerplate that we can copy-paste

when entering the cluster, only a single core is executing since PULP uses a fork/join model

...

concetto di balanced workloads

...

importante distribuire i dati in maniera coalescente per sfruttare data locality

...

concetto di overhead code per coordinare la parallelizzazione

- è importante ridurre al minimo questo overhead altrimento il gain che otteniamo dalla parallelizzazione viene ridotto di molto
  - ad esempio, posso semplificare la divisione del lavoro, togliendo il ceiling, se sono sicuro che i dati siano un multiplo del numero dei thread

## static vs dynamic worksharing

the static approaches are the best solution when the execution time is known a priori

- se questo non fosse vero (ad esempio ho degli if nel workload) magari distribuisco i dati in maniera bilanciata, tuttavia, alcuni thread impiegano più di altri, e quindi l'esecuzione non è bilanciata

# Amdahl's law

ci permette di rispondere a domande come:

- qual'è la percentuale di parte parallelizzabile che il mio programma deve avere se voglio uno speedup di almeno 16 con 32 core?
- qual'è lo speedup massimo che posso ottenere se avessi un numero infinito di core?

...

le stime ottenute con questa legge sono ottimistiche

- abbiamo un limite al parallelismo (vedi numero di iterazioni di un loop)
- parallele overheads

# Parallel overheads

...

# Power management

in parallel systems the considerations are basically the same for single-core systems, except that in the parallel case, each core has its own power domain with the relative multiple states

we have an additional power domain called (uncore or peripheral power domain) used when at least one core is active that includes the HW units that are not included in the cores but which are on the same processor chip (e.g., DMA, peripheral interfaces, memory subsystem, interconnect, clock)

- even if only a single core is active, we still need memories, dmas, peripherals etc. to be active aswell for the correct function of the core

...

**NB**: parallel execution can save energy

- we're executing for less time, and when cores are not executing we can put them in a low power state
- power aumenta poco, tempo diminuisce molto
