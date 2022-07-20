# What is DLX
The **DLX** (pronounced "Deluxe") is a [RISC](https://en.wikipedia.org/wiki/Reduced_instruction_set_computer "Reduced instruction set computer") [processor](https://en.wikipedia.org/wiki/Central_processing_unit "Central processing unit") [architecture](https://en.wikipedia.org/wiki/Computer_architecture "Computer architecture") designed by [John L. Hennessy](https://en.wikipedia.org/wiki/John_L._Hennessy "John L. Hennessy") and [David A. Patterson](https://en.wikipedia.org/wiki/David_Patterson_(computer_scientist) "David Patterson (computer scientist)"), the principal designers of the [Stanford MIPS](https://en.wikipedia.org/wiki/Stanford_MIPS "Stanford MIPS") and the [Berkeley RISC](https://en.wikipedia.org/wiki/Berkeley_RISC "Berkeley RISC") designs (respectively), the two benchmark examples of RISC design (named after the Berkeley design).

The DLX is essentially a cleaned up (and modernized) simplified Stanford MIPS CPU. The DLX has a simple [32-bit](https://en.wikipedia.org/wiki/32-bit_computing "32-bit computing") load/store architecture, somewhat unlike the modern [MIPS architecture](https://en.wikipedia.org/wiki/MIPS_architecture "MIPS architecture") CPU. As the DLX was intended primarily for teaching purposes, the DLX design is widely used in [university](https://en.wikipedia.org/wiki/University "University")-level computer architecture courses.

# unpipelined DLX simulator

This project contains files for a simple **unpipelined** DLX simulator. and **pipelined** DLX simulator. The simulator can be compiled using the "make" command at the UNIX prompt. Two
sample input programs "prog1" and "prog2" are provided. The simulator can
execute these programs using: "sim prog1".

The architecture simulator is implemented in sim.c. This file will be
modified to enhance the architecture simulated. 

 The **pipelined** DLX simulator is implemented in new_sim.c. 

The file assemble.c contains a parser to read the input program. You won't have to modify this file unless you want to add additional instructions to the simulator. The following instructions are supported by the simulator at present:
ADD, ADDI, SUB, SUBI, J, BEQZ, BNEZ, LW, and SW.

The simulator checks for out of bounds instruction memory and data memory
access. It also check if register 0 is overwritten. A fail safe mechanism
checks if the simulator is stuck in an infinite loop.

<img width="686" alt="image" src="https://user-images.githubusercontent.com/67783915/179966303-430bbc89-1dd7-4ef2-8204-9e81ee28708f.png">
