interfaces between the mcu chip and the outside world (sensors/actuators, eg: a camera, also external memory)

this is typically done at various levels of speed

# Serial Interfaces

## I2C (Inter-Integrated Circuit Bus)

Used for communication with external peripherals, for example:

- EEPROMs
- thermal sensors
- real-time clocks

low power, low bandwidth communication and requires a minimal number of wires

- very cheap but you can't communicate a lot of data

...

OSS: the data-rate of the lines in an upper bound (it specifies the switching speed) of the actual speed of transfer of the data (the protocol has a lot of overhead)

the protocol is conceptually similar to APB bus used in on-chip communication

**half duplex synchronous communication scheme**

- synchronous means that the master generates a clock (SCL) that the data (SDA) is synchronized to
- the data can only travel in one direction at a time

...

the boxes are communincation actors (initiators/targets)

- if nobody wants to communicate on the line, this means that no one wants to change the voltage on the SDA/SCL wires, thus the pull-up resistors pull-up the voltage to VDD
- **wired-end concept**:
  - if the transistors are off, nobody is writing, so the wires are pulled-up to vdd
  - if the transistor is on, it becomes equivalent to a resistance much smaller to the pull-up ones;
    - la tensione sul filo diventa il risultato della caduta di potenziale
    - almost zero, because the resistance is much smaller (fai i calcoli)
    - **accedendere il transistor fa cadere la tensione sui fili, viceversa spegnerli alza la tensione** -> this is the communication
- clearly, **only actor at a time can drive its transistor (one initiator at a time)**
  - this must be guaranteed by the protocol

...

asserting the start bit means turning on its transistor

**everytime the clock goes high, we're telling the target to look at the data**

- this is why it's called a serial protocol, every bit of information is transmitted one at a time

...

we have a **periodic ack of the data** to make sure that the data has been successfully tranfered

- pensa al target di una scrittura che è spento, senza ack il master penserebbe di aver trasferito correttamente i dati

the protocol overhead is the start, acks, direction, etc periods

start bit starts the clock, stop bit stops the clock

Oss guardando la read: siccome abbiamo solo un data wire, la comunicazione è per forza half-duplex

**clock-stretching**:

- the slave can be slower than the master and it may get overwhelmed by the data-rate of the master
- the master is driving the clock, but it also monitors it
- if it sees that the clock is being set to zero by the target, he knows that the data needs to be held a bit longer on the data line making the processing possible by the target

## SPI (Serial Peeripheral Interface)

I2C is commonly used, SPI a bit more used and a bit faster and more flexible (and a bit more resource hungry)

full-duplex, synchronous scheme

- 2 data wires (MISO and MOSI)

**one chip select per target**

we have four combinations of configuration that specify when to sample the data on the MISO and MOSI lines (what clock edge)

**no ack, no clock stretching contrarily to I2C**

- More complex behavior than simple data streaming can be mapped on top of SPI protocol by leveraging the full-duplex capability
    – e.g. command + address + data streaming
- **you have to implement it on your own!** (guarda esempio slide 27)

the first n bits of the communication are used by the master to configure the peripheral

`does this mean that every peripheral has a higher level protocol that specifies how to communicate with it? Do i need to read the manual for every peripheral?`

## UART

commonly used for transceivers (blueetooth, wireless stuff, usb)

point-to-point bus

- Essentially a parallel2serial (TX), serial2parallel (RX) converter couple

asynchronous -> no shared clock!

- we don't need to have a clock signal because the clock of the actors is much faster than the baud-rate (bit/s) of the data transfer
  - we have many chances to sample the data in a single transfer-clock cycle
- **this works because the communication is much slower than the clock of the devices, this is the assumption needed for asynchronous communication**

Highly configurable

- simplex, full-duplex or half-duplex
- parity bit

La comunicazione avviene più o meno come sempre

- abbiamo uno start bit e degli stop bits
- non sembra ci sia un address e quindi è un interfaccia 1 a 1

The UART protocol can also include a handshake:

- request-to-send (RTS) is active when the transmitter wishes to send data
- clear-to-send (CTS) is active when the receiver can receive data
- exchange happens when CTS and RTS are both asserted
- can be connected in “crossed” configuration if both devices can act as transmitters in the communication

## Hyperbus

a bit more comples, higher bandwidth protocol (still not close to LPDDR, DDR, HBM, etc...)

mostly used to communicate with external memories

 we have a 8-bit parallelism

- in a single bit slot we can transmit a byte
- faster
- more wires = more cost

we're communicating data in both edges (dual-data rate)

- doubles the speed

---

NB: studiare questi protocolli (anche quelli on chip), è importante per varie ragioni:

- scrivere firmware richiede conoscere i protocolli di comunicazione per configurare correttamente la comunicazione
  - se non conosco cos'è il baud-rate, i comandi, etc. come faccio?
- performance analysis: voglio stimare quanto il mio sistema riesce ad andare forte teoricamente
- debug e riconoscimento dei colli di bottiglia: se il data transfer ci sta mettento troppo tempo, capiamo se è colpa del protocollo e vediamo con cosa sostituirlo o come risolvere
