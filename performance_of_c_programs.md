...

## volatile

prevents compiler memory optimizations of a variable.

- something out of our control may happen to this variable
- compiler pls, don't infer any property on this variable

accessing a mmapped device with a volatile pointer

- since its an mmapped device, if i want to read/write it twice, i want that to really happen
- i need to mark it volatile so the compiler doesn't optimize away the reads/writes
  - if the compiler sees two reads without a write to that location in between, it assumes that the value hasn't changed (if not volatile) and only does the first read
  - similarly, if there are two writes without a read in between, the compiler keeps only the last write

## integers

on riscv (in particular, we use RV32) the size of ints are fixed (thank god)

# Memory Model of a C program

global variables in c are default initialized to zero

- initializing them to zero or not initializing them is the same.
- this means that a global initialized to zero is treated as uninitialized and put in the BSS segment
  - int x; int y=0; -> both in BSS

the BSS is interesting because we don't need to allocate it in the binary file, we just need it at runtime

Heap and Stack are initially empty in the binary, because they grow while executing the program

# Linker script

We have a memory map and a linker that puts everything together

The linker script is a text file that specifies the mapping between the segments in the memory map and the physical memory areas available

Defines a load memory address and an execution address

- il mio programma potrebbe stare in una SD ma è chiaro che eseguirlo da li sarebbe estremamente lento
- specifichiamo dove il programma e dove caricarlo al momento dell'esecuzione

Stack e Heap sono inizialmente vuoti, nel linker script definiamo il loro bound

Defines symbols useful for the program

# ABI

an architecture may have multiple ABIs

specifying the ABI is as important as specifying the architecture in the context of binary interoperability (pensa a linking the librerie, oppure system call)

For example it's important that we use the same registers for the same purpose

- a -> arguments
- t -> temporary
- s -> saved
- sp
- ra

## Stack frame layout

stack arguments == spilled registers

## Calling convention

Caller-Saved (t0-t6, a0-a7)

- caller needs to save these registers (if used) before the function call, and restores them after the call

Callee-Saved (s0-s11)

- callee needs to save these registers at the start of the body, and restores them at the end of the function body

Oss:

- From the compiler point of view, it is optimal to start by using the caller saved registers, bacause i don't need to spend time saving and restoring registers while executing the function body
- if we need more registers we start using the callee save register and we perform a register spill (spill the registers of the caller)
- we may even need to do a second order spilling if even the calle-saved registers are not enough
  - i spill my register
  - then i restore my previous values
  - the i restore the values of the caller
- in this case most of the time is spent handling spilling, and we may even consider swithcing architectures if our code is stressing too much the RF

---

##

**do-while is more convenient in ES bacause we have only a single branch**

- i don't need to check if i can enter the loop body

---

# Floating Point Assembly

abbiamo un dedicated register file

i ruoli dei fp registers sono gli stessi così come la loro calling convention

- fa
- fs
- ft

---

# Performance metrics

performance e speedup

...

from the c program we can't deduce the time it takes to execute the program

so we compile, because from the assembly we can

- we have the instructions
- from them we get the total number of cycles
  - a bit more tricky

To get the number of cycles we need some architectural information

...

"it's enough to focus on what happens at the execution stage"

Forward branches are predicted as not taken (i.e., go to the next instruction), backward branches are predicted as taken (i.e., jump to the address); mispredictions implies a stalls of 3 cycles

...

if i cannot compute the cycles (maybe because i don't have the assembly code) i can estimate the average CPI

in our simple microcontrollers the optimistic CPI is 1

- clearly we can't achieve this (complex multicycle instructions, latency of memory access, data and control hazards)
