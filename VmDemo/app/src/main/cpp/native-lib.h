//
// Created by dalunlun on 2019/6/27.
//

#ifndef VMDEMO_NATIVE_LIB_H
#define VMDEMO_NATIVE_LIB_H

#include "include/DVMInNeed.h"
#include <string>

using namespace std;

JNIEnv *env = nullptr;

jmethodID dvmResolveMethod(const ClassObject *referrer, u4 methodIdx, MethodType methodType,
                           jclass *methodToCallClazz);

bool dvmResolveField(const ClassObject *referrer, u4 ifieldIdx, jobject obj, s8 *res,
                     const char **ppName);

bool dvmResolveSetField(const ClassObject *referrer, u4 ifieldIdx, jobject obj, u8 res,
                        const char **ppName);

void
dvmResolveMethodSign(const ClassObject *referrer, const DexMethodId *pMethodId, string *methodSign);

void
dvmResolveMethodSign(const ClassObject *referrer, const DexProto *pDexProto, string *methodSign);

inline u4 dvmFloatToU4(float val);

jstring dvmResolveString(const ClassObject *referrer, u4 stringIdx);

jclass dvmResolveClass(const ClassObject *referrer, u4 classIdx);

jclass dvmResolveClass(const ClassObject *referrer, u4 classIdx, string *clazzNameString);

jarray dvmAllocArrayByClass(const s4 length, const ClassObject *referrer, u4 classIdx);

string getClassNameByJClass(jclass clazz);

void
dvmInterpret(JNIEnv *env, jobject instance, const Method *curMethod, u4 *reg, const u2 *curInsns,
             jvalue *pResult);

void
dvmCallMethod(JNIEnv *env, jobject instance, const Method *method, const u2 *insns, jvalue *pResult,
              ...);

bool checkForNull(jobject obj);

ClassObject *getClassObjectByJClass(jclass clazz);

ClassObject *getClassObjectByJObject(jobject obj);

bool dvmInterpHandleFillArrayData(jarray arrayObj, const u2 *arrayData);

s4 dvmInterpHandlePackedSwitch(const u2 *switchData, s4 testVal);

s4 dvmInterpHandleSparseSwitch(const u2 *switchData, s4 testVal);

inline s4 s4FromSwitchData(const void *switchData) {
    return *(s4 *) switchData;
}

bool dvmCanPutArrayElement(const jobject obj, const jobject arrayObj);

void debugInvokeMethod(const Method *method, const jvalue retVal, const jvalue *vars);

int dvmFindCatchBlock(const Method *method, size_t pcOff, jobject exception);

string dvmHumanReadableDescriptor(const char *descriptor);

string classNameFromIndex(const Method *method, int ref, VerifyErrorRefType refType, int flags);

string fieldNameFromIndex(const Method *method, int ref, VerifyErrorRefType refType, int flags);

string methodNameFromIndex(const Method *method, int ref, VerifyErrorRefType refType, int flags);

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

void dvmThrowVerificationError(const Method *method, int kind, int ref);
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


#endif //VMDEMO_NATIVE_LIB_H
