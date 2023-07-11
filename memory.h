/**************************************************************
 *
 *                     memory.h
 *
 *     Assignment: UM
 *     Authors:  John Berg (jberg02), Alex Shriver (ashriv02)
 *     Date:    04.12.23
 *
 *     Purpose: The interface of the memory module
 *
 **************************************************************/
#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <stdio.h>
#include <seq.h>
#include <uarray.h>
#include "stdint.h"

typedef uint32_t um_instruction;

int segment_new(Seq_T memory, Seq_T unmapped, unsigned num_words);
void segment_free(Seq_T memory, Seq_T unmapped, unsigned index);
int load_segment(Seq_T memory, Seq_T unmapped, unsigned index);
um_instruction *segment_at(Seq_T memory, unsigned index, unsigned offset);
void free_memory(Seq_T memory, Seq_T unmapped);
int num_instructions(Seq_T memory);

#endif