//
// Created by ChenDalunlun on 2019/8/27.
//

#ifndef SHELLAPPLICATION_VMINTERPRETER_H
#define SHELLAPPLICATION_VMINTERPRETER_H
#include <cstdint>
#include <jni.h>
#include "common/Dalvik.h"
#include "Dex.h"

struct CodeItemData;

struct VmMethod{
    const DexFile* dexFile;
    const char* name;
    const char* clazzDescriptor;
    const DexProtoId* protoId;
    u4 accessFlags;
    const CodeItemData* code;
};

struct VmField{
    const DexFile* dexFile;
    const char* name;
    const char* clazzDescriptor;
    u4 accessFlags;
};

using namespace std;

const DexFile* initDexFileInArt(const uint8_t *buf, size_t size);

jmethodID dvmResolveMethod(const VmMethod *method, u4 methodIdx, MethodType methodType,
                           jclass *methodToCallClazz);

bool dvmResolveField(const VmMethod *method, u4 ifieldIdx, jobject obj, s8 *res,
                     const char **ppName);

bool dvmResolveSetField(const VmMethod *method, u4 ifieldIdx, jobject obj, u8 res,
                        const char **ppName);

void
dvmResolveMethodSign(const VmMethod *method, const DexMethodId *pMethodId, string *methodSign);

void
dvmResolveMethodSign(const DexFile *pDexFile, const DexProtoId *pDexProtoId, string *methodSign);

inline u4 dvmFloatToU4(float val);

jstring dvmResolveString(const VmMethod *method, u4 stringIdx);

jclass dvmResolveClass(const VmMethod *method, u4 classIdx);

jclass dvmResolveClass(const VmMethod *method, u4 classIdx, string *clazzNameString);

jarray dvmAllocArrayByClass(const s4 length, const VmMethod *method, u4 classIdx);


void
dvmInterpret(JNIEnv *env, jobject instance, const VmMethod *curMethod, u4 *reg, jvalue *pResult);

void
dvmCallMethod(JNIEnv *env, jobject instance, const VmMethod *method, jvalue *pResult, ...);

bool checkForNull(jobject obj);

bool dvmInterpretHandleFillArrayData(jarray arrayObj, const u2 *arrayData);

s4 dvmInterpretHandlePackedSwitch(const u2 *switchData, s4 testVal);

s4 dvmInterpretHandleSparseSwitch(const u2 *switchData, s4 testVal);

inline s4 s4FromSwitchData(const void *switchData) {
    return *(s4 *) switchData;
}

bool dvmCanPutArrayElement(const jobject obj, const jobject arrayObj);

void debugInvokeMethod(jmethodID jniMethod, const jvalue retVal, const jvalue *vars);

int dvmFindCatchBlock(const VmMethod *method, size_t pcOff, jobject exception);

const VmMethod * initVmMethod(jmethodID jniMethod);

void deleteVmMethod(const VmMethod* method);

const VmMethod* initVmMethodNoCode(jmethodID jniMethod);

const CodeItemData *getCodeItem(const VmMethod *method);

string dvmHumanReadableDescriptor(const char *descriptor);

string classNameFromIndex(const VmMethod *method, int ref, VerifyErrorRefType refType, int flags);

string fieldNameFromIndex(const VmMethod *method, int ref, VerifyErrorRefType refType, int flags);

string methodNameFromIndex(const VmMethod *method, int ref, VerifyErrorRefType refType, int flags);

void dvmThrowClassCastException(jclass actual, jclass desired);

void dvmThrowArithmeticException(const char *msg);

void dvmThrowNullPointerException(const char *msg);

void dvmThrowInternalError(const char *msg);

void dvmThrowArrayIndexOutOfBoundsException(int length, int index);

void dvmThrowNegativeArraySizeException(s4 size);

void dvmThrowRuntimeException(const char *msg);

void dvmThrowIncompatibleClassChangeError(const char *msg);

void
dvmThrowArrayStoreExceptionIncompatibleElement(const jobject obj, const jobject arrayObj);

void dvmThrowVerificationError(const VmMethod *method, int kind, int ref);

const string getClassDescriptorByJClass(jclass clazz);
//enum Opcode {
//    // BEGIN(libdex-opcode-enum); GENERATED AUTOMATICALLY BY opcode-gen
//    OP_NOP = 0x00,                    // i t
//    OP_MOVE = 0x01,                   // i
//    OP_MOVE_FROM16 = 0x02,            // i t
//    OP_MOVE_16 = 0x03,                // i
//    OP_MOVE_WIDE = 0x04,              // i
//    OP_MOVE_WIDE_FROM16 = 0x05,       // i t
//    OP_MOVE_WIDE_16 = 0x06,           // i
//    OP_MOVE_OBJECT = 0x07,            // i t
//    OP_MOVE_OBJECT_FROM16 = 0x08,     // i t
//    OP_MOVE_OBJECT_16 = 0x09,         // i
//    OP_MOVE_RESULT = 0x0a,            // i t
//    OP_MOVE_RESULT_WIDE = 0x0b,       // i t
//    OP_MOVE_RESULT_OBJECT = 0x0c,     // i t
//    OP_MOVE_EXCEPTION = 0x0d,         // i t
//    OP_RETURN_VOID = 0x0e,            // i t
//    OP_RETURN = 0x0f,                 // i t
//    OP_RETURN_WIDE = 0x10,            // i t
//    OP_RETURN_OBJECT = 0x11,          // i
//    OP_CONST_4 = 0x12,                // i t
//    OP_CONST_16 = 0x13,               // i t
//    OP_CONST = 0x14,                  // i t
//    OP_CONST_HIGH16 = 0x15,           // i t
//    OP_CONST_WIDE_16 = 0x16,          // i t
//    OP_CONST_WIDE_32 = 0x17,          // i
//    OP_CONST_WIDE = 0x18,             // i
//    OP_CONST_WIDE_HIGH16 = 0x19,      // i t
//    OP_CONST_STRING = 0x1a,           // i t
//    OP_CONST_STRING_JUMBO = 0x1b,     // i
//    OP_CONST_CLASS = 0x1c,            // i
//    OP_MONITOR_ENTER = 0x1d,          // i
//    OP_MONITOR_EXIT = 0x1e,           // i
//    OP_CHECK_CAST = 0x1f,             // i t
//    OP_INSTANCE_OF = 0x20,            // i t
//    OP_ARRAY_LENGTH = 0x21,           // i t
//    OP_NEW_INSTANCE = 0x22,           // i t
//    OP_NEW_ARRAY = 0x23,              // i t
//    OP_FILLED_NEW_ARRAY = 0x24,       // i
//    OP_FILLED_NEW_ARRAY_RANGE = 0x25, // i
//    OP_FILL_ARRAY_DATA = 0x26,        // i t
//    OP_THROW = 0x27,                  // i
//    OP_GOTO = 0x28,                   // i t
//    OP_GOTO_16 = 0x29,                // i t
//    OP_GOTO_32 = 0x2a,                // i
//    OP_PACKED_SWITCH = 0x2b,          // i
//    OP_SPARSE_SWITCH = 0x2c,          // i
//    OP_CMPL_FLOAT = 0x2d,             // i
//    OP_CMPG_FLOAT = 0x2e,             // i t
//    OP_CMPL_DOUBLE = 0x2f,            // i
//    OP_CMPG_DOUBLE = 0x30,            // i
//    OP_CMP_LONG = 0x31,               // i t
//    OP_IF_EQ = 0x32,                  // i t
//    OP_IF_NE = 0x33,                  // i
//    OP_IF_LT = 0x34,                  // i
//    OP_IF_GE = 0x35,                  // i t
//    OP_IF_GT = 0x36,                  // i
//    OP_IF_LE = 0x37,                  // i
//    OP_IF_EQZ = 0x38,                 // i t
//    OP_IF_NEZ = 0x39,                 // i
//    OP_IF_LTZ = 0x3a,                 // i
//    OP_IF_GEZ = 0x3b,                 // i t
//    OP_IF_GTZ = 0x3c,                 // i
//    OP_IF_LEZ = 0x3d,                 // i t
//    OP_UNUSED_3E = 0x3e,
//    OP_UNUSED_3F = 0x3f,
//    OP_UNUSED_40 = 0x40,
//    OP_UNUSED_41 = 0x41,
//    OP_UNUSED_42 = 0x42,
//    OP_UNUSED_43 = 0x43,
//    OP_AGET = 0x44,                   // i t
//    OP_AGET_WIDE = 0x45,              // i
//    OP_AGET_OBJECT = 0x46,            // i t
//    OP_AGET_BOOLEAN = 0x47,           // i
//    OP_AGET_BYTE = 0x48,              // i t
//    OP_AGET_CHAR = 0x49,              // i
//    OP_AGET_SHORT = 0x4a,             // i
//    OP_APUT = 0x4b,                   // i t
//    OP_APUT_WIDE = 0x4c,              // i
//    OP_APUT_OBJECT = 0x4d,            // i t
//    OP_APUT_BOOLEAN = 0x4e,           // i
//    OP_APUT_BYTE = 0x4f,              // i t
//    OP_APUT_CHAR = 0x50,              // i t
//    OP_APUT_SHORT = 0x51,             // i
//    OP_IGET = 0x52,                   // i
//    OP_IGET_WIDE = 0x53,              // i
//    OP_IGET_OBJECT = 0x54,            // i t
//    OP_IGET_BOOLEAN = 0x55,           // i
//    OP_IGET_BYTE = 0x56,              // i
//    OP_IGET_CHAR = 0x57,              // i
//    OP_IGET_SHORT = 0x58,             // i
//    OP_IPUT = 0x59,                   // i
//    OP_IPUT_WIDE = 0x5a,              // i
//    OP_IPUT_OBJECT = 0x5b,            // i t
//    OP_IPUT_BOOLEAN = 0x5c,           // i
//    OP_IPUT_BYTE = 0x5d,              // i
//    OP_IPUT_CHAR = 0x5e,              // i
//    OP_IPUT_SHORT = 0x5f,             // i
//    OP_SGET = 0x60,                   // i t
//    OP_SGET_WIDE = 0x61,              // i
//    OP_SGET_OBJECT = 0x62,            // i t
//    OP_SGET_BOOLEAN = 0x63,           // i
//    OP_SGET_BYTE = 0x64,              // i
//    OP_SGET_CHAR = 0x65,              // i
//    OP_SGET_SHORT = 0x66,             // i
//    OP_SPUT = 0x67,                   // i
//    OP_SPUT_WIDE = 0x68,              // i
//    OP_SPUT_OBJECT = 0x69,            // i t
//    OP_SPUT_BOOLEAN = 0x6a,           // i
//    OP_SPUT_BYTE = 0x6b,              // i
//    OP_SPUT_CHAR = 0x6c,              // i
//    OP_SPUT_SHORT = 0x6d,             // i
//    OP_INVOKE_VIRTUAL = 0x6e,         // i t
//    OP_INVOKE_SUPER = 0x6f,           // i t
//    OP_INVOKE_DIRECT = 0x70,          // i t
//    OP_INVOKE_STATIC = 0x71,          // i t
//    OP_INVOKE_INTERFACE = 0x72,       // i t
//    OP_UNUSED_73 = 0x73,
//    OP_INVOKE_VIRTUAL_RANGE = 0x74,   // i t
//    OP_INVOKE_SUPER_RANGE = 0x75,     // i
//    OP_INVOKE_DIRECT_RANGE = 0x76,    // i
//    OP_INVOKE_STATIC_RANGE = 0x77,    // i
//    OP_INVOKE_INTERFACE_RANGE = 0x78, // i
//    OP_UNUSED_79 = 0x79,
//    OP_UNUSED_7A = 0x7a,
//    OP_NEG_INT = 0x7b,                // i
//    OP_NOT_INT = 0x7c,                // i
//    OP_NEG_LONG = 0x7d,               // i
//    OP_NOT_LONG = 0x7e,               // i
//    OP_NEG_FLOAT = 0x7f,              // i t
//    OP_NEG_DOUBLE = 0x80,             // i
//    OP_INT_TO_LONG = 0x81,            // i t
//    OP_INT_TO_FLOAT = 0x82,           // i t
//    OP_INT_TO_DOUBLE = 0x83,          // i t
//    OP_LONG_TO_INT = 0x84,            // i t
//    OP_LONG_TO_FLOAT = 0x85,          // i t
//    OP_LONG_TO_DOUBLE = 0x86,         // i
//    OP_FLOAT_TO_INT = 0x87,           // i t
//    OP_FLOAT_TO_LONG = 0x88,          // i t
//    OP_FLOAT_TO_DOUBLE = 0x89,        // i t
//    OP_DOUBLE_TO_INT = 0x8a,          // i t
//    OP_DOUBLE_TO_LONG = 0x8b,         // i t
//    OP_DOUBLE_TO_FLOAT = 0x8c,        // i t
//    OP_INT_TO_BYTE = 0x8d,            // i t
//    OP_INT_TO_CHAR = 0x8e,            // i t
//    OP_INT_TO_SHORT = 0x8f,           // i
//    OP_ADD_INT = 0x90,                // i t
//    OP_SUB_INT = 0x91,                // i t
//    OP_MUL_INT = 0x92,                // i t
//    OP_DIV_INT = 0x93,                // i t
//    OP_REM_INT = 0x94,                // i
//    OP_AND_INT = 0x95,                // i
//    OP_OR_INT = 0x96,                 // i
//    OP_XOR_INT = 0x97,                // i
//    OP_SHL_INT = 0x98,                // i
//    OP_SHR_INT = 0x99,                // i t
//    OP_USHR_INT = 0x9a,               // i t
//    OP_ADD_LONG = 0x9b,               // i
//    OP_SUB_LONG = 0x9c,               // i
//    OP_MUL_LONG = 0x9d,               // i t
//    OP_DIV_LONG = 0x9e,               // i t
//    OP_REM_LONG = 0x9f,               // i t
//    OP_AND_LONG = 0xa0,               // i t
//    OP_OR_LONG = 0xa1,                // i
//    OP_XOR_LONG = 0xa2,               // i
//    OP_SHL_LONG = 0xa3,               // i
//    OP_SHR_LONG = 0xa4,               // i
//    OP_USHR_LONG = 0xa5,              // i t
//    OP_ADD_FLOAT = 0xa6,              // i
//    OP_SUB_FLOAT = 0xa7,              // i
//    OP_MUL_FLOAT = 0xa8,              // i
//    OP_DIV_FLOAT = 0xa9,              // i t
//    OP_REM_FLOAT = 0xaa,              // i
//    OP_ADD_DOUBLE = 0xab,             // i t
//    OP_SUB_DOUBLE = 0xac,             // i
//    OP_MUL_DOUBLE = 0xad,             // i t
//    OP_DIV_DOUBLE = 0xae,             // i
//    OP_REM_DOUBLE = 0xaf,             // i
//    OP_ADD_INT_2ADDR = 0xb0,          // i
//    OP_SUB_INT_2ADDR = 0xb1,          // i
//    OP_MUL_INT_2ADDR = 0xb2,          // i
//    OP_DIV_INT_2ADDR = 0xb3,          // i
//    OP_REM_INT_2ADDR = 0xb4,          // i
//    OP_AND_INT_2ADDR = 0xb5,          // i
//    OP_OR_INT_2ADDR = 0xb6,           // i
//    OP_XOR_INT_2ADDR = 0xb7,          // i
//    OP_SHL_INT_2ADDR = 0xb8,          // i
//    OP_SHR_INT_2ADDR = 0xb9,          // i
//    OP_USHR_INT_2ADDR = 0xba,         // i
//    OP_ADD_LONG_2ADDR = 0xbb,         // i t
//    OP_SUB_LONG_2ADDR = 0xbc,         // i
//    OP_MUL_LONG_2ADDR = 0xbd,         // i
//    OP_DIV_LONG_2ADDR = 0xbe,         // i
//    OP_REM_LONG_2ADDR = 0xbf,         // i
//    OP_AND_LONG_2ADDR = 0xc0,         // i t
//    OP_OR_LONG_2ADDR = 0xc1,          // i
//    OP_XOR_LONG_2ADDR = 0xc2,         // i t
//    OP_SHL_LONG_2ADDR = 0xc3,         // i
//    OP_SHR_LONG_2ADDR = 0xc4,         // i
//    OP_USHR_LONG_2ADDR = 0xc5,        // i
//    OP_ADD_FLOAT_2ADDR = 0xc6,        // i t
//    OP_SUB_FLOAT_2ADDR = 0xc7,        // i
//    OP_MUL_FLOAT_2ADDR = 0xc8,        // i
//    OP_DIV_FLOAT_2ADDR = 0xc9,        // i
//    OP_REM_FLOAT_2ADDR = 0xca,        // i
//    OP_ADD_DOUBLE_2ADDR = 0xcb,       // i t
//    OP_SUB_DOUBLE_2ADDR = 0xcc,       // i
//    OP_MUL_DOUBLE_2ADDR = 0xcd,       // i
//    OP_DIV_DOUBLE_2ADDR = 0xce,       // i t
//    OP_REM_DOUBLE_2ADDR = 0xcf,       // i
//    OP_ADD_INT_LIT16 = 0xd0,          // i
//    OP_RSUB_INT = 0xd1,               // i
//    OP_MUL_INT_LIT16 = 0xd2,          // i
//    OP_DIV_INT_LIT16 = 0xd3,          // i
//    OP_REM_INT_LIT16 = 0xd4,          // i
//    OP_AND_INT_LIT16 = 0xd5,          // i
//    OP_OR_INT_LIT16 = 0xd6,           // i
//    OP_XOR_INT_LIT16 = 0xd7,          // i
//    OP_ADD_INT_LIT8 = 0xd8,           // i t
//    OP_RSUB_INT_LIT8 = 0xd9,          // i
//    OP_MUL_INT_LIT8 = 0xda,           // i
//    OP_DIV_INT_LIT8 = 0xdb,           // i t
//    OP_REM_INT_LIT8 = 0xdc,           // i t
//    OP_AND_INT_LIT8 = 0xdd,           // i
//    OP_OR_INT_LIT8 = 0xde,            // i
//    OP_XOR_INT_LIT8 = 0xdf,           // i
//    OP_SHL_INT_LIT8 = 0xe0,           // i
//    OP_SHR_INT_LIT8 = 0xe1,           // i
//    OP_USHR_INT_LIT8 = 0xe2,          // i
//    OP_IGET_VOLATILE = 0xe3,          // i
//    OP_IPUT_VOLATILE = 0xe4,          // i
//    OP_SGET_VOLATILE = 0xe5,          // i
//    OP_SPUT_VOLATILE = 0xe6,          // i
//    OP_IGET_OBJECT_VOLATILE = 0xe7,   // i
//    OP_IGET_WIDE_VOLATILE = 0xe8,     // i
//    OP_IPUT_WIDE_VOLATILE = 0xe9,     // i
//    OP_SGET_WIDE_VOLATILE = 0xea,     // i
//    OP_SPUT_WIDE_VOLATILE = 0xeb,     // i
//    OP_BREAKPOINT = 0xec,
//    OP_THROW_VERIFICATION_ERROR = 0xed,
//    OP_EXECUTE_INLINE = 0xee,
//    OP_EXECUTE_INLINE_RANGE = 0xef,
//    OP_INVOKE_OBJECT_INIT_RANGE = 0xf0,
//    OP_RETURN_VOID_BARRIER = 0xf1,
//    OP_IGET_QUICK = 0xf2,                 // i
//    OP_IGET_WIDE_QUICK = 0xf3,            // i
//    OP_IGET_OBJECT_QUICK = 0xf4,          // i
//    OP_IPUT_QUICK = 0xf5,                 // i
//    OP_IPUT_WIDE_QUICK = 0xf6,            // i
//    OP_IPUT_OBJECT_QUICK = 0xf7,          // i
//    OP_INVOKE_VIRTUAL_QUICK = 0xf8,       // i t
//    OP_INVOKE_VIRTUAL_QUICK_RANGE = 0xf9, // i
//    OP_INVOKE_SUPER_QUICK = 0xfa,         // i
//    OP_INVOKE_SUPER_QUICK_RANGE = 0xfb,   // i
//    OP_IPUT_OBJECT_VOLATILE = 0xfc,       // i
//    OP_SGET_OBJECT_VOLATILE = 0xfd,       // i
//    OP_SPUT_OBJECT_VOLATILE = 0xfe,       // i
//    OP_UNUSED_FF = 0xff,
//    // END(libdex-opcode-enum)
//};


#define dvmThrowNew(clazzName, message)                                                                                                                                  \
       {                                                                                        \
            jclass clazz = (*env).FindClass(clazzName);                                         \
            assert(clazz!= nullptr);                                                            \
            (*env).ThrowNew(clazz, message);                                                    \
       }


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
        LOGD("+ APUT[%d]=0x%08llx", GET_REGISTER(vsrc2),                      \
                (jlong)GET_REGISTER##_regsize(vdst));                          \
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
        if(!dvmResolveField(curMethod, ref, obj, &resVal, &name)){          \
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
#define HANDLE_IPUT_X(_opcode, _opname, _regsize)           \
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
        if(!dvmResolveSetField(curMethod, ref, obj, resVal, &name)){ \
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
        if(!dvmResolveField(curMethod, ref, nullptr, &resVal, &name)){   \
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
        if(!dvmResolveSetField(curMethod, ref, nullptr, resVal, &name)){   \
            GOTO_exceptionThrown();                                         \
        }                                                                   \
        LOGD("+ SPUT '%s'=0x%08llx", name,                                  \
            (u8)GET_REGISTER##_regsize(vdst));                              \
    }                                                                       \
    FINISH(2);


#endif //SHELLAPPLICATION_VMINTERPRETER_H
