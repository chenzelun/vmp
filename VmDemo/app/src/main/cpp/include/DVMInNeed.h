//
// Created by ChenD on 2019/6/25.
//

#ifndef SHELLAPPLICATION_DVMINNEED_H
#define SHELLAPPLICATION_DVMINNEED_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <jni.h>
#include <vector>
#include <string>


#include <jni.h>
#include <android/log.h>
#include <cstring>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,__FUNCTION__,__VA_ARGS__) // 定义LOGD类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,__FUNCTION__,__VA_ARGS__) // 定义LOGE类型

/*
 * kMaxOpcodeValue: the highest possible raw (unpacked) opcode value
 *
 * kNumPackedOpcodes: the highest possible packed opcode value of a
 * valid Dalvik opcode, plus one
 *
 * TODO: Change this once the rest of the code is prepared to deal with
 * extended opcodes.
 */
// BEGIN(libdex-maximum-values); GENERATED AUTOMATICALLY BY opcode-gen
#define kMaxOpcodeValue 0xffff
#define kNumPackedOpcodes 0x100
// END(libdex-maximum-values); GENERATED AUTOMATICALLY BY opcode-gen

/*
 * Switch table and array data signatures are a code unit consisting
 * of "NOP" (0x00) in the low-order byte and a non-zero identifying
 * code in the high-order byte. (A true NOP is 0x0000.)
 */
#define kPackedSwitchSignature  0x0100
#define kSparseSwitchSignature  0x0200
#define kArrayDataSignature     0x0300

/*
 * Enumeration of all Dalvik opcodes, where the enumeration value
 * associated with each is the corresponding packed opcode number.
 * This is different than the opcode value from the Dalvik bytecode
 * spec for opcode values >= 0xff; see dexOpcodeFromCodeUnit() below.
 *
 * A note about the "breakpoint" opcode. This instruction is special,
 * in that it should never be seen by anything but the debug
 * interpreter. During debugging it takes the place of an arbitrary
 * opcode, which means operations like "tell me the opcode width so I
 * can find the next instruction" aren't possible. (This is
 * correctable, but probably not useful.)
 */
enum Opcode {
    // BEGIN(libdex-opcode-enum); GENERATED AUTOMATICALLY BY opcode-gen
            OP_NOP = 0x00,
    OP_MOVE = 0x01,
    OP_MOVE_FROM16 = 0x02,
    OP_MOVE_16 = 0x03,
    OP_MOVE_WIDE = 0x04,
    OP_MOVE_WIDE_FROM16 = 0x05,
    OP_MOVE_WIDE_16 = 0x06,
    OP_MOVE_OBJECT = 0x07,
    OP_MOVE_OBJECT_FROM16 = 0x08,
    OP_MOVE_OBJECT_16 = 0x09,
    OP_MOVE_RESULT = 0x0a,
    OP_MOVE_RESULT_WIDE = 0x0b,
    OP_MOVE_RESULT_OBJECT = 0x0c,
    OP_MOVE_EXCEPTION = 0x0d,
    OP_RETURN_VOID = 0x0e,
    OP_RETURN = 0x0f,
    OP_RETURN_WIDE = 0x10,
    OP_RETURN_OBJECT = 0x11,
    OP_CONST_4 = 0x12,
    OP_CONST_16 = 0x13,
    OP_CONST = 0x14,
    OP_CONST_HIGH16 = 0x15,
    OP_CONST_WIDE_16 = 0x16,
    OP_CONST_WIDE_32 = 0x17,
    OP_CONST_WIDE = 0x18,
    OP_CONST_WIDE_HIGH16 = 0x19,
    OP_CONST_STRING = 0x1a,
    OP_CONST_STRING_JUMBO = 0x1b,
    OP_CONST_CLASS = 0x1c,
    OP_MONITOR_ENTER = 0x1d,
    OP_MONITOR_EXIT = 0x1e,
    OP_CHECK_CAST = 0x1f,
    OP_INSTANCE_OF = 0x20,
    OP_ARRAY_LENGTH = 0x21,
    OP_NEW_INSTANCE = 0x22,
    OP_NEW_ARRAY = 0x23,
    OP_FILLED_NEW_ARRAY = 0x24,
    OP_FILLED_NEW_ARRAY_RANGE = 0x25,
    OP_FILL_ARRAY_DATA = 0x26,
    OP_THROW = 0x27,
    OP_GOTO = 0x28,
    OP_GOTO_16 = 0x29,
    OP_GOTO_32 = 0x2a,
    OP_PACKED_SWITCH = 0x2b,
    OP_SPARSE_SWITCH = 0x2c,
    OP_CMPL_FLOAT = 0x2d,
    OP_CMPG_FLOAT = 0x2e,
    OP_CMPL_DOUBLE = 0x2f,
    OP_CMPG_DOUBLE = 0x30,
    OP_CMP_LONG = 0x31,
    OP_IF_EQ = 0x32,
    OP_IF_NE = 0x33,
    OP_IF_LT = 0x34,
    OP_IF_GE = 0x35,
    OP_IF_GT = 0x36,
    OP_IF_LE = 0x37,
    OP_IF_EQZ = 0x38,
    OP_IF_NEZ = 0x39,
    OP_IF_LTZ = 0x3a,
    OP_IF_GEZ = 0x3b,
    OP_IF_GTZ = 0x3c,
    OP_IF_LEZ = 0x3d,
    OP_UNUSED_3E = 0x3e,
    OP_UNUSED_3F = 0x3f,
    OP_UNUSED_40 = 0x40,
    OP_UNUSED_41 = 0x41,
    OP_UNUSED_42 = 0x42,
    OP_UNUSED_43 = 0x43,
    OP_AGET = 0x44,
    OP_AGET_WIDE = 0x45,
    OP_AGET_OBJECT = 0x46,
    OP_AGET_BOOLEAN = 0x47,
    OP_AGET_BYTE = 0x48,
    OP_AGET_CHAR = 0x49,
    OP_AGET_SHORT = 0x4a,
    OP_APUT = 0x4b,
    OP_APUT_WIDE = 0x4c,
    OP_APUT_OBJECT = 0x4d,
    OP_APUT_BOOLEAN = 0x4e,
    OP_APUT_BYTE = 0x4f,
    OP_APUT_CHAR = 0x50,
    OP_APUT_SHORT = 0x51,
    OP_IGET = 0x52,
    OP_IGET_WIDE = 0x53,
    OP_IGET_OBJECT = 0x54,
    OP_IGET_BOOLEAN = 0x55,
    OP_IGET_BYTE = 0x56,
    OP_IGET_CHAR = 0x57,
    OP_IGET_SHORT = 0x58,
    OP_IPUT = 0x59,
    OP_IPUT_WIDE = 0x5a,
    OP_IPUT_OBJECT = 0x5b,
    OP_IPUT_BOOLEAN = 0x5c,
    OP_IPUT_BYTE = 0x5d,
    OP_IPUT_CHAR = 0x5e,
    OP_IPUT_SHORT = 0x5f,
    OP_SGET = 0x60,
    OP_SGET_WIDE = 0x61,
    OP_SGET_OBJECT = 0x62,
    OP_SGET_BOOLEAN = 0x63,
    OP_SGET_BYTE = 0x64,
    OP_SGET_CHAR = 0x65,
    OP_SGET_SHORT = 0x66,
    OP_SPUT = 0x67,
    OP_SPUT_WIDE = 0x68,
    OP_SPUT_OBJECT = 0x69,
    OP_SPUT_BOOLEAN = 0x6a,
    OP_SPUT_BYTE = 0x6b,
    OP_SPUT_CHAR = 0x6c,
    OP_SPUT_SHORT = 0x6d,
    OP_INVOKE_VIRTUAL = 0x6e,
    OP_INVOKE_SUPER = 0x6f,
    OP_INVOKE_DIRECT = 0x70,
    OP_INVOKE_STATIC = 0x71,
    OP_INVOKE_INTERFACE = 0x72,
    OP_UNUSED_73 = 0x73,
    OP_INVOKE_VIRTUAL_RANGE = 0x74,
    OP_INVOKE_SUPER_RANGE = 0x75,
    OP_INVOKE_DIRECT_RANGE = 0x76,
    OP_INVOKE_STATIC_RANGE = 0x77,
    OP_INVOKE_INTERFACE_RANGE = 0x78,
    OP_UNUSED_79 = 0x79,
    OP_UNUSED_7A = 0x7a,
    OP_NEG_INT = 0x7b,
    OP_NOT_INT = 0x7c,
    OP_NEG_LONG = 0x7d,
    OP_NOT_LONG = 0x7e,
    OP_NEG_FLOAT = 0x7f,
    OP_NEG_DOUBLE = 0x80,
    OP_INT_TO_LONG = 0x81,
    OP_INT_TO_FLOAT = 0x82,
    OP_INT_TO_DOUBLE = 0x83,
    OP_LONG_TO_INT = 0x84,
    OP_LONG_TO_FLOAT = 0x85,
    OP_LONG_TO_DOUBLE = 0x86,
    OP_FLOAT_TO_INT = 0x87,
    OP_FLOAT_TO_LONG = 0x88,
    OP_FLOAT_TO_DOUBLE = 0x89,
    OP_DOUBLE_TO_INT = 0x8a,
    OP_DOUBLE_TO_LONG = 0x8b,
    OP_DOUBLE_TO_FLOAT = 0x8c,
    OP_INT_TO_BYTE = 0x8d,
    OP_INT_TO_CHAR = 0x8e,
    OP_INT_TO_SHORT = 0x8f,
    OP_ADD_INT = 0x90,
    OP_SUB_INT = 0x91,
    OP_MUL_INT = 0x92,
    OP_DIV_INT = 0x93,
    OP_REM_INT = 0x94,
    OP_AND_INT = 0x95,
    OP_OR_INT = 0x96,
    OP_XOR_INT = 0x97,
    OP_SHL_INT = 0x98,
    OP_SHR_INT = 0x99,
    OP_USHR_INT = 0x9a,
    OP_ADD_LONG = 0x9b,
    OP_SUB_LONG = 0x9c,
    OP_MUL_LONG = 0x9d,
    OP_DIV_LONG = 0x9e,
    OP_REM_LONG = 0x9f,
    OP_AND_LONG = 0xa0,
    OP_OR_LONG = 0xa1,
    OP_XOR_LONG = 0xa2,
    OP_SHL_LONG = 0xa3,
    OP_SHR_LONG = 0xa4,
    OP_USHR_LONG = 0xa5,
    OP_ADD_FLOAT = 0xa6,
    OP_SUB_FLOAT = 0xa7,
    OP_MUL_FLOAT = 0xa8,
    OP_DIV_FLOAT = 0xa9,
    OP_REM_FLOAT = 0xaa,
    OP_ADD_DOUBLE = 0xab,
    OP_SUB_DOUBLE = 0xac,
    OP_MUL_DOUBLE = 0xad,
    OP_DIV_DOUBLE = 0xae,
    OP_REM_DOUBLE = 0xaf,
    OP_ADD_INT_2ADDR = 0xb0,
    OP_SUB_INT_2ADDR = 0xb1,
    OP_MUL_INT_2ADDR = 0xb2,
    OP_DIV_INT_2ADDR = 0xb3,
    OP_REM_INT_2ADDR = 0xb4,
    OP_AND_INT_2ADDR = 0xb5,
    OP_OR_INT_2ADDR = 0xb6,
    OP_XOR_INT_2ADDR = 0xb7,
    OP_SHL_INT_2ADDR = 0xb8,
    OP_SHR_INT_2ADDR = 0xb9,
    OP_USHR_INT_2ADDR = 0xba,
    OP_ADD_LONG_2ADDR = 0xbb,
    OP_SUB_LONG_2ADDR = 0xbc,
    OP_MUL_LONG_2ADDR = 0xbd,
    OP_DIV_LONG_2ADDR = 0xbe,
    OP_REM_LONG_2ADDR = 0xbf,
    OP_AND_LONG_2ADDR = 0xc0,
    OP_OR_LONG_2ADDR = 0xc1,
    OP_XOR_LONG_2ADDR = 0xc2,
    OP_SHL_LONG_2ADDR = 0xc3,
    OP_SHR_LONG_2ADDR = 0xc4,
    OP_USHR_LONG_2ADDR = 0xc5,
    OP_ADD_FLOAT_2ADDR = 0xc6,
    OP_SUB_FLOAT_2ADDR = 0xc7,
    OP_MUL_FLOAT_2ADDR = 0xc8,
    OP_DIV_FLOAT_2ADDR = 0xc9,
    OP_REM_FLOAT_2ADDR = 0xca,
    OP_ADD_DOUBLE_2ADDR = 0xcb,
    OP_SUB_DOUBLE_2ADDR = 0xcc,
    OP_MUL_DOUBLE_2ADDR = 0xcd,
    OP_DIV_DOUBLE_2ADDR = 0xce,
    OP_REM_DOUBLE_2ADDR = 0xcf,
    OP_ADD_INT_LIT16 = 0xd0,
    OP_RSUB_INT = 0xd1,
    OP_MUL_INT_LIT16 = 0xd2,
    OP_DIV_INT_LIT16 = 0xd3,
    OP_REM_INT_LIT16 = 0xd4,
    OP_AND_INT_LIT16 = 0xd5,
    OP_OR_INT_LIT16 = 0xd6,
    OP_XOR_INT_LIT16 = 0xd7,
    OP_ADD_INT_LIT8 = 0xd8,
    OP_RSUB_INT_LIT8 = 0xd9,
    OP_MUL_INT_LIT8 = 0xda,
    OP_DIV_INT_LIT8 = 0xdb,
    OP_REM_INT_LIT8 = 0xdc,
    OP_AND_INT_LIT8 = 0xdd,
    OP_OR_INT_LIT8 = 0xde,
    OP_XOR_INT_LIT8 = 0xdf,
    OP_SHL_INT_LIT8 = 0xe0,
    OP_SHR_INT_LIT8 = 0xe1,
    OP_USHR_INT_LIT8 = 0xe2,
    OP_IGET_VOLATILE = 0xe3,
    OP_IPUT_VOLATILE = 0xe4,
    OP_SGET_VOLATILE = 0xe5,
    OP_SPUT_VOLATILE = 0xe6,
    OP_IGET_OBJECT_VOLATILE = 0xe7,
    OP_IGET_WIDE_VOLATILE = 0xe8,
    OP_IPUT_WIDE_VOLATILE = 0xe9,
    OP_SGET_WIDE_VOLATILE = 0xea,
    OP_SPUT_WIDE_VOLATILE = 0xeb,
    OP_BREAKPOINT = 0xec,
    OP_THROW_VERIFICATION_ERROR = 0xed,
    OP_EXECUTE_INLINE = 0xee,
    OP_EXECUTE_INLINE_RANGE = 0xef,
    OP_INVOKE_OBJECT_INIT_RANGE = 0xf0,
    OP_RETURN_VOID_BARRIER = 0xf1,
    OP_IGET_QUICK = 0xf2,
    OP_IGET_WIDE_QUICK = 0xf3,
    OP_IGET_OBJECT_QUICK = 0xf4,
    OP_IPUT_QUICK = 0xf5,
    OP_IPUT_WIDE_QUICK = 0xf6,
    OP_IPUT_OBJECT_QUICK = 0xf7,
    OP_INVOKE_VIRTUAL_QUICK = 0xf8,
    OP_INVOKE_VIRTUAL_QUICK_RANGE = 0xf9,
    OP_INVOKE_SUPER_QUICK = 0xfa,
    OP_INVOKE_SUPER_QUICK_RANGE = 0xfb,
    OP_IPUT_OBJECT_VOLATILE = 0xfc,
    OP_SGET_OBJECT_VOLATILE = 0xfd,
    OP_SPUT_OBJECT_VOLATILE = 0xfe,
    OP_UNUSED_FF = 0xff,
    // END(libdex-opcode-enum)
};


/*
 * Macro used to generate a computed goto table for use in implementing
 * an interpreter in C.
 */
#define DEFINE_GOTO_TABLE(_name) \
    static const void* _name[kNumPackedOpcodes] = {                      \
        /* BEGIN(libdex-goto-table); GENERATED AUTOMATICALLY BY opcode-gen */ \
        H(OP_NOP),                                                            \
        H(OP_MOVE),                                                           \
        H(OP_MOVE_FROM16),                                                    \
        H(OP_MOVE_16),                                                        \
        H(OP_MOVE_WIDE),                                                      \
        H(OP_MOVE_WIDE_FROM16),                                               \
        H(OP_MOVE_WIDE_16),                                                   \
        H(OP_MOVE_OBJECT),                                                    \
        H(OP_MOVE_OBJECT_FROM16),                                             \
        H(OP_MOVE_OBJECT_16),                                                 \
        H(OP_MOVE_RESULT),                                                    \
        H(OP_MOVE_RESULT_WIDE),                                               \
        H(OP_MOVE_RESULT_OBJECT),                                             \
        H(OP_MOVE_EXCEPTION),                                                 \
        H(OP_RETURN_VOID),                                                    \
        H(OP_RETURN),                                                         \
        H(OP_RETURN_WIDE),                                                    \
        H(OP_RETURN_OBJECT),                                                  \
        H(OP_CONST_4),                                                        \
        H(OP_CONST_16),                                                       \
        H(OP_CONST),                                                          \
        H(OP_CONST_HIGH16),                                                   \
        H(OP_CONST_WIDE_16),                                                  \
        H(OP_CONST_WIDE_32),                                                  \
        H(OP_CONST_WIDE),                                                     \
        H(OP_CONST_WIDE_HIGH16),                                              \
        H(OP_CONST_STRING),                                                   \
        H(OP_CONST_STRING_JUMBO),                                             \
        H(OP_CONST_CLASS),                                                    \
        H(OP_MONITOR_ENTER),                                                  \
        H(OP_MONITOR_EXIT),                                                   \
        H(OP_CHECK_CAST),                                                     \
        H(OP_INSTANCE_OF),                                                    \
        H(OP_ARRAY_LENGTH),                                                   \
        H(OP_NEW_INSTANCE),                                                   \
        H(OP_NEW_ARRAY),                                                      \
        H(OP_FILLED_NEW_ARRAY),                                               \
        H(OP_FILLED_NEW_ARRAY_RANGE),                                         \
        H(OP_FILL_ARRAY_DATA),                                                \
        H(OP_THROW),                                                          \
        H(OP_GOTO),                                                           \
        H(OP_GOTO_16),                                                        \
        H(OP_GOTO_32),                                                        \
        H(OP_PACKED_SWITCH),                                                  \
        H(OP_SPARSE_SWITCH),                                                  \
        H(OP_CMPL_FLOAT),                                                     \
        H(OP_CMPG_FLOAT),                                                     \
        H(OP_CMPL_DOUBLE),                                                    \
        H(OP_CMPG_DOUBLE),                                                    \
        H(OP_CMP_LONG),                                                       \
        H(OP_IF_EQ),                                                          \
        H(OP_IF_NE),                                                          \
        H(OP_IF_LT),                                                          \
        H(OP_IF_GE),                                                          \
        H(OP_IF_GT),                                                          \
        H(OP_IF_LE),                                                          \
        H(OP_IF_EQZ),                                                         \
        H(OP_IF_NEZ),                                                         \
        H(OP_IF_LTZ),                                                         \
        H(OP_IF_GEZ),                                                         \
        H(OP_IF_GTZ),                                                         \
        H(OP_IF_LEZ),                                                         \
        H(OP_UNUSED_3E),                                                      \
        H(OP_UNUSED_3F),                                                      \
        H(OP_UNUSED_40),                                                      \
        H(OP_UNUSED_41),                                                      \
        H(OP_UNUSED_42),                                                      \
        H(OP_UNUSED_43),                                                      \
        H(OP_AGET),                                                           \
        H(OP_AGET_WIDE),                                                      \
        H(OP_AGET_OBJECT),                                                    \
        H(OP_AGET_BOOLEAN),                                                   \
        H(OP_AGET_BYTE),                                                      \
        H(OP_AGET_CHAR),                                                      \
        H(OP_AGET_SHORT),                                                     \
        H(OP_APUT),                                                           \
        H(OP_APUT_WIDE),                                                      \
        H(OP_APUT_OBJECT),                                                    \
        H(OP_APUT_BOOLEAN),                                                   \
        H(OP_APUT_BYTE),                                                      \
        H(OP_APUT_CHAR),                                                      \
        H(OP_APUT_SHORT),                                                     \
        H(OP_IGET),                                                           \
        H(OP_IGET_WIDE),                                                      \
        H(OP_IGET_OBJECT),                                                    \
        H(OP_IGET_BOOLEAN),                                                   \
        H(OP_IGET_BYTE),                                                      \
        H(OP_IGET_CHAR),                                                      \
        H(OP_IGET_SHORT),                                                     \
        H(OP_IPUT),                                                           \
        H(OP_IPUT_WIDE),                                                      \
        H(OP_IPUT_OBJECT),                                                    \
        H(OP_IPUT_BOOLEAN),                                                   \
        H(OP_IPUT_BYTE),                                                      \
        H(OP_IPUT_CHAR),                                                      \
        H(OP_IPUT_SHORT),                                                     \
        H(OP_SGET),                                                           \
        H(OP_SGET_WIDE),                                                      \
        H(OP_SGET_OBJECT),                                                    \
        H(OP_SGET_BOOLEAN),                                                   \
        H(OP_SGET_BYTE),                                                      \
        H(OP_SGET_CHAR),                                                      \
        H(OP_SGET_SHORT),                                                     \
        H(OP_SPUT),                                                           \
        H(OP_SPUT_WIDE),                                                      \
        H(OP_SPUT_OBJECT),                                                    \
        H(OP_SPUT_BOOLEAN),                                                   \
        H(OP_SPUT_BYTE),                                                      \
        H(OP_SPUT_CHAR),                                                      \
        H(OP_SPUT_SHORT),                                                     \
        H(OP_INVOKE_VIRTUAL),                                                 \
        H(OP_INVOKE_SUPER),                                                   \
        H(OP_INVOKE_DIRECT),                                                  \
        H(OP_INVOKE_STATIC),                                                  \
        H(OP_INVOKE_INTERFACE),                                               \
        H(OP_UNUSED_73),                                                      \
        H(OP_INVOKE_VIRTUAL_RANGE),                                           \
        H(OP_INVOKE_SUPER_RANGE),                                             \
        H(OP_INVOKE_DIRECT_RANGE),                                            \
        H(OP_INVOKE_STATIC_RANGE),                                            \
        H(OP_INVOKE_INTERFACE_RANGE),                                         \
        H(OP_UNUSED_79),                                                      \
        H(OP_UNUSED_7A),                                                      \
        H(OP_NEG_INT),                                                        \
        H(OP_NOT_INT),                                                        \
        H(OP_NEG_LONG),                                                       \
        H(OP_NOT_LONG),                                                       \
        H(OP_NEG_FLOAT),                                                      \
        H(OP_NEG_DOUBLE),                                                     \
        H(OP_INT_TO_LONG),                                                    \
        H(OP_INT_TO_FLOAT),                                                   \
        H(OP_INT_TO_DOUBLE),                                                  \
        H(OP_LONG_TO_INT),                                                    \
        H(OP_LONG_TO_FLOAT),                                                  \
        H(OP_LONG_TO_DOUBLE),                                                 \
        H(OP_FLOAT_TO_INT),                                                   \
        H(OP_FLOAT_TO_LONG),                                                  \
        H(OP_FLOAT_TO_DOUBLE),                                                \
        H(OP_DOUBLE_TO_INT),                                                  \
        H(OP_DOUBLE_TO_LONG),                                                 \
        H(OP_DOUBLE_TO_FLOAT),                                                \
        H(OP_INT_TO_BYTE),                                                    \
        H(OP_INT_TO_CHAR),                                                    \
        H(OP_INT_TO_SHORT),                                                   \
        H(OP_ADD_INT),                                                        \
        H(OP_SUB_INT),                                                        \
        H(OP_MUL_INT),                                                        \
        H(OP_DIV_INT),                                                        \
        H(OP_REM_INT),                                                        \
        H(OP_AND_INT),                                                        \
        H(OP_OR_INT),                                                         \
        H(OP_XOR_INT),                                                        \
        H(OP_SHL_INT),                                                        \
        H(OP_SHR_INT),                                                        \
        H(OP_USHR_INT),                                                       \
        H(OP_ADD_LONG),                                                       \
        H(OP_SUB_LONG),                                                       \
        H(OP_MUL_LONG),                                                       \
        H(OP_DIV_LONG),                                                       \
        H(OP_REM_LONG),                                                       \
        H(OP_AND_LONG),                                                       \
        H(OP_OR_LONG),                                                        \
        H(OP_XOR_LONG),                                                       \
        H(OP_SHL_LONG),                                                       \
        H(OP_SHR_LONG),                                                       \
        H(OP_USHR_LONG),                                                      \
        H(OP_ADD_FLOAT),                                                      \
        H(OP_SUB_FLOAT),                                                      \
        H(OP_MUL_FLOAT),                                                      \
        H(OP_DIV_FLOAT),                                                      \
        H(OP_REM_FLOAT),                                                      \
        H(OP_ADD_DOUBLE),                                                     \
        H(OP_SUB_DOUBLE),                                                     \
        H(OP_MUL_DOUBLE),                                                     \
        H(OP_DIV_DOUBLE),                                                     \
        H(OP_REM_DOUBLE),                                                     \
        H(OP_ADD_INT_2ADDR),                                                  \
        H(OP_SUB_INT_2ADDR),                                                  \
        H(OP_MUL_INT_2ADDR),                                                  \
        H(OP_DIV_INT_2ADDR),                                                  \
        H(OP_REM_INT_2ADDR),                                                  \
        H(OP_AND_INT_2ADDR),                                                  \
        H(OP_OR_INT_2ADDR),                                                   \
        H(OP_XOR_INT_2ADDR),                                                  \
        H(OP_SHL_INT_2ADDR),                                                  \
        H(OP_SHR_INT_2ADDR),                                                  \
        H(OP_USHR_INT_2ADDR),                                                 \
        H(OP_ADD_LONG_2ADDR),                                                 \
        H(OP_SUB_LONG_2ADDR),                                                 \
        H(OP_MUL_LONG_2ADDR),                                                 \
        H(OP_DIV_LONG_2ADDR),                                                 \
        H(OP_REM_LONG_2ADDR),                                                 \
        H(OP_AND_LONG_2ADDR),                                                 \
        H(OP_OR_LONG_2ADDR),                                                  \
        H(OP_XOR_LONG_2ADDR),                                                 \
        H(OP_SHL_LONG_2ADDR),                                                 \
        H(OP_SHR_LONG_2ADDR),                                                 \
        H(OP_USHR_LONG_2ADDR),                                                \
        H(OP_ADD_FLOAT_2ADDR),                                                \
        H(OP_SUB_FLOAT_2ADDR),                                                \
        H(OP_MUL_FLOAT_2ADDR),                                                \
        H(OP_DIV_FLOAT_2ADDR),                                                \
        H(OP_REM_FLOAT_2ADDR),                                                \
        H(OP_ADD_DOUBLE_2ADDR),                                               \
        H(OP_SUB_DOUBLE_2ADDR),                                               \
        H(OP_MUL_DOUBLE_2ADDR),                                               \
        H(OP_DIV_DOUBLE_2ADDR),                                               \
        H(OP_REM_DOUBLE_2ADDR),                                               \
        H(OP_ADD_INT_LIT16),                                                  \
        H(OP_RSUB_INT),                                                       \
        H(OP_MUL_INT_LIT16),                                                  \
        H(OP_DIV_INT_LIT16),                                                  \
        H(OP_REM_INT_LIT16),                                                  \
        H(OP_AND_INT_LIT16),                                                  \
        H(OP_OR_INT_LIT16),                                                   \
        H(OP_XOR_INT_LIT16),                                                  \
        H(OP_ADD_INT_LIT8),                                                   \
        H(OP_RSUB_INT_LIT8),                                                  \
        H(OP_MUL_INT_LIT8),                                                   \
        H(OP_DIV_INT_LIT8),                                                   \
        H(OP_REM_INT_LIT8),                                                   \
        H(OP_AND_INT_LIT8),                                                   \
        H(OP_OR_INT_LIT8),                                                    \
        H(OP_XOR_INT_LIT8),                                                   \
        H(OP_SHL_INT_LIT8),                                                   \
        H(OP_SHR_INT_LIT8),                                                   \
        H(OP_USHR_INT_LIT8),                                                  \
        H(OP_IGET_VOLATILE),                                                  \
        H(OP_IPUT_VOLATILE),                                                  \
        H(OP_SGET_VOLATILE),                                                  \
        H(OP_SPUT_VOLATILE),                                                  \
        H(OP_IGET_OBJECT_VOLATILE),                                           \
        H(OP_IGET_WIDE_VOLATILE),                                             \
        H(OP_IPUT_WIDE_VOLATILE),                                             \
        H(OP_SGET_WIDE_VOLATILE),                                             \
        H(OP_SPUT_WIDE_VOLATILE),                                             \
        H(OP_BREAKPOINT),                                                     \
        H(OP_THROW_VERIFICATION_ERROR),                                       \
        H(OP_EXECUTE_INLINE),                                                 \
        H(OP_EXECUTE_INLINE_RANGE),                                           \
        H(OP_INVOKE_OBJECT_INIT_RANGE),                                       \
        H(OP_RETURN_VOID_BARRIER),                                            \
        H(OP_IGET_QUICK),                                                     \
        H(OP_IGET_WIDE_QUICK),                                                \
        H(OP_IGET_OBJECT_QUICK),                                              \
        H(OP_IPUT_QUICK),                                                     \
        H(OP_IPUT_WIDE_QUICK),                                                \
        H(OP_IPUT_OBJECT_QUICK),                                              \
        H(OP_INVOKE_VIRTUAL_QUICK),                                           \
        H(OP_INVOKE_VIRTUAL_QUICK_RANGE),                                     \
        H(OP_INVOKE_SUPER_QUICK),                                             \
        H(OP_INVOKE_SUPER_QUICK_RANGE),                                       \
        H(OP_IPUT_OBJECT_VOLATILE),                                           \
        H(OP_SGET_OBJECT_VOLATILE),                                           \
        H(OP_SPUT_OBJECT_VOLATILE),                                           \
        H(OP_UNUSED_FF),                                                      \
        /* END(libdex-goto-table) */                                          \
    };

/*
* These match the definitions in the VM specification.
*/
typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;
typedef int8_t s1;
typedef int16_t s2;
typedef int32_t s4;
typedef int64_t s8;


/*
 * Configuration defines.  These affect the C implementations, i.e. the
 * portable interpreter(s) and C stubs.
 *
 * Some defines are controlled by the Makefile, e.g.:
 *   WITH_INSTR_CHECKS
 *   WITH_TRACKREF_CHECKS
 *   EASY_GDB
 *   NDEBUG
 */

#define NDEBUG

#ifdef WITH_INSTR_CHECKS            /* instruction-level paranoia (slow!) */
# define CHECK_BRANCH_OFFSETS
# define CHECK_REGISTER_INDICES
#endif

/*
 * Some architectures require 64-bit alignment for access to 64-bit data
 * types.  We can't just use pointers to copy 64-bit values out of our
 * interpreted register set, because gcc may assume the pointer target is
 * aligned and generate invalid code.
 *
 * There are two common approaches:
 *  (1) Use a union that defines a 32-bit pair and a 64-bit value.
 *  (2) Call memcpy().
 *
 * Depending upon what compiler you're using and what options are specified,
 * one may be faster than the other.  For example, the compiler might
 * convert a memcpy() of 8 bytes into a series of instructions and omit
 * the call.  The union version could cause some strange side-effects,
 * e.g. for a while ARM gcc thought it needed separate storage for each
 * inlined instance, and generated instructions to zero out ~700 bytes of
 * stack space at the top of the interpreter.
 *
 * The default is to use memcpy().  The current gcc for ARM seems to do
 * better with the union.
 */
#if defined(__ARM_EABI__)
# define NO_UNALIGN_64__UNION
#endif
/*
 * MIPS ABI requires 64-bit alignment for access to 64-bit data types.
 *
 * Use memcpy() to do the transfer
 */
#if defined(__mips__)
/* # define NO_UNALIGN_64__UNION */
#endif


//#define LOG_INSTR                   /* verbose debugging */
/* set and adjust ANDROID_LOG_TAGS='*:i jdwp:i dalvikvm:i dalvikvmi:i' */

/*
 * Export another copy of the PC on every instruction; this is largely
 * redundant with EXPORT_PC and the debugger code.  This value can be
 * compared against what we have stored on the stack with EXPORT_PC to
 * help ensure that we aren't missing any export calls.
 */
#if WITH_EXTRA_GC_CHECKS > 1
# define EXPORT_EXTRA_PC() (self->currentPc2 = pc)
#else
# define EXPORT_EXTRA_PC()
#endif

/*
 * Adjust the program counter.  "_offset" is a signed int, in 16-bit units.
 *
 * Assumes the existence of "const u2* pc" and "const u2* curMethod->insns".
 *
 * We don't advance the program counter until we finish an instruction or
 * branch, because we do want to have to unroll the PC if there's an
 * exception.
 */
#ifdef CHECK_BRANCH_OFFSETS
# define ADJUST_PC(_offset) do {                                            \
        int myoff = _offset;        /* deref only once */                   \
        if (pc + myoff < curMethod->insns ||                                \
            pc + myoff >= curMethod->insns + dvmGetMethodInsnsSize(curMethod)) \
        {                                                                   \
            char* desc;                                                     \
            desc = dexProtoCopyMethodDescriptor(&curMethod->prototype);     \
            ALOGE("Invalid branch %d at 0x%04x in %s.%s %s",                 \
                myoff, (int) (pc - curMethod->insns),                       \
                curMethod->clazz->descriptor, curMethod->name, desc);       \
            free(desc);                                                     \
            dvmAbort();                                                     \
        }                                                                   \
        pc += myoff;                                                        \
        EXPORT_EXTRA_PC();                                                  \
    } while (false)
#else
// ok
# define ADJUST_PC(_offset) do {                                            \
        pc += _offset;                                                      \
        EXPORT_EXTRA_PC();                                                  \
    } while (false)
#endif


static const char kSpacing[] = "            ";


/* get a long from an array of u4 */
static inline s8 getLongFromArray(const u4 *ptr, int idx) {
#if defined(NO_UNALIGN_64__UNION)
    union {
        s8 ll;
        u4 parts[2];
    } conv;

    ptr += idx;
    conv.parts[0] = ptr[0];
    conv.parts[1] = ptr[1];
    return conv.ll;
#else
    s8 val;
    memcpy(&val, &ptr[idx], 8);
    return val;
#endif
}

/* store a long into an array of u4 */
static inline void putLongToArray(u4 *ptr, int idx, s8 val) {
#if defined(NO_UNALIGN_64__UNION)
    union {
        s8 ll;
        u4 parts[2];
    } conv;

    ptr += idx;
    conv.ll = val;
    ptr[0] = conv.parts[0];
    ptr[1] = conv.parts[1];
#else
    memcpy(&ptr[idx], &val, 8);
#endif
}

/* get a double from an array of u4 */
static inline double getDoubleFromArray(const u4 *ptr, int idx) {
#if defined(NO_UNALIGN_64__UNION)
    union {
        double d;
        u4 parts[2];
    } conv;

    ptr += idx;
    conv.parts[0] = ptr[0];
    conv.parts[1] = ptr[1];
    return conv.d;
#else
    double dval;
    memcpy(&dval, &ptr[idx], 8);
    return dval;
#endif
}

/* store a double into an array of u4 */
static inline void putDoubleToArray(u4 *ptr, int idx, double dval) {
#if defined(NO_UNALIGN_64__UNION)
    union {
        double d;
        u4 parts[2];
    } conv;

    ptr += idx;
    conv.d = dval;
    ptr[0] = conv.parts[0];
    ptr[1] = conv.parts[1];
#else
    memcpy(&ptr[idx], &dval, 8);
#endif
}

/*
 * If enabled, validate the register number on every access.  Otherwise,
 * just do an array access.
 *
 * Assumes the existence of "u4* fp".
 *
 * "_idx" may be referenced more than once.
 */
// ok
# define GET_REGISTER(_idx)                 (reg[(_idx)])
// ok
# define SET_REGISTER(_idx, _val)           (reg[(_idx)] = (_val))
// ok
# define GET_REGISTER_AS_OBJECT(_idx)       ((jobject) reg[(_idx)])
// ok
# define SET_REGISTER_AS_OBJECT(_idx, _val) (reg[(_idx)] = (u4)(_val))
// ok
# define GET_REGISTER_INT(_idx)             ((s4)GET_REGISTER(_idx))
// ok
# define SET_REGISTER_INT(_idx, _val)       SET_REGISTER(_idx, (s4)_val)
// ok
# define GET_REGISTER_WIDE(_idx)            getLongFromArray(reg, (_idx))
// ok
# define SET_REGISTER_WIDE(_idx, _val)      putLongToArray(reg, (_idx), (_val))
// ok
# define GET_REGISTER_FLOAT(_idx)           (*((float*) &reg[(_idx)]))
// ok
# define SET_REGISTER_FLOAT(_idx, _val)     (*((float*) &reg[(_idx)]) = (_val))
// ok
# define GET_REGISTER_DOUBLE(_idx)          getDoubleFromArray(reg, (_idx))
// ok
# define SET_REGISTER_DOUBLE(_idx, _val)    putDoubleToArray(reg, (_idx), (_val))

/*
 * Get 16 bits from the specified offset of the program counter.  We always
 * want to load 16 bits at a time from the instruction stream -- it's more
 * efficient than 8 and won't have the alignment problems that 32 might.
 *
 * Assumes existence of "const u2* pc".
 */
// ok
#define FETCH(_offset)     (pc[(_offset)])

/*
 * Extract instruction byte from 16-bit fetch (_inst is a u2).
 */
// ok
#define INST_INST(_inst)    ((_inst) & 0xff)

/*
 * Replace the opcode (used when handling breakpoints).  _opcode is a u1.
 */
#define INST_REPLACE_OP(_inst, _opcode) (((_inst) & 0xff00) | _opcode)

/*
 * Extract the "vA, vB" 4-bit registers from the instruction word (_inst is u2).
 */
// ok
#define INST_A(_inst)       (((_inst) >> 8) & 0x0f)
// ok
#define INST_B(_inst)       ((_inst) >> 12)

/*
 * Get the 8-bit "vAA" 8-bit register index from the instruction word.
 * (_inst is u2)
 */
// ok
#define INST_AA(_inst)      ((_inst) >> 8)


/* File: portable/stubdefs.cpp */
/*
 * In the C mterp stubs, "goto" is a function call followed immediately
 * by a return.
 */

#define GOTO_TARGET_DECL(_target, ...)

#define GOTO_TARGET(_target, ...) _target:

#define GOTO_TARGET_END

/* ugh */
#define STUB_HACK(x)
#define JIT_STUB_HACK(x)

/*
 * InterpSave's pc and fp must be valid when breaking out to a
 * "Reportxxx" routine.  Because the portable interpreter uses local
 * variables for these, we must flush prior.  Stubs, however, use
 * the interpSave vars directly, so this is a nop for stubs.
 */
#define PC_FP_TO_SELF()                                                    \
    self->interpSave.pc = pc;                                              \
    self->interpSave.curFrame = fp;
#define PC_TO_SELF() self->interpSave.pc = pc;

/*
 * Instruction framing.  For a switch-oriented implementation this is
 * case/break, for a threaded implementation it's a goto label and an
 * instruction fetch/computed goto.
 *
 * Assumes the existence of "const u2* pc" and (for threaded operation)
 * "u2 inst".
 */
# define H(_op)             &&op_##_op
// ok
# define HANDLE_OPCODE(_op) //op_##_op:
// ok
# define FINISH(_offset) {                                                  \
        ADJUST_PC(_offset);                                                 \
        inst = FETCH(0);                                                    \
        curOp = INST_INST(inst);                                            \
        goto NEXT_INSNS;                                                    \
    }
# define FINISH_BKPT(_opcode) {                                             \
        goto *handlerTable[_opcode];                                        \
    }

#define OP_END

/*
 * The "goto" targets just turn into goto statements.  The "arguments" are
 * passed through local variables.
 */

#define GOTO_exceptionThrown() goto exceptionThrown;

#define GOTO_returnFromMethod() goto returnFromMethod;

// ok
#define GOTO_invoke(_target, _methodCallRange)                              \
    do {                                                                    \
        methodCallRange = _methodCallRange;                                 \
        goto _target;                                                       \
    } while(false)

/* for this, the "args" are already in the locals */
// ok
#define GOTO_invokeMethod(_methodCallRange, _methodToCall, _vsrc1, _vdst) goto invokeMethod;

// ok
#define GOTO_bail() goto bail;

/*
 * Periodically check for thread suspension.
 *
 * While we're at it, see if a debugger has attached or the profiler has
 * started.  If so, switch to a different "goto" table.
 */
//#define PERIODIC_CHECKS(_pcadj) {                              \
//        if (dvmCheckSuspendQuick(self)) {                                   \
//            EXPORT_PC();  /* need for precise GC */                         \
//            dvmCheckSuspendPending(self);                                   \
//        }                                                                   \
//    }
#define PERIODIC_CHECKS(_pcadj) {                              \
    }

/* File: c/opcommon.cpp */
/* forward declarations of goto targets */
GOTO_TARGET_DECL(filledNewArray, bool methodCallRange);
GOTO_TARGET_DECL(invokeVirtual, bool methodCallRange);
GOTO_TARGET_DECL(invokeSuper, bool methodCallRange);
GOTO_TARGET_DECL(invokeInterface, bool methodCallRange);
GOTO_TARGET_DECL(invokeDirect, bool methodCallRange);
GOTO_TARGET_DECL(invokeStatic, bool methodCallRange);
GOTO_TARGET_DECL(invokeVirtualQuick, bool methodCallRange);
GOTO_TARGET_DECL(invokeSuperQuick, bool methodCallRange);
GOTO_TARGET_DECL(invokeMethod, bool methodCallRange, const Method *methodToCall,
                 u2 count, u2 regs);
GOTO_TARGET_DECL(returnFromMethod);
GOTO_TARGET_DECL(exceptionThrown);

/*
 * ===========================================================================
 *
 * What follows are opcode definitions shared between multiple opcodes with
 * minor substitutions handled by the C pre-processor.  These should probably
 * use the mterp substitution mechanism instead, with the code here moved
 * into common fragment files (like the asm "binop.S"), although it's hard
 * to give up the C preprocessor in favor of the much simpler text subst.
 *
 * ===========================================================================
 */

// ok
#define HANDLE_NUMCONV(_opcode, _opname, _fromtype, _totype)                \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s v%d,v%d", (_opname), vdst, vsrc1);                       \
        SET_REGISTER##_totype(vdst,                                         \
            GET_REGISTER##_fromtype(vsrc1));                                \
        FINISH(1);

// ok
#define HANDLE_FLOAT_TO_INT(_opcode, _opname, _fromvtype, _fromrtype, \
        _tovtype, _tortype)                                                 \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
    {                                                                       \
        /* spec defines specific handling for +/- inf and NaN values */     \
        _fromvtype val;                                                     \
        _tovtype intMin, intMax, result;                                    \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s v%d,v%d", (_opname), vdst, vsrc1);                       \
        val = GET_REGISTER##_fromrtype(vsrc1);                              \
        intMin = (_tovtype) 1 << (sizeof(_tovtype) * 8 -1);                 \
        intMax = ~intMin;                                                   \
        result = (_tovtype) val;                                            \
        if (val >= intMax)          /* +inf */                              \
            result = intMax;                                                \
        else if (val <= intMin)     /* -inf */                              \
            result = intMin;                                                \
        else if (val != val)        /* NaN */                               \
            result = 0;                                                     \
        else                                                                \
            result = (_tovtype) val;                                        \
        SET_REGISTER##_tortype(vdst, result);                               \
    }                                                                       \
    FINISH(1);

// ok
#define HANDLE_INT_TO_SMALL(_opcode, _opname, _type)                        \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|int-to-%s v%d,v%d", (_opname), vdst, vsrc1);                \
        SET_REGISTER(vdst, (_type) GET_REGISTER(vsrc1));                    \
        FINISH(1);

// ok
/* NOTE: the comparison result is always a signed 4-byte integer */
#define HANDLE_OP_CMPX(_opcode, _opname, _varType, _type, _nanVal)          \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        int result;                                                         \
        u2 regs;                                                            \
        _varType val1, val2;                                                \
        vdst = INST_AA(inst);                                               \
        regs = FETCH(1);                                                    \
        vsrc1 = regs & 0xff;                                                \
        vsrc2 = regs >> 8;                                                  \
        LOGD("|cmp%s v%d,v%d,v%d", (_opname), vdst, vsrc1, vsrc2);         \
        val1 = GET_REGISTER##_type(vsrc1);                                  \
        val2 = GET_REGISTER##_type(vsrc2);                                  \
        if (val1 == val2)                                                   \
            result = 0;                                                     \
        else if (val1 < val2)                                               \
            result = -1;                                                    \
        else if (val1 > val2)                                               \
            result = 1;                                                     \
        else                                                                \
            result = (_nanVal);                                             \
        LOGD("+ result=%d", result);                                       \
        SET_REGISTER(vdst, result);                                         \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_IF_XX(_opcode, _opname, _cmp)                             \
    HANDLE_OPCODE(_opcode /*vA, vB, +CCCC*/)                                \
        vsrc1 = INST_A(inst);                                               \
        vsrc2 = INST_B(inst);                                               \
        if ((s4) GET_REGISTER(vsrc1) _cmp (s4) GET_REGISTER(vsrc2)) {       \
            int branchOffset = (s2)FETCH(1);    /* sign-extended */         \
            LOGD("|if-%s v%d,v%d,+0x%04x", (_opname), vsrc1, vsrc2,        \
                branchOffset);                                              \
            LOGD("> branch taken");                                        \
            if (branchOffset < 0)                                           \
                PERIODIC_CHECKS(branchOffset);                              \
            FINISH(branchOffset);                                           \
        } else {                                                            \
            LOGD("|if-%s v%d,v%d,-", (_opname), vsrc1, vsrc2);             \
            FINISH(2);                                                      \
        }

// ok
#define HANDLE_OP_IF_XXZ(_opcode, _opname, _cmp)                            \
    HANDLE_OPCODE(_opcode /*vAA, +BBBB*/)                                   \
        vsrc1 = INST_AA(inst);                                              \
        if ((s4) GET_REGISTER(vsrc1) _cmp 0) {                              \
            int branchOffset = (s2)FETCH(1);    /* sign-extended */         \
            LOGD("|if-%s v%d,+0x%04x", (_opname), vsrc1, branchOffset);    \
            LOGD("> branch taken");                                        \
            if (branchOffset < 0)                                           \
                PERIODIC_CHECKS(branchOffset);                              \
            FINISH(branchOffset);                                           \
        } else {                                                            \
            LOGD("|if-%s v%d,-", (_opname), vsrc1);                        \
            FINISH(2);                                                      \
        }

// ok
#define HANDLE_UNOP(_opcode, _opname, _pfx, _sfx, _type)                    \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s v%d,v%d", (_opname), vdst, vsrc1);                       \
        SET_REGISTER##_type(vdst, _pfx GET_REGISTER##_type(vsrc1) _sfx);    \
        FINISH(1);



// ok
#define HANDLE_OP_X_INT(_opcode, _opname, _op, _chkdiv)                     \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        u2 srcRegs;                                                         \
        vdst = INST_AA(inst);                                               \
        srcRegs = FETCH(1);                                                 \
        vsrc1 = srcRegs & 0xff;                                             \
        vsrc2 = srcRegs >> 8;                                               \
        LOGD("|%s-int v%d,v%d", (_opname), vdst, vsrc1);                   \
        if (_chkdiv != 0) {                                                 \
            s4 firstVal, secondVal, result;                                 \
            firstVal = GET_REGISTER(vsrc1);                                 \
            secondVal = GET_REGISTER(vsrc2);                                \
            if (secondVal == 0) {                                           \
                dvmThrowArithmeticException("divide by zero");              \
                GOTO_exceptionThrown();                                     \
            }                                                               \
            if ((u4)firstVal == 0x80000000 && secondVal == -1) {            \
                if (_chkdiv == 1)                                           \
                    result = firstVal;  /* division */                      \
                else                                                        \
                    result = 0;         /* remainder */                     \
            } else {                                                        \
                result = firstVal _op secondVal;                            \
            }                                                               \
            SET_REGISTER(vdst, result);                                     \
        } else {                                                            \
            /* non-div/rem case */                                          \
            SET_REGISTER(vdst,                                              \
                (s4) GET_REGISTER(vsrc1) _op (s4) GET_REGISTER(vsrc2));     \
        }                                                                   \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_SHX_INT(_opcode, _opname, _cast, _op)                     \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        u2 srcRegs;                                                         \
        vdst = INST_AA(inst);                                               \
        srcRegs = FETCH(1);                                                 \
        vsrc1 = srcRegs & 0xff;                                             \
        vsrc2 = srcRegs >> 8;                                               \
        LOGD("|%s-int v%d,v%d", (_opname), vdst, vsrc1);                   \
        SET_REGISTER(vdst,                                                  \
            _cast GET_REGISTER(vsrc1) _op (GET_REGISTER(vsrc2) & 0x1f));    \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_X_INT_LIT16(_opcode, _opname, _op, _chkdiv)               \
    HANDLE_OPCODE(_opcode /*vA, vB, #+CCCC*/)                               \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        vsrc2 = FETCH(1);                                                   \
        LOGD("|%s-int/lit16 v%d,v%d,#+0x%04x",                              \
            (_opname), vdst, vsrc1, vsrc2);                                 \
        if (_chkdiv != 0) {                                                 \
            s4 firstVal, result;                                            \
            firstVal = GET_REGISTER(vsrc1);                                 \
            if ((s2) vsrc2 == 0) {                                          \
                dvmThrowArithmeticException("divide by zero");              \
                GOTO_exceptionThrown();                                     \
            }                                                               \
            if ((u4)firstVal == 0x80000000 && ((s2) vsrc2) == -1) {         \
                /* won't generate /lit16 instr for this; check anyway */    \
                if (_chkdiv == 1)                                           \
                    result = firstVal;  /* division */                      \
                else                                                        \
                    result = 0;         /* remainder */                     \
            } else {                                                        \
                result = firstVal _op (s2) vsrc2;                           \
            }                                                               \
            SET_REGISTER(vdst, result);                                     \
        } else {                                                            \
            /* non-div/rem case */                                          \
            SET_REGISTER(vdst, GET_REGISTER(vsrc1) _op (s2) vsrc2);         \
        }                                                                   \
        FINISH(2);

// ok
#define HANDLE_OP_X_INT_LIT8(_opcode, _opname, _op, _chkdiv)                \
    HANDLE_OPCODE(_opcode /*vAA, vBB, #+CC*/)                               \
    {                                                                       \
        u2 litInfo;                                                         \
        vdst = INST_AA(inst);                                               \
        litInfo = FETCH(1);                                                 \
        vsrc1 = litInfo & 0xff;                                             \
        vsrc2 = litInfo >> 8;       /* constant */                          \
        LOGD("|%s-int/lit8 v%d,v%d,#+0x%02x",                               \
            (_opname), vdst, vsrc1, vsrc2);                                 \
        if (_chkdiv != 0) {                                                 \
            s4 firstVal, result;                                            \
            firstVal = GET_REGISTER(vsrc1);                                 \
            if ((s1) vsrc2 == 0) {                                          \
                dvmThrowArithmeticException("divide by zero");              \
                GOTO_exceptionThrown();                                     \
            }                                                               \
            if ((u4)firstVal == 0x80000000 && ((s1) vsrc2) == -1) {         \
                if (_chkdiv == 1)                                           \
                    result = firstVal;  /* division */                      \
                else                                                        \
                    result = 0;         /* remainder */                     \
            } else {                                                        \
                result = firstVal _op ((s1) vsrc2);                         \
            }                                                               \
            SET_REGISTER(vdst, result);                                     \
        } else {                                                            \
            SET_REGISTER(vdst,                                              \
                (s4) GET_REGISTER(vsrc1) _op (s1) vsrc2);                   \
        }                                                                   \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_SHX_INT_LIT8(_opcode, _opname, _cast, _op)                \
    HANDLE_OPCODE(_opcode /*vAA, vBB, #+CC*/)                               \
    {                                                                       \
        u2 litInfo;                                                         \
        vdst = INST_AA(inst);                                               \
        litInfo = FETCH(1);                                                 \
        vsrc1 = litInfo & 0xff;                                             \
        vsrc2 = litInfo >> 8;       /* constant */                          \
        LOGD("|%s-int/lit8 v%d,v%d,#+0x%02x",                              \
            (_opname), vdst, vsrc1, vsrc2);                                 \
        SET_REGISTER(vdst,                                                  \
            _cast GET_REGISTER(vsrc1) _op (vsrc2 & 0x1f));                  \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_X_INT_2ADDR(_opcode, _opname, _op, _chkdiv)               \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s-int-2addr v%d,v%d", (_opname), vdst, vsrc1);             \
        if (_chkdiv != 0) {                                                 \
            s4 firstVal, secondVal, result;                                 \
            firstVal = GET_REGISTER(vdst);                                  \
            secondVal = GET_REGISTER(vsrc1);                                \
            if (secondVal == 0) {                                           \
                dvmThrowArithmeticException("divide by zero");              \
                GOTO_exceptionThrown();                                     \
            }                                                               \
            if ((u4)firstVal == 0x80000000 && secondVal == -1) {            \
                if (_chkdiv == 1)                                           \
                    result = firstVal;  /* division */                      \
                else                                                        \
                    result = 0;         /* remainder */                     \
            } else {                                                        \
                result = firstVal _op secondVal;                            \
            }                                                               \
            SET_REGISTER(vdst, result);                                     \
        } else {                                                            \
            SET_REGISTER(vdst,                                              \
                (s4) GET_REGISTER(vdst) _op (s4) GET_REGISTER(vsrc1));      \
        }                                                                   \
        FINISH(1);

// ok
#define HANDLE_OP_SHX_INT_2ADDR(_opcode, _opname, _cast, _op)               \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s-int-2addr v%d,v%d", (_opname), vdst, vsrc1);             \
        SET_REGISTER(vdst,                                                  \
            _cast GET_REGISTER(vdst) _op (GET_REGISTER(vsrc1) & 0x1f));     \
        FINISH(1);

// ok
#define HANDLE_OP_X_LONG(_opcode, _opname, _op, _chkdiv)                    \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        u2 srcRegs;                                                         \
        vdst = INST_AA(inst);                                               \
        srcRegs = FETCH(1);                                                 \
        vsrc1 = srcRegs & 0xff;                                             \
        vsrc2 = srcRegs >> 8;                                               \
        LOGD("|%s-long v%d,v%d,v%d", (_opname), vdst, vsrc1, vsrc2);       \
        if (_chkdiv != 0) {                                                 \
            s8 firstVal, secondVal, result;                                 \
            firstVal = GET_REGISTER_WIDE(vsrc1);                            \
            secondVal = GET_REGISTER_WIDE(vsrc2);                           \
            if (secondVal == 0LL) {                                         \
                dvmThrowArithmeticException("divide by zero");              \
                GOTO_exceptionThrown();                                     \
            }                                                               \
            if ((u8)firstVal == 0x8000000000000000ULL &&                    \
                secondVal == -1LL)                                          \
            {                                                               \
                if (_chkdiv == 1)                                           \
                    result = firstVal;  /* division */                      \
                else                                                        \
                    result = 0;         /* remainder */                     \
            } else {                                                        \
                result = firstVal _op secondVal;                            \
            }                                                               \
            SET_REGISTER_WIDE(vdst, result);                                \
        } else {                                                            \
            SET_REGISTER_WIDE(vdst,                                         \
                (s8) GET_REGISTER_WIDE(vsrc1) _op (s8) GET_REGISTER_WIDE(vsrc2)); \
        }                                                                   \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_SHX_LONG(_opcode, _opname, _cast, _op)                    \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        u2 srcRegs;                                                         \
        vdst = INST_AA(inst);                                               \
        srcRegs = FETCH(1);                                                 \
        vsrc1 = srcRegs & 0xff;                                             \
        vsrc2 = srcRegs >> 8;                                               \
        LOGD("|%s-long v%d,v%d,v%d", (_opname), vdst, vsrc1, vsrc2);       \
        SET_REGISTER_WIDE(vdst,                                             \
            _cast GET_REGISTER_WIDE(vsrc1) _op (GET_REGISTER(vsrc2) & 0x3f)); \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_X_LONG_2ADDR(_opcode, _opname, _op, _chkdiv)              \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s-long-2addr v%d,v%d", (_opname), vdst, vsrc1);            \
        if (_chkdiv != 0) {                                                 \
            s8 firstVal, secondVal, result;                                 \
            firstVal = GET_REGISTER_WIDE(vdst);                             \
            secondVal = GET_REGISTER_WIDE(vsrc1);                           \
            if (secondVal == 0LL) {                                         \
                dvmThrowArithmeticException("divide by zero");              \
                GOTO_exceptionThrown();                                     \
            }                                                               \
            if ((u8)firstVal == 0x8000000000000000ULL &&                    \
                secondVal == -1LL)                                          \
            {                                                               \
                if (_chkdiv == 1)                                           \
                    result = firstVal;  /* division */                      \
                else                                                        \
                    result = 0;         /* remainder */                     \
            } else {                                                        \
                result = firstVal _op secondVal;                            \
            }                                                               \
            SET_REGISTER_WIDE(vdst, result);                                \
        } else {                                                            \
            SET_REGISTER_WIDE(vdst,                                         \
                (s8) GET_REGISTER_WIDE(vdst) _op (s8)GET_REGISTER_WIDE(vsrc1));\
        }                                                                   \
        FINISH(1);

// ok
#define HANDLE_OP_SHX_LONG_2ADDR(_opcode, _opname, _cast, _op)              \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s-long-2addr v%d,v%d", (_opname), vdst, vsrc1);            \
        SET_REGISTER_WIDE(vdst,                                             \
            _cast GET_REGISTER_WIDE(vdst) _op (GET_REGISTER(vsrc1) & 0x3f)); \
        FINISH(1);
// ok
#define HANDLE_OP_X_FLOAT(_opcode, _opname, _op)                            \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        u2 srcRegs;                                                         \
        vdst = INST_AA(inst);                                               \
        srcRegs = FETCH(1);                                                 \
        vsrc1 = srcRegs & 0xff;                                             \
        vsrc2 = srcRegs >> 8;                                               \
        LOGD("|%s-float v%d,v%d,v%d", (_opname), vdst, vsrc1, vsrc2);      \
        SET_REGISTER_FLOAT(vdst,                                            \
            GET_REGISTER_FLOAT(vsrc1) _op GET_REGISTER_FLOAT(vsrc2));       \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_X_DOUBLE(_opcode, _opname, _op)                           \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        u2 srcRegs;                                                         \
        vdst = INST_AA(inst);                                               \
        srcRegs = FETCH(1);                                                 \
        vsrc1 = srcRegs & 0xff;                                             \
        vsrc2 = srcRegs >> 8;                                               \
        LOGD("|%s-double v%d,v%d,v%d", (_opname), vdst, vsrc1, vsrc2);     \
        SET_REGISTER_DOUBLE(vdst,                                           \
            GET_REGISTER_DOUBLE(vsrc1) _op GET_REGISTER_DOUBLE(vsrc2));     \
    }                                                                       \
    FINISH(2);


// ok
#define HANDLE_OP_X_FLOAT_2ADDR(_opcode, _opname, _op)                      \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s-float-2addr v%d,v%d", (_opname), vdst, vsrc1);           \
        SET_REGISTER_FLOAT(vdst,                                            \
            GET_REGISTER_FLOAT(vdst) _op GET_REGISTER_FLOAT(vsrc1));        \
        FINISH(1);

// ok
#define HANDLE_OP_X_DOUBLE_2ADDR(_opcode, _opname, _op)                     \
    HANDLE_OPCODE(_opcode /*vA, vB*/)                                       \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);                                               \
        LOGD("|%s-double-2addr v%d,v%d", (_opname), vdst, vsrc1);          \
        SET_REGISTER_DOUBLE(vdst,                                           \
            GET_REGISTER_DOUBLE(vdst) _op GET_REGISTER_DOUBLE(vsrc1));      \
        FINISH(1);


// ok
#define HANDLE_OP_AGET(_opcode, _opname, _jtype, _ctype, _regsize)        \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        _ctype##Array arrayObj;                                             \
        u2 arrayInfo;                                                       \
        vdst = INST_AA(inst);                                               \
        arrayInfo = FETCH(1);                                               \
        vsrc1 = arrayInfo & 0xff;    /* array ptr */                        \
        vsrc2 = arrayInfo >> 8;      /* index */                            \
        LOGD("|aget%s v%d,v%d,v%d", (_opname), vdst, vsrc1, vsrc2);        \
        arrayObj = (_ctype##Array) GET_REGISTER(vsrc1);                      \
        if (!checkForNull((jobject) arrayObj)){                             \
             GOTO_exceptionThrown();                                        \
        }                                                                   \
        if (GET_REGISTER(vsrc2) >= (*env).GetArrayLength(arrayObj)) {       \
            dvmThrowArrayIndexOutOfBoundsException(                         \
                (*env).GetArrayLength(arrayObj), GET_REGISTER(vsrc2));      \
            GOTO_exceptionThrown();                                        \
        }                                                                   \
        u8 tmpBuf[1];                                                       \
        (*env).Get##_jtype##ArrayRegion(arrayObj,                           \
                GET_REGISTER(vsrc2), 1, (_ctype*)tmpBuf);                   \
        SET_REGISTER##_regsize(vdst, *((_ctype*)tmpBuf));                  \
        LOGD("+ AGET[%d]=%#x", GET_REGISTER(vsrc2), GET_REGISTER(vdst));    \
    }                                                                       \
    FINISH(2);

// ok
#define HANDLE_OP_APUT(_opcode, _opname, _jtype, _ctype, _regsize)          \
    HANDLE_OPCODE(_opcode /*vAA, vBB, vCC*/)                                \
    {                                                                       \
        _ctype##Array arrayObj;                                             \
        u2 arrayInfo;                                                       \
        vdst = INST_AA(inst);       /* AA: source value */                  \
        arrayInfo = FETCH(1);                                               \
        vsrc1 = arrayInfo & 0xff;   /* BB: array ptr */                     \
        vsrc2 = arrayInfo >> 8;     /* CC: index */                         \
        LOGD("|aput%s v%d,v%d,v%d", (_opname), vdst, vsrc1, vsrc2);         \
        arrayObj = (_ctype##Array) GET_REGISTER(vsrc1);                     \
        if (!checkForNull((jobject) arrayObj)){                             \
            GOTO_exceptionThrown();                                         \
        }                                                                   \
        if (GET_REGISTER(vsrc2) >= (*env).GetArrayLength(arrayObj)) {       \
            dvmThrowArrayIndexOutOfBoundsException(                         \
                (*env).GetArrayLength(arrayObj), GET_REGISTER(vsrc2));      \
            GOTO_bail();                                                     \
        }                                                                   \
        LOGD("+ APUT[%d]=0x%08x", GET_REGISTER(vsrc2),                      \
                GET_REGISTER##_regsize(vdst));                              \
        u8 tmpBuf[1];                                                        \
        *((_ctype*)tmpBuf) = GET_REGISTER##_regsize(vdst);                    \
        (*env).Set##_jtype##ArrayRegion(arrayObj,                           \
                GET_REGISTER(vsrc2), 1, (_ctype*)tmpBuf);                  \
    }                                                                       \
    FINISH(2);

/*
 * It's possible to get a bad value out of a field with sub-32-bit stores
 * because the -quick versions always operate on 32 bits.  Consider:
 *   short foo = -1  (sets a 32-bit register to 0xffffffff)
 *   iput-quick foo  (writes all 32 bits to the field)
 *   short bar = 1   (sets a 32-bit register to 0x00000001)
 *   iput-short      (writes the low 16 bits to the field)
 *   iget-quick foo  (reads all 32 bits from the field, yielding 0xffff0001)
 * This can only happen when optimized and non-optimized code has interleaved
 * access to the same field.  This is unlikely but possible.
 *
 * The easiest way to fix this is to always read/write 32 bits at a time.  On
 * a device with a 16-bit data bus this is sub-optimal.  (The alternative
 * approach is to have sub-int versions of iget-quick, but now we're wasting
 * Dalvik instruction space and making it less likely that handler code will
 * already be in the CPU i-cache.)
 */

// ok
#define HANDLE_IGET_X(_opcode, _opname, _ctype, _regsize)                   \
    HANDLE_OPCODE(_opcode /*vA, vB, field@CCCC*/)                           \
    {                                                                       \
        jfieldID ifield;                                                    \
        jobject obj;                                                        \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);   /* object ptr */                            \
        ref = FETCH(1);         /* field ref */                             \
        LOGD("|iget%s v%d,v%d,field@0x%04x", (_opname), vdst, vsrc1, ref);  \
        obj = (jobject) GET_REGISTER(vsrc1);                                \
        if (!checkForNull(obj)){                                            \
            GOTO_exceptionThrown();                                         \
        }                                                                   \
        const char* name;                                                   \
        s8 resVal;                                                          \
        if(!dvmResolveField(curMethod->clazz, ref, obj, &resVal, &name)){   \
            GOTO_exceptionThrown();                                         \
        }                                                                   \
        SET_REGISTER##_regsize(vdst, (_ctype)resVal);                       \
        LOGD("+ IGET '%s'=0x%08llx", name,                                  \
            (u8) GET_REGISTER##_regsize(vdst));                             \
    }                                                                       \
    FINISH(2);

//// ok
//#define HANDLE_IGET_X_QUICK(_opcode, _opname, _ftype, _regsize)             \
//   HANDLE_OPCODE(_opcode /*vA, vB, field@CCCC*/)                           \
//   {                                                                       \
//       jobject obj;                                                        \
//       vdst = INST_A(inst);                                                \
//        vsrc1 = INST_B(inst);   /* object ptr */                            \
//        ref = FETCH(1);         /* field offset */                          \
//        LOGD("|iget%s-quick v%d,v%d,field@+%u",                            \
//            (_opname), vdst, vsrc1, ref);                                   \
//        obj = (jobject) GET_REGISTER(vsrc1);                                \
//        if(!checkForNull(obj)){                                             \
//            GOTO_bail();                                                    \
//        }                                                                 \
//        Object* cObj = nullptr;                                             \
//        getObjectOrThreadByJObject(obj, &cObj, nullptr);                    \
//        SET_REGISTER##_regsize(vdst, dvmGetField##_ftype(cObj, ref));        \
//        LOGD("+ IGETQ %d=0x%08llx", ref,                                   \
//            (u8) GET_REGISTER##_regsize(vdst));                             \
//    }                                                                       \
//    FINISH(2);
//

   // ok
#define HANDLE_IPUT_X(_opcode, _opname,  _regsize)           \
    HANDLE_OPCODE(_opcode /*vA, vB, field@CCCC*/)                           \
    {                                                                       \
        jfieldID ifield;                                                  \
        jobject obj;                                                         \
        vdst = INST_A(inst);                                                \
        vsrc1 = INST_B(inst);   /* object ptr */                            \
        ref = FETCH(1);         /* field ref */                             \
        LOGD("|iput%s v%d,v%d,field@0x%04x", (_opname), vdst, vsrc1, ref); \
        obj = (jobject) GET_REGISTER(vsrc1);                                \
        if (!checkForNull(obj)){                                            \
            GOTO_exceptionThrown();                                         \
        }                                                                   \
        const char* name;                                                   \
        u8 resVal = (u8)GET_REGISTER##_regsize(vdst);                       \
        if(!dvmResolveSetField(curMethod->clazz, ref, obj, resVal, &name)){ \
            GOTO_exceptionThrown();                                         \
        }                                                                   \
        LOGD("+ IPUT '%s'=0x%08llx", name,                                  \
            (u8) GET_REGISTER##_regsize(vdst));                             \
    }                                                                       \
    FINISH(2);

//#define HANDLE_IPUT_X_QUICK(_opcode, _opname, _ftype, _regsize)             \
//    HANDLE_OPCODE(_opcode /*vA, vB, field@CCCC*/)                           \
//    {                                                                       \
//        jobject obj;                                                        \
//        vdst = INST_A(inst);                                                \
//        vsrc1 = INST_B(inst);   /* object ptr */                            \
//        ref = FETCH(1);         /* field offset */                          \
//        LOGD("|iput%s-quick v%d,v%d,field@0x%04x",                         \
//            (_opname), vdst, vsrc1, ref);                                   \
//        obj = (jobject) GET_REGISTER(vsrc1);                                \
//        if (!checkForNull(obj)){                                            \
//            GOTO_bail();                                                    \
//        }                                                                   \
//        dvmSetField##_ftype(obj, ref, GET_REGISTER##_regsize(vdst));        \
//        LOGD("+ IPUTQ %d=0x%08llx", ref,                                   \
//            (u8) GET_REGISTER##_regsize(vdst));                             \
//    }                                                                       \
//    FINISH(2);

/*
 * The JIT needs dvmDexGetResolvedField() to return non-null.
 * Because the portable interpreter is not involved with the JIT
 * and trace building, we only need the extra check here when this
 * code is massaged into a stub called from an assembly interpreter.
 * This is controlled by the JIT_STUB_HACK maco.
 */

// ok
#define HANDLE_SGET_X(_opcode, _opname, _ctype, _regsize)                   \
    HANDLE_OPCODE(_opcode /*vAA, field@BBBB*/)                              \
    {                                                                       \
        jfieldID sfield;                                                    \
        vdst = INST_AA(inst);                                               \
        ref = FETCH(1);         /* field ref */                             \
        LOGD("|sget%s v%d,sfield@0x%04x", (_opname), vdst, ref);            \
        const char* name;                                                   \
        s8 resVal;                                                          \
        if(!dvmResolveField(curMethod->clazz, ref, nullptr, &resVal, &name)){   \
            GOTO_exceptionThrown();                                         \
        }                                                                   \
        SET_REGISTER##_regsize(vdst, (_ctype)resVal);                       \
        LOGD("+ SGET '%s'=0x%08llx", name,                                   \
            (u8)GET_REGISTER##_regsize(vdst));                              \
    }                                                                       \
    FINISH(2);


    // ok
#define HANDLE_SPUT_X(_opcode, _opname, _regsize)                           \
    HANDLE_OPCODE(_opcode /*vAA, field@BBBB*/)                              \
    {                                                                       \
        jfieldID sfield;                                                    \
        vdst = INST_AA(inst);                                               \
        ref = FETCH(1);         /* field ref */                             \
        LOGD("|sput%s v%d,sfield@0x%04x", (_opname), vdst, ref);            \
        const char* name;                                                   \
        u8 resVal = (u8)GET_REGISTER##_regsize(vdst);                       \
        if(!dvmResolveSetField(curMethod->clazz, ref, nullptr, resVal, &name)){   \
            GOTO_exceptionThrown();                                         \
        }                                                                   \
        LOGD("+ SPUT '%s'=0x%08llx", name,                                  \
            (u8)GET_REGISTER##_regsize(vdst));                              \
    }                                                                       \
    FINISH(2);


#define HAVE_LITTLE_ENDIAN

#if defined(HAVE_ENDIAN_H)
# include <endian.h>
#else /*not HAVE_ENDIAN_H*/
# define __BIG_ENDIAN 4321
# define __LITTLE_ENDIAN 1234
# if defined(HAVE_LITTLE_ENDIAN)
#  define __BYTE_ORDER __LITTLE_ENDIAN
# else
#  define __BYTE_ORDER __BIG_ENDIAN
# endif
#endif /*not HAVE_ENDIAN_H*/

#if !defined(NDEBUG) && defined(WITH_DALVIK_ASSERT)
# undef assert
# define assert(x) \
    ((x) ? ((void)0) : (ALOGE("ASSERT FAILED (%s:%d): %s", \
        __FILE__, __LINE__, #x), *(int*)39=39, (void)0) )
#endif

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define LIKELY(exp) (__builtin_expect((exp) != 0, true))
#define UNLIKELY(exp) (__builtin_expect((exp) != 0, false))

#define ALIGN_UP(x, n) (((size_t)(x) + (n) - 1) & ~((n) - 1))
#define ALIGN_DOWN(x, n) ((size_t)(x) & -(n))
#define ALIGN_UP_TO_PAGE_SIZE(p) ALIGN_UP(p, SYSTEM_PAGE_SIZE)
#define ALIGN_DOWN_TO_PAGE_SIZE(p) ALIGN_DOWN(p, SYSTEM_PAGE_SIZE)

#define CLZ(x) __builtin_clz(x)


/*
 * These match the definitions in the VM specification.
 */
typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;
typedef int8_t s1;
typedef int16_t s2;
typedef int32_t s4;
typedef int64_t s8;

/*
 * Storage for primitive types and object references.
 *
 * Some parts of the code (notably object field access) assume that values
 * are "left aligned", i.e. given "JValue jv", "jv.i" and "*((s4*)&jv)"
 * yield the same result.  This seems to be guaranteed by gcc on big- and
 * little-endian systems.
 */
struct Object;

union JValue {
#if defined(HAVE_LITTLE_ENDIAN)
    u1 z;
    s1 b;
    u2 c;
    s2 s;
    s4 i;
    s8 j;
    float f;
    double d;
    Object *l;
#endif
#if defined(HAVE_BIG_ENDIAN)
    struct {
        u1    _z[3];
        u1    z;
    };
    struct {
        s1    _b[3];
        s1    b;
    };
    struct {
        u2    _c;
        u2    c;
    };
    struct {
        s2    _s;
        s2    s;
    };
    s4      i;
    s8      j;
    float   f;
    double  d;
    void*   l;
#endif
};

#define OFFSETOF_MEMBER(t, f)         \
  (reinterpret_cast<char*>(           \
     &reinterpret_cast<t*>(16)->f) -  \
   reinterpret_cast<char*>(16))

#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))


class Method;

/*
 * Native function pointer type.
 *
 * "args[0]" holds the "this" pointer for virtual methods.
 *
 * The "Bridge" form is a super-set of the "Native" form; in many places
 * they are used interchangeably.  Currently, all functions have all
 * arguments passed in, but some functions only care about the first two.
 * Passing extra arguments to a C function is (mostly) harmless.
 */
typedef void (*DalvikBridgeFunc)(const u4 *args, JValue *pResult,
                                 const Method *method, struct Thread *self);

typedef void (*DalvikNativeFunc)(const u4 *args, JValue *pResult);


/* vm-internal access flags and related definitions */
enum AccessFlags {
    ACC_MIRANDA = 0x8000,       // method (internal to VM)
    JAVA_FLAGS_MASK = 0xffff,       // bits set from Java sources (low 16)
};

/* Use the top 16 bits of the access flags field for
 * other class flags.  Code should use the *CLASS_FLAG*()
 * macros to set/get these flags.
 */
enum ClassFlags {
    CLASS_ISFINALIZABLE = (1 << 31), // class/ancestor overrides finalize()
    CLASS_ISARRAY = (1 << 30), // class is a "[*"
    CLASS_ISOBJECTARRAY = (1 << 29), // class is a "[L*" or "[[*"
    CLASS_ISCLASS = (1 << 28), // class is *the* class Class

    CLASS_ISREFERENCE = (1 << 27), // class is a soft/weak/phantom ref
    // only ISREFERENCE is set --> soft
    CLASS_ISWEAKREFERENCE = (1 << 26), // class is a weak reference
    CLASS_ISFINALIZERREFERENCE = (1 << 25), // class is a finalizer reference
    CLASS_ISPHANTOMREFERENCE = (1 << 24), // class is a phantom reference

    CLASS_MULTIPLE_DEFS = (1 << 23), // DEX verifier: defs in multiple DEXs

    /* unlike the others, these can be present in the optimized DEX file */
            CLASS_ISOPTIMIZED = (1 << 17), // class may contain opt instrs
    CLASS_ISPREVERIFIED = (1 << 16), // class has been pre-verified
};

/*
 * Get/set class flags.
 */
#define SET_CLASS_FLAG(clazz, flag) \
    do { (clazz)->accessFlags |= (flag); } while (0)

#define CLEAR_CLASS_FLAG(clazz, flag) \
    do { (clazz)->accessFlags &= ~(flag); } while (0)

#define IS_CLASS_FLAG_SET(clazz, flag) \
    (((clazz)->accessFlags & (flag)) != 0)

#define GET_CLASS_FLAG_GROUP(clazz, flags) \
    ((u4)((clazz)->accessFlags & (flags)))

/*
 * Use the top 16 bits of the access flags field for other method flags.
 * Code should use the *METHOD_FLAG*() macros to set/get these flags.
 */
enum MethodFlags {
    METHOD_ISWRITABLE = (1 << 31),  // the method's code is writable
};

/*
 * Get/set method flags.
 */
#define SET_METHOD_FLAG(method, flag) \
    do { (method)->accessFlags |= (flag); } while (0)

#define CLEAR_METHOD_FLAG(method, flag) \
    do { (method)->accessFlags &= ~(flag); } while (0)

#define IS_METHOD_FLAG_SET(method, flag) \
    (((method)->accessFlags & (flag)) != 0)

#define GET_METHOD_FLAG_GROUP(method, flags) \
    ((u4)((method)->accessFlags & (flags)))

/* current state of the class, increasing as we progress */
enum ClassStatus {
    CLASS_ERROR = -1,

    CLASS_NOTREADY = 0,
    CLASS_IDX = 1,    /* loaded, DEX idx in super or ifaces */
    CLASS_LOADED = 2,    /* DEX idx values resolved */
    CLASS_RESOLVED = 3,    /* part of linking */
    CLASS_VERIFYING = 4,    /* in the process of being verified */
    CLASS_VERIFIED = 5,    /* logically part of linking; done pre-init */
    CLASS_INITIALIZING = 6,    /* class init in progress */
    CLASS_INITIALIZED = 7,    /* ready to go */
};

/*
 * Definitions for packing refOffsets in ClassObject.
 */
/*
 * A magic value for refOffsets. Ignore the bits and walk the super
 * chain when this is the value.
 * [This is an unlikely "natural" value, since it would be 30 non-ref instance
 * fields followed by 2 ref instance fields.]
 */
#define CLASS_WALK_SUPER ((unsigned int)(3))
#define CLASS_SMALLEST_OFFSET (sizeof(struct Object))
#define CLASS_BITS_PER_WORD (sizeof(unsigned long int) * 8)
#define CLASS_OFFSET_ALIGNMENT 4
#define CLASS_HIGH_BIT ((unsigned int)1 << (CLASS_BITS_PER_WORD - 1))
/*
 * Given an offset, return the bit number which would encode that offset.
 * Local use only.
 */
#define _CLASS_BIT_NUMBER_FROM_OFFSET(byteOffset) \
    (((unsigned int)(byteOffset) - CLASS_SMALLEST_OFFSET) / \
     CLASS_OFFSET_ALIGNMENT)
/*
 * Is the given offset too large to be encoded?
 */
#define CLASS_CAN_ENCODE_OFFSET(byteOffset) \
    (_CLASS_BIT_NUMBER_FROM_OFFSET(byteOffset) < CLASS_BITS_PER_WORD)
/*
 * Return a single bit, encoding the offset.
 * Undefined if the offset is too large, as defined above.
 */
#define CLASS_BIT_FROM_OFFSET(byteOffset) \
    (CLASS_HIGH_BIT >> _CLASS_BIT_NUMBER_FROM_OFFSET(byteOffset))
/*
 * Return an offset, given a bit number as returned from CLZ.
 */
#define CLASS_OFFSET_FROM_CLZ(rshift) \
    (((int)(rshift) * CLASS_OFFSET_ALIGNMENT) + CLASS_SMALLEST_OFFSET)


class ClassObject;

/*
 * Used for iftable in ClassObject.
 */
struct InterfaceEntry {
    /* pointer to interface class */
    ClassObject *clazz;

    /*
     * Index into array of vtable offsets.  This points into the ifviPool,
     * which holds the vtables for all interfaces declared by this class.
     */
    int *methodIndexArray;
};


/*
 * There are three types of objects:
 *  Class objects - an instance of java.lang.Class
 *  Array objects - an object created with a "new array" instruction
 *  Data objects - an object that is neither of the above
 *
 * We also define String objects.  At present they're equivalent to
 * DataObject, but that may change.  (Either way, they make some of the
 * code more obvious.)
 *
 * All objects have an Object header followed by type-specific data.
 */
struct Object {
    /* ptr to class object */
    ClassObject *clazz;

    /*
     * A word containing either a "thin" lock or a "fat" monitor.  See
     * the comments in Sync.c for a description of its layout.
     */
    u4 lock;
};

/*
 * Properly initialize an Object.
 * void DVM_OBJECT_INIT(Object *obj, ClassObject *clazz_)
 */
#define DVM_OBJECT_INIT(obj, clazz_) \
    dvmSetFieldObject(obj, OFFSETOF_MEMBER(Object, clazz), clazz_)

/*
 * Data objects have an Object header followed by their instance data.
 */
struct DataObject : Object {
    /* variable #of u4 slots; u8 uses 2 slots */
    u4 instanceData[1];
};

class ArrayObject;

/*
 * Strings are used frequently enough that we may want to give them their
 * own unique type.
 *
 * Using a dedicated type object to access the instance data provides a
 * performance advantage but makes the java/lang/String.java implementation
 * fragile.
 *
 * Currently this is just equal to DataObject, and we pull the fields out
 * like we do for any other object.
 */
struct StringObject : Object {
    /* variable #of u4 slots; u8 uses 2 slots */
    u4 instanceData[1];

    /** Returns this string's length in characters. */
    int length() const;

    /**
     * Returns this string's length in bytes when encoded as modified UTF-8.
     * Does not include a terminating NUL byte.
     */
    int utfLength() const;

    /** Returns this string's char[] as an ArrayObject. */
    ArrayObject *array() const;

    /** Returns this string's char[] as a u2*. */
    const u2 *chars() const;
};


/*
 * Array objects have these additional fields.
 *
 * We don't currently store the size of each element.  Usually it's implied
 * by the instruction.  If necessary, the width can be derived from
 * the first char of obj->clazz->descriptor.
 */
struct ArrayObject : Object {
    /* number of elements; immutable after init */
    u4 length;

    /*
     * Array contents; actual size is (length * sizeof(type)).  This is
     * declared as u8 so that the compiler inserts any necessary padding
     * (e.g. for EABI); the actual allocation may be smaller than 8 bytes.
     */
    u8 contents[1];
};

/*
 * For classes created early and thus probably in the zygote, the
 * InitiatingLoaderList is kept in gDvm. Later classes use the structure in
 * Object Class. This helps keep zygote pages shared.
 */
struct InitiatingLoaderList {
    /* a list of initiating loader Objects; grown and initialized on demand */
    Object **initiatingLoaders;
    /* count of loaders in the above list */
    int initiatingLoaderCount;
};

/*
 * Generic field header.  We pass this around when we want a generic Field
 * pointer (e.g. for reflection stuff).  Testing the accessFlags for
 * ACC_STATIC allows a proper up-cast.
 */
struct Field {
    ClassObject *clazz;          /* class in which the field is declared */
    const char *name;
    const char *signature;      /* e.g. "I", "[C", "Landroid/os/Debug;" */
    u4 accessFlags;
};

/*
 * Static field.
 */
struct StaticField : Field {
    JValue value;          /* initially set from DEX for primitives */
};

/*
 * Instance field.
 */
struct InstField : Field {
    /*
     * This field indicates the byte offset from the beginning of the
     * (Object *) to the actual instance data; e.g., byteOffset==0 is
     * the same as the object pointer (bug!), and byteOffset==4 is 4
     * bytes farther.
     */
    int byteOffset;
};


/*
 * gcc-style inline management -- ensures we have a copy of all functions
 * in the library, so code that links against us will work whether or not
 * it was built with optimizations enabled.
 */
#ifndef _DEX_GEN_INLINES             /* only defined by DexInlines.c */
# define DEX_INLINE extern __inline__
#else
# define DEX_INLINE
#endif

/* DEX file magic number */
#define DEX_MAGIC       "dex\n"

/* current version, encoded in 4 bytes of ASCII */
#define DEX_MAGIC_VERS  "036\0"

/*
 * older but still-recognized version (corresponding to Android API
 * levels 13 and earlier
 */
#define DEX_MAGIC_VERS_API_13  "035\0"

/* same, but for optimized DEX header */
#define DEX_OPT_MAGIC   "dey\n"
#define DEX_OPT_MAGIC_VERS  "036\0"

#define DEX_DEP_MAGIC   "deps"

/*
 * 160-bit SHA-1 digest.
 */
enum {
    kSHA1DigestLen = 20,
    kSHA1DigestOutputLen = kSHA1DigestLen * 2 + 1
};

/* general constants */
enum {
    kDexEndianConstant = 0x12345678,    /* the endianness indicator */
    kDexNoIndex = 0xffffffff,           /* not a valid index value */
};

/*
 * Enumeration of all the primitive types.
 */
enum PrimitiveType {
    PRIM_NOT = 0,       /* value is a reference type, not a primitive type */
    PRIM_VOID = 1,
    PRIM_BOOLEAN = 2,
    PRIM_BYTE = 3,
    PRIM_SHORT = 4,
    PRIM_CHAR = 5,
    PRIM_INT = 6,
    PRIM_LONG = 7,
    PRIM_FLOAT = 8,
    PRIM_DOUBLE = 9,
};

/*
 * access flags and masks; the "standard" ones are all <= 0x4000
 *
 * Note: There are related declarations in vm/oo/Object.h in the ClassFlags
 * enum.
 */
enum {
    ACC_PUBLIC = 0x00000001,       // class, field, method, ic
    ACC_PRIVATE = 0x00000002,       // field, method, ic
    ACC_PROTECTED = 0x00000004,       // field, method, ic
    ACC_STATIC = 0x00000008,       // field, method, ic
    ACC_FINAL = 0x00000010,       // class, field, method, ic
    ACC_SYNCHRONIZED = 0x00000020,       // method (only allowed on natives)
    ACC_SUPER = 0x00000020,       // class (not used in Dalvik)
    ACC_VOLATILE = 0x00000040,       // field
    ACC_BRIDGE = 0x00000040,       // method (1.5)
    ACC_TRANSIENT = 0x00000080,       // field
    ACC_VARARGS = 0x00000080,       // method (1.5)
    ACC_NATIVE = 0x00000100,       // method
    ACC_INTERFACE = 0x00000200,       // class, ic
    ACC_ABSTRACT = 0x00000400,       // class, method, ic
    ACC_STRICT = 0x00000800,       // method
    ACC_SYNTHETIC = 0x00001000,       // field, method, ic
    ACC_ANNOTATION = 0x00002000,       // class, ic (1.5)
    ACC_ENUM = 0x00004000,       // class, field, ic (1.5)
    ACC_CONSTRUCTOR = 0x00010000,       // method (Dalvik only)
    ACC_DECLARED_SYNCHRONIZED =
    0x00020000,       // method (Dalvik only)
    ACC_CLASS_MASK =
    (ACC_PUBLIC | ACC_FINAL | ACC_INTERFACE | ACC_ABSTRACT
     | ACC_SYNTHETIC | ACC_ANNOTATION | ACC_ENUM),
    ACC_INNER_CLASS_MASK =
    (ACC_CLASS_MASK | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC),
    ACC_FIELD_MASK =
    (ACC_PUBLIC | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC | ACC_FINAL
     | ACC_VOLATILE | ACC_TRANSIENT | ACC_SYNTHETIC | ACC_ENUM),
    ACC_METHOD_MASK =
    (ACC_PUBLIC | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC | ACC_FINAL
     | ACC_SYNCHRONIZED | ACC_BRIDGE | ACC_VARARGS | ACC_NATIVE
     | ACC_ABSTRACT | ACC_STRICT | ACC_SYNTHETIC | ACC_CONSTRUCTOR
     | ACC_DECLARED_SYNCHRONIZED),
};

/* annotation constants */
enum {
    kDexVisibilityBuild = 0x00,     /* annotation visibility */
    kDexVisibilityRuntime = 0x01,
    kDexVisibilitySystem = 0x02,

    kDexAnnotationByte = 0x00,
    kDexAnnotationShort = 0x02,
    kDexAnnotationChar = 0x03,
    kDexAnnotationInt = 0x04,
    kDexAnnotationLong = 0x06,
    kDexAnnotationFloat = 0x10,
    kDexAnnotationDouble = 0x11,
    kDexAnnotationString = 0x17,
    kDexAnnotationType = 0x18,
    kDexAnnotationField = 0x19,
    kDexAnnotationMethod = 0x1a,
    kDexAnnotationEnum = 0x1b,
    kDexAnnotationArray = 0x1c,
    kDexAnnotationAnnotation = 0x1d,
    kDexAnnotationNull = 0x1e,
    kDexAnnotationBoolean = 0x1f,

    kDexAnnotationValueTypeMask = 0x1f,     /* low 5 bits */
    kDexAnnotationValueArgShift = 5,
};

/* map item type codes */
enum {
    kDexTypeHeaderItem = 0x0000,
    kDexTypeStringIdItem = 0x0001,
    kDexTypeTypeIdItem = 0x0002,
    kDexTypeProtoIdItem = 0x0003,
    kDexTypeFieldIdItem = 0x0004,
    kDexTypeMethodIdItem = 0x0005,
    kDexTypeClassDefItem = 0x0006,
    kDexTypeMapList = 0x1000,
    kDexTypeTypeList = 0x1001,
    kDexTypeAnnotationSetRefList = 0x1002,
    kDexTypeAnnotationSetItem = 0x1003,
    kDexTypeClassDataItem = 0x2000,
    kDexTypeCodeItem = 0x2001,
    kDexTypeStringDataItem = 0x2002,
    kDexTypeDebugInfoItem = 0x2003,
    kDexTypeAnnotationItem = 0x2004,
    kDexTypeEncodedArrayItem = 0x2005,
    kDexTypeAnnotationsDirectoryItem = 0x2006,
};

/* auxillary data section chunk codes */
enum {
    kDexChunkClassLookup = 0x434c4b50,   /* CLKP */
    kDexChunkRegisterMaps = 0x524d4150,   /* RMAP */

    kDexChunkEnd = 0x41454e44,   /* AEND */
};

/* debug info opcodes and constants */
enum {
    DBG_END_SEQUENCE = 0x00,
    DBG_ADVANCE_PC = 0x01,
    DBG_ADVANCE_LINE = 0x02,
    DBG_START_LOCAL = 0x03,
    DBG_START_LOCAL_EXTENDED = 0x04,
    DBG_END_LOCAL = 0x05,
    DBG_RESTART_LOCAL = 0x06,
    DBG_SET_PROLOGUE_END = 0x07,
    DBG_SET_EPILOGUE_BEGIN = 0x08,
    DBG_SET_FILE = 0x09,
    DBG_FIRST_SPECIAL = 0x0a,
    DBG_LINE_BASE = -4,
    DBG_LINE_RANGE = 15,
};

/*
 * Direct-mapped "header_item" struct.
 */
struct DexHeader {
    u1 magic[8];           /* includes version number */
    u4 checksum;           /* adler32 checksum */
    u1 signature[kSHA1DigestLen]; /* SHA-1 hash */
    u4 fileSize;           /* length of entire file */
    u4 headerSize;         /* offset to start of next section */
    u4 endianTag;
    u4 linkSize;
    u4 linkOff;
    u4 mapOff;
    u4 stringIdsSize;
    u4 stringIdsOff;
    u4 typeIdsSize;
    u4 typeIdsOff;
    u4 protoIdsSize;
    u4 protoIdsOff;
    u4 fieldIdsSize;
    u4 fieldIdsOff;
    u4 methodIdsSize;
    u4 methodIdsOff;
    u4 classDefsSize;
    u4 classDefsOff;
    u4 dataSize;
    u4 dataOff;
};

/*
 * Direct-mapped "map_item".
 */
struct DexMapItem {
    u2 type;              /* type code (see kDexType* above) */
    u2 unused;
    u4 size;              /* count of items of the indicated type */
    u4 offset;            /* file offset to the start of data */
};

/*
 * Direct-mapped "map_list".
 */
struct DexMapList {
    u4 size;               /* #of entries in list */
    DexMapItem list[1];     /* entries */
};

/*
 * Direct-mapped "string_id_item".
 */
struct DexStringId {
    u4 stringDataOff;      /* file offset to string_data_item */
};

/*
 * Direct-mapped "type_id_item".
 */
struct DexTypeId {
    u4 descriptorIdx;      /* index into stringIds list for type descriptor */
};

/*
 * Direct-mapped "field_id_item".
 */
struct DexFieldId {
    u2 classIdx;           /* index into typeIds list for defining class */
    u2 typeIdx;            /* index into typeIds for field type */
    u4 nameIdx;            /* index into stringIds for field name */
};

/*
 * Direct-mapped "method_id_item".
 */
struct DexMethodId {
    u2 classIdx;           /* index into typeIds list for defining class */
    u2 protoIdx;           /* index into protoIds for method prototype */
    u4 nameIdx;            /* index into stringIds for method name */
};

/*
 * Direct-mapped "proto_id_item".
 */
struct DexProtoId {
    u4 shortyIdx;          /* index into stringIds for shorty descriptor */
    u4 returnTypeIdx;      /* index into typeIds list for return type */
    u4 parametersOff;      /* file offset to type_list for parameter types */
};

/*
 * Direct-mapped "class_def_item".
 */
struct DexClassDef {
    u4 classIdx;           /* index into typeIds for this class */
    u4 accessFlags;
    u4 superclassIdx;      /* index into typeIds for superclass */
    u4 interfacesOff;      /* file offset to DexTypeList */
    u4 sourceFileIdx;      /* index into stringIds for source file name */
    u4 annotationsOff;     /* file offset to annotations_directory_item */
    u4 classDataOff;       /* file offset to class_data_item */
    u4 staticValuesOff;    /* file offset to DexEncodedArray */
};

/*
 * Direct-mapped "type_item".
 */
struct DexTypeItem {
    u2 typeIdx;            /* index into typeIds */
};

/*
 * Direct-mapped "type_list".
 */
struct DexTypeList {
    u4 size;               /* #of entries in list */
    DexTypeItem list[1];    /* entries */
};

/*
 * Direct-mapped "code_item".
 *
 * The "catches" table is used when throwing an exception,
 * "debugInfo" is used when displaying an exception stack trace or
 * debugging. An offset of zero indicates that there are no entries.
 */
struct DexCode {
    u2 registersSize;
    u2 insSize;
    u2 outsSize;
    u2 triesSize;
    u4 debugInfoOff;       /* file offset to debug info stream */
    u4 insnsSize;          /* size of the insns array, in u2 units */
    u2 insns[1];
    /* followed by optional u2 padding */
    /* followed by try_item[triesSize] */
    /* followed by uleb128 handlersSize */
    /* followed by catch_handler_item[handlersSize] */
};

/*
 * Direct-mapped "try_item".
 */
struct DexTry {
    u4 startAddr;          /* start address, in 16-bit code units */
    u2 insnCount;          /* instruction count, in 16-bit code units */
    u2 handlerOff;         /* offset in encoded handler data to handlers */
};

/*
 * Link table.  Currently undefined.
 */
struct DexLink {
    u1 bleargh;
};


/*
 * Direct-mapped "annotations_directory_item".
 */
struct DexAnnotationsDirectoryItem {
    u4 classAnnotationsOff;  /* offset to DexAnnotationSetItem */
    u4 fieldsSize;           /* count of DexFieldAnnotationsItem */
    u4 methodsSize;          /* count of DexMethodAnnotationsItem */
    u4 parametersSize;       /* count of DexParameterAnnotationsItem */
    /* followed by DexFieldAnnotationsItem[fieldsSize] */
    /* followed by DexMethodAnnotationsItem[methodsSize] */
    /* followed by DexParameterAnnotationsItem[parametersSize] */
};

/*
 * Direct-mapped "field_annotations_item".
 */
struct DexFieldAnnotationsItem {
    u4 fieldIdx;
    u4 annotationsOff;             /* offset to DexAnnotationSetItem */
};

/*
 * Direct-mapped "method_annotations_item".
 */
struct DexMethodAnnotationsItem {
    u4 methodIdx;
    u4 annotationsOff;             /* offset to DexAnnotationSetItem */
};

/*
 * Direct-mapped "parameter_annotations_item".
 */
struct DexParameterAnnotationsItem {
    u4 methodIdx;
    u4 annotationsOff;             /* offset to DexAnotationSetRefList */
};

/*
 * Direct-mapped "annotation_set_ref_item".
 */
struct DexAnnotationSetRefItem {
    u4 annotationsOff;             /* offset to DexAnnotationSetItem */
};

/*
 * Direct-mapped "annotation_set_ref_list".
 */
struct DexAnnotationSetRefList {
    u4 size;
    DexAnnotationSetRefItem list[1];
};

/*
 * Direct-mapped "annotation_set_item".
 */
struct DexAnnotationSetItem {
    u4 size;
    u4 entries[1];                 /* offset to DexAnnotationItem */
};

/*
 * Direct-mapped "annotation_item".
 *
 * NOTE: this structure is byte-aligned.
 */
struct DexAnnotationItem {
    u1 visibility;
    u1 annotation[1];              /* data in encoded_annotation format */
};

/*
 * Direct-mapped "encoded_array".
 *
 * NOTE: this structure is byte-aligned.
 */
struct DexEncodedArray {
    u1 array[1];                   /* data in encoded_array format */
};

/*
 * Lookup table for classes.  It provides a mapping from class name to
 * class definition.  Used by dexFindClass().
 *
 * We calculate this at DEX optimization time and embed it in the file so we
 * don't need the same hash table in every VM.  This is slightly slower than
 * a hash table with direct pointers to the items, but because it's shared
 * there's less of a penalty for using a fairly sparse table.
 */
struct DexClassLookup {
    int size;                       // total size, including "size"
    int numEntries;                 // size of table[]; always power of 2
    struct {
        u4 classDescriptorHash;    // class descriptor hash code
        int classDescriptorOffset;  // in bytes, from start of DEX
        int classDefOffset;         // in bytes, from start of DEX
    } table[1];
};

/*
 * Header added by DEX optimization pass.  Values are always written in
 * local byte and structure padding.  The first field (magic + version)
 * is guaranteed to be present and directly readable for all expected
 * compiler configurations; the rest is version-dependent.
 *
 * Try to keep this simple and fixed-size.
 */
struct DexOptHeader {
    u1 magic[8];           /* includes version number */

    u4 dexOffset;          /* file offset of DEX header */
    u4 dexLength;
    u4 depsOffset;         /* offset of optimized DEX dependency table */
    u4 depsLength;
    u4 optOffset;          /* file offset of optimized data tables */
    u4 optLength;

    u4 flags;              /* some info flags */
    u4 checksum;           /* adler32 checksum covering deps/opt */

    /* pad for 64-bit alignment if necessary */
};

#define DEX_OPT_FLAG_BIG            (1<<1)  /* swapped to big-endian */

#define DEX_INTERFACE_CACHE_SIZE    128     /* must be power of 2 */

/*
 * Structure representing a DEX file.
 *
 * Code should regard DexFile as opaque, using the API calls provided here
 * to access specific structures.
 */
struct DexFile {
    /* directly-mapped "opt" header */
    const DexOptHeader *pOptHeader;

    /* pointers to directly-mapped structs and arrays in base DEX */
    const DexHeader *pHeader;
    const DexStringId *pStringIds;
    const DexTypeId *pTypeIds;
    const DexFieldId *pFieldIds;
    const DexMethodId *pMethodIds;
    const DexProtoId *pProtoIds;
    const DexClassDef *pClassDefs;
    const DexLink *pLinkData;

    /*
     * These are mapped out of the "auxillary" section, and may not be
     * included in the file.
     */
    const DexClassLookup *pClassLookup;
    const void *pRegisterMapPool;       // RegisterMapClassPool

    /* points to start of DEX file data */
    const u1 *baseAddr;

    /* track memory overhead for auxillary structures */
    int overhead;

    /* additional app-specific data structures associated with the DEX */
    //void*               auxData;
};

/* bit values for "flags" argument to dexFileParse */
enum {
    kDexParseDefault = 0,
    kDexParseVerifyChecksum = 1,
    kDexParseContinueOnError = (1 << 1),
};


/*
 * Use this to keep track of mapped segments.
 */
struct MemMapping {
    void *addr;           /* start of data */
    size_t length;         /* length of data */

    void *baseAddr;       /* page-aligned base address */
    size_t baseLength;     /* length of mapping */
};


/*
 * Some additional VM data structures that are associated with the DEX file.
 */
struct DvmDex {
    /* pointer to the DexFile we're associated with */
    DexFile *pDexFile;

    /* clone of pDexFile->pHeader (it's used frequently enough) */
    const DexHeader *pHeader;

    /* interned strings; parallel to "stringIds" */
    struct StringObject **pResStrings;

    /* resolved classes; parallel to "typeIds" */
    struct ClassObject **pResClasses;

    /* resolved methods; parallel to "methodIds" */
    struct Method **pResMethods;

    /* resolved instance fields; parallel to "fieldIds" */
    /* (this holds both InstField and StaticField) */
    struct Field **pResFields;

    /* interface method lookup cache */
    struct AtomicCache *pInterfaceCache;

    /* shared memory region with file contents */
    bool isMappedReadOnly;
    MemMapping memMap;

    jobject dex_object;

    /* lock ensuring mutual exclusion during updates */
    pthread_mutex_t modLock;
};


/*
 * This defines the amount of space we leave for field slots in the
 * java.lang.Class definition.  If we alter the class to have more than
 * this many fields, the VM will abort at startup.
 */
#define CLASS_FIELD_SLOTS   4

/*
 * Class objects have many additional fields.  This is used for both
 * classes and interfaces, including synthesized classes (arrays and
 * primitive types).
 *
 * Class objects are unusual in that they have some fields allocated with
 * the system malloc (or LinearAlloc), rather than on the GC heap.  This is
 * handy during initialization, but does require special handling when
 * discarding java.lang.Class objects.
 *
 * The separation of methods (direct vs. virtual) and fields (class vs.
 * instance) used in Dalvik works out pretty well.  The only time it's
 * annoying is when enumerating or searching for things with reflection.
 */
struct ClassObject : Object {
    /* leave space for instance data; we could access fields directly if we
       freeze the definition of java/lang/Class */
    u4 instanceData[CLASS_FIELD_SLOTS];

    /* UTF-8 descriptor for the class; from constant pool, or on heap
       if generated ("[C") */
    const char *descriptor;
    char *descriptorAlloc;

    /* access flags; low 16 bits are defined by VM spec */
    u4 accessFlags;

    /* VM-unique class serial number, nonzero, set very early */
    u4 serialNumber;

    /* DexFile from which we came; needed to resolve constant pool entries */
    /* (will be NULL for VM-generated, e.g. arrays and primitive classes) */
    DvmDex *pDvmDex;

    /* state of class initialization */
    ClassStatus status;

    /* if class verify fails, we must return same error on subsequent tries */
    ClassObject *verifyErrorClass;

    /* threadId, used to check for recursive <clinit> invocation */
    u4 initThreadId;

    /*
     * Total object size; used when allocating storage on gc heap.  (For
     * interfaces and abstract classes this will be zero.)
     */
    size_t objectSize;

    /* arrays only: class object for base element, for instanceof/checkcast
       (for String[][][], this will be String) */
    ClassObject *elementClass;

    /* arrays only: number of dimensions, e.g. int[][] is 2 */
    int arrayDim;

    /* primitive type index, or PRIM_NOT (-1); set for generated prim classes */
    PrimitiveType primitiveType;

    /* superclass, or NULL if this is java.lang.Object */
    ClassObject *super;

    /* defining class loader, or NULL for the "bootstrap" system loader */
    Object *classLoader;

    /* initiating class loader list */
    /* NOTE: for classes with low serialNumber, these are unused, and the
       values are kept in a table in gDvm. */
    InitiatingLoaderList initiatingLoaderList;

    /* array of interfaces this class implements directly */
    int interfaceCount;
    ClassObject **interfaces;

    /* static, private, and <init> methods */
    int directMethodCount;
    Method *directMethods;

    /* virtual methods defined in this class; invoked through vtable */
    int virtualMethodCount;
    Method *virtualMethods;

    /*
     * Virtual method table (vtable), for use by "invoke-virtual".  The
     * vtable from the superclass is copied in, and virtual methods from
     * our class either replace those from the super or are appended.
     */
    int vtableCount;
    Method **vtable;

    /*
     * Interface table (iftable), one entry per interface supported by
     * this class.  That means one entry for each interface we support
     * directly, indirectly via superclass, or indirectly via
     * superinterface.  This will be null if neither we nor our superclass
     * implement any interfaces.
     *
     * Why we need this: given "class Foo implements Face", declare
     * "Face faceObj = new Foo()".  Invoke faceObj.blah(), where "blah" is
     * part of the Face interface.  We can't easily use a single vtable.
     *
     * For every interface a concrete class implements, we create a list of
     * virtualMethod indices for the methods in the interface.
     */
    int iftableCount;
    InterfaceEntry *iftable;

    /*
     * The interface vtable indices for iftable get stored here.  By placing
     * them all in a single pool for each class that implements interfaces,
     * we decrease the number of allocations.
     */
    int ifviPoolCount;
    int *ifviPool;

    /* instance fields
     *
     * These describe the layout of the contents of a DataObject-compatible
     * Object.  Note that only the fields directly defined by this class
     * are listed in ifields;  fields defined by a superclass are listed
     * in the superclass's ClassObject.ifields.
     *
     * All instance fields that refer to objects are guaranteed to be
     * at the beginning of the field list.  ifieldRefCount specifies
     * the number of reference fields.
     */
    int ifieldCount;
    int ifieldRefCount; // number of fields that are object refs
    InstField *ifields;

    /* bitmap of offsets of ifields */
    u4 refOffsets;

    /* source file name, if known */
    const char *sourceFile;

    /* static fields */
    int sfieldCount;
    StaticField sfields[0]; /* MUST be last item */
};

/*
 * Method prototype structure, which refers to a protoIdx in a
 * particular DexFile.
 */
struct DexProto {
    const DexFile *dexFile;     /* file the idx refers to */
    u4 protoIdx;                /* index into proto_ids table of dexFile */
};


/*
 * This is a single variable-size structure.  It may be allocated on the
 * heap or mapped out of a (post-dexopt) DEX file.
 *
 * 32-bit alignment of the structure is NOT guaranteed.  This makes it a
 * little awkward to deal with as a structure; to avoid accidents we use
 * only byte types.  Multi-byte values are little-endian.
 *
 * Size of (format==FormatNone): 1 byte
 * Size of (format==FormatCompact8): 4 + (1 + regWidth) * numEntries
 * Size of (format==FormatCompact16): 4 + (2 + regWidth) * numEntries
 */
struct RegisterMap {
    /* header */
    u1 format;         /* enum RegisterMapFormat; MUST be first entry */
    u1 regWidth;       /* bytes per register line, 1+ */
    u1 numEntries[2];  /* number of entries */

    /* raw data starts here; need not be aligned */
    u1 data[1];
};


/*
 * A method.  We create one of these for every method in every class
 * we load, so try to keep the size to a minimum.
 *
 * Much of this comes from and could be accessed in the data held in shared
 * memory.  We hold it all together here for speed.  Everything but the
 * pointers could be held in a shared table generated by the optimizer;
 * if we're willing to convert them to offsets and take the performance
 * hit (e.g. "meth->insns" becomes "baseAddr + meth->insnsOffset") we
 * could move everything but "nativeFunc".
 */
struct Method {
    /* the class we are a part of */
    ClassObject *clazz;

    /* access flags; low 16 bits are defined by spec (could be u2?) */
    u4 accessFlags;

    /*
     * For concrete virtual methods, this is the offset of the method
     * in "vtable".
     *
     * For abstract methods in an interface class, this is the offset
     * of the method in "iftable[n]->methodIndexArray".
     */
    u2 methodIndex;

    /*
     * Method bounds; not needed for an abstract method.
     *
     * For a native method, we compute the size of the argument list, and
     * set "insSize" and "registerSize" equal to it.
     */
    u2 registersSize;  /* ins + locals */
    u2 outsSize;
    u2 insSize;

    /* method name, e.g. "<init>" or "eatLunch" */
    const char *name;

    /*
     * Method prototype descriptor string (return and argument types).
     *
     * TODO: This currently must specify the DexFile as well as the proto_ids
     * index, because generated Proxy classes don't have a DexFile.  We can
     * remove the DexFile* and reduce the size of this struct if we generate
     * a DEX for proxies.
     */
    DexProto prototype;

    /* short-form method descriptor string */
    const char *shorty;

    /*
     * The remaining items are not used for abstract or native methods.
     * (JNI is currently hijacking "insns" as a function pointer, set
     * after the first call.  For internal-native this stays null.)
     */

    /* the actual code */
    const u2 *insns;          /* instructions, in memory-mapped .dex */

    /* JNI: cached argument and return-type hints */
    int jniArgInfo;

    /*
     * JNI: native method ptr; could be actual function or a JNI bridge.  We
     * don't currently discriminate between DalvikBridgeFunc and
     * DalvikNativeFunc; the former takes an argument superset (i.e. two
     * extra args) which will be ignored.  If necessary we can use
     * insns==NULL to detect JNI bridge vs. internal native.
     */
    DalvikBridgeFunc nativeFunc;

    /*
     * JNI: true if this static non-synchronized native method (that has no
     * reference arguments) needs a JNIEnv* and jclass/jobject. Libcore
     * uses this.
     */
    bool fastJni;

    /*
     * JNI: true if this method has no reference arguments. This lets the JNI
     * bridge avoid scanning the shorty for direct pointers that need to be
     * converted to local references.
     *
     * TODO: replace this with a list of indexes of the reference arguments.
     */
    bool noRef;

    /*
     * JNI: true if we should log entry and exit. This is the only way
     * developers can log the local references that are passed into their code.
     * Used for debugging JNI problems in third-party code.
     */
    bool shouldTrace;

    /*
     * Register map data, if available.  This will point into the DEX file
     * if the data was computed during pre-verification, or into the
     * linear alloc area if not.
     */
    const RegisterMap *registerMap;

    /* set if method was called during method profiling */
    bool inProfile;
};


#if defined(CHECK_MUTEX) && !defined(__USE_UNIX98)
/* glibc lacks this unless you #define __USE_UNIX98 */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
enum { PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP };
#endif

/*
 * Current status; these map to JDWP constants, so don't rearrange them.
 * (If you do alter this, update the strings in dvmDumpThread and the
 * conversion table in VMThread.java.)
 *
 * Note that "suspended" is orthogonal to these values (so says JDWP).
 */
enum ThreadStatus {
    THREAD_UNDEFINED = -1,       /* makes enum compatible with int32_t */

    /* these match up with JDWP values */
            THREAD_ZOMBIE = 0,        /* TERMINATED */
    THREAD_RUNNING = 1,        /* RUNNABLE or running now */
    THREAD_TIMED_WAIT = 2,        /* TIMED_WAITING in Object.wait() */
    THREAD_MONITOR = 3,        /* BLOCKED on a monitor */
    THREAD_WAIT = 4,        /* WAITING in Object.wait() */
    /* non-JDWP states */
            THREAD_INITIALIZING = 5,        /* allocated, not yet running */
    THREAD_STARTING = 6,        /* started, not yet on thread list */
    THREAD_NATIVE = 7,        /* off in a JNI native method */
    THREAD_VMWAIT = 8,        /* waiting on a VM resource */
    THREAD_SUSPENDED = 9,        /* suspended, usually by GC or debugger */
};

/* thread priorities, from java.lang.Thread */
enum {
    THREAD_MIN_PRIORITY = 1,
    THREAD_NORM_PRIORITY = 5,
    THREAD_MAX_PRIORITY = 10,
};


/* initialization */
bool dvmThreadStartup(void);

void dvmThreadShutdown(void);

void dvmSlayDaemons(void);


#define kJniLocalRefMin         64
#define kJniLocalRefMax         512     /* arbitrary; should be plenty */
#define kInternalRefDefault     32      /* equally arbitrary */
#define kInternalRefMax         4096    /* mainly a sanity check */

#define kMinStackSize       (512 + STACK_OVERFLOW_RESERVE)
#define kDefaultStackSize   (16*1024)   /* four 4K pages */
#define kMaxStackSize       (256*1024 + STACK_OVERFLOW_RESERVE)

/*
 * Interpreter control struction.  Packed into a long long to enable
 * atomic updates.
 */
union InterpBreak {
    volatile int64_t all;
    struct {
        uint16_t subMode;
        uint8_t breakFlags;
        int8_t unused;   /* for future expansion */
#ifndef DVM_NO_ASM_INTERP
        void *curHandlerTable;
#else
        int32_t    unused1;
#endif
    } ctl;
};


/*
 * For x86 JIT. In the lowered code sequences for bytecodes, at most 10
 * temporary variables may be live at the same time. Therefore, at most
 * 10 temporary variables can be spilled at the same time.
*/
#define MAX_SPILL_JIT_IA 10

/*
 * Execution mode, e.g. interpreter vs. JIT.
 */
enum ExecutionMode {
    kExecutionModeUnknown = 0,
    kExecutionModeInterpPortable,
    kExecutionModeInterpFast,
#if defined(WITH_JIT)
    kExecutionModeJit,
#endif
#if defined(WITH_JIT)  /* IA only */
    kExecutionModeNcgO0,
    kExecutionModeNcgO1,
#endif
};

/*
 * Execution sub modes, e.g. debugging, profiling, etc.
 * Treated as bit flags for fast access.  These values are used directly
 * by assembly code in the mterp interpeter and may also be used by
 * code generated by the JIT.  Take care when changing.
 */
enum ExecutionSubModes {
    kSubModeNormal = 0x0000,   /* No active subMode */
    kSubModeMethodTrace = 0x0001,
    kSubModeEmulatorTrace = 0x0002,
    kSubModeInstCounting = 0x0004,
    kSubModeDebuggerActive = 0x0008,
    kSubModeSuspendPending = 0x0010,
    kSubModeCallbackPending = 0x0020,
    kSubModeCountedStep = 0x0040,
    kSubModeCheckAlways = 0x0080,
    kSubModeSampleTrace = 0x0100,
    kSubModeJitTraceBuild = 0x4000,
    kSubModeJitSV = 0x8000,
    kSubModeDebugProfile = (kSubModeMethodTrace |
                            kSubModeEmulatorTrace |
                            kSubModeInstCounting |
                            kSubModeDebuggerActive)
};

/*
 * Interpreter break flags.  When set, causes the interpreter to
 * break from normal execution and invoke the associated callback
 * handler.
 */

enum InterpBreakFlags {
    kInterpNoBreak = 0x00,    /* Don't check */
    kInterpSingleStep = 0x01,    /* Check between each inst */
    kInterpSafePoint = 0x02,    /* Check at safe points */
};

/*
 * Mapping between subModes and required check intervals.  Note: in
 * the future we might want to make this mapping target-dependent.
 */
#define SINGLESTEP_BREAK_MASK ( kSubModeInstCounting | \
                                kSubModeDebuggerActive | \
                                kSubModeCountedStep | \
                                kSubModeCheckAlways | \
                                kSubModeJitSV | \
                                kSubModeJitTraceBuild )

#define SAFEPOINT_BREAK_MASK  ( kSubModeSuspendPending | \
                                kSubModeCallbackPending )

typedef bool (*SafePointCallback)(struct Thread *thread, void *arg);

/*
 * Identify which break and submode flags should be local
 * to an interpreter activation.
 */
#define LOCAL_SUBMODE (kSubModeJitTraceBuild)

struct InterpSaveState {
    const u2 *pc;         // Dalvik PC
    u4 *curFrame;   // Dalvik frame pointer
    const Method *method;    // Method being executed
    DvmDex *methodClassDex;
    JValue retval;
    void *bailPtr;
#if defined(WITH_TRACKREF_CHECKS)
    int             debugTrackedRefStart;
#else
    int unused;        // Keep struct size constant
#endif
    struct InterpSaveState *prev;  // To follow nested activations
} __attribute__ ((__packed__));

#ifdef WITH_JIT
/*
 * NOTE: Only entry points dispatched via [self + #offset] are put
 * in this struct, and there are six of them:
 * 1) dvmJitToInterpNormal: find if there is a corresponding compilation for
 *    the new dalvik PC. If so, chain the originating compilation with the
 *    target then jump to it. If the destination trace doesn't exist, update
 *    the profile count for that Dalvik PC.
 * 2) dvmJitToInterpNoChain: similar to dvmJitToInterpNormal but chaining is
 *    not performed.
 * 3) dvmJitToInterpPunt: use the fast interpreter to execute the next
 *    instruction(s) and stay there as long as it is appropriate to return
 *    to the compiled land. This is used when the jit'ed code is about to
 *    throw an exception.
 * 4) dvmJitToInterpSingleStep: use the portable interpreter to execute the
 *    next instruction only and return to pre-specified location in the
 *    compiled code to resume execution. This is mainly used as debugging
 *    feature to bypass problematic opcode implementations without
 *    disturbing the trace formation.
 * 5) dvmJitToTraceSelect: Similar to dvmJitToInterpNormal except for the
 *    profiling operation. If the new Dalvik PC is dominated by an already
 *    translated trace, directly request a new translation if the destinaion
 *    trace doesn't exist.
 * 6) dvmJitToBackwardBranch: special case for SELF_VERIFICATION when the
 *    destination Dalvik PC is included by the trace itself.
 */
struct JitToInterpEntries {
    void (*dvmJitToInterpNormal)(void);
    void (*dvmJitToInterpNoChain)(void);
    void (*dvmJitToInterpPunt)(void);
    void (*dvmJitToInterpSingleStep)(void);
    void (*dvmJitToInterpTraceSelect)(void);
#if defined(WITH_SELF_VERIFICATION)
    void (*dvmJitToInterpBackwardBranch)(void);
#else
    void (*unused)(void);  // Keep structure size constant
#endif
};

/* States of the interpreter when serving a JIT-related request */
enum JitState {
    /* Entering states in the debug interpreter */
    kJitNot = 0,               // Non-JIT related reasons */
    kJitTSelectRequest = 1,    // Request a trace (subject to filtering)
    kJitTSelectRequestHot = 2, // Request a hot trace (bypass the filter)
    kJitSelfVerification = 3,  // Self Verification Mode

    /* Operational states in the debug interpreter */
    kJitTSelect = 4,           // Actively selecting a trace
    kJitTSelectEnd = 5,        // Done with the trace - wrap it up
    kJitDone = 6,              // No further JIT actions for interpBreak
};

#if defined(WITH_SELF_VERIFICATION)
enum SelfVerificationState {
    kSVSIdle = 0,           // Idle
    kSVSStart = 1,          // Shadow space set up, running compiled code
    kSVSPunt = 2,           // Exiting compiled code by punting
    kSVSSingleStep = 3,     // Exiting compiled code by single stepping
    kSVSNoProfile = 4,      // Exiting compiled code and don't collect profiles
    kSVSTraceSelect = 5,    // Exiting compiled code and compile the next pc
    kSVSNormal = 6,         // Exiting compiled code normally
    kSVSNoChain = 7,        // Exiting compiled code by no chain
    kSVSBackwardBranch = 8, // Exiting compiled code with backward branch trace
    kSVSDebugInterp = 9,    // Normal state restored, running debug interpreter
};
#endif

/* Number of entries in the 2nd level JIT profiler filter cache */
#define JIT_TRACE_THRESH_FILTER_SIZE 32
/* Number of low dalvik pc address bits to include in 2nd level filter key */
#define JIT_TRACE_THRESH_FILTER_PC_BITS 16
#define MAX_JIT_RUN_LEN 64

enum JitHint {
   kJitHintNone = 0,
   kJitHintTaken = 1,         // Last inst in run was taken branch
   kJitHintNotTaken = 2,      // Last inst in run was not taken branch
   kJitHintNoBias = 3,        // Last inst in run was unbiased branch
};

/*
 * Element of a Jit trace description. If the isCode bit is set, it describes
 * a contiguous sequence of Dalvik byte codes.
 */
struct JitCodeDesc {
    unsigned numInsts:8;     // Number of Byte codes in run
    unsigned runEnd:1;       // Run ends with last byte code
    JitHint hint:7;          // Hint to apply to final code of run
    u2 startOffset;          // Starting offset for trace run
};

/*
 * A complete list of trace runs passed to the compiler looks like the
 * following:
 *   frag1
 *   frag2
 *   frag3
 *   meta1
 *     :
 *   metan
 *   frag4
 *
 * frags 1-4 have the "isCode" field set and describe the location/length of
 * real code traces, while metas 1-n are misc information.
 * The meaning of the meta content is loosely defined. It is usually the code
 * fragment right before the first meta field (frag3 in this case) to
 * understand and parse them. Frag4 could be a dummy one with 0 "numInsts" but
 * the "runEnd" field set.
 *
 * For example, if a trace run contains a method inlining target, the class
 * descriptor/loader of "this" and the currently resolved method pointer are
 * three instances of meta information stored there.
 */
struct JitTraceRun {
    union {
        JitCodeDesc frag;
        void*       meta;
    } info;
    u4 isCode:1;
    u4 unused:31;
};

#if defined(ARCH_IA32)
/*
 * JIT code genarator optimization level
 */
enum JitOptLevel {
    kJitOptLevelO0 = 0,
    kJitOptLevelO1 = 1,
};
#endif  // #if defined(ARCH_IA32)
#endif


/*
 * Maintain a table of indirect references.  Used for local/global JNI
 * references.
 *
 * The table contains object references that are part of the GC root set.
 * When an object is added we return an IndirectRef that is not a valid
 * pointer but can be used to find the original value in O(1) time.
 * Conversions to and from indirect refs are performed on JNI method calls
 * in and out of the VM, so they need to be very fast.
 *
 * To be efficient for JNI local variable storage, we need to provide
 * operations that allow us to operate on segments of the table, where
 * segments are pushed and popped as if on a stack.  For example, deletion
 * of an entry should only succeed if it appears in the current segment,
 * and we want to be able to strip off the current segment quickly when
 * a method returns.  Additions to the table must be made in the current
 * segment even if space is available in an earlier area.
 *
 * A new segment is created when we call into native code from interpreted
 * code, or when we handle the JNI PushLocalFrame function.
 *
 * The GC must be able to scan the entire table quickly.
 *
 * In summary, these must be very fast:
 *  - adding or removing a segment
 *  - adding references to a new segment
 *  - converting an indirect reference back to an Object
 * These can be a little slower, but must still be pretty quick:
 *  - adding references to a "mature" segment
 *  - removing individual references
 *  - scanning the entire table straight through
 *
 * If there's more than one segment, we don't guarantee that the table
 * will fill completely before we fail due to lack of space.  We do ensure
 * that the current segment will pack tightly, which should satisfy JNI
 * requirements (e.g. EnsureLocalCapacity).
 *
 * To make everything fit nicely in 32-bit integers, the maximum size of
 * the table is capped at 64K.
 *
 * None of the table functions are synchronized.
 */

/*
 * Indirect reference definition.  This must be interchangeable with JNI's
 * jobject, and it's convenient to let null be null, so we use void*.
 *
 * We need a 16-bit table index and a 2-bit reference type (global, local,
 * weak global).  Real object pointers will have zeroes in the low 2 or 3
 * bits (4- or 8-byte alignment), so it's useful to put the ref type
 * in the low bits and reserve zero as an invalid value.
 *
 * The remaining 14 bits can be used to detect stale indirect references.
 * For example, if objects don't move, we can use a hash of the original
 * Object* to make sure the entry hasn't been re-used.  (If the Object*
 * we find there doesn't match because of heap movement, we could do a
 * secondary check on the preserved hash value; this implies that creating
 * a global/local ref queries the hash value and forces it to be saved.)
 *
 * A more rigorous approach would be to put a serial number in the extra
 * bits, and keep a copy of the serial number in a parallel table.  This is
 * easier when objects can move, but requires 2x the memory and additional
 * memory accesses on add/get.  It will catch additional problems, e.g.:
 * create iref1 for obj, delete iref1, create iref2 for same obj, lookup
 * iref1.  A pattern based on object bits will miss this.
 *
 * For now, we use a serial number.
 */
typedef void *IndirectRef;

/* magic failure value; must not pass dvmIsHeapAddress() */
#define kInvalidIndirectRefObject reinterpret_cast<Object*>(0xdead4321)

#define kClearedJniWeakGlobal reinterpret_cast<Object*>(0xdead1234)

/*
 * Indirect reference kind, used as the two low bits of IndirectRef.
 *
 * For convenience these match up with enum jobjectRefType from jni.h.
 */
enum IndirectRefKind {
    kIndirectKindInvalid = 0,
    kIndirectKindLocal = 1,
    kIndirectKindGlobal = 2,
    kIndirectKindWeakGlobal = 3
};

/*
 * Information we store for each slot in the reference table.
 */
struct IndirectRefSlot {
    Object *obj;        /* object pointer itself, NULL if the slot is unused */
    u4 serial;     /* slot serial number */
};

/* use as initial value for "cookie", and when table has only one segment */
#define IRT_FIRST_SEGMENT   0

/*
 * Table definition.
 *
 * For the global reference table, the expected common operations are
 * adding a new entry and removing a recently-added entry (usually the
 * most-recently-added entry).  For JNI local references, the common
 * operations are adding a new entry and removing an entire table segment.
 *
 * If "alloc_entries_" is not equal to "max_entries_", the table may expand
 * when entries are added, which means the memory may move.  If you want
 * to keep pointers into "table" rather than offsets, you must use a
 * fixed-size table.
 *
 * If we delete entries from the middle of the list, we will be left with
 * "holes".  We track the number of holes so that, when adding new elements,
 * we can quickly decide to do a trivial append or go slot-hunting.
 *
 * When the top-most entry is removed, any holes immediately below it are
 * also removed.  Thus, deletion of an entry may reduce "topIndex" by more
 * than one.
 *
 * To get the desired behavior for JNI locals, we need to know the bottom
 * and top of the current "segment".  The top is managed internally, and
 * the bottom is passed in as a function argument (the VM keeps it in a
 * slot in the interpreted stack frame).  When we call a native method or
 * push a local frame, the current top index gets pushed on, and serves
 * as the new bottom.  When we pop a frame off, the value from the stack
 * becomes the new top index, and the value stored in the previous frame
 * becomes the new bottom.
 *
 * To avoid having to re-scan the table after a pop, we want to push the
 * number of holes in the table onto the stack.  Because of our 64K-entry
 * cap, we can combine the two into a single unsigned 32-bit value.
 * Instead of a "bottom" argument we take a "cookie", which includes the
 * bottom index and the count of holes below the bottom.
 *
 * We need to minimize method call/return overhead.  If we store the
 * "cookie" externally, on the interpreted call stack, the VM can handle
 * pushes and pops with a single 4-byte load and store.  (We could also
 * store it internally in a public structure, but the local JNI refs are
 * logically tied to interpreted stack frames anyway.)
 *
 * Common alternative implementation: make IndirectRef a pointer to the
 * actual reference slot.  Instead of getting a table and doing a lookup,
 * the lookup can be done instantly.  Operations like determining the
 * type and deleting the reference are more expensive because the table
 * must be hunted for (i.e. you have to do a pointer comparison to see
 * which table it's in), you can't move the table when expanding it (so
 * realloc() is out), and tricks like serial number checking to detect
 * stale references aren't possible (though we may be able to get similar
 * benefits with other approaches).
 *
 * TODO: consider a "lastDeleteIndex" for quick hole-filling when an
 * add immediately follows a delete; must invalidate after segment pop
 * (which could increase the cost/complexity of method call/return).
 * Might be worth only using it for JNI globals.
 *
 * TODO: may want completely different add/remove algorithms for global
 * and local refs to improve performance.  A large circular buffer might
 * reduce the amortized cost of adding global references.
 *
 * TODO: if we can guarantee that the underlying storage doesn't move,
 * e.g. by using oversized mmap regions to handle expanding tables, we may
 * be able to avoid having to synchronize lookups.  Might make sense to
 * add a "synchronized lookup" call that takes the mutex as an argument,
 * and either locks or doesn't lock based on internal details.
 */
union IRTSegmentState {
    u4 all;
    struct {
        u4 topIndex:16;            /* index of first unused entry */
        u4 numHoles:16;            /* #of holes in entire table */
    } parts;
};

class iref_iterator {
private:

    IndirectRefSlot *table_;
    size_t i_;
    size_t capacity_;
};


struct IndirectRefTable {

    /* semi-public - read/write by interpreter in native call handler */
    IRTSegmentState segmentState;

    /*
     * private:
     *
     * TODO: we can't make these private as long as the interpreter
     * uses offsetof, since private member data makes us non-POD.
     */
    /* bottom of the stack */
    IndirectRefSlot *table_;
    /* bit mask, ORed into all irefs */
    IndirectRefKind kind_;
    /* #of entries we have space for */
    size_t alloc_entries_;
    /* max #of entries allowed */
    size_t max_entries_;

};


/*
 * Table definition.
 *
 * The expected common operations are adding a new entry and removing a
 * recently-added entry (usually the most-recently-added entry).
 *
 * If "allocEntries" is not equal to "maxEntries", the table may expand when
 * entries are added, which means the memory may move.  If you want to keep
 * pointers into "table" rather than offsets, use a fixed-size table.
 *
 * (This structure is still somewhat transparent; direct access to
 * table/nextEntry is allowed.)
 */
struct ReferenceTable {
    Object **nextEntry;          /* top of the list */
    Object **table;              /* bottom of the list */

    int allocEntries;       /* #of entries we have space for */
    int maxEntries;         /* max #of entries allowed */
};


/*
 * Every Object has a monitor associated with it, but not every Object is
 * actually locked.  Even the ones that are locked do not need a
 * full-fledged monitor until a) there is actual contention or b) wait()
 * is called on the Object.
 *
 * For Dalvik, we have implemented a scheme similar to the one described
 * in Bacon et al.'s "Thin locks: featherweight synchronization for Java"
 * (ACM 1998).  Things are even easier for us, though, because we have
 * a full 32 bits to work with.
 *
 * The two states of an Object's lock are referred to as "thin" and
 * "fat".  A lock may transition from the "thin" state to the "fat"
 * state and this transition is referred to as inflation.  Once a lock
 * has been inflated it remains in the "fat" state indefinitely.
 *
 * The lock value itself is stored in Object.lock.  The LSB of the
 * lock encodes its state.  When cleared, the lock is in the "thin"
 * state and its bits are formatted as follows:
 *
 *    [31 ---- 19] [18 ---- 3] [2 ---- 1] [0]
 *     lock count   thread id  hash state  0
 *
 * When set, the lock is in the "fat" state and its bits are formatted
 * as follows:
 *
 *    [31 ---- 3] [2 ---- 1] [0]
 *      pointer   hash state  1
 *
 * For an in-depth description of the mechanics of thin-vs-fat locking,
 * read the paper referred to above.
 */

class Thread;

/*
 * Monitors provide:
 *  - mutually exclusive access to resources
 *  - a way for multiple threads to wait for notification
 *
 * In effect, they fill the role of both mutexes and condition variables.
 *
 * Only one thread can own the monitor at any time.  There may be several
 * threads waiting on it (the wait call unlocks it).  One or more waiting
 * threads may be getting interrupted or notified at any given time.
 *
 * TODO: the various members of monitor are not SMP-safe.
 */
struct Monitor {
    Thread *owner;          /* which thread currently owns the lock? */
    int lockCount;      /* owner's recursive lock depth */
    Object *obj;            /* what object are we part of [debug only] */

    Thread *waitSet;    /* threads currently waiting on this monitor */

    pthread_mutex_t lock;

    Monitor *next;

    /*
     * Who last acquired this monitor, when lock sampling is enabled.
     * Even when enabled, ownerMethod may be NULL.
     */
    const Method *ownerMethod;
    u4 ownerPc;
};

/*
 * Error constants.
 */
enum JdwpError {
    ERR_NONE = 0,
    ERR_INVALID_THREAD = 10,
    ERR_INVALID_THREAD_GROUP = 11,
    ERR_INVALID_PRIORITY = 12,
    ERR_THREAD_NOT_SUSPENDED = 13,
    ERR_THREAD_SUSPENDED = 14,
    ERR_INVALID_OBJECT = 20,
    ERR_INVALID_CLASS = 21,
    ERR_CLASS_NOT_PREPARED = 22,
    ERR_INVALID_METHODID = 23,
    ERR_INVALID_LOCATION = 24,
    ERR_INVALID_FIELDID = 25,
    ERR_INVALID_FRAMEID = 30,
    ERR_NO_MORE_FRAMES = 31,
    ERR_OPAQUE_FRAME = 32,
    ERR_NOT_CURRENT_FRAME = 33,
    ERR_TYPE_MISMATCH = 34,
    ERR_INVALID_SLOT = 35,
    ERR_DUPLICATE = 40,
    ERR_NOT_FOUND = 41,
    ERR_INVALID_MONITOR = 50,
    ERR_NOT_MONITOR_OWNER = 51,
    ERR_INTERRUPT = 52,
    ERR_INVALID_CLASS_FORMAT = 60,
    ERR_CIRCULAR_CLASS_DEFINITION = 61,
    ERR_FAILS_VERIFICATION = 62,
    ERR_ADD_METHOD_NOT_IMPLEMENTED = 63,
    ERR_SCHEMA_CHANGE_NOT_IMPLEMENTED = 64,
    ERR_INVALID_TYPESTATE = 65,
    ERR_HIERARCHY_CHANGE_NOT_IMPLEMENTED = 66,
    ERR_DELETE_METHOD_NOT_IMPLEMENTED = 67,
    ERR_UNSUPPORTED_VERSION = 68,
    ERR_NAMES_DONT_MATCH = 69,
    ERR_CLASS_MODIFIERS_CHANGE_NOT_IMPLEMENTED = 70,
    ERR_METHOD_MODIFIERS_CHANGE_NOT_IMPLEMENTED = 71,
    ERR_NOT_IMPLEMENTED = 99,
    ERR_NULL_POINTER = 100,
    ERR_ABSENT_INFORMATION = 101,
    ERR_INVALID_EVENT_TYPE = 102,
    ERR_ILLEGAL_ARGUMENT = 103,
    ERR_OUT_OF_MEMORY = 110,
    ERR_ACCESS_DENIED = 111,
    ERR_VM_DEAD = 112,
    ERR_INTERNAL = 113,
    ERR_UNATTACHED_THREAD = 115,
    ERR_INVALID_TAG = 500,
    ERR_ALREADY_INVOKING = 502,
    ERR_INVALID_INDEX = 503,
    ERR_INVALID_LENGTH = 504,
    ERR_INVALID_STRING = 506,
    ERR_INVALID_CLASS_LOADER = 507,
    ERR_INVALID_ARRAY = 508,
    ERR_TRANSPORT_LOAD = 509,
    ERR_TRANSPORT_INIT = 510,
    ERR_NATIVE_METHOD = 511,
    ERR_INVALID_COUNT = 512,
};


/*
 * ClassStatus constants.  These are bit flags that can be ORed together.
 */
enum JdwpClassStatus {
    CS_VERIFIED = 0x01,
    CS_PREPARED = 0x02,
    CS_INITIALIZED = 0x04,
    CS_ERROR = 0x08,
};

/*
 * EventKind constants.
 */
enum JdwpEventKind {
    EK_SINGLE_STEP = 1,
    EK_BREAKPOINT = 2,
    EK_FRAME_POP = 3,
    EK_EXCEPTION = 4,
    EK_USER_DEFINED = 5,
    EK_THREAD_START = 6,
    EK_THREAD_END = 7,
    EK_CLASS_PREPARE = 8,
    EK_CLASS_UNLOAD = 9,
    EK_CLASS_LOAD = 10,
    EK_FIELD_ACCESS = 20,
    EK_FIELD_MODIFICATION = 21,
    EK_EXCEPTION_CATCH = 30,
    EK_METHOD_ENTRY = 40,
    EK_METHOD_EXIT = 41,
    EK_VM_INIT = 90,
    EK_VM_DEATH = 99,
    EK_VM_DISCONNECTED = 100,  /* "Never sent across JDWP */
    EK_VM_START = EK_VM_INIT,
    EK_THREAD_DEATH = EK_THREAD_END,
};

/*
 * Values for "modKind" in EventRequest.Set.
 */
enum JdwpModKind {
    MK_COUNT = 1,
    MK_CONDITIONAL = 2,
    MK_THREAD_ONLY = 3,
    MK_CLASS_ONLY = 4,
    MK_CLASS_MATCH = 5,
    MK_CLASS_EXCLUDE = 6,
    MK_LOCATION_ONLY = 7,
    MK_EXCEPTION_ONLY = 8,
    MK_FIELD_ONLY = 9,
    MK_STEP = 10,
    MK_INSTANCE_ONLY = 11,
};

/*
 * InvokeOptions constants (bit flags).
 */
enum JdwpInvokeOptions {
    INVOKE_SINGLE_THREADED = 0x01,
    INVOKE_NONVIRTUAL = 0x02,
};

/*
 * StepDepth constants.
 */
enum JdwpStepDepth {
    SD_INTO = 0,    /* step into method calls */
    SD_OVER = 1,    /* step over method calls */
    SD_OUT = 2,    /* step out of current method */
};

/*
 * StepSize constants.
 */
enum JdwpStepSize {
    SS_MIN = 0,    /* step by minimum (e.g. 1 bytecode inst) */
    SS_LINE = 1,    /* if possible, step to next line */
};

/*
 * SuspendPolicy constants.
 */
enum JdwpSuspendPolicy {
    SP_NONE = 0,    /* suspend no threads */
    SP_EVENT_THREAD = 1,    /* suspend event thread */
    SP_ALL = 2,    /* suspend all threads */
};

/*
 * SuspendStatus constants.
 */
enum JdwpSuspendStatus {
    SUSPEND_STATUS_NOT_SUSPENDED = 0,
    SUSPEND_STATUS_SUSPENDED = 1,
};

/*
 * ThreadStatus constants.
 */
enum JdwpThreadStatus {
    TS_ZOMBIE = 0,
    TS_RUNNING = 1,        // RUNNING
    TS_SLEEPING = 2,        // (in Thread.sleep())
    TS_MONITOR = 3,        // WAITING (monitor wait)
    TS_WAIT = 4,        // (in Object.wait())
};

/*
 * TypeTag constants.
 */
enum JdwpTypeTag {
    TT_CLASS = 1,
    TT_INTERFACE = 2,
    TT_ARRAY = 3,
};

/*
 * Tag constants.
 */
enum JdwpType {
    JT_ARRAY = '[',
    JT_BYTE = 'B',
    JT_CHAR = 'C',
    JT_OBJECT = 'L',
    JT_FLOAT = 'F',
    JT_DOUBLE = 'D',
    JT_INT = 'I',
    JT_LONG = 'J',
    JT_SHORT = 'S',
    JT_VOID = 'V',
    JT_BOOLEAN = 'Z',
    JT_STRING = 's',
    JT_THREAD = 't',
    JT_THREAD_GROUP = 'g',
    JT_CLASS_LOADER = 'l',
    JT_CLASS_OBJECT = 'c',
};


/*
 * Fundamental types.
 *
 * ObjectId and RefTypeId must be the same size.
 */
typedef u4 FieldId;     /* static or instance field */
typedef u4 MethodId;    /* any kind of method, including constructors */
typedef u8 ObjectId;    /* any object (threadID, stringID, arrayID, etc) */
typedef u8 RefTypeId;   /* like ObjectID, but unique for Class objects */
typedef u8 FrameId;     /* short-lived stack frame ID */


/*
 * Holds a JDWP "location".
 */
struct JdwpLocation {
    u1 typeTag;        /* class or interface? */
    RefTypeId classId;        /* method->clazz */
    MethodId methodId;       /* method in which "idx" resides */
    u8 idx;            /* relative index into code block */
};

/*
 * How we talk to the debugger.
 */
enum JdwpTransportType {
    kJdwpTransportUnknown = 0,
    kJdwpTransportSocket,       /* transport=dt_socket */
    kJdwpTransportAndroidAdb,   /* transport=dt_android_adb */
};

/*
 * Holds collection of JDWP initialization parameters.
 */
struct JdwpStartupParams {
    JdwpTransportType transport;
    bool server;
    bool suspend;
    char host[64];
    short port;
    /* more will be here someday */
};


/*
 * Used by StepControl to track a set of addresses associated with
 * a single line.
 */
struct AddressSet {
    u4 setSize;
    u1 set[1];
};


/*
 * Single-step management.
 */
struct StepControl {
    /* request */
    JdwpStepSize size;
    JdwpStepDepth depth;
    Thread *thread;         /* don't deref; for comparison only */

    /* current state */
    bool active;
    const Method *method;
    int line;           /* line #; could be -1 */
    const AddressSet *pAddressSet;    /* if non-null, address set for line */
    int frameDepth;
};

/*
 * Invoke-during-breakpoint support.
 */
struct DebugInvokeReq {
    /* boolean; only set when we're in the tail end of an event handler */
    bool ready;

    /* boolean; set if the JDWP thread wants this thread to do work */
    bool invokeNeeded;

    /* request */
    Object *obj;        /* not used for ClassType.InvokeMethod */
    Object *thread;
    ClassObject *clazz;
    Method *method;
    u4 numArgs;
    u8 *argArray;   /* will be NULL if numArgs==0 */
    u4 options;

    /* result */
    JdwpError err;
    u1 resultTag;
    JValue resultValue;
    ObjectId exceptObj;

    /* condition variable to wait on while the method executes */
    pthread_mutex_t lock;
    pthread_cond_t cv;
};


/*
 * Method trace state.  This is currently global.  In theory we could make
 * most of this per-thread.
 */
struct MethodTraceState {
    /* active state */
    pthread_mutex_t startStopLock;
    pthread_cond_t threadExitCond;
    FILE *traceFile;
    bool directToDdms;
    int bufferSize;
    int flags;

    int traceEnabled;
    u1 *buf;
    volatile int curOffset;
    u8 startWhen;
    int overflow;

    int traceVersion;
    size_t recordSize;

    bool samplingEnabled;
    pthread_t samplingThreadHandle;
};

/*
 * Memory allocation profiler state.  This is used both globally and
 * per-thread.
 *
 * If you add a field here, zero it out in dvmStartAllocCounting().
 */
struct AllocProfState {
    bool enabled;            // is allocation tracking enabled?

    int allocCount;         // #of objects allocated
    int allocSize;          // cumulative size of objects

    int failedAllocCount;   // #of times an allocation failed
    int failedAllocSize;    // cumulative size of failed allocations

    int freeCount;          // #of objects freed
    int freeSize;           // cumulative size of freed objects

    int gcCount;            // #of times an allocation triggered a GC

    int classInitCount;     // #of initialized classes
    u8 classInitTime;      // cumulative time spent in class init (nsec)
};

/*
 * Returns current method tracing mode.
 */
enum TracingMode {
    TRACING_INACTIVE,
    METHOD_TRACING_ACTIVE,
    SAMPLE_PROFILING_ACTIVE,
};


/*
 * Our per-thread data.
 *
 * These are allocated on the system heap.
 */
struct Thread {
    /*
     * Interpreter state which must be preserved across nested
     * interpreter invocations (via JNI callbacks).  Must be the first
     * element in Thread.
     */
    InterpSaveState interpSave;

    /* small unique integer; useful for "thin" locks and debug messages */
    u4 threadId;

    /*
     * Begin interpreter state which does not need to be preserved, but should
     * be located towards the beginning of the Thread structure for
     * efficiency.
     */

    /*
     * interpBreak contains info about the interpreter mode, as well as
     * a count of the number of times the thread has been suspended.  When
     * the count drops to zero, the thread resumes.
     */
    InterpBreak interpBreak;

    /*
     * "dbgSuspendCount" is the portion of the suspend count that the
     * debugger is responsible for.  This has to be tracked separately so
     * that we can recover correctly if the debugger abruptly disconnects
     * (suspendCount -= dbgSuspendCount).  The debugger should not be able
     * to resume GC-suspended threads, because we ignore the debugger while
     * a GC is in progress.
     *
     * Both of these are guarded by gDvm.threadSuspendCountLock.
     *
     * Note the non-debug component will rarely be other than 1 or 0 -- (not
     * sure it's even possible with the way mutexes are currently used.)
     */

    int suspendCount;
    int dbgSuspendCount;

    u1 *cardTable;

    /* current limit of stack; flexes for StackOverflowError */
    const u1 *interpStackEnd;

    /* FP of bottom-most (currently executing) stack frame on interp stack */
    void *XcurFrame;
    /* current exception, or NULL if nothing pending */
    Object *exception;

    bool debugIsMethodEntry;
    /* interpreter stack size; our stacks are fixed-length */
    int interpStackSize;
    bool stackOverflowed;

    /* thread handle, as reported by pthread_self() */
    pthread_t handle;

    /* Assembly interpreter handler tables */
#ifndef DVM_NO_ASM_INTERP
    void *mainHandlerTable;   // Table of actual instruction handler
    void *altHandlerTable;    // Table of breakout handlers
#else
    void*       unused0;            // Consume space to keep offsets
    void*       unused1;            //   the same between builds with
#endif

    /*
     * singleStepCount is a countdown timer used with the breakFlag
     * kInterpSingleStep.  If kInterpSingleStep is set in breakFlags,
     * singleStepCount will decremented each instruction execution.
     * Once it reaches zero, the kInterpSingleStep flag in breakFlags
     * will be cleared.  This can be used to temporarily prevent
     * execution from re-entering JIT'd code or force inter-instruction
     * checks by delaying the reset of curHandlerTable to mainHandlerTable.
     */
    int singleStepCount;

#ifdef WITH_JIT
    struct JitToInterpEntries jitToInterpEntries;
    /*
     * Whether the current top VM frame is in the interpreter or JIT cache:
     *   NULL    : in the interpreter
     *   non-NULL: entry address of the JIT'ed code (the actual value doesn't
     *             matter)
     */
    void*             inJitCodeCache;
    unsigned char*    pJitProfTable;
    int               jitThreshold;
    const void*       jitResumeNPC;     // Translation return point
    const u4*         jitResumeNSP;     // Native SP at return point
    const u2*         jitResumeDPC;     // Dalvik inst following single-step
    JitState    jitState;
    int         icRechainCount;
    const void* pProfileCountdown;
    const ClassObject* callsiteClass;
    const Method*     methodToCall;
#endif

    /* JNI local reference tracking */
    IndirectRefTable jniLocalRefTable;

#if defined(WITH_JIT)
#if defined(WITH_SELF_VERIFICATION)
    /* Buffer for register state during self verification */
    struct ShadowSpace* shadowSpace;
#endif
    int         currTraceRun;
    int         totalTraceLen;  // Number of Dalvik insts in trace
    const u2*   currTraceHead;  // Start of the trace we're building
    const u2*   currRunHead;    // Start of run we're building
    int         currRunLen;     // Length of run in 16-bit words
    const u2*   lastPC;         // Stage the PC for the threaded interpreter
    const Method*  traceMethod; // Starting method of current trace
    intptr_t    threshFilter[JIT_TRACE_THRESH_FILTER_SIZE];
    JitTraceRun trace[MAX_JIT_RUN_LEN];
#endif

    /*
     * Thread's current status.  Can only be changed by the thread itself
     * (i.e. don't mess with this from other threads).
     */
    volatile ThreadStatus status;

    /* thread ID, only useful under Linux */
    pid_t systemTid;

    /* start (high addr) of interp stack (subtract size to get malloc addr) */
    u1 *interpStackStart;

    /* the java/lang/Thread that we are associated with */
    Object *threadObj;

    /* the JNIEnv pointer associated with this thread */
    JNIEnv *jniEnv;

    /* internal reference tracking */
    ReferenceTable internalLocalRefTable;


    /* JNI native monitor reference tracking (initialized on first use) */
    ReferenceTable jniMonitorRefTable;

    /* hack to make JNI_OnLoad work right */
    Object *classLoaderOverride;

    /* mutex to guard the interrupted and the waitMonitor members */
    pthread_mutex_t waitMutex;

    /* pointer to the monitor lock we're currently waiting on */
    /* guarded by waitMutex */
    /* TODO: consider changing this to Object* for better JDWP interaction */
    Monitor *waitMonitor;

    /* thread "interrupted" status; stays raised until queried or thrown */
    /* guarded by waitMutex */
    bool interrupted;

    /* links to the next thread in the wait set this thread is part of */
    struct Thread *waitNext;

    /* object to sleep on while we are waiting for a monitor */
    pthread_cond_t waitCond;

    /*
     * Set to true when the thread is in the process of throwing an
     * OutOfMemoryError.
     */
    bool throwingOOME;

    /* links to rest of thread list; grab global lock before traversing */
    struct Thread *prev;
    struct Thread *next;

    /* used by threadExitCheck when a thread exits without detaching */
    int threadExitCheckCount;

    /* JDWP invoke-during-breakpoint support */
    DebugInvokeReq invokeReq;

    /* base time for per-thread CPU timing (used by method profiling) */
    bool cpuClockBaseSet;
    u8 cpuClockBase;

    /* previous stack trace sample and length (used by sampling profiler) */
    const Method **stackTraceSample;
    size_t stackTraceSampleLength;

    /* memory allocation profiling state */
    AllocProfState allocProf;

#ifdef WITH_JNI_STACK_CHECK
    u4          stackCrc;
#endif

#if WITH_EXTRA_GC_CHECKS > 1
    /* PC, saved on every instruction; redundant with StackSaveArea */
    const u2*   currentPc2;
#endif

    /* Safepoint callback state */
    pthread_mutex_t callbackMutex;
    SafePointCallback callback;
    void *callbackArg;

#if defined(ARCH_IA32) && defined(WITH_JIT)
    u4 spillRegion[MAX_SPILL_JIT_IA];
#endif
};

/* start point for an internal thread; mimics pthread args */
typedef void *(*InternalThreadStart)(void *arg);

/* args for internal thread creation */
struct InternalStartArgs {
    /* inputs */
    InternalThreadStart func;
    void *funcArg;
    char *name;
    Object *group;
    bool isDaemon;
    /* result */
    volatile Thread **pThread;
    volatile int *pCreateStatus;
};


/*
 * Thread suspend/resume, used by the GC and debugger.
 */
enum SuspendCause {
    SUSPEND_NOT = 0,
    SUSPEND_FOR_GC,
    SUSPEND_FOR_DEBUG,
    SUSPEND_FOR_DEBUG_EVENT,
    SUSPEND_FOR_STACK_DUMP,
    SUSPEND_FOR_DEX_OPT,
    SUSPEND_FOR_VERIFY,
    SUSPEND_FOR_HPROF,
    SUSPEND_FOR_SAMPLING,
#if defined(WITH_JIT)
    SUSPEND_FOR_TBL_RESIZE,  // jit-table resize
    SUSPEND_FOR_IC_PATCH,    // polymorphic callsite inline-cache patch
    SUSPEND_FOR_CC_RESET,    // code-cache reset
    SUSPEND_FOR_REFRESH,     // Reload data cached in interpState
#endif
};

struct RawDexFile {
    char *cacheFileName;
    DvmDex *pDvmDex;
};

/*
 * One entry in the hash table.
 */
struct ZipHashEntry {
    const char *name;
    unsigned short nameLen;
};

/*
 * Read-only Zip archive.
 *
 * We want "open" and "find entry by name" to be fast operations, and
 * we want to use as little memory as possible.  We memory-map the zip
 * central directory, and load a hash table with pointers to the filenames
 * (which aren't null-terminated).  The other fields are at a fixed offset
 * from the filename, so we don't need to extract those (but we do need
 * to byte-read and endian-swap them every time we want them).
 *
 * It's possible that somebody has handed us a massive (~1GB) zip archive,
 * so we can't expect to mmap the entire file.
 *
 * To speed comparisons when doing a lookup by name, we could make the mapping
 * "private" (copy-on-write) and null-terminate the filenames after verifying
 * the record structure.  However, this requires a private mapping of
 * every page that the Central Directory touches.  Easier to tuck a copy
 * of the string length into the hash table entry.
 */
struct ZipArchive {
    /* open Zip archive */
    int mFd;

    /* mapped central directory area */
    off_t mDirectoryOffset;
    MemMapping mDirectoryMap;

    /* number of entries in the Zip archive */
    int mNumEntries;

    /*
     * We know how many entries are in the Zip archive, so we can have a
     * fixed-size hash table.  We probe on collisions.
     */
    int mHashTableSize;
    ZipHashEntry *mHashTable;
};

struct JarFile {
    ZipArchive archive;
    //MemMapping  map;
    char *cacheFileName;
    DvmDex *pDvmDex;
};

struct DexOrJar {
    char *fileName;
    bool isDex;
    bool okayToFree;
    RawDexFile *pRawDexFile;
    JarFile *pJarFile;
    u1 *pDexMemory; // malloc()ed memory, if any
};


/*
 * One of these for each -ea/-da/-esa/-dsa on the command line.
 */
struct AssertionControl {
    char *pkgOrClass;         /* package/class string, or NULL for esa/dsa */
    int pkgOrClassLen;      /* string length, for quick compare */
    bool enable;             /* enable or disable */
    bool isPackage;          /* string ended with "..."? */
};

/*
 * Register map generation mode.  Only applicable when generateRegisterMaps
 * is enabled.  (The "disabled" state is not folded into this because
 * there are callers like dexopt that want to enable/disable without
 * specifying the configuration details.)
 *
 * "TypePrecise" is slower and requires additional storage for the register
 * maps, but allows type-precise GC.  "LivePrecise" is even slower and
 * requires additional heap during processing, but allows live-precise GC.
 */
enum RegisterMapMode {
    kRegisterMapModeUnknown = 0,
    kRegisterMapModeTypePrecise,
    kRegisterMapModeLivePrecise
};

/*
 * Profiler clock source.
 */
enum ProfilerClockSource {
    kProfilerClockSourceThreadCpu,
    kProfilerClockSourceWall,
    kProfilerClockSourceDual,
};

/*
 * Global DEX optimizer control.  Determines the circumstances in which we
 * try to rewrite instructions in the DEX file.
 *
 * Optimizing is performed ahead-of-time by dexopt and, in some cases, at
 * load time by the VM.
 */
enum DexOptimizerMode {
    OPTIMIZE_MODE_UNKNOWN = 0,
    OPTIMIZE_MODE_NONE,         /* never optimize (except "essential") */
    OPTIMIZE_MODE_VERIFIED,     /* only optimize verified classes (default) */
    OPTIMIZE_MODE_ALL,          /* optimize verified & unverified (risky) */
    OPTIMIZE_MODE_FULL          /* fully opt verified classes at load time */
};


/*
 * Global verification mode.  These must be in order from least verification
 * to most.  If we're using "exact GC", we may need to perform some of
 * the verification steps anyway.
 */
enum DexClassVerifyMode {
    VERIFY_MODE_UNKNOWN = 0,
    VERIFY_MODE_NONE,
    VERIFY_MODE_REMOTE,
    VERIFY_MODE_ALL
};


/*
 * The classpath and bootclasspath differ in that only the latter is
 * consulted when looking for classes needed by the VM.  When searching
 * for an arbitrary class definition, we start with the bootclasspath,
 * look for optional packages (a/k/a standard extensions), and then try
 * the classpath.
 *
 * In Dalvik, a class can be found in one of two ways:
 *  - in a .dex file
 *  - in a .dex file named specifically "classes.dex", which is held
 *    inside a jar file
 *
 * These two may be freely intermixed in a classpath specification.
 * Ordering is significant.
 */
enum ClassPathEntryKind {
    kCpeUnknown = 0,
    kCpeJar,
    kCpeDex,
    kCpeLastEntry       /* used as sentinel at end of array */
};

struct ClassPathEntry {
    ClassPathEntryKind kind;
    char *fileName;
    void *ptr;            /* JarFile* or DexFile* */
};

/*
 * One entry in the hash table.  "data" values are expected to be (or have
 * the same characteristics as) valid pointers.  In particular, a NULL
 * value for "data" indicates an empty slot, and HASH_TOMBSTONE indicates
 * a no-longer-used slot that must be stepped over during probing.
 *
 * Attempting to add a NULL or tombstone value is an error.
 *
 * When an entry is released, we will call (HashFreeFunc)(entry->data).
 */
struct HashEntry {
    u4 hashValue;
    void *data;
};

#define HASH_TOMBSTONE ((void*) 0xcbcacccd)

/*
 * This function will be used to free entries in the table.  This can be
 * NULL if no free is required, free(), or a custom function.
 */
typedef void (*HashFreeFunc)(void *ptr);

/*
 * Expandable hash table.
 *
 * This structure should be considered opaque.
 */
struct HashTable {
    int tableSize;          /* must be power of 2 */
    int numEntries;         /* current #of "live" entries */
    int numDeadEntries;     /* current #of tombstone entries */
    HashEntry *pEntries;           /* array on heap */
    HashFreeFunc freeFunc;
    pthread_mutex_t lock;
};

/*
 * Expanding bitmap, used for tracking resources.  Bits are numbered starting
 * from zero.
 *
 * All operations on a BitVector are unsynchronized.
 */
struct BitVector {
    bool expandable;     /* expand bitmap if we run out? */
    u4 storageSize;    /* current size, in 32-bit words */
    u4 *storage;
};


struct GcMarkStack {
    /* Highest address (exclusive)
     */
    const Object **limit;

    /* Current top of the stack (exclusive)
     */
    const Object **top;

    /* Lowest address (inclusive)
     */
    const Object **base;

    /* Maximum stack size, in bytes.
     */
    size_t length;
};

struct HeapBitmap {
    /* The bitmap data, which points to an mmap()ed area of zeroed
     * anonymous memory.
     */
    unsigned long *bits;

    /* The size of the used memory pointed to by bits, in bytes.  This
     * value changes when the bitmap is shrunk.
     */
    size_t bitsLen;

    /* The real size of the memory pointed to by bits.  This is the
     * number of bytes we requested from the allocator and does not
     * change.
     */
    size_t allocLen;

    /* The base address, which corresponds to the first bit in
     * the bitmap.
     */
    uintptr_t base;

    /* The highest pointer value ever returned by an allocation
     * from this heap.  I.e., the highest address that may correspond
     * to a set bit.  If there are no bits set, (max < base).
     */
    uintptr_t max;
};

/* This is declared publicly so that it can be included in gDvm.gcHeap.
 */
struct GcMarkContext {
    HeapBitmap *bitmap;
    GcMarkStack stack;
    const char *immuneLimit;
    const void *finger;   // only used while scanning/recursing.
};

struct Heap {
    /* The mspace to allocate from.
     */
    void *msp;

    /* The largest size that this heap is allowed to grow to.
     */
    size_t maximumSize;

    /* Number of bytes allocated from this mspace for objects,
     * including any overhead.  This value is NOT exact, and
     * should only be used as an input for certain heuristics.
     */
    size_t bytesAllocated;

    /* Number of bytes allocated from this mspace at which a
     * concurrent garbage collection will be started.
     */
    size_t concurrentStartBytes;

    /* Number of objects currently allocated from this mspace.
     */
    size_t objectsAllocated;

    /*
     * The lowest address of this heap, inclusive.
     */
    char *base;

    /*
     * The highest address of this heap, exclusive.
     */
    char *limit;

    /*
     * If the heap has an mspace, the current high water mark in
     * allocations requested via dvmHeapSourceMorecore.
     */
    char *brk;
};

#define HEAP_SOURCE_MAX_HEAP_COUNT 2

struct HeapSource {
    /* Target ideal heap utilization ratio; range 1..HEAP_UTILIZATION_MAX
     */
    size_t targetUtilization;

    /* The starting heap size.
     */
    size_t startSize;

    /* The largest that the heap source as a whole is allowed to grow.
     */
    size_t maximumSize;

    /*
     * The largest size we permit the heap to grow.  This value allows
     * the user to limit the heap growth below the maximum size.  This
     * is a work around until we can dynamically set the maximum size.
     * This value can range between the starting size and the maximum
     * size but should never be set below the current footprint of the
     * heap.
     */
    size_t growthLimit;

    /* The desired max size of the heap source as a whole.
     */
    size_t idealSize;

    /* The maximum number of bytes allowed to be allocated from the
     * active heap before a GC is forced.  This is used to "shrink" the
     * heap in lieu of actual compaction.
     */
    size_t softLimit;

    /* Minimum number of free bytes. Used with the target utilization when
     * setting the softLimit. Never allows less bytes than this to be free
     * when the heap size is below the maximum size or growth limit.
     */
    size_t minFree;

    /* Maximum number of free bytes. Used with the target utilization when
     * setting the softLimit. Never allows more bytes than this to be free
     * when the heap size is below the maximum size or growth limit.
     */
    size_t maxFree;

    /* The heaps; heaps[0] is always the active heap,
     * which new objects should be allocated from.
     */
    Heap heaps[HEAP_SOURCE_MAX_HEAP_COUNT];

    /* The current number of heaps.
     */
    size_t numHeaps;

    /* True if zygote mode was active when the HeapSource was created.
     */
    bool sawZygote;

    /*
     * The base address of the virtual memory reservation.
     */
    char *heapBase;

    /*
     * The length in bytes of the virtual memory reservation.
     */
    size_t heapLength;

    /*
     * The live object bitmap.
     */
    HeapBitmap liveBits;

    /*
     * The mark bitmap.
     */
    HeapBitmap markBits;

    /*
     * Native allocations.
     */
    int32_t nativeBytesAllocated;
    size_t nativeFootprintGCWatermark;
    size_t nativeFootprintLimit;
    bool nativeNeedToRunFinalization;

    /*
     * State for the GC daemon.
     */
    bool hasGcThread;
    pthread_t gcThread;
    bool gcThreadShutdown;
    pthread_mutex_t gcThreadMutex;
    pthread_cond_t gcThreadCond;
    bool gcThreadTrimNeeded;
};

struct GcHeap {
    HeapSource *heapSource;

    /* Linked lists of subclass instances of java/lang/ref/Reference
     * that we find while recursing.  The "next" pointers are hidden
     * in the Reference objects' pendingNext fields.  These lists are
     * cleared and rebuilt each time the GC runs.
     */
    Object *softReferences;
    Object *weakReferences;
    Object *finalizerReferences;
    Object *phantomReferences;

    /* The list of Reference objects that need to be enqueued.
     */
    Object *clearedReferences;

    /* The current state of the mark step.
     * Only valid during a GC.
     */
    GcMarkContext markContext;

    /* GC's card table */
    u1 *cardTableBase;
    size_t cardTableLength;
    size_t cardTableMaxLength;
    size_t cardTableOffset;

    /* Is the GC running?  Used to avoid recursive calls to GC.
     */
    bool gcRunning;

    /*
     * Debug control values
     */
    int ddmHpifWhen;
    int ddmHpsgWhen;
    int ddmHpsgWhat;
    int ddmNhsgWhen;
    int ddmNhsgWhat;
};


/*
 * One entry in the cache.  We store two keys (e.g. the classes that are
 * arguments to "instanceof") and one result (e.g. a boolean value).
 *
 * Must be exactly 16 bytes.
 */
struct AtomicCacheEntry {
    u4 key1;
    u4 key2;
    u4 value;
    volatile u4 version;    /* version and lock flag */
};

/*
 * One cache.
 *
 * Thought: we might be able to save a few cycles by storing the cache
 * struct and "entries" separately, avoiding an indirection.  (We already
 * handle "numEntries" separately in ATOMIC_CACHE_LOOKUP.)
 */
struct AtomicCache {
    AtomicCacheEntry *entries;        /* array of entries */
    int numEntries;             /* #of entries, must be power of 2 */

    void *entryAlloc;             /* memory allocated for entries */

    /* cache stats; note we don't guarantee atomic increments for these */
    int trivial;                /* cache access not required */
    int fail;                   /* contention failure */
    int hits;                   /* found entry in cache */
    int misses;                 /* entry was for other keys */
    int fills;                  /* entry was empty */
};

/*
 * Virtual/direct calls to "method" are replaced with an execute-inline
 * instruction with index "idx".
 */
struct InlineSub {
    Method *method;
    int inlineIdx;
};

/*
 * Linear allocation state.  We could tuck this into the start of the
 * allocated region, but that would prevent us from sharing the rest of
 * that first page.
 */
struct LinearAllocHdr {
    int curOffset;          /* offset where next data goes */
    pthread_mutex_t lock;       /* controls updates to this struct */

    char *mapAddr;            /* start of mmap()ed region */
    int mapLength;          /* length of region */
    int firstOffset;        /* for chasing through */

    short *writeRefCount;      /* for ENFORCE_READ_ONLY */
};

/*
 * Data structure used to track buffer use.
 */
struct ExpandBuf {
    u1 *storage;
    int curLen;
    int maxLen;
};

struct JdwpTransport {
    bool (*startup)(struct JdwpState *state, const JdwpStartupParams *pParams);

    bool (*accept)(struct JdwpState *state);

    bool (*establish)(struct JdwpState *state);

    void (*close)(struct JdwpState *state);

    void (*shutdown)(struct JdwpState *state);

    void (*free)(struct JdwpState *state);

    bool (*isConnected)(struct JdwpState *state);

    bool (*awaitingHandshake)(struct JdwpState *state);

    bool (*processIncoming)(struct JdwpState *state);

    bool (*sendRequest)(struct JdwpState *state, ExpandBuf *pReq);

    bool (*sendBufferedRequest)(struct JdwpState *state,
                                const struct iovec *iov, int iovcnt);
};

/*
 * Event modifiers.  A JdwpEvent may have zero or more of these.
 */
union JdwpEventMod {
    u1 modKind;                /* JdwpModKind */
    struct {
        u1 modKind;
        int count;
    } count;
    struct {
        u1 modKind;
        u4 exprId;
    } conditional;
    struct {
        u1 modKind;
        ObjectId threadId;
    } threadOnly;
    struct {
        u1 modKind;
        RefTypeId refTypeId;
    } classOnly;
    struct {
        u1 modKind;
        char *classPattern;
    } classMatch;
    struct {
        u1 modKind;
        char *classPattern;
    } classExclude;
    struct {
        u1 modKind;
        JdwpLocation loc;
    } locationOnly;
    struct {
        u1 modKind;
        u1 caught;
        u1 uncaught;
        RefTypeId refTypeId;
    } exceptionOnly;
    struct {
        u1 modKind;
        RefTypeId refTypeId;
        FieldId fieldId;
    } fieldOnly;
    struct {
        u1 modKind;
        ObjectId threadId;
        int size;           /* JdwpStepSize */
        int depth;          /* JdwpStepDepth */
    } step;
    struct {
        u1 modKind;
        ObjectId objectId;
    } instanceOnly;
};

/*
 * One of these for every registered event.
 *
 * We over-allocate the struct to hold the modifiers.
 */
struct JdwpEvent {
    JdwpEvent *prev;           /* linked list */
    JdwpEvent *next;

    JdwpEventKind eventKind;      /* what kind of event is this? */
    JdwpSuspendPolicy suspendPolicy;  /* suspend all, none, or self? */
    int modCount;       /* #of entries in mods[] */
    u4 requestId;      /* serial#, reported to debugger */

    JdwpEventMod mods[1];        /* MUST be last field in struct */
};

/*
 * State for JDWP functions.
 */
struct JdwpState {
    JdwpStartupParams params;

    /* wait for creation of the JDWP thread */
    pthread_mutex_t threadStartLock;
    pthread_cond_t threadStartCond;

    int debugThreadStarted;
    pthread_t debugThreadHandle;
    ObjectId debugThreadId;
    bool run;

    const JdwpTransport *transport;
    void *netState;

    /* for wait-for-debugger */
    pthread_mutex_t attachLock;
    pthread_cond_t attachCond;

    /* time of last debugger activity, in milliseconds */
    s8 lastActivityWhen;

    /* global counters and a mutex to protect them */
    u4 requestSerial;
    u4 eventSerial;
    pthread_mutex_t serialLock;

    /*
     * Events requested by the debugger (breakpoints, class prep, etc).
     */
    int numEvents;      /* #of elements in eventList */
    JdwpEvent *eventList;      /* linked list of events */
    pthread_mutex_t eventLock;      /* guards numEvents/eventList */

    /*
     * Synchronize suspension of event thread (to avoid receiving "resume"
     * events before the thread has finished suspending itself).
     */
    pthread_mutex_t eventThreadLock;
    pthread_cond_t eventThreadCond;
    ObjectId eventThreadId;

    /*
     * DDM support.
     */
    bool ddmActive;
};

/*
 * This represents a breakpoint inserted in the instruction stream.
 *
 * The debugger may ask us to create the same breakpoint multiple times.
 * We only remove the breakpoint when the last instance is cleared.
 */
struct Breakpoint {
    Method *method;                 /* method we're associated with */
    u2 *addr;                   /* absolute memory address */
    u1 originalOpcode;         /* original 8-bit opcode value */
    int setCount;               /* #of times this breakpoint was set */
};

/*
 * Set of breakpoints.
 */
struct BreakpointSet {
    /* grab lock before reading or writing anything else in here */
    pthread_mutex_t lock;

    /* vector of breakpoint structures */
    int alloc;
    int count;
    Breakpoint *breakpoints;
};

#define kMaxAllocRecordStackDepth   16      /* max 255 */

/*
 * Record the details of an allocation.
 */
struct AllocRecord {
    ClassObject *clazz;      /* class allocated in this block */
    u4 size;       /* total size requested */
    u2 threadId;   /* simple thread ID; could be recycled */

    /* stack trace elements; unused entries have method==NULL */
    struct {
        const Method *method;   /* which method we're executing in */
        int pc;         /* current execution offset, in 16-bit units */
    } stackElem[kMaxAllocRecordStackDepth];
};


/*
 * All fields are initialized to zero.
 *
 * Storage allocated here must be freed by a subsystem shutdown function.
 */
struct DvmGlobals {
    /*
     * Some options from the command line or environment.
     */
    char *bootClassPathStr;
    char *classPathStr;

    size_t heapStartingSize;
    size_t heapMaximumSize;
    size_t heapGrowthLimit;
    bool lowMemoryMode;
    double heapTargetUtilization;
    size_t heapMinFree;
    size_t heapMaxFree;
    size_t stackSize;
    size_t mainThreadStackSize;

    bool verboseGc;
    bool verboseJni;
    bool verboseClass;
    bool verboseShutdown;

    bool jdwpAllowed;        // debugging allowed for this process?
    bool jdwpConfigured;     // has debugging info been provided?
    JdwpTransportType jdwpTransport;
    bool jdwpServer;
    char *jdwpHost;
    int jdwpPort;
    bool jdwpSuspend;

    ProfilerClockSource profilerClockSource;

    /*
     * Lock profiling threshold value in milliseconds.  Acquires that
     * exceed threshold are logged.  Acquires within the threshold are
     * logged with a probability of $\frac{time}{threshold}$ .  If the
     * threshold is unset no additional logging occurs.
     */
    u4 lockProfThreshold;

    int (*vfprintfHook)(FILE *, const char *, va_list);

    void (*exitHook)(int);

    void (*abortHook)(void);

    bool (*isSensitiveThreadHook)(void);

    char *jniTrace;
    bool reduceSignals;
    bool noQuitHandler;
    bool verifyDexChecksum;
    char *stackTraceFile;     // for SIGQUIT-inspired output

    bool logStdio;

    DexOptimizerMode dexOptMode;
    DexClassVerifyMode classVerifyMode;

    bool generateRegisterMaps;
    RegisterMapMode registerMapMode;

    bool monitorVerification;

    bool dexOptForSmp;

    /*
     * GC option flags.
     */
    bool preciseGc;
    bool preVerify;
    bool postVerify;
    bool concurrentMarkSweep;
    bool verifyCardTable;
    bool disableExplicitGc;

    int assertionCtrlCount;
    AssertionControl *assertionCtrl;

    ExecutionMode executionMode;

    bool commonInit; /* whether common stubs are generated */
    bool constInit; /* whether global constants are initialized */

    /*
     * VM init management.
     */
    bool initializing;
    bool optimizing;

    /*
     * java.lang.System properties set from the command line with -D.
     * This is effectively a set, where later entries override earlier
     * ones.
     */
    std::vector<std::string> *properties;

    /*
     * Where the VM goes to find system classes.
     */
    ClassPathEntry *bootClassPath;
    /* used by the DEX optimizer to load classes from an unfinished DEX */
    DvmDex *bootClassPathOptExtra;
    bool optimizingBootstrapClass;

    /*
     * Loaded classes, hashed by class name.  Each entry is a ClassObject*,
     * allocated in GC space.
     */
    HashTable *loadedClasses;

    /*
     * Value for the next class serial number to be assigned.  This is
     * incremented as we load classes.  Failed loads and races may result
     * in some numbers being skipped, and the serial number is not
     * guaranteed to start at 1, so the current value should not be used
     * as a count of loaded classes.
     */
    volatile int classSerialNumber;

    /*
     * Classes with a low classSerialNumber are probably in the zygote, and
     * their InitiatingLoaderList is not used, to promote sharing. The list is
     * kept here instead.
     */
    InitiatingLoaderList *initiatingLoaderList;

    /*
     * Interned strings.
     */

    /* A mutex that guards access to the interned string tables. */
    pthread_mutex_t internLock;

    /* Hash table of strings interned by the user. */
    HashTable *internedStrings;

    /* Hash table of strings interned by the class loader. */
    HashTable *literalStrings;

    /*
     * Classes constructed directly by the vm.
     */

    /* the class Class */
    ClassObject *classJavaLangClass;

    /* synthetic classes representing primitive types */
    ClassObject *typeVoid;
    ClassObject *typeBoolean;
    ClassObject *typeByte;
    ClassObject *typeShort;
    ClassObject *typeChar;
    ClassObject *typeInt;
    ClassObject *typeLong;
    ClassObject *typeFloat;
    ClassObject *typeDouble;

    /* synthetic classes for arrays of primitives */
    ClassObject *classArrayBoolean;
    ClassObject *classArrayByte;
    ClassObject *classArrayShort;
    ClassObject *classArrayChar;
    ClassObject *classArrayInt;
    ClassObject *classArrayLong;
    ClassObject *classArrayFloat;
    ClassObject *classArrayDouble;

    /*
     * Quick lookups for popular classes used internally.
     */
    ClassObject *classJavaLangClassArray;
    ClassObject *classJavaLangClassLoader;
    ClassObject *classJavaLangObject;
    ClassObject *classJavaLangObjectArray;
    ClassObject *classJavaLangString;
    ClassObject *classJavaLangThread;
    ClassObject *classJavaLangVMThread;
    ClassObject *classJavaLangThreadGroup;
    ClassObject *classJavaLangStackTraceElement;
    ClassObject *classJavaLangStackTraceElementArray;
    ClassObject *classJavaLangAnnotationAnnotationArray;
    ClassObject *classJavaLangAnnotationAnnotationArrayArray;
    ClassObject *classJavaLangReflectAccessibleObject;
    ClassObject *classJavaLangReflectConstructor;
    ClassObject *classJavaLangReflectConstructorArray;
    ClassObject *classJavaLangReflectField;
    ClassObject *classJavaLangReflectFieldArray;
    ClassObject *classJavaLangReflectMethod;
    ClassObject *classJavaLangReflectMethodArray;
    ClassObject *classJavaLangReflectProxy;
    ClassObject *classJavaLangSystem;
    ClassObject *classJavaNioDirectByteBuffer;
    ClassObject *classLibcoreReflectAnnotationFactory;
    ClassObject *classLibcoreReflectAnnotationMember;
    ClassObject *classLibcoreReflectAnnotationMemberArray;
    ClassObject *classOrgApacheHarmonyDalvikDdmcChunk;
    ClassObject *classOrgApacheHarmonyDalvikDdmcDdmServer;
    ClassObject *classJavaLangRefFinalizerReference;

    /*
     * classes representing exception types. The names here don't include
     * packages, just to keep the use sites a bit less verbose. All are
     * in java.lang, except where noted.
     */
    ClassObject *exAbstractMethodError;
    ClassObject *exArithmeticException;
    ClassObject *exArrayIndexOutOfBoundsException;
    ClassObject *exArrayStoreException;
    ClassObject *exClassCastException;
    ClassObject *exClassCircularityError;
    ClassObject *exClassFormatError;
    ClassObject *exClassNotFoundException;
    ClassObject *exError;
    ClassObject *exExceptionInInitializerError;
    ClassObject *exFileNotFoundException; /* in java.io */
    ClassObject *exIOException;           /* in java.io */
    ClassObject *exIllegalAccessError;
    ClassObject *exIllegalAccessException;
    ClassObject *exIllegalArgumentException;
    ClassObject *exIllegalMonitorStateException;
    ClassObject *exIllegalStateException;
    ClassObject *exIllegalThreadStateException;
    ClassObject *exIncompatibleClassChangeError;
    ClassObject *exInstantiationError;
    ClassObject *exInstantiationException;
    ClassObject *exInternalError;
    ClassObject *exInterruptedException;
    ClassObject *exLinkageError;
    ClassObject *exNegativeArraySizeException;
    ClassObject *exNoClassDefFoundError;
    ClassObject *exNoSuchFieldError;
    ClassObject *exNoSuchFieldException;
    ClassObject *exNoSuchMethodError;
    ClassObject *exNullPointerException;
    ClassObject *exOutOfMemoryError;
    ClassObject *exRuntimeException;
    ClassObject *exStackOverflowError;
    ClassObject *exStaleDexCacheError;    /* in dalvik.system */
    ClassObject *exStringIndexOutOfBoundsException;
    ClassObject *exThrowable;
    ClassObject *exTypeNotPresentException;
    ClassObject *exUnsatisfiedLinkError;
    ClassObject *exUnsupportedOperationException;
    ClassObject *exVerifyError;
    ClassObject *exVirtualMachineError;

    /* method offsets - Object */
    int voffJavaLangObject_equals;
    int voffJavaLangObject_hashCode;
    int voffJavaLangObject_toString;

    /* field offsets - String */
    int offJavaLangString_value;
    int offJavaLangString_count;
    int offJavaLangString_offset;
    int offJavaLangString_hashCode;

    /* field offsets - Thread */
    int offJavaLangThread_vmThread;
    int offJavaLangThread_group;
    int offJavaLangThread_daemon;
    int offJavaLangThread_name;
    int offJavaLangThread_priority;
    int offJavaLangThread_uncaughtHandler;
    int offJavaLangThread_contextClassLoader;

    /* method offsets - Thread */
    int voffJavaLangThread_run;

    /* field offsets - ThreadGroup */
    int offJavaLangThreadGroup_name;
    int offJavaLangThreadGroup_parent;

    /* field offsets - VMThread */
    int offJavaLangVMThread_thread;
    int offJavaLangVMThread_vmData;

    /* method offsets - ThreadGroup */
    int voffJavaLangThreadGroup_removeThread;

    /* field offsets - Throwable */
    int offJavaLangThrowable_stackState;
    int offJavaLangThrowable_cause;

    /* method offsets - ClassLoader */
    int voffJavaLangClassLoader_loadClass;

    /* direct method pointers - ClassLoader */
    Method *methJavaLangClassLoader_getSystemClassLoader;

    /* field offsets - java.lang.reflect.* */
    int offJavaLangReflectConstructor_slot;
    int offJavaLangReflectConstructor_declClass;
    int offJavaLangReflectField_slot;
    int offJavaLangReflectField_declClass;
    int offJavaLangReflectMethod_slot;
    int offJavaLangReflectMethod_declClass;

    /* field offsets - java.lang.ref.Reference */
    int offJavaLangRefReference_referent;
    int offJavaLangRefReference_queue;
    int offJavaLangRefReference_queueNext;
    int offJavaLangRefReference_pendingNext;

    /* field offsets - java.lang.ref.FinalizerReference */
    int offJavaLangRefFinalizerReference_zombie;

    /* method pointers - java.lang.ref.ReferenceQueue */
    Method *methJavaLangRefReferenceQueueAdd;

    /* method pointers - java.lang.ref.FinalizerReference */
    Method *methJavaLangRefFinalizerReferenceAdd;

    /* constructor method pointers; no vtable involved, so use Method* */
    Method *methJavaLangStackTraceElement_init;
    Method *methJavaLangReflectConstructor_init;
    Method *methJavaLangReflectField_init;
    Method *methJavaLangReflectMethod_init;
    Method *methOrgApacheHarmonyLangAnnotationAnnotationMember_init;

    /* static method pointers - android.lang.annotation.* */
    Method *methOrgApacheHarmonyLangAnnotationAnnotationFactory_createAnnotation;

    /* direct method pointers - java.lang.reflect.Proxy */
    Method *methJavaLangReflectProxy_constructorPrototype;

    /* field offsets - java.lang.reflect.Proxy */
    int offJavaLangReflectProxy_h;

    /* direct method pointer - java.lang.System.runFinalization */
    Method *methJavaLangSystem_runFinalization;

    /* field offsets - java.io.FileDescriptor */
    int offJavaIoFileDescriptor_descriptor;

    /* direct method pointers - dalvik.system.NativeStart */
    Method *methDalvikSystemNativeStart_main;
    Method *methDalvikSystemNativeStart_run;

    /* assorted direct buffer helpers */
    Method *methJavaNioDirectByteBuffer_init;
    int offJavaNioBuffer_capacity;
    int offJavaNioBuffer_effectiveDirectAddress;

    /* direct method pointers - org.apache.harmony.dalvik.ddmc.DdmServer */
    Method *methDalvikDdmcServer_dispatch;
    Method *methDalvikDdmcServer_broadcast;

    /* field offsets - org.apache.harmony.dalvik.ddmc.Chunk */
    int offDalvikDdmcChunk_type;
    int offDalvikDdmcChunk_data;
    int offDalvikDdmcChunk_offset;
    int offDalvikDdmcChunk_length;

    /*
     * Thread list.  This always has at least one element in it (main),
     * and main is always the first entry.
     *
     * The threadListLock is used for several things, including the thread
     * start condition variable.  Generally speaking, you must hold the
     * threadListLock when:
     *  - adding/removing items from the list
     *  - waiting on or signaling threadStartCond
     *  - examining the Thread struct for another thread (this is to avoid
     *    one thread freeing the Thread struct while another thread is
     *    perusing it)
     */
    Thread *threadList;
    pthread_mutex_t threadListLock;

    pthread_cond_t threadStartCond;

    /*
     * The thread code grabs this before suspending all threads.  There
     * are a few things that can cause a "suspend all":
     *  (1) the GC is starting;
     *  (2) the debugger has sent a "suspend all" request;
     *  (3) a thread has hit a breakpoint or exception that the debugger
     *      has marked as a "suspend all" event;
     *  (4) the SignalCatcher caught a signal that requires suspension.
     *  (5) (if implemented) the JIT needs to perform a heavyweight
     *      rearrangement of the translation cache or JitTable.
     *
     * Because we use "safe point" self-suspension, it is never safe to
     * do a blocking "lock" call on this mutex -- if it has been acquired,
     * somebody is probably trying to put you to sleep.  The leading '_' is
     * intended as a reminder that this lock is special.
     */
    pthread_mutex_t _threadSuspendLock;

    /*
     * Guards Thread->suspendCount for all threads, and
     * provides the lock for the condition variable that all suspended threads
     * sleep on (threadSuspendCountCond).
     *
     * This has to be separate from threadListLock because of the way
     * threads put themselves to sleep.
     */
    pthread_mutex_t threadSuspendCountLock;

    /*
     * Suspended threads sleep on this.  They should sleep on the condition
     * variable until their "suspend count" is zero.
     *
     * Paired with "threadSuspendCountLock".
     */
    pthread_cond_t threadSuspendCountCond;

    /*
     * Sum of all threads' suspendCount fields. Guarded by
     * threadSuspendCountLock.
     */
    int sumThreadSuspendCount;

    /*
     * MUTEX ORDERING: when locking multiple mutexes, always grab them in
     * this order to avoid deadlock:
     *
     *  (1) _threadSuspendLock      (use lockThreadSuspend())
     *  (2) threadListLock          (use dvmLockThreadList())
     *  (3) threadSuspendCountLock  (use lockThreadSuspendCount())
     */


    /*
     * Thread ID bitmap.  We want threads to have small integer IDs so
     * we can use them in "thin locks".
     */
    BitVector *threadIdMap;

    /*
     * Manage exit conditions.  The VM exits when all non-daemon threads
     * have exited.  If the main thread returns early, we need to sleep
     * on a condition variable.
     */
    int nonDaemonThreadCount;   /* must hold threadListLock to access */
    pthread_cond_t vmExitCond;

    /*
     * The set of DEX files loaded by custom class loaders.
     */
    HashTable *userDexFiles;

    /*
     * JNI global reference table.
     */
    IndirectRefTable jniGlobalRefTable;
    IndirectRefTable jniWeakGlobalRefTable;
    pthread_mutex_t jniGlobalRefLock;
    pthread_mutex_t jniWeakGlobalRefLock;

    /*
     * JNI pinned object table (used for primitive arrays).
     */
    ReferenceTable jniPinRefTable;
    pthread_mutex_t jniPinRefLock;

    /*
     * Native shared library table.
     */
    HashTable *nativeLibs;

    /*
     * GC heap lock.  Functions like gcMalloc() acquire this before making
     * any changes to the heap.  It is held throughout garbage collection.
     */
    pthread_mutex_t gcHeapLock;

    /*
     * Condition variable to queue threads waiting to retry an
     * allocation.  Signaled after a concurrent GC is completed.
     */
    pthread_cond_t gcHeapCond;

    /* Opaque pointer representing the heap. */
    GcHeap *gcHeap;

    /* The card table base, modified as needed for marking cards. */
    u1 *biasedCardTableBase;

    /*
     * Pre-allocated throwables.
     */
    Object *outOfMemoryObj;
    Object *internalErrorObj;
    Object *noClassDefFoundErrorObj;

    /* Monitor list, so we can free them */
    /*volatile*/ Monitor *monitorList;

    /* Monitor for Thread.sleep() implementation */
    Monitor *threadSleepMon;

    /* set when we create a second heap inside the zygote */
    bool newZygoteHeapAllocated;

    /*
     * TLS keys.
     */
    pthread_key_t pthreadKeySelf;       /* Thread*, for dvmThreadSelf */

    /*
     * Cache results of "A instanceof B".
     */
    AtomicCache *instanceofCache;

    /* inline substitution table, used during optimization */
    InlineSub *inlineSubs;

    /*
     * Bootstrap class loader linear allocator.
     */
    LinearAllocHdr *pBootLoaderAlloc;

    /*
     * Compute some stats on loaded classes.
     */
    int numLoadedClasses;
    int numDeclaredMethods;
    int numDeclaredInstFields;
    int numDeclaredStaticFields;

    /* when using a native debugger, set this to suppress watchdog timers */
    bool nativeDebuggerActive;

    /*
     * JDWP debugger support.
     *
     * Note: Each thread will normally determine whether the debugger is active
     * for it by referring to its subMode flags.  "debuggerActive" here should be
     * seen as "debugger is making requests of 1 or more threads".
     */
    bool debuggerConnected;      /* debugger or DDMS is connected */
    bool debuggerActive;         /* debugger is making requests */
    JdwpState *jdwpState;

    /*
     * Registry of objects known to the debugger.
     */
    HashTable *dbgRegistry;

    /*
     * Debugger breakpoint table.
     */
    BreakpointSet *breakpointSet;

    /*
     * Single-step control struct.  We currently only allow one thread to
     * be single-stepping at a time, which is all that really makes sense,
     * but it's possible we may need to expand this to be per-thread.
     */
    StepControl stepControl;

    /*
     * DDM features embedded in the VM.
     */
    bool ddmThreadNotification;

    /*
     * Zygote (partially-started process) support
     */
    bool zygote;

    /*
     * Used for tracking allocations that we report to DDMS.  When the feature
     * is enabled (through a DDMS request) the "allocRecords" pointer becomes
     * non-NULL.
     */
    pthread_mutex_t allocTrackerLock;
    AllocRecord *allocRecords;
    int allocRecordHead;        /* most-recently-added entry */
    int allocRecordCount;       /* #of valid entries */
    int allocRecordMax;         /* Number of allocated entries. */

    /*
     * When a profiler is enabled, this is incremented.  Distinct profilers
     * include "dmtrace" method tracing, emulator method tracing, and
     * possibly instruction counting.
     *
     * The purpose of this is to have a single value that shows whether any
     * profiling is going on.  Individual thread will normally check their
     * thread-private subMode flags to take any profiling action.
     */
    volatile int activeProfilers;

    /*
     * State for method-trace profiling.
     */
    MethodTraceState methodTrace;
    Method *methodTraceGcMethod;
    Method *methodTraceClassPrepMethod;

    /*
     * State for emulator tracing.
     */
    void *emulatorTracePage;
    int emulatorTraceEnableCount;

    /*
     * Global state for memory allocation profiling.
     */
    AllocProfState allocProf;

    /*
     * Pointers to the original methods for things that have been inlined.
     * This makes it easy for us to output method entry/exit records for
     * the method calls we're not actually making.  (Used by method
     * profiling.)
     */
    Method **inlinedMethods;

    /*
     * Dalvik instruction counts (kNumPackedOpcodes entries).
     */
    int *executedInstrCounts;
    int instructionCountEnableCount;

    /*
     * Signal catcher thread (for SIGQUIT).
     */
    pthread_t signalCatcherHandle;
    bool haltSignalCatcher;

    /*
     * Stdout/stderr conversion thread.
     */
    bool haltStdioConverter;
    bool stdioConverterReady;
    pthread_t stdioConverterHandle;
    pthread_mutex_t stdioConverterLock;
    pthread_cond_t stdioConverterCond;
    int stdoutPipe[2];
    int stderrPipe[2];

    /*
     * pid of the system_server process. We track it so that when system server
     * crashes the Zygote process will be killed and restarted.
     */
    pid_t systemServerPid;

    int kernelGroupScheduling;

//#define COUNT_PRECISE_METHODS
#ifdef COUNT_PRECISE_METHODS
    PointerSet* preciseMethods;
#endif

    /* some RegisterMap statistics, useful during development */
    void *registerMapStats;

#ifdef VERIFIER_STATS
    VerifierStats verifierStats;
#endif

    /* String pointed here will be deposited on the stack frame of dvmAbort */
    const char *lastMessage;
};

/*
 * This holds some meta-data while we construct the set of register maps
 * for a DEX file.
 *
 * In particular, it keeps track of our temporary mmap region so we can
 * free it later.
 */
struct RegisterMapBuilder {
    /* public */
    void *data;
    size_t size;

    /* private */
    MemMapping memMap;
};

#define DALVIK_VM_BUILD         27
#define CPU_CACHE_WIDTH         32
#define CPU_CACHE_WIDTH_1       (CPU_CACHE_WIDTH-1)

/*
 * Our data structures for JNIEnv and JavaVM.
 *
 * Native code thinks it has a pointer to a pointer.  We know better.
 */
struct JavaVMExt;

struct JNIEnvExt {
    const struct JNINativeInterface *funcTable;     /* must be first */

    const struct JNINativeInterface *baseFuncTable;

    u4 envThreadId;
    Thread *self;

    /* if nonzero, we are in a "critical" JNI call */
    int critical;

    struct JNIEnvExt *prev;
    struct JNIEnvExt *next;
};


/* flags for dvmMalloc */
enum {
    ALLOC_DEFAULT = 0x00,
    ALLOC_DONT_TRACK = 0x01,  /* don't add to internal tracking list */
    ALLOC_NON_MOVING = 0x02,
};


/*
 * "Direct" and "virtual" methods are stored independently.  The type of call
 * used to invoke the method determines which list we search, and whether
 * we travel up into superclasses.
 *
 * (<clinit>, <init>, and methods declared "private" or "static" are stored
 * in the "direct" list.  All others are stored in the "virtual" list.)
 */
enum MethodType {
    METHOD_UNKNOWN = 0,
    METHOD_DIRECT,      // <init>, private
    METHOD_STATIC,      // static
    METHOD_VIRTUAL,     // virtual, super
    METHOD_INTERFACE,    // interface
    METHOD_SUPER        // super
};

/* expanded form of a class_data_item header */
struct DexClassDataHeader {
    u4 staticFieldsSize;
    u4 instanceFieldsSize;
    u4 directMethodsSize;
    u4 virtualMethodsSize;
};

/* expanded form of encoded_field */
struct DexField {
    u4 fieldIdx;    /* index to a field_id_item */
    u4 accessFlags;
};

/* expanded form of encoded_method */
struct DexMethod {
    u4 methodIdx;    /* index to a method_id_item */
    u4 accessFlags;
    u4 codeOff;      /* file offset to a code_item */
};

/* expanded form of class_data_item. Note: If a particular item is
 * absent (e.g., no static fields), then the corresponding pointer
 * is set to NULL. */
struct DexClassData {
    DexClassDataHeader header;
    DexField *staticFields;
    DexField *instanceFields;
    DexMethod *directMethods;
    DexMethod *virtualMethods;
};

#define kVerifyErrorRefTypeShift 6

/*
 * An enumeration of problems that can turn up during verification.
 */
enum VerifyError {
    VERIFY_ERROR_NONE = 0,      /* no error; must be zero */
    VERIFY_ERROR_GENERIC,       /* VerifyError */

    VERIFY_ERROR_NO_CLASS,      /* NoClassDefFoundError */
    VERIFY_ERROR_NO_FIELD,      /* NoSuchFieldError */
    VERIFY_ERROR_NO_METHOD,     /* NoSuchMethodError */
    VERIFY_ERROR_ACCESS_CLASS,  /* IllegalAccessError */
    VERIFY_ERROR_ACCESS_FIELD,  /* IllegalAccessError */
    VERIFY_ERROR_ACCESS_METHOD, /* IllegalAccessError */
    VERIFY_ERROR_CLASS_CHANGE,  /* IncompatibleClassChangeError */
    VERIFY_ERROR_INSTANTIATION, /* InstantiationError */
};

/*
 * Identifies the type of reference in the instruction that generated the
 * verify error (e.g. VERIFY_ERROR_ACCESS_CLASS could come from a method,
 * field, or class reference).
 *
 * This must fit in two bits.
 */
enum VerifyErrorRefType {
    VERIFY_ERROR_REF_CLASS  = 0,
    VERIFY_ERROR_REF_FIELD  = 1,
    VERIFY_ERROR_REF_METHOD = 2,
};

#define kThrowShow_accessFromClass     1

#endif //SHELLAPPLICATION_HEADERINNEED_H
