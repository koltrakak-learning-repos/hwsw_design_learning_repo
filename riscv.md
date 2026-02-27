Riscv extensions are a way to specilize a processor for a specific workload

OSS: the processor with only I is Turing Complete, it can run any program.

- However, since it has only a minimal set of instructions it won't run many programs efficiently
- by adding more instructions we can achieve higher efficiency

Example: suppose we want to work with floats but we have only a RV32I core

- the compiler will still accept the program with floats
- it will emulate floating point operations with integer operations
- many more instructions and clocks wrt having the floating point extension instructions
  - il tradeoff è che per supportare la F extension devo riservare molti transistor e area del mio chip per la logica hardware di quest'estensione

---

there are many free opcodes to specify custom instructions for your specific purposes

- different implementations may have different custom instruction that accelerate different tasks

## Prefixed tree (instruction length encoding)

LSB of the instruction tells how long the instruction is

Allows RISC-V to have Compressed instructions

In this class we're going to be using 16bit and 32bit instructions

- using a mix of the two allows more compact code which is important in ES that have tight memory constraints

```
TODO: capisci cos'è la macro-op fusion
```

---

# How to build riscv cores

we're going to use a small processor

- pipeline a 4 stadi
  - IF
  - ID
  - Ex
    - separated for arithmetic and memory
  - WB
- it has a lot of special instructions
