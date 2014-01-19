/*
 * fcml_assembler.h
 *
 *  Created on: 08-02-2013
 *      Author: tAs
 */

#ifndef FCML_ASSEMBLER_H_
#define FCML_ASSEMBLER_H_

#include "fcml_ceh.h"
#include "fcml_coll.h"
#include "fcml_common.h"
#include "fcml_dialect.h"
#include "fcml_types.h"
#include "fcml_optimizers.h"
#include "fcml_choosers.h"

/* This structure and type definition represents an abstract assembler. */
typedef struct fcml_st_asm_assembler {
} fcml_st_asm_assembler;

typedef struct fcml_st_asm_assembler_configuration {
    // Generic assembler configuration flags.
    fcml_bool choose_sib_encoding;
    fcml_bool choose_abs_encoding;
    fcml_bool force_rex_prefix;
    fcml_bool force_three_byte_VEX;
    // Optimizers.
    fcml_fnp_asm_optimizer optimizer;
    fcml_uint16_t optimizer_flags;
    // Choosers.
    fcml_fnp_asm_instruction_chooser chooser;
} fcml_st_asm_assembler_configuration;

typedef struct fcml_st_asm_assembled_instruction {
    // Next assembled instruction in the chain.
    struct fcml_st_asm_assembled_instruction *next;
#ifdef FCML_DEBUG
	// Index of addressing mode used to assemble instruction.
	fcml_uint8_t __def_index;
#endif
	// Error and warning messages related to assembled instruction.
	fcml_st_ceh_error_container errors;
	// Instruction machine code.
	fcml_uint8_t *code;
	// Instruction code length in bytes.
	fcml_usize code_length;
} fcml_st_asm_assembled_instruction;

typedef struct fcml_st_asm_assembler_result {
	fcml_st_ceh_error_container errors;
	// List of assembled instructions.
	fcml_st_asm_assembled_instruction *instructions;
	// Instruction chosen by used instruction chooser; otherwise null.
	fcml_st_asm_assembled_instruction *chosen_instruction;
	// Number of assembled instructions.
	fcml_usize number_of_instructions;
} fcml_st_asm_assembler_result;

typedef struct fcml_st_asm_assembler_context {
    // Assembler instance that should be used to assemble instructions.
    fcml_st_asm_assembler *assembler;
    // Assembler behavior can be configured here.
	fcml_st_asm_assembler_configuration configuration;
	// Processor addressing mode 16/32/64-bit.
	fcml_en_addr_form addr_form;
	// Default address/operand size attribute (See 'D' flag of segment descriptor.)
	fcml_data_size address_size_attribute;
	fcml_data_size operand_size_attribute;
	// Instruction pointer used for relative addressing address calculation.
	fcml_st_instruction_pointer ip;
} fcml_st_asm_assembler_context;

fcml_ceh_error fcml_fn_assembler_init( fcml_st_dialect *context, fcml_st_asm_assembler **assembler );
fcml_ceh_error fcml_fn_assemble( fcml_st_asm_assembler_context *context, const fcml_st_instruction *instruction, fcml_st_asm_assembler_result **result );
void fcml_fn_assembler_result_free( fcml_st_asm_assembler_result *result );
void fcml_fn_assembler_free( fcml_st_asm_assembler *assembler );

#endif /* FCML_ASSEMBLER_H_ */