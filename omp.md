...

NB:

- with the parallel pragma we're just declaring an amount of parallelism to the compiler
- the actual number that are going to be executing in parallel depends on the hardware capability of the hardware
- OMP astrae il parallelismo della macchina e permette di scrivere parallel programs
  - se poi la macchina supporta un grado di parallelismo minore rispetto a quello dichiarato, il runtime sequenzializzerà i thread (performance pessime dato che si spreca tempo a fare context switch)
  - nel caso di pulp crea il numero di thread massimo suportato dall'hardware

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

- for
- master, single
- sections

usanti all'interno di una parallel region

...

**NB**: puoi unire la parallel region e una for region in un unica pragma

```
last private

...

the value of the last thread private variable is copied to the private variable of the first thread

dopo la parallel region esegue il thread 0, e quindi, senza last_private(i), il valore di i sarebbe l'UB di quello del thread 0
```

### overhead of static scheduling

ricordiamoci sempre che quando parallelizziamo dobbiamo sempre fare attenzione all'overhead che introduciamo.

Nel caso dello static scheduling dei loop, we need to compute the bounds for each chunking iteration (solo 1 nel caso di static scheduling?)

## dynamic scheduling

static scheduling funziona bene solo quando ogni iterazione impiega lo stesso tempo

questo non è sempre vero

- pensa ad algoritmi la cui durata è data-dependent (es: se leggo un valore grande da un sensore ci metto tanto, altrimenti no)

...

le iterazioni vengono suddivise in singoli task che vengono eseguiti dai thread appena sono liberi

nella pratica abbiamo bisogno di una coda ordinata di task da cui i thread che hanno finito il lavoro poppano il prossimo task/iterazione.

siccome i thread accedono concorrentemente alla coda, bisogno garantire mutua esclusione tramite locking (overhead)

the overhead of dynamic scheduling is always higher than that of static scheduling (locking)

**NB**: notiamo che usare chunk da una singola iterazione introduce più overhead

- dobbiamo trovare un buon compromesso tra overhead e bilanciamento del workload
- difficile scegliere la chunk size giusta -> o fai analisi statistiche sul dataset/profilazione, oppure fai trial and error
- rischio di peggiorare

# synchronization

## barrier

\#pragma omp barrier

## critical

vale per la prossima istruzione o blocco

...

il numero di istruzioni nella sezione critica deve essere minimo rispetto al numero totale di istruzioni che stiamo eseguendo in parallello. Altrimenti invece di parallelizzare, i thread aspettano di continuo, ottenendo nella pratia una sequenzializzazione di fatto

### reduction

più efficiente di usare critical dato che adotta schemi di riduzione in cui si sequenzializza solo alla fine quando si fa la somma delle somme parziali

dobbiamo specificare anche l'operazione

- serve al compilatore per capire
  - come ridurre
  - come inizializzare le variabili per le riduzioni parziali

## master and single

The master directive denotes a structured block that is only executed by the master thread. single does the same thing but it doesn't necessarily need to be the master thread

a differenze degli altri work-sharing constructs, per master non c'è una barrier implicita alla fine del blocco

- bisogna inserire una \#pragma omp barrier esplicitamente

**single invece la inserisce esplicitamente**

# Tasking model

The for pragma allows to exploit data parallelism in loops

Conditions on the core id can be used to express task parallelism

OpenMP provides a dedicated work-sharing construct without asking for a specific core:

- \#pragma omp sections
- \#pragma omp section (singolare)
