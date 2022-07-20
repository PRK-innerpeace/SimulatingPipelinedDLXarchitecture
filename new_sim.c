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
//int   A,B;                                     // ID register read values
int   mem_addr;                                // data memory address
int   Cond=0;                                    // branch condition test
//int   LMD;                                     // data memory output
int   ALU_input1,ALU_input2;        // ALU intputs and output
int   wrote_r0=0;                              // register R0 written?
int   code_length;                             // lines of code in inst mem
int   cycle;                                   // simulation cycle count
int   inst_executed;                          // number of instr executed
int   ForwardA;
int   ForwardB;
int   ForwardC;
int   ForwardD;


void	WB_stage();
void	MEM_stage();
void	EX_stage();
void	ID_stage();
void    IF_stage();

struct pipeline {
	int NPC;
	instruction IR;
	int A;
	int B;
	int ALU_output;
	int valid; 
	int LMD;

};

struct pipeline IF_ID, ID_EX, EX_MEM, MEM_WB;

/* Simulate one cycle of the DLX processor */


  /* ------------------------------ IF stage ------------------------------ */
  /* check if instruction memory access is within bounds */
 void  IF_stage(){
	  
  if (PC < 0 || PC >= code_length){
    printf("Exception: out-of-bounds inst memory access at PC=%d\n",PC);
    exit(0);
  }
  
  // pc的更改在这里进行？ 根据brench的结果判断 0：PC+1,1:branch那边计算的pc当然这个pc值来自NPC加上imm
  if (Cond)
  {
  PC=EX_MEM.NPC
  }
  else
  {
	PC = NPC; 
  }
  IR=inst_mem[PC];   /* read instruction memory */

  NPC=PC+1;         /* increment PC */
  
  IF_ID.IR=IR;
  IF_ID.NPC=NPC; 

  
  }
  
void ID_stage(){
ID_EX.
  /* ------------------------------ ID stage ------------------------------ */
  ID_EX.A=int_regs[IF_ID.IR.rs];   /* read registers */
  ID_EX.B=int_regs[IF_ID.IR.rt];
  
  /* Calculate branch condition codes 
     and change PC if condition is true */
	
  ID_EX.IR = IF_ID.IR;
  ID_EX.NPC=IF_ID.NPC;
	
}

void EX_stage(){
  /* ------------------------------ EX stage ------------------------------ */
  /* set ALU inputs */
  
  ALU_input1 = ID_EX.A;

  if (ID_EX.IR.op == ADDI || ID_EX.IR.op == SUBI ||
  ID_EX.IR.op == LW   || ID_EX.IR.op == SW )
  ALU_input2 = ID_EX.IR.imm;
  else
  ALU_input2 = ID_EX.B;

  /* calculate ALU output */
  if(ID_EX.IR.op == SUB || ID_EX.IR.op == SUBI)
  EX_MEM.ALU_output = ALU_input1 - ALU_input2;
  else
  EX_MEM.ALU_output = ALU_input1 + ALU_input2;

	
  EX_MEM.NPC=ID_EX.NPC+ID_EX.IR.imm;
  EX_MEM.A=ID_EX.A;
  EX_MEM.B=ID_EX.B;
  EX_MEM.IR=ID_EX.IR;
  


}

void	MEM_stage(){
  /* ------------------------------ MEM stage ----------------------------- */
	mem_addr = EX_MEM.ALU_output;

	/* check if data memory access is within bounds */
	if (EX_MEM.IR.op == LW || EX_MEM.IR.op == SW){
	if (mem_addr < 0 || mem_addr >= MAX_WORDS_OF_DATA){
		printf("Exception: out-of-bounds data memory access at PC=%d\n",PC);
		exit(0);
	}
	}
  
	if (EX_MEM.IR.op==BEQZ)
	Cond=(EX_MEM.A==0);         /* condition is true if A is 0 (beqz) */
	else if (EX_MEM.IR.op==BNEZ)
	Cond=(EX_MEM.A!=0);         /* condition is true if A is not 0 (bnez) */
	else if (EX_MEM.IR.op==J)
	Cond=1;              /* condition is alway true for jump instructions */
	else
	Cond=0;              /* condition is false for all other instructions */

	if(EX_MEM.IR.op == LW)               /* read memory for lw instruction */
	MEM_WB.LMD = data_mem[mem_addr];
	else if (EX_MEM.IR.op == SW)         /* or write to memory for sw instruction */
	data_mem[mem_addr] = EX_MEM.B;
	
	MEM_WB.ALU_output = EX_MEM.ALU_output;	
	MEM_WB.IR = EX_MEM.IR;
 	
}

void WB_stage(){
  /* ------------------------------ WB stage ------------------------------ */
  /* write to register and check if output register is R0 */
  
  if (MEM_WB.IR.op == ADD || MEM_WB.IR.op == SUB) {
    int_regs[MEM_WB.IR.rd]= MEM_WB.ALU_output;
    wrote_r0=(MEM_WB.IR.rd==R0);
  }
  else if (MEM_WB.IR.op == ADDI || MEM_WB.IR.op == SUBI) {
    int_regs[MEM_WB.IR.rt]=MEM_WB.ALU_output;
    wrote_r0=(MEM_WB.IR.rt==R0);
  } 
  else if (MEM_WB.IR.op == LW) {
    int_regs[MEM_WB.IR.rt]=MEM_WB.LMD;
    wrote_r0=(MEM_WB.IR.rt==R0);
  }



  /* if output register is R0, exit with error */
  if (wrote_r0){
    printf("Exception: Attempt to overwrite R0 at PC=%d\n",PC);
    exit(0);
  }
	  inst_executed++;

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
  PC=-1;                   /* first instruction to execute from inst mem */
  int_regs[R0]=0;         /* register R0 is alway zero */
  inst_executed=0;


  /* Main simulator loop */
  while (PC!=code_length){
    
	WB_stage();
	MEM_stage();
	  
	EX_stage();

	 
	ID_stage();
	IF_stage();


    cycle+=1;                  /* update cycle count */


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
