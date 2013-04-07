/*
 * ira_def.h
 *
 *  Created on: 27-12-2012
 *      Author: tAs
 */

#ifndef IRA_DEF_H_
#define IRA_DEF_H_

#include "fcml_types.h"
#include "fcml_common.h"

#define FCML_OS_EOSA		0xFFFF
#define FCML_OS_EASA		0xFFFE

/* Constants used to encode operand size on one byte. Used only in instruction descriptions. */

// Take into account that every size is given in a number of bytes.
#define FCML_EOS_UNDEFINED	0
#define FCML_EOS_512B		FCML_EOS_UNDEFINED
#define FCML_EOS_BYTE		1
#define FCML_EOS_WORD		2
#define FCML_EOS_DWORD		4
#define FCML_EOS_FWORD		6
#define FCML_EOS_QWORD		8
#define FCML_EOS_TBYTE		10
#define FCML_EOS_OWORD		16
#define FCML_EOS_YWORD		32

// Dynamic encoded operand sizes calculated at runtime.

// Operand size calculated by Effective Operand Size Attribute and Effective Address Size Attribute.
#define FCML_EOS_EOSA		0xFF
#define FCML_EOS_EASA		0xFE

// Operand size calculated on L field.
#define FCML_EOS_L			0xFD

// Oprand sizes that cannot be simply written as number of bytes.
#define FCML_EOS_14_28		0xFC
#define FCML_EOS_94_108		0xFB
#define FCML_EOS_32_64		0xFA

// True if encoded operand size is a dynamic one.
#define FCML_IS_EOS_DYNAMIC(x)	( ( x & 0x80 ) != 0 )

/* Structures used to describe instructions with they all allowed addressing modes. */
typedef struct fcml_st_def_addr_mode_desc {
	// Mnemonic, if there is another mnemonic available for this opcode.
	fcml_string mnemonic_override;
	// Flags describing allowed prefixes.
	fcml_uint16_t allowed_prefixes;
	// Some flags that contains various information about opcode.
	fcml_uint32_t opcode_flags;
	// Opcode bytes.
	fcml_uint8_t opcode[3];
	// Addressing of instruction operands.
	fcml_uint32_t opperand_1;
	fcml_uint32_t opperand_2;
	fcml_uint32_t opperand_3;
	fcml_uint32_t opperand_4;
} fcml_st_def_addr_mode_desc;

/* Describes one addressing mode of instruction. */
typedef struct fcml_st_def_instruction_description {
	// Mnemonic.
	fcml_string mnemonic;
	// Type of the instruction.
	fcml_uint8_t instruction_type;
	// Number of opcodes' descriptions.
	fcml_uint8_t opcode_desc_count;
	// Opcodes' descriptions.
	struct fcml_st_def_addr_mode_desc *addr_modes;
} fcml_st_def_instruction_description;

/* Operands encoding */

#define FCML_IA_INSTRUCTION(x,y) { x, FCML_IT_IA, ( sizeof( y ) / sizeof( struct fcml_st_def_addr_mode_desc ) ), y }

/*********************************
 * Addressing modes.
 *********************************/

/* Instruction types. */

// Intel IA.
#define FCML_IT_IA			0x00

// Operand type (source/destination).
#define FCML_OA_R		0x80000000
#define FCML_OA_W		0x40000000
#define FCML_OA_RW		( FCML_OA_READ | FCML_OA_W )

#define _IRA_NA		0x00000000

// Immediate data.
#define FCML_OP_IMM_BASE							0x01000000
#define FCML_OP_IMM(encoded_imm_size, encoded_ex_imm_size)			( FCML_OP_IMM_BASE | encoded_imm_size << 8 | encoded_ex_imm_size )

// Register explicitly set.
#define FCML_OP_EXPLICIT_REG_BASE					0x02000000
#define FCML_OP_EXPLICIT_REG(reg_type, reg_num, encoded_reg_size)	( FCML_OP_EXPLICIT_REG_BASE | reg_type << 12 | reg_num << 8 | encoded_reg_size )

// Register field in opcode byte.
#define FCML_OP_OPCODE_REG_BASE						0x03000000
#define FCML_OP_OPCODE_REG(reg_type, encoded_reg_size)		( FCML_OP_OPCODE_REG_BASE | reg_type << 8 | encoded_reg_size )

// Relative addressing.
#define FCML_OP_IMMEDIATE_DIS_RELATIVE_BASE			0x04000000
#define FCML_OP_IMMEDIATE_DIS_RELATIVE( encoded_imm_size )	( FCML_OP_IMMEDIATE_DIS_RELATIVE_BASE | encoded_imm_size )

// Far pointers.
#define FCML_OP_FAR_POINTER							0x05000000

// Far indirect pointer.
#define FCML_OP_FAR_POINTER_INDIRECT				0x06000000

// Addressing by explicit GPR register. (Used by CMPS for instance.)
#define FCML_OP_EXPLICIT_GPS_REG_ADDRESSING_BASE	0x07000000
#define FCML_OP_EXPLICIT_GPS_REG_ADDRESSING(reg_num, encoded_operand_size, encoded_segment_register)	( FCML_OP_EXPLICIT_GPS_REG_ADDRESSING_BASE | reg_num << 16 | encoded_operand_size << 8 | encoded_segment_register )

// It allows defining explicit IMM8 operand type. See INT instruction.
#define FCML_OP_EXPLICIT_IB_BASE					0x08000000
#define FCML_OP_EXPLICIT_IB(value)				( FCML_OP_EXPLICIT_IB_BASE | value )

// Segment relative addressing.
#define FCML_OP_SEGMENT_RELATIVE_OFFSET_BASE		0x09000000
#define FCML_OP_SEGMENT_RELATIVE_OFFSET( operand_size, encoded_segment_register )	( FCML_OP_SEGMENT_RELATIVE_OFFSET_BASE | operand_size << 8 | encoded_segment_register )

/********************************/
/*      ModR/M encoding.        */
/********************************/

// Allows to encode all common ModR/M based addressing modes using only one macro.

#define _IRA_RMF_R		0x01
#define _IRA_RMF_M		0x02
#define _IRA_RMF_RM		( _IRA_RMF_R | _IRA_RMF_M )

#define FCML_OP_RM_BASE								0x0A000000
#define FCML_OP_RM(reg_type, encoded_register_operand_size, encoded_memory_operand_size, flags )		( FCML_OP_RM_BASE | encoded_memory_operand_size << 16 | encoded_register_operand_size << 8 | reg_type << 4 | flags )
#define FCML_OP_RM_W(reg_type, encoded_register_operand_size, encoded_memory_operand_size, flags )		( FCML_OP_RM(reg_type, encoded_register_operand_size, encoded_memory_operand_size, flags) | FCML_OA_W )

#define FCML_OP_R_BASE								0x0B000000
#define FCML_OP_R( reg_type, encoded_register_operand_size )	( FCML_OP_R_BASE | ( encoded_register_operand_size << 4 ) | reg_type )

/******************************/
/* XOP/VEX specific encoding. */
/******************************/

#define FCML_OP_VEX_VVVV_REG_BASE						0x0C000000
#define FCML_OP_VEX_VVVV_REG( reg_type, encoded_register_size )	( FCML_OP_VEX_VVVV_REG_BASE | ( encoded_register_size << 4 ) | reg_type )

#define FCML_OP_OPERAND_IS4							0x0D000000

/**************************/
/* VSIB operand decoding. */
/**************************/

// Vector index register.
#define FCML_VSIB_XMM	0x01
#define FCML_VSIB_YMM	0x02

// Index value size.
#define FCML_VSIB_IS_32	0x01
#define FCML_VSIB_IS_64	0x02

#define FCML_OP_VSIB_BASE						0x0E000000
#define FCML_OP_VSIB( vector_index_register, index_value_size )		( FCML_OP_VSIB_BASE | vector_index_register << 2 | index_value_size )

/*******************************/
/* Segment registers encoding. */
/*******************************/

#define FCML_SEG_ALLOW_OVERRIDE								0x80
#define FCML_SEG_DENY_OVERRIDE								0x00
#define FCML_SEG_ENCODE_REGISTER( reg_num, override )		( reg_num | override )
#define FCML_SEG_DECODE_IS_OVERRIDE_ALLOWED( encoded )		( FCML_SEG_ALLOW_OVERRIDE & encoded )
#define FCML_SEG_DECODE_REGISTER( encoded )					( encoded & ~FCML_SEG_ALLOW_OVERRIDE )

// Shorthands

#define _IRA_OPERAND_MODRM_RM_8				FCML_OP_RM(IRA_REG_GPR, FCML_EOS_BYTE, FCML_EOS_BYTE, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_8_W			( _IRA_OPERAND_MODRM_RM_8 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_16			FCML_OP_RM(IRA_REG_GPR, FCML_EOS_WORD, FCML_EOS_WORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_16_W			( _IRA_OPERAND_MODRM_RM_16 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_32			FCML_OP_RM(IRA_REG_GPR, FCML_EOS_DWORD, FCML_EOS_DWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_32_W			( _IRA_OPERAND_MODRM_RM_32 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_64			FCML_OP_RM(IRA_REG_GPR, FCML_EOS_QWORD, FCML_EOS_QWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_64_W			( _IRA_OPERAND_MODRM_RM_64 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM				FCML_OP_RM(IRA_REG_GPR, FCML_EOS_EOSA, FCML_EOS_EOSA, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_W				( _IRA_OPERAND_MODRM_RM | FCML_OA_W )
#define _IRA_OPERAND_MODRM_R_8				FCML_OP_R(IRA_REG_GPR, FCML_EOS_BYTE )
#define _IRA_OPERAND_MODRM_R_8_W			( _IRA_OPERAND_MODRM_R_8 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_R_16				FCML_OP_R(IRA_REG_GPR, FCML_EOS_WORD )
#define _IRA_OPERAND_MODRM_R_16_W			( _IRA_OPERAND_MODRM_R_16 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_R_32				FCML_OP_R(IRA_REG_GPR, FCML_EOS_DWORD )
#define _IRA_OPERAND_MODRM_R_32_W			( _IRA_OPERAND_MODRM_R_32 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_R_64				FCML_OP_R(IRA_REG_GPR, FCML_EOS_QWORD )
#define _IRA_OPERAND_MODRM_R_64_W			( _IRA_OPERAND_MODRM_R_64 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_R				FCML_OP_R(IRA_REG_GPR, FCML_EOS_EOSA )
#define _IRA_OPERAND_MODRM_R_W				( _IRA_OPERAND_MODRM_R | FCML_OA_W )
#define _IRA_OPERAND_MODRM_M_8				FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_BYTE, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_8_W			(_IRA_OPERAND_MODRM_M_8 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_16				FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_WORD, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_16_W			(_IRA_OPERAND_MODRM_M_16 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_32				FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_DWORD, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_32_W			(_IRA_OPERAND_MODRM_M_32 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_64				FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_QWORD, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_64_W			(_IRA_OPERAND_MODRM_M_64 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_80				FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_TBYTE, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_80_W			(_IRA_OPERAND_MODRM_M_80 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_128			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_OWORD, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_128_W			(_IRA_OPERAND_MODRM_M_128 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_256			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_YWORD, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_256_W			(_IRA_OPERAND_MODRM_M_256 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_14_28			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_14_28, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_14_28_W		(_IRA_OPERAND_MODRM_M_14_28 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_94_108			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_94_108, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_94_108_W		(_IRA_OPERAND_MODRM_M_94_108 | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_512B			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_512B, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_512B_W			(_IRA_OPERAND_MODRM_M_512B | FCML_OA_W)
#define _IRA_OPERAND_MODRM_M_UNDEF			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_UNDEFINED, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_UNDEF_W		(_IRA_OPERAND_MODRM_M_UNDEF | FCML_OA_W)

#define _IRA_OPERAND_MODRM_MM_OSA			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_32_64, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_RM_MMX			FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_QWORD, FCML_EOS_QWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_MMX_W			( _IRA_OPERAND_MODRM_RM_MMX | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_MMX_32		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_QWORD, FCML_EOS_DWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_MMX_32_W		( _IRA_OPERAND_MODRM_RM_MMX_32 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_R_MMX			FCML_OP_R(IRA_REG_SIMD, FCML_EOS_QWORD )
#define _IRA_OPERAND_MODRM_R_MMX_W			( _IRA_OPERAND_MODRM_R_MMX | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_YMM_256		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_YWORD, FCML_EOS_YWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_YMM_256_W		( _IRA_OPERAND_MODRM_RM_YMM_256 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_XMM_128		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_OWORD, FCML_EOS_OWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_XMM_128_W		( _IRA_OPERAND_MODRM_RM_XMM_128 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_XMM_64		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_OWORD, FCML_EOS_QWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_XMM_64_W		( _IRA_OPERAND_MODRM_RM_XMM_64 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_XMM_32		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_OWORD, FCML_EOS_DWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_XMM_32_W		( _IRA_OPERAND_MODRM_RM_XMM_32 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_XMM_16		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_OWORD, FCML_EOS_WORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_XMM_16_W		( _IRA_OPERAND_MODRM_RM_XMM_16 | FCML_OA_W )

#define _IRA_OPERAND_MODRM_R_XMM			FCML_OP_R(IRA_REG_SIMD, FCML_EOS_OWORD )
#define _IRA_OPERAND_MODRM_R_XMM_W			( _IRA_OPERAND_MODRM_R_XMM | FCML_OA_W )
#define _IRA_OPERAND_MODRM_R_YMM			FCML_OP_R(IRA_REG_SIMD, FCML_EOS_YWORD )
#define _IRA_OPERAND_MODRM_R_YMM_W			( _IRA_OPERAND_MODRM_R_YMM | FCML_OA_W )

#define _IRA_OPERAND_MODRM_RM_SIMD_L		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_L, FCML_EOS_L, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_W		( _IRA_OPERAND_MODRM_RM_SIMD_L | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_128	FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_L, FCML_EOS_OWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_128_W	( _IRA_OPERAND_MODRM_RM_SIMD_L_128 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_64		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_L, FCML_EOS_QWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_64_W	( _IRA_OPERAND_MODRM_RM_SIMD_L_64 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_32		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_L, FCML_EOS_DWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_32_W	( _IRA_OPERAND_MODRM_RM_SIMD_L_32 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_16		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_L, FCML_EOS_WORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_16_W	( _IRA_OPERAND_MODRM_RM_SIMD_L_16 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_8		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_L, FCML_EOS_BYTE, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_L_8_W	( _IRA_OPERAND_MODRM_RM_SIMD_L_8 | FCML_OA_W )

#define _IRA_OPERAND_MODRM_RM_SIMD_E		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_EOSA, FCML_EOS_EOSA, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_W		( _IRA_OPERAND_MODRM_RM_SIMD_E | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_128	FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_EOSA, FCML_EOS_OWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_128_W	( _IRA_OPERAND_MODRM_RM_SIMD_E_128 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_64		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_EOSA, FCML_EOS_QWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_64_W	( _IRA_OPERAND_MODRM_RM_SIMD_E_64 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_32		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_EOSA, FCML_EOS_DWORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_32_W	( _IRA_OPERAND_MODRM_RM_SIMD_E_32 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_16		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_EOSA, FCML_EOS_WORD, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_16_W	( _IRA_OPERAND_MODRM_RM_SIMD_E_16 | FCML_OA_W )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_8		FCML_OP_RM(IRA_REG_SIMD, FCML_EOS_EOSA, FCML_EOS_BYTE, _IRA_RMF_RM )
#define _IRA_OPERAND_MODRM_RM_SIMD_E_8_W	( _IRA_OPERAND_MODRM_RM_SIMD_E_8 | FCML_OA_W )

#define _IRA_OPERAND_MODRM_R_SIMD_L			FCML_OP_R(IRA_REG_SIMD, FCML_EOS_L )
#define _IRA_OPERAND_MODRM_R_SIMD_L_W		( _IRA_OPERAND_MODRM_R_SIMD_L | FCML_OA_W )
#define _IRA_OPERAND_MODRM_R_SIMD_E			FCML_OP_R(IRA_REG_SIMD, FCML_EOS_EOSA )
#define _IRA_OPERAND_MODRM_R_SIMD_E_W		( _IRA_OPERAND_MODRM_R_SIMD_E | FCML_OA_W )
#define _IRA_OPERAND_MODRM_M_SIMD_L			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_L, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_SIMD_L_W		( _IRA_OPERAND_MODRM_M_SIMD_L | FCML_OA_W )
#define _IRA_OPERAND_MODRM_M_SIMD_E			FCML_OP_RM(IRA_NO_REG, FCML_EOS_UNDEFINED, FCML_EOS_EOSA, _IRA_RMF_M )
#define _IRA_OPERAND_MODRM_M_SIMD_E_W		( _IRA_OPERAND_MODRM_M_SIMD | FCML_OA_W )

// Shorthands for relative addressing.

#define _IRA_OPERAND_IMMEDIATE_DIS_RELATIVE_EOSA	FCML_OP_IMMEDIATE_DIS_RELATIVE( FCML_EOS_UNDEFINED )
#define _IRA_OPERAND_IMMEDIATE_DIS_RELATIVE_R_8		FCML_OP_IMMEDIATE_DIS_RELATIVE( FCML_EOS_BYTE )

// Shorthands for VVVV addressing.

#define _IRA_VEX_VVVV_SIMD_REG		FCML_OP_VEX_VVVV_REG( IRA_REG_SIMD, FCML_EOS_L )
#define _IRA_VEX_VVVV_XMM_REG		FCML_OP_VEX_VVVV_REG( IRA_REG_SIMD, FCML_EOS_OWORD )
#define _IRA_VEX_VVVV_YMM_REG		FCML_OP_VEX_VVVV_REG( IRA_REG_SIMD, FCML_EOS_YWORD )

// Shorthands for IMM.

#define _IRA_OPERAND_IB						FCML_OP_IMM( FCML_EOS_BYTE, FCML_EOS_UNDEFINED )
#define _IRA_OPERAND_IB_EX_EOSA				FCML_OP_IMM( FCML_EOS_BYTE, FCML_EOS_EOSA )
#define _IRA_OPERAND_IW						FCML_OP_IMM( FCML_EOS_WORD, FCML_EOS_UNDEFINED )
#define _IRA_OPERAND_IW_EX_EOSA				FCML_OP_IMM( FCML_EOS_WORD, FCML_EOS_EOSA )
#define _IRA_OPERAND_ID						FCML_OP_IMM( FCML_EOS_DWORD, FCML_EOS_UNDEFINED )
#define _IRA_OPERAND_ID_EX_EOSA				FCML_OP_IMM( FCML_EOS_DWORD, FCML_EOS_EOSA )
#define _IRA_OPERAND_IMM_EOSA				FCML_OP_IMM( FCML_EOS_EOSA, FCML_EOS_UNDEFINED )

/* Externals. */

extern struct fcml_st_def_instruction_description fcml_ext_instructions_def[];

#define _IRA_EMPTY_MNEMONIC	""

/* Functions. */

/* Methods. */

//! Decoding given encoded operand size.
/*!
 * \brief Decodes given operand size using provided EOSA. Some functions have operand size described on 8 bits using
 * IRA_EOS_ constants. This form of operand size description notation is used to allow encoding more than 128
 * bits operand sizes using only one byte.
 *
 * \param encoded_operand_size Size directive to decode.
 * \return Decoded operand size.
 */
uint16_t _ira_common_decode_8b_operand_size( uint8_t encoded_operand_size );

uint16_t fcml_fn_def_decode_operand_size( uint8_t encoded_operand_size, uint8_t effective_operand_size_attribute, uint8_t effective_address_size_attribute, fcml_bool l_prefix );


#endif /* IRA_DEF_H_ */