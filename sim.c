#include <stdio.h>
#include <string.h>
#include "globals.h"

// Max cycles simulator will execute -- to stop a runaway simulator
#define FAIL_SAFE_LIMIT  500000

struct instruction   inst_mem[MAX_LINES_OF_CODE];  // instruction memory
struct instruction   IR;                       // instruction register
int   data_mem[MAX_WORDS_OF_DATA];             // data memory
int   int_regs[16];                            // integer register file
int   PC,NPC;                                  // PC and next PC
int   A,B;                                     // ID register read values
int   mem_addr;                                // data memory address
int   Cond;                                    // branch condition test
int   LMD;                                     // data memory output
int   ALU_input1,ALU_input2,ALU_output;        // ALU intputs and output
int   wrote_r0=0;                              // register R0 written?
int   code_length;                             // lines of code in inst mem
int   cycle;                                   // simulation cycle count
int   inst_executed;                          // number of instr executed


/* Simulate one cycle of the DLX processor */
void Simulate_DLX_cycle(){

  /* ------------------------------ IF stage ------------------------------ */
  /* check if instruction memory access is within bounds */
  if (PC < 0 || PC >= code_length){
    printf("Exception: out-of-bounds inst memory access at PC=%d\n",PC);
    exit(0);
  }
  IR=inst_mem[PC];    /* read instruction memory */
  NPC=PC+1;           /* increment PC */


  /* ------------------------------ ID stage ------------------------------ */
  A=int_regs[IR.rs];   /* read registers */
  B=int_regs[IR.rt];

  /* Calculate branch condition codes 
     and change PC if condition is true */
  if (IR.op==BEQZ)
    Cond=(A==0);         /* condition is true if A is 0 (beqz) */
  else if (IR.op==BNEZ)
    Cond=(A!=0);         /* condition is true if A is not 0 (bnez) */
  else if (IR.op==J)
    Cond=1;              /* condition is alway true for jump instructions */
  else
    Cond=0;              /* condition is false for all other instructions */

  if (Cond)              /* change NPC if condition is true */
    NPC=NPC+IR.imm;


  /* ------------------------------ EX stage ------------------------------ */
  /* set ALU inputs */
  ALU_input1 = A;

  if (IR.op == ADDI || IR.op == SUBI || 
      IR.op == LW   || IR.op == SW )
    ALU_input2 = IR.imm;
  else
    ALU_input2 = B;

  /* calculate ALU output */
  if(IR.op == SUB || IR.op == SUBI)
    ALU_output = ALU_input1 - ALU_input2;
  else
    ALU_output = ALU_input1 + ALU_input2;


  /* ------------------------------ MEM stage ----------------------------- */
  mem_addr = ALU_output;

  /* check if data memory access is within bounds */
  if (IR.op == LW || IR.op == SW){
    if (mem_addr < 0 || mem_addr >= MAX_WORDS_OF_DATA){
      printf("Exception: out-of-bounds data memory access at PC=%d\n",PC);
      exit(0);
    }
  }

  if(IR.op == LW)               /* read memory for lw instruction */
    LMD = data_mem[mem_addr];
  else if (IR.op == SW)         /* or write to memory for sw instruction */
    data_mem[mem_addr] = B;


  /* ------------------------------ WB stage ------------------------------ */
  /* write to register and check if output register is R0 */
  if (IR.op == ADD || IR.op == SUB) {
    int_regs[IR.rd]=ALU_output;
    wrote_r0=(IR.rd==R0);
  }
  else if (IR.op == ADDI || IR.op == SUBI) {
    int_regs[IR.rt]=ALU_output;
    wrote_r0=(IR.rt==R0);
  } 
  else if (IR.op == LW) {
    int_regs[IR.rt]=LMD;
    wrote_r0=(IR.rt==R0);
  }

  inst_executed++;

  /* if output register is R0, exit with error */
  if (wrote_r0){
    printf("Exception: Attempt to overwrite R0 at PC=%d\n",PC);
    exit(0);
  }

}


main(int argc, char**argv){
  int i;

  if(argc!=2) {  /* Check command line inputs */
    printf("Usage: sim [program]\n");
    exit(0);
  }

  /* assemble input program */
  AssembleSimpleDLX(argv[1],inst_mem,&code_length);

  /* set initial simulator values */
  cycle=0;                /* simulator cycle count */
  PC=0;                   /* first instruction to execute from inst mem */
  int_regs[R0]=0;         /* register R0 is alway zero */
  inst_executed=0;


  /* Main simulator loop */
  while (PC!=code_length){
    Simulate_DLX_cycle();      /* simulate one cycle */
    PC=NPC;                    /* update PC          */
    cycle+=5;                  /* update cycle count */

    /* check if simuator is stuck in an infinite loop */
    if (cycle>FAIL_SAFE_LIMIT){    
      printf("\n\n *** Runaway program? (Program halted.) ***\n\n");
      break;
    }
  }


  /* print final register values and simulator statistics */
  printf("Final register file values:\n");
  for (i=0;i<16;i+=4){
    printf("  R%-2d: %-10d  R%-2d: %-10d",i,int_regs[i],i+1,int_regs[i+1]);
    printf("  R%-2d: %-10d  R%-2d: %-10d\n",i+2,int_regs[i+2],i+3,int_regs[i+3]);
  }
  printf("\nCycles executed: %d\n",cycle);
  printf("IPC:  %6.3f\n", (float)inst_executed/(float)cycle);
  printf("CPI:  %6.3f\n", (float)cycle/(float)inst_executed);

}
