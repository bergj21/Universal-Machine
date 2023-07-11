/**************************************************************
 *
 *                     instruction.h
 *
 *     Assignment: UM
 *     Authors:  John Berg (jberg02), Alex Shriver (ashriv02)
 *     Date:    04.12.23
 *
 *     Purpose: The implementation for each of the UM instructions. 
 *
 **************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <bitpack.h>
#include "assert.h"
#include "except.h"
#include "instructions.h"

Except_T Invalid_Instruction = { "Instruction detected with invalid opcode" };

/* The input value for EOF */
#define INPUT_EOF ~0

/********** handle_instruction ********
* Purpose:
*      Extract the operation code and call the corresponding instruction 
* Inputs:
*	um_instruction word: The current instruction in the program
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      Frees the memory sequences and the register array
* Expects:
*      
* Notes
*      
************************/
bool handle_instruction(um_instruction word, machine_state *ms)
{
        assert(ms != NULL);
        /* Extract the values in the current instruction word*/
        um_opcode opcode = Bitpack_getu(word, 4, 28);
        um_register C = Bitpack_getu(word, 3, 0);
        um_register B = Bitpack_getu(word, 3, 3);
        um_register A = Bitpack_getu(word, 3, 6);
        
        /* Call the instruction with the corresponding op code */
        switch (opcode) {
                case CMOV:      conditional_move(A, B, C, ms->registers);
                                return true;
                case SLOAD:     segmented_load(A, B, C, ms);
                                return true;
                case SSTORE:    segmented_store(A, B, C, ms);
                                return true;
                case ADD:       add(A, B, C, ms->registers);
                                return true;
                case MUL:       multiply(A, B, C, ms->registers);
                                return true;
                case DIV:       division(A, B, C, ms->registers);
                                return true;
                case NAND:      nand(A, B, C, ms->registers);
                                return true;
                case HALT:      return false;
                case MAP:       map_segment(B, C, ms);
                                return true;
                case UNMAP:     unmap_segment(C, ms);
                                return true;
                case OUT:       output(C, ms->registers);
                                return true;
                case IN:        input(C, ms->registers);
                                return true;
                case LOADP:     load_program(B, C, ms);
                                return true;

                case LV:        
                                A = Bitpack_getu(word, 3, 25);
                                uint32_t val = Bitpack_getu(word, 25, 0);
                                load_value(A, ms->registers, val);
                                return true;
        }
        /* Throw an exception when a invalid op code is passed in */
        RAISE(Invalid_Instruction);   /* is the only invalid instruction if 
                                        the opcode is not in this range? */
        return false;
}



/********** conditional_move ********
* Purpose:
*      Changes the value of a register under a condition  
* Inputs:
*	um_register A, B, C : The registers that are being dealt with
*      UArray_T registers: The array of registers
* Return/Effects:
*      If register C not equal to zero then moves the value in register B into A
* Expects:
*      
* Notes
*      
************************/
void conditional_move(um_register A, um_register B, um_register C, 
                                                        UArray_T registers) 
{
        assert(registers != NULL);
        if (*(uint32_t *) UArray_at(registers, C) != 0) 
        {
                uint32_t *reg_A = UArray_at(registers, A);
                *reg_A = *(uint32_t *)UArray_at(registers, B);
        }

}


/********** segmented_load ********
* Purpose:
*      Loads a value from memory into a registers  
* Inputs:
*	um_register A, B, C : The registers that are being dealt with
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      Loads the value at m[r[B]][r[C]] into register A
* Expects:
*      
* Notes
*      This function will interact with the memory module using the segment_at 
*                                                               function.
************************/
void segmented_load(um_register A, um_register B, um_register C, 
                                                        machine_state *ms) 
{
        assert(ms != NULL);
        uint32_t *reg_A = UArray_at(ms->registers, A);
        um_instruction val = *segment_at(ms->memory, 
                                *(uint32_t *) UArray_at(ms->registers, B), 
                                *(uint32_t *) UArray_at(ms->registers, C));
        *reg_A = val;
}


/********** segmented_store ********
* Purpose:
*      Loads the value of a register into memory
* Inputs:
*	um_register A, B, C : The registers that are being dealt with. 
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      Stores the value in register C at located m[r[B]][r[C]] in memory
* Expects:
*      
* Notes
*      This function will interact with the memory module using the 
*      segment_at function.
************************/
void segmented_store(um_register A, um_register B, um_register C, 
                                                        machine_state *ms) 
{
        assert(ms != NULL);
        uint32_t reg_C = *(uint32_t *) UArray_at(ms->registers, C);
        um_instruction *dst = segment_at(ms->memory, 
                                *(uint32_t *) UArray_at(ms->registers, A), 
                                *(uint32_t *) UArray_at(ms->registers, B));
        *dst = reg_C;
}


/********** add ********
* Purpose:
*      Takes the sum of two values   
* Inputs:
*	um_register A, B, C : The registers that are being dealt with
*      UArray_T registers: The array of registers
* Return/Effects:
*      Stores the sum of register B and C into register A
* Expects:
*      
* Notes
*      
************************/
void add(um_register A, um_register B, um_register C, UArray_T registers) 
{
        assert(registers != NULL);
        uint32_t *reg_A = UArray_at(registers, A);
        *reg_A = (*(uint32_t *)UArray_at(registers, B) + 
                  *(uint32_t *)UArray_at(registers, C));
}


/********** multiply ********
* Purpose:
*      Multiplies two values
* Inputs:
*	um_register A, B, C : The registers that are being dealt with
*      UArray_T registers: The array of registers
* Return/Effects:
*      Stores the product of register B and C into register A
* Expects:
*      
* Notes
*      
************************/
void multiply(um_register A, um_register B, um_register C, UArray_T registers) 
{
        assert(registers != NULL);
        uint32_t *reg_A = UArray_at(registers, A);
        *reg_A = (*(uint32_t *)UArray_at(registers, B) * 
                  *(uint32_t *)UArray_at(registers, C));
}


/********** division ********
* Purpose:
*      Divides two values
* Inputs:
*	um_register A, B, C : The registers that are being dealt with
*      UArray_T registers: The array of registers
* Return/Effects:
*      Stores the division of register B and C into register A
* Expects:
*      
* Notes
*      
************************/
void division(um_register A, um_register B, um_register C, UArray_T registers) 
{
        assert(registers != NULL);
        uint32_t *reg_A = UArray_at(registers, A);
        uint32_t reg_B = *(uint32_t *) UArray_at(registers, B);
        uint32_t reg_C = *(uint32_t *) UArray_at(registers, C);
        assert(reg_C != 0);
        *reg_A = (reg_B / reg_C);
}


/********** nand ********
* Purpose:
*      Performs bitwise NAND on two values
* Inputs:
*	um_register A, B, C : The registers that are being dealt with
*      UArray_T registers: The array of registers
* Return/Effects:
*      Stores the bitwise NAND of register B and C into register A
* Expects:
*      
* Notes
*      
************************/
void nand(um_register A, um_register B, um_register C, UArray_T registers) 
{
        assert(registers != NULL);
        uint32_t *reg_A = UArray_at(registers, A);
        uint32_t reg_B = *(uint32_t *) UArray_at(registers, B);
        uint32_t reg_C = *(uint32_t *) UArray_at(registers, C);
        *reg_A = ~(reg_B & reg_C);
}


/********** map_segment ********
* Purpose:
*      A new segment is created
* Inputs:
*	um_register B, C : The registers that are being dealt with
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      Maps a new segment with the number of words equal to register C. The new 
*	segment is mapped as $m[$r[B]]
* Expects:
*      
* Notes
*      This function will interact with the memory module using the 
*      segment_new function.
************************/
void map_segment(um_register B, um_register C, machine_state *ms) 
{
        assert(ms != NULL);
        uint32_t reg_C = *(uint32_t *) UArray_at(ms->registers, C);
        uint32_t *reg_B = UArray_at(ms->registers, B);
        *reg_B = segment_new(ms->memory, ms->unmapped, reg_C);
}


/********** unmap_segment ********
* Purpose:
*      A segment is unmapped
* Inputs:
*	um_register C : The registers that are being dealt with
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      The segment $m[$r[C]] is unmapped. Future Map Segment instructions may 
*      reuse 
*	the identifier $r[C].
* Expects:
*      The segment to unmap must not be 0 and must be mapped
* Notes
*      This function will interact with the memory module using the 
*      segment_free function.
************************/
void unmap_segment(um_register C, machine_state *ms) 
{
        assert(ms != NULL);
        uint32_t reg_C = *(uint32_t *) UArray_at(ms->registers, C);
        assert(reg_C != 0);
        segment_free(ms->memory, ms->unmapped, reg_C);
}


/********** output ********
* Purpose:
*      Outputs a value to stdout
* Inputs:
*	um_register C : The register that is being dealt with
*      UArray_T registers: The array of registers
* Return/Effects:
*      Outputs the value in register C to stdout
* Expects:
*      Only values from 0-255 are allowed
* Notes
*      
************************/
void output(um_register C, UArray_T registers) 
{
        assert(registers != NULL);
        uint32_t reg_C = *(uint32_t *) UArray_at(registers, C);
        assert(reg_C <= 255);
        putchar(reg_C);
}


/********** input ********
* Purpose:
*      Waits for input on the I/O device 
* Inputs:
*	um_register C : The registers that are being dealt with
*      UArray_T registers: The array of registers
* Return/Effects:
*      Takes input from stdin and stores the value in register C
* Expects:
*       Must be a value from 0 to 255. If the end of input has been signaled, 
*	then $r[C] is loaded with a full 32-bit word in which every bit is 1.
* Notes
*      
************************/
void input(um_register C, UArray_T registers) 
{
        assert(registers != NULL);
        int input = getc(stdin);

        uint32_t *reg_C = UArray_at(registers, C);
        /* If the input is EOF then inert all 1s into register */
        if (input == EOF) {
                *reg_C = INPUT_EOF;
                return;
        }
        *reg_C = input;
}


/********** load_program ********
* Purpose:
*      A program is loaded into segment 0
* Inputs:
*	um_register B, C : The registers that are being dealt with
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      Segment $m[$r[B]] is duplicated, and the duplicate replaces $m[0], which 
*	is abandoned. The program counter is set to point to $m[0][$r[C]].
* Expects:
*      
* Notes
*      This function will interact with the memory module using the 
*      duplicate_segment function.
************************/
void load_program(um_register B, um_register C, machine_state *ms) 
{
        assert(ms != NULL);
        uint32_t reg_B = *(uint32_t *) UArray_at(ms->registers, B);
        uint32_t reg_C = *(uint32_t *) UArray_at(ms->registers, C);
        load_segment(ms->memory, ms->unmapped, reg_B);
        /* Make sure that the program counter is in bounds */
        assert(reg_C < (uint32_t) UArray_length(Seq_get(ms->memory, 0)));
        /* Update the program counter */
        ms->program_counter = reg_C;
}


/********** load_value ********
* Purpose:
*      A value is loaded into registers
* Inputs:
*	um_register B, C : The registers that are being dealt with
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      Loads the value in the instruction word into register A
* Expects:
*      
* Notes
*      
************************/
void load_value(um_register A, UArray_T registers, uint32_t value) 
{
        assert(registers != NULL);
        uint32_t *reg_A = UArray_at(registers, A);
        *reg_A = value;
}