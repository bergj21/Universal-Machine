/**************************************************************
 *
 *                     memory.c
 *
 *     Assignment: UM
 *     Authors:  John Berg (jberg02), Alex Shriver (ashriv02)
 *     Date:    04.12.23
 *
 *     Purpose: Handles the memory representation of the UM emulator.
 *              Utilizes a Hanson Sequence with elements of Hanson UArrays to 
 *              represent segments of memory. Maintains a sequence of the 
 *              unmapped memory segments. 
 *
 **************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <seq.h>
#include <uarray.h>
#include "assert.h"

typedef uint32_t um_instruction;
#define MAX_INDEX 4294967295

/********** segment_new ********
* Purpose:
*      Initializes a new segment into memory
* Inputs:
*	Seq_T memory: The sequence storing all the segments
*	Seq_T unmapped: The sequence storing the currently unmapped segment 
                                                                        indices
*      int num_words: The number of instructions this segment will hold
* Return/Effects:
*      Creates a new segment with the number of instructions equal to register 
*       C and returns the memory index this segment was stored.
* Expects:
*      
* Notes
*      
************************/
int segment_new(Seq_T memory, Seq_T unmapped, unsigned num_words)
{
        assert(memory != NULL);
        assert(unmapped != NULL);
        assert(num_words > 0);  // check if this is worng/ unnecessary

        /* Initialize the new segment */
        UArray_T arr = UArray_new(num_words, sizeof(um_instruction));
        assert(arr != NULL);
        /* Insert segment in the back if there are no unmapped segments */
        if (Seq_length(unmapped) == 0) {
                Seq_addhi(memory, arr);
                /* checks for resource exhaustion */
                assert((Seq_length(memory) - 1) <= MAX_INDEX);
                return Seq_length(memory) - 1;
        } else { /* If unmapped segments then reuse that segment index */
                int *top = (int *) Seq_get(unmapped, 0);
                int index = *top;
                /* Remove that segment ID from the unmapped sequence */
                Seq_remlo(unmapped);
                free(top);
                /* Insert the new segment into the available spot */
                Seq_put(memory, index, arr);
                /* checks for resource exhaustion */
                assert((Seq_length(memory) - 1) <= MAX_INDEX);
                return index;
        }
}


/********** segment_free ********
* Purpose:
*      Frees the segment 
* Inputs:
*	Seq_T memory: The sequence storing all the segments
*	Seq_T unmapped: The sequence storing the currently unmapped segment 
                                                                indices
*       int index: The number of instructions this segment will hold
* Return/Effects:
*      Frees the segment at a specified index and inserts the segment index 
*      into the unmapped sequence.
* Expects:
*      
* Notes
*      
************************/
void segment_free(Seq_T memory, Seq_T unmapped, unsigned index) 
{
        assert(memory != NULL);
        assert(unmapped != NULL);
        assert(index < (unsigned) Seq_length(memory));
        /* Get the sequence that will be removed */
        UArray_T remove = Seq_get(memory, index);
        assert(remove != NULL);
        UArray_free(&remove);
        Seq_put(memory, index, NULL);
        /* Insert the index of the freed segment into the unmapped sequence */
        int *free_index = malloc(sizeof(int));
        assert(free_index != NULL);
        *free_index = index;
        Seq_addhi(unmapped, free_index);
}


/********** load_segment ********
* Purpose:
*      Loads a specific segment into the 0 slot 
* Inputs:
*	Seq_T memory: The sequence storing all the segments
*	Seq_T unmapped: The sequence storing the currently unmapped segment 
                                                                indices
*      int index: The number of instructions this segment will hold
* Return/Effects:
*      Duplicates the specified segment and places it into the 0 segment slot, 
*	abandoning the old 0 segment. 
* Expects:
*      
* Notes
*      
************************/
void load_segment(Seq_T memory, Seq_T unmapped, unsigned index) 
{
        assert(memory != NULL);
        assert(unmapped != NULL);
        /* Do not load any segment if the index is the zero segment */
        if (index == 0) {
                return;
        }
        assert(index < (unsigned) Seq_length(memory));

        /* Get the current zero segment */
        UArray_T zero_seg = Seq_get(memory, 0);
        assert(zero_seg != NULL);
        /* Free that current zero segment */
        UArray_free(&zero_seg);

        /* Get the segment that is being loaded */
        UArray_T segment = Seq_get(memory, index);
        assert(segment != NULL);
        /* Duplicate the segment */
        UArray_T copy = UArray_copy(segment, UArray_length(segment));
        /* Load the duplicated segment into the zero segment slot */
        Seq_put(memory, 0, copy);
}


/********** segment_at ********
* Purpose:
*      Grabs a specific word in memory 
* Inputs:
*	Seq_T memory: The sequence storing all the segments
*	Seq_T unmapped: The sequence storing the currently unmapped segment 
*                       indices
*      int index: The number of instructions this segment will hold
* Return/Effects:
*      Returns a pointer to the word at a given index and offset 
* Expects:
*      
* Notes
*      
************************/
um_instruction *segment_at(Seq_T memory, unsigned index, unsigned offset) 
{
        assert(memory != NULL);
        assert(index < (unsigned) Seq_length(memory));
        UArray_T segment = Seq_get(memory, index);
        assert(segment != NULL);
        assert(offset < (unsigned) UArray_length(segment));
        um_instruction *word = UArray_at(segment, offset);
        return word;
}


/********** free_memory ********
* Purpose:
*      Frees the entire memory unit 
* Inputs:
*	Seq_T memory: The sequence storing all the segments
* Return/Effects:
*      Frees each segment (UArray_T) in memory 
* Expects:
*      
* Notes
*      
************************/
void free_memory(Seq_T memory, Seq_T unmapped) 
{
        assert(memory != NULL);
        assert(unmapped != NULL);
        /* Free the contents of main memory  */
        for (int i = 0; i < Seq_length(memory); i++) {
                UArray_T remove = Seq_get(memory, i);
                if (remove != NULL) {
                        UArray_free(&remove);
                }
        }
        Seq_free(&memory);
        /* Free the contents of the unmapped memory sequence */
        for (int j = 0; j < Seq_length(unmapped); j++) {
                int *index = Seq_get(unmapped, j);
                free(index);
        }
        Seq_free(&unmapped);
}

/********** num_instructions ********
* Purpose:
*      Determine the number of instructions in the zero segment
* Inputs:
*	Seq_T memory: The sequence storing all the segments
* Return/Effects:
*      Returns the length of the zero segment in main memory 
* Expects:
*      The zero segment not to be null
* Notes
*      
************************/
int num_instructions(Seq_T memory)
{
        assert(memory != NULL);
        UArray_T program = Seq_get(memory, 0);
        assert(program != NULL);
        return UArray_length(program);
}