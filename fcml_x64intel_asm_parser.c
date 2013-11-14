/*
 * fcml_x64intel_asm_parser.c
 *
 *  Created on: 20-03-2013
 *      Author: tAs
 */

#include <string.h>
#include "fcml_ceh.h"
#include "fcml_env.h"
#include "fcml_common.h"
#include "fcml_errors.h"
#include "fcml_parser_data.h"
#include "fcml_x64intel_parser.tab.h"
#include "fcml_x64intel_lexer.h"
#include "fcml_x64intel_asm_parser.h"

void yyerror( struct fcml_st_parser_data *pd, const char *error ) {
	// Stores parser error into standard container.
	fcml_fn_ceh_add_error( &(pd->errors), (const fcml_string)error, FCML_EN_X64IP_ERROR_INVALID_SYNTAX, FCML_EN_CEH_EL_ERROR );
}

fcml_ceh_error fcml_x64iap_parse( fcml_string asm_code, fcml_st_x64iap_parser_result **result ) {

	fcml_ceh_error error = FCML_CEH_GEC_NO_ERROR;

	*result = NULL;

	fcml_st_parser_data parser = {0};

	/* Set up scanner. */
	if ( yylex_init_extra( &parser, &(parser.scannerInfo) ) ) {
		return FCML_CEH_GEC_OUT_OF_MEMORY;
	}

	/*Instruction size is limited to prevent from parser's stack and buffer overflow.*/
	if( strlen( asm_code ) > FCML_X64IAP_MAX_INSTRUCTION_LEN ) {
		yylex_destroy( parser.scannerInfo );
		return FCML_CEH_GEC_INVALID_INPUT;
	}

	yy_scan_string( asm_code, parser.scannerInfo );

	fcml_st_x64iap_parser_result *tmp_result = (fcml_st_x64iap_parser_result*)fcml_fn_env_memory_alloc_clear( sizeof( fcml_st_x64iap_parser_result ) );
	if( tmp_result == NULL ) {
		return FCML_CEH_GEC_OUT_OF_MEMORY;
	}

	memset( tmp_result, 0, sizeof( fcml_st_x64iap_parser_result ) );

	int yyresult = yyparse(&parser);

	yylex_destroy( parser.scannerInfo );

	// Copy errors from parser.
	tmp_result->errors = parser.errors;

	if( yyresult ) {
		*result = tmp_result;
		switch( yyresult ) {
		case 1: /*Syntax error.*/
			error = FCML_CEH_GEC_INVALID_INPUT;
			break;
		case 2: /*Out of memory*/
			error = FCML_CEH_GEC_OUT_OF_MEMORY;
			break;
		}
		return error;
	}

	error = fcml_fn_ast_to_cif_converter( parser.tree, &(tmp_result->errors), &(tmp_result->instruction) );
	if( error ) {
		// Free instruction, because it might haven't been fully parsed.
		fcml_fn_ast_free_converted_cif( tmp_result->instruction );
		tmp_result->instruction = NULL;
	}

	fcml_fn_ast_free_node( parser.tree );

	*result = tmp_result;

	return error;
}

void fcml_x64iap_free( fcml_st_x64iap_parser_result *result ) {
	if( result ) {
		// Frees parsed instruction, potential errors and warnings and result structure itself.
		if( result->instruction ) {
			fcml_fn_ast_free_converted_cif( result->instruction );
		}
		fcml_fn_ceh_free_errors_only( &(result->errors) );
		fcml_fn_env_memory_free( result );
	}
}
