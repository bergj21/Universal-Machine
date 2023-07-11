/**************************************************************
 *
 *                     driver.c
 *
 *     Assignment: UM
 *     Authors:  John Berg (jberg02), Alex Shriver (ashriv02)
 *     Date:    04.12.23
 *
 *     Purpose: Serve as the driver for the UM executable program.
 *              Handles the um file and utilizes the memory and instructions 
 *              modules to run the UM.
 *
 **************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <seq.h>
#include <uarray.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <bitpack.h>
#include "memory.h"
#include "assert.h"
#include "instructions.h"

/* Number of registers */
#define NUM_REGISTERS 8
/* The initial program counter */
#define INITIAL_COUNTER 0
/* The index of the zero segment */
#define ZERO_SEGMENT 0

void initialize_machine_state(machine_state *ms, FILE *fp, char *filename);
void drive_program(machine_state *ms);
void free_program(machine_state *ms);

int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Invalid usage. Try: ./um [um binary file]\n");
                return EXIT_FAILURE;
        }
        FILE *fp = fopen(argv[1], "rb");
        assert(fp != NULL);

        machine_state ms;
        initialize_machine_state(&ms, fp, argv[argc - 1]);
        drive_program(&ms);
        free_program(&ms);

        fclose(fp);
        return EXIT_SUCCESS;
}

/********** initialize_machine_state ********
* Purpose:
*      Initialize the initial state of the UM 
* Inputs:
*       machine_state *ms: The machine state struct that holds the UM ADT’s
*	File *file: The input .um program
* Return/Effects:
*      The machine state struct will be updated to initialize the machine state
* Expects:
*      A .um file with instructions
* Notes
*      This function will interact with the memory module using the 
*      segment_new() function.
************************/
void initialize_machine_state(machine_state *ms, FILE *fp, char *filename) 
{
        assert(ms != NULL);
        assert(fp != NULL);
        /* Extract the size of the file using the stat method */
        struct stat buf;          
        int ret = stat(filename, &buf);
        assert(ret == 0);
        int filesize = buf.st_size;
        /* Determine the number of instruction words based off the size  */
        int numWords = filesize / 4;
        /* Initialize the sequences used in memory */
        Seq_T unmapped = Seq_new(0);
        assert(unmapped != NULL);
        Seq_T memory = Seq_new(0);
        assert(memory != NULL);
        /* Initialize the zero segment */
        int segNum = segment_new(memory, unmapped, numWords);
        assert(segNum == 0);
        /* Construct the instruction word using the bit pack interface */
        for (int i = 0; i < numWords; i++) {
                um_instruction word = 0;
                word = Bitpack_newu(word, 8, 24, getc(fp));
                word = Bitpack_newu(word, 8, 16, getc(fp));
                word = Bitpack_newu(word, 8, 8, getc(fp));
                word = Bitpack_newu(word, 8, 0, getc(fp));
                /* Insert the instruction into the zero segment */
                um_instruction *location = segment_at(memory, ZERO_SEGMENT, i);
                *location = word;
        }
        /* Initialize the array of registers */
        UArray_T registers = UArray_new(NUM_REGISTERS, sizeof(uint32_t));
        assert(registers != NULL);
        /* Update the machine state array with the initialized values */
        ms->memory = memory;
        ms->unmapped = unmapped;
        ms->registers = registers;
        ms->program_counter = INITIAL_COUNTER;
}


/********** drive_program ********
* Purpose:
*      The driver method that performs the fetch and decode aspect of the UM
* Inputs:
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      This function will continuously fetch and decode until the end of the
*      program
* Expects:
*      
* Notes
*      This function will interact with the instruction module using the 
*      handle_instruction function. 
************************/
void drive_program(machine_state *ms) 
{     
        assert(ms != NULL);  
        bool drive = true;
        while (drive && ms->program_counter < num_instructions(ms->memory)) {
                /* Grab the current instruction in the zero segment */
                um_instruction word = *segment_at(ms->memory, 0, 
                                                        ms->program_counter);
                /* Update the program counter */
                ms->program_counter++;
                /* Perform the instruction */
                drive = handle_instruction(word, ms);
        }
        assert(!drive);
}


/********** free_program ********
* Purpose:
*      Clean up all the memory allocated to conduct the UM
* Inputs:
*      machine_state *ms: The machine state struct that holds the UM ADT’s
* Return/Effects:
*      Frees the memory sequences and the register array
* Expects:
*      
* Notes
*      This function will interact with the memory module using the
*      free_memory function. 
************************/
void free_program(machine_state *ms) 
{
        assert(ms != NULL);
        free_memory(ms->memory, ms->unmapped);
        UArray_free(&ms->registers);
}