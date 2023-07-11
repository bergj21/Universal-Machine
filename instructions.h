/**************************************************************
 *
 *                     instruction.h
 *
 *     Assignment: UM
 *     Authors:  John Berg (jberg02), Alex Shriver (ashriv02)
 *     Date:    04.12.23
 *
 *     Purpose: Interface of the instructions module
 *
 **************************************************************/
#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <seq.h>
#include <uarray.h>
#include "memory.h"

/*   machine_state
 *   This struct contains the infrastructure necessary for running the UM
 *    
 *   Elements:
 *      Seq_T memory:         models the segmented memory of the emulator
 *      Seq_T unmapped:       identifies memory segments that have been unmapped
 *      UArray_T registers:   represents the 8 32-bit registers of the UM
 *      int program_counter:  identifies the current instruction
 *  
 */
struct machine_state {
        Seq_T memory;
        Seq_T unmapped;
        UArray_T registers;
        int program_counter;
};

typedef struct machine_state machine_state;

typedef enum um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } um_register;

typedef enum um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} um_opcode;

bool handle_instruction(um_instruction word, machine_state *ms);
void conditional_move(um_register A, um_register B, um_register C, 
                                                        UArray_T registers);
void segmented_load(um_register A, um_register B, um_register C, 
                                                        machine_state *ms);
void segmented_store(um_register A, um_register B, um_register C, 
                                                        machine_state *ms);
void add(um_register A, um_register B, um_register C, UArray_T registers);
void multiply(um_register A, um_register B, um_register C, UArray_T registers);
void division(um_register A, um_register B, um_register C, UArray_T registers);
void nand(um_register A, um_register B, um_register C, UArray_T registers);
void map_segment(um_register B, um_register C, machine_state *ms);
void unmap_segment(um_register C, machine_state *ms);
void output(um_register C, UArray_T registers);
void input(um_register C, UArray_T registers);
void load_program(um_register B, um_register C, machine_state *ms);
void load_value(um_register A, UArray_T registers, uint32_t val);

#endif