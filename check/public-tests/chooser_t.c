/*
 * FCML - Free Code Manipulation Library.
 * Copyright (C) 2010-2020 Slawomir Wojtasiak
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "chooser_t.h"

#include <stdio.h>
#include <string.h>

#include <fcml_assembler.h>
#include <fcml_choosers.h>
#include <fcml_common.h>
#include <fcml_common_utils.h>
#include <fcml_disassembler.h>
#include <fcml_optimizers.h>
#include <fcml_renderer.h>
#include <fcml_types.h>

#include "instructions_base_t.h"

fcml_bool fcml_tf_chooser_suite_init(void) {
	return FCML_TRUE;
}

fcml_bool fcml_tf_chooser_suite_cleanup(void) {
	return FCML_TRUE;
}

void fcml_fn_chooser_default(void) {

	/* Instruction "adc ax,8042h" can be assembled in two ways.*/
	/* 1: 0x66, 0x15, 0x42, 0x80*/
	/* 2: 0x66, 0x81, 0xd0, 0x42, 0x80*/
	/* Default instruction chooser is responsible for choosing the shortest form.*/

	fcml_st_assembler_context context = {0};
	context.assembler = assembler_intel;
	context.entry_point.op_mode = FCML_OM_32_BIT;
	context.entry_point.ip = 0x00401000;

	fcml_st_instruction instruction = {0};
	instruction.mnemonic = "adc";
	instruction.operands[0] = FCML_REG( fcml_reg_AX );
	instruction.operands[1] = FCML_IMM16( 0x8042 );

	fcml_st_assembler_result result;

	fcml_fn_assembler_result_prepare( &result );

	if( !fcml_fn_assemble( &context, &instruction, &result ) ) {
		STF_ASSERT_PTR_NOT_NULL( result.chosen_instruction );
		if( result.chosen_instruction ) {
			STF_ASSERT_EQUAL( 4, result.chosen_instruction->code_length );
			if( result.chosen_instruction->code_length == 4 ) {
				STF_ASSERT_EQUAL( 0x66, result.chosen_instruction->code[0] );
				STF_ASSERT_EQUAL( 0x15, result.chosen_instruction->code[1] );
				STF_ASSERT_EQUAL( 0x42, result.chosen_instruction->code[2] );
				STF_ASSERT_EQUAL( 0x80, result.chosen_instruction->code[3] );
			}
		}
		fcml_fn_assembler_result_free( &result );
	} else {
		STF_FAIL("Can not assemble instruction.");
	}

}

void fcml_fn_chooser_null_optimizer_all_forms(void) {

    /* Instruction "adc ax,8042h" can be assembled in two ways.*/
    /* 1: 0x66, 0x15, 0x42, 0x80*/
    /* 2: 0x66, 0x81, 0xd0, 0x42, 0x80*/
    /* Null instruction chooser is responsible for choosing them all. Optimizer should also try all allowed ASA/OSA combinations.*/

    fcml_ceh_error error;

    fcml_st_assembler_context context = {0};
    context.configuration.optimizer_flags = FCML_OPTF_ALL_FORMS;
    context.configuration.chooser = &fcml_fn_asm_no_instruction_chooser;
    context.assembler = assembler_intel;
    context.entry_point.op_mode = FCML_OM_32_BIT;
    context.entry_point.ip = 0x00401000;

    fcml_st_instruction instruction = {0};
    instruction.mnemonic = "adc";
    instruction.operands[0] = FCML_REG( fcml_reg_AX );
    instruction.operands[1] = FCML_IMM16( 0x8042 );

    fcml_st_assembler_result result;

    fcml_fn_assembler_result_prepare( &result );

    if( !fcml_fn_assemble( &context, &instruction, &result ) ) {

        STF_ASSERT_PTR_NULL( result.chosen_instruction );

        /* Disassemble and render every instruction available.*/

        fcml_st_disassembler_context d_context = {0};
        d_context.entry_point.op_mode = FCML_OM_32_BIT;
        d_context.entry_point.ip = 0x00401000;
        d_context.disassembler = disassembler_intel;

        fcml_int flags = 0;

        /* Iterate over all instructions.*/

        fcml_st_assembled_instruction *instruction = result.instructions;

        while( instruction ) {

            /* Disassemble instructions one by one.*/

            d_context.code = instruction->code;
            d_context.code_length = instruction->code_length;

            fcml_st_disassembler_result dasm_result;

            fcml_fn_disassembler_result_prepare( &dasm_result );

            error = fcml_fn_disassemble( &d_context, &dasm_result );
            if( error ) {
                break;
            }

            /* Render instructions one by one.*/

            fcml_char buffer[FCML_REND_MAX_BUFF_LEN];

            fcml_st_render_config config = {0};
            config.render_flags = FCML_REND_FLAG_RENDER_CODE;

            error = fcml_fn_render( dialect_intel, &config, buffer, sizeof( buffer ), &dasm_result );
            if( error ) {
                /* Free disassembling result.*/
                fcml_fn_disassembler_result_free( &dasm_result );
                break;
            }

            fcml_fn_disassembler_result_free( &dasm_result );

            if( strcmp( FCML_TEXT( "666781d04280 adc ax,-32702" ), buffer ) == 0 ) {
                flags |= 0x01;
            }

            if( strcmp( FCML_TEXT( "6681d04280 adc ax,-32702" ), buffer ) == 0 ) {
                flags |= 0x02;
            }

            if( strcmp( FCML_TEXT( "6667154280 adc ax,-32702" ), buffer ) == 0 ) {
                flags |= 0x04;
            }

            if( strcmp( FCML_TEXT( "66154280 adc ax,-32702" ), buffer ) == 0) {
                flags |= 0x08;
            }

            instruction = instruction->next;
        }

        fcml_fn_assembler_result_free( &result );

        STF_ASSERT_EQUAL( error, 0 );
        STF_ASSERT_EQUAL( flags, 0x0000000F );

    } else {
        STF_FAIL("Can not assemble instruction.");
    }

}

fcml_stf_test_case fcml_ti_chooser[] = {
	{ "fcml_fn_chooser_default", fcml_fn_chooser_default },
	{ "fcml_fn_chooser_null_optimizer_all_forms", fcml_fn_chooser_null_optimizer_all_forms },
	FCML_STF_NULL_TEST
};

fcml_stf_test_suite fcml_si_chooser = {
	"suite-fcml-chooser", fcml_tf_chooser_suite_init, fcml_tf_chooser_suite_cleanup, fcml_ti_chooser
};

