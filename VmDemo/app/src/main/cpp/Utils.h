//
// Created by ChenD on 2019/5/20.
//

#ifndef SHELL_UTIL_H
#define SHELL_UTIL_H

#include <jni.h>
#include <android/log.h>
#include <sys/system_properties.h>
#include <string>
#include "include/AndHook.h"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,__FUNCTION__,__VA_ARGS__) // 定义LOGD类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,__FUNCTION__,__VA_ARGS__) // 定义LOGE类型

#define BUF_SIZE 1024

using namespace std;

bool HookNativeInline(const char *soPath, const char *signature, void *my_func,
                      void **ori_func);

void GetSymbolByDLOpen(const char *soPath, const char *signature, void **method);

bool
HookJava(JNIEnv *env, const char *clazzPath, const char *methodName, const char *methodSignature,
         const void *my_func, jmethodID *ori_func);


/*
 * Reads a signed LEB128 value, updating the given pointer to point
 * just past the end of the read value. This function tolerates
 * non-zero high-order bits in the fifth encoded byte.
 */
inline int readSignedLeb128(const u1 **pStream) {
    const u1 *ptr = *pStream;
    int result = *(ptr++);

    if (result <= 0x7f) {
        result = (result << 25) >> 25;
    } else {
        int cur = *(ptr++);
        result = (result & 0x7f) | ((cur & 0x7f) << 7);
        if (cur <= 0x7f) {
            result = (result << 18) >> 18;
        } else {
            cur = *(ptr++);
            result |= (cur & 0x7f) << 14;
            if (cur <= 0x7f) {
                result = (result << 11) >> 11;
            } else {
                cur = *(ptr++);
                result |= (cur & 0x7f) << 21;
                if (cur <= 0x7f) {
                    result = (result << 4) >> 4;
                } else {
                    /*
                     * Note: We don't check to see if cur is out of
                     * range here, meaning we tolerate garbage in the
                     * high four-order bits.
                     */
                    cur = *(ptr++);
                    result |= cur << 28;
                }
            }
        }
    }

    *pStream = ptr;
    return result;
}

/*
 * Reads an unsigned LEB128 value, updating the given pointer to point
 * just past the end of the read value. This function tolerates
 * non-zero high-order bits in the fifth encoded byte.
 */
inline int readUnsignedLeb128(const u1 **pStream) {
    const u1 *ptr = *pStream;
    int result = *(ptr++);

    if (result > 0x7f) {
        int cur = *(ptr++);
        result = (result & 0x7f) | ((cur & 0x7f) << 7);
        if (cur > 0x7f) {
            cur = *(ptr++);
            result |= (cur & 0x7f) << 14;
            if (cur > 0x7f) {
                cur = *(ptr++);
                result |= (cur & 0x7f) << 21;
                if (cur > 0x7f) {
                    /*
                     * Note: We don't check to see if cur is out of
                     * range here, meaning we tolerate garbage in the
                     * high four-order bits.
                     */
                    cur = *(ptr++);
                    result |= cur << 28;
                }
            }
        }
    }

    *pStream = ptr;
    return result;
}

/* return the DexMapList of the file, if any */
inline const DexMapList *dexGetMap(const DexFile *pDexFile) {
    u4 mapOff = pDexFile->pHeader->mapOff;

    if (mapOff == 0) {
        return NULL;
    } else {
        return (const DexMapList *) (pDexFile->baseAddr + mapOff);
    }
}

/* return the const char* string data referred to by the given string_id */
inline const char *dexGetStringData(const DexFile *pDexFile,
                                    const DexStringId *pStringId) {
    const u1 *ptr = pDexFile->baseAddr + pStringId->stringDataOff;

    // Skip the uleb128 length.
    while (*(ptr++) > 0x7f) /* empty */ ;

    return (const char *) ptr;
}

/* return the StringId with the specified index */
inline const DexStringId *dexGetStringId(const DexFile *pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->stringIdsSize);
    return &pDexFile->pStringIds[idx];
}

/* return the UTF-8 encoded string with the specified string_id index */
inline const char *dexStringById(const DexFile *pDexFile, u4 idx) {
    const DexStringId *pStringId = dexGetStringId(pDexFile, idx);
    return dexGetStringData(pDexFile, pStringId);
}

/* Return the UTF-8 encoded string with the specified string_id index,
 * also filling in the UTF-16 size (number of 16-bit code points).*/
const char *dexStringAndSizeById(const DexFile *pDexFile, u4 idx,
                                 u4 *utf16Size);

/* return the TypeId with the specified index */
inline const DexTypeId *dexGetTypeId(const DexFile *pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->typeIdsSize);
    return &pDexFile->pTypeIds[idx];
}

/*
 * Get the descriptor string associated with a given type index.
 * The caller should not free() the returned string.
 */
inline const char *dexStringByTypeIdx(const DexFile *pDexFile, u4 idx) {
    const DexTypeId *typeId = dexGetTypeId(pDexFile, idx);
    return dexStringById(pDexFile, typeId->descriptorIdx);
}

/* return the MethodId with the specified index */
inline const DexMethodId *dexGetMethodId(const DexFile *pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->methodIdsSize);
    return &pDexFile->pMethodIds[idx];
}

/* return the FieldId with the specified index */
inline const DexFieldId *dexGetFieldId(const DexFile *pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->fieldIdsSize);
    return &pDexFile->pFieldIds[idx];
}

/* return the ProtoId with the specified index */
inline const DexProtoId *dexGetProtoId(const DexFile *pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->protoIdsSize);
    return &pDexFile->pProtoIds[idx];
}

/*
 * Get the parameter list from a ProtoId. The returns NULL if the ProtoId
 * does not have a parameter list.
 */
inline const DexTypeList *dexGetProtoParameters(
        const DexFile *pDexFile, const DexProtoId *pProtoId) {
    if (pProtoId->parametersOff == 0) {
        return NULL;
    }
    return (const DexTypeList *)
            (pDexFile->baseAddr + pProtoId->parametersOff);
}

/* return the ClassDef with the specified index */
inline const DexClassDef *dexGetClassDef(const DexFile *pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->classDefsSize);
    return &pDexFile->pClassDefs[idx];
}

/* given a ClassDef pointer, recover its index */
inline u4 dexGetIndexForClassDef(const DexFile *pDexFile,
                                 const DexClassDef *pClassDef) {
    assert(pClassDef >= pDexFile->pClassDefs &&
           pClassDef < pDexFile->pClassDefs + pDexFile->pHeader->classDefsSize);
    return pClassDef - pDexFile->pClassDefs;
}

/* get the interface list for a DexClass */
inline const DexTypeList *dexGetInterfacesList(const DexFile *pDexFile,
                                               const DexClassDef *pClassDef) {
    if (pClassDef->interfacesOff == 0)
        return NULL;
    return (const DexTypeList *)
            (pDexFile->baseAddr + pClassDef->interfacesOff);
}

/* return the Nth entry in a DexTypeList. */
inline const DexTypeItem *dexGetTypeItem(const DexTypeList *pList,
                                         u4 idx) {
    assert(idx < pList->size);
    return &pList->list[idx];
}

/* return the type_idx for the Nth entry in a TypeList */
inline u4 dexTypeListGetIdx(const DexTypeList *pList, u4 idx) {
    const DexTypeItem *pItem = dexGetTypeItem(pList, idx);
    return pItem->typeIdx;
}

/* get the static values list for a DexClass */
inline const DexEncodedArray *dexGetStaticValuesList(
        const DexFile *pDexFile, const DexClassDef *pClassDef) {
    if (pClassDef->staticValuesOff == 0)
        return NULL;
    return (const DexEncodedArray *)
            (pDexFile->baseAddr + pClassDef->staticValuesOff);
}

/* get the annotations directory item for a DexClass */
inline const DexAnnotationsDirectoryItem *dexGetAnnotationsDirectoryItem(
        const DexFile *pDexFile, const DexClassDef *pClassDef) {
    if (pClassDef->annotationsOff == 0)
        return NULL;
    return (const DexAnnotationsDirectoryItem *)
            (pDexFile->baseAddr + pClassDef->annotationsOff);
}

/* get the source file string */
inline const char *dexGetSourceFile(
        const DexFile *pDexFile, const DexClassDef *pClassDef) {
    if (pClassDef->sourceFileIdx == 0xffffffff)
        return NULL;
    return dexStringById(pDexFile, pClassDef->sourceFileIdx);
}

/* get the size, in bytes, of a DexCode */
size_t dexGetDexCodeSize(const DexCode *pCode);

/* Get the list of "tries" for the given DexCode. */
inline const DexTry *dexGetTries(const DexCode *pCode) {
    const u2 *insnsEnd = &pCode->insns[pCode->insnsSize];

    // Round to four bytes.
    if ((((uintptr_t) insnsEnd) & 3) != 0) {
        insnsEnd++;
    }

    return (const DexTry *) insnsEnd;
}

/* Get the base of the encoded data for the given DexCode. */
inline const u1 *dexGetCatchHandlerData(const DexCode *pCode) {
    const DexTry *pTries = dexGetTries(pCode);
    return (const u1 *) &pTries[pCode->triesSize];
}

/* get a pointer to the start of the debugging data */
inline const u1 *dexGetDebugInfoStream(const DexFile *pDexFile,
                                       const DexCode *pCode) {
    if (pCode->debugInfoOff == 0) {
        return NULL;
    } else {
        return pDexFile->baseAddr + pCode->debugInfoOff;
    }
}

/* DexClassDef convenience - get class descriptor */
inline const char *dexGetClassDescriptor(const DexFile *pDexFile,
                                         const DexClassDef *pClassDef) {
    return dexStringByTypeIdx(pDexFile, pClassDef->classIdx);
}

/* DexClassDef convenience - get superclass descriptor */
inline const char *dexGetSuperClassDescriptor(const DexFile *pDexFile,
                                              const DexClassDef *pClassDef) {
    if (pClassDef->superclassIdx == 0)
        return NULL;
    return dexStringByTypeIdx(pDexFile, pClassDef->superclassIdx);
}

/* DexClassDef convenience - get class_data_item pointer */
inline const DexClassData *dexGetClassData(const DexFile *pDexFile,
                                           const DexClassDef *pClassDef) {
    if (pClassDef->classDataOff == 0)
        return NULL;
    return reinterpret_cast<const DexClassData *>(pDexFile->baseAddr + pClassDef->classDataOff);
}

/* Get an annotation set at a particular offset. */
inline const DexAnnotationSetItem *dexGetAnnotationSetItem(
        const DexFile *pDexFile, u4 offset) {
    if (offset == 0) {
        return NULL;
    }
    return (const DexAnnotationSetItem *) (pDexFile->baseAddr + offset);
}

/* get the class' annotation set */
inline const DexAnnotationSetItem *dexGetClassAnnotationSet(
        const DexFile *pDexFile, const DexAnnotationsDirectoryItem *pAnnoDir) {
    return dexGetAnnotationSetItem(pDexFile, pAnnoDir->classAnnotationsOff);
}

/* get the class' field annotation list */
inline const DexFieldAnnotationsItem *dexGetFieldAnnotations(
        const DexFile *pDexFile, const DexAnnotationsDirectoryItem *pAnnoDir) {
    if (pAnnoDir->fieldsSize == 0)
        return NULL;

    // Skip past the header to the start of the field annotations.
    return (const DexFieldAnnotationsItem *) &pAnnoDir[1];
}

/* get field annotation list size */
inline int dexGetFieldAnnotationsSize(const DexFile *pDexFile,
                                      const DexAnnotationsDirectoryItem *pAnnoDir) {
    return pAnnoDir->fieldsSize;
}

/* return a pointer to the field's annotation set */
inline const DexAnnotationSetItem *dexGetFieldAnnotationSetItem(
        const DexFile *pDexFile, const DexFieldAnnotationsItem *pItem) {
    return dexGetAnnotationSetItem(pDexFile, pItem->annotationsOff);
}

/* get the class' method annotation list */
inline const DexMethodAnnotationsItem *dexGetMethodAnnotations(
        const DexFile *pDexFile, const DexAnnotationsDirectoryItem *pAnnoDir) {
    if (pAnnoDir->methodsSize == 0)
        return NULL;

    /*
     * Skip past the header and field annotations to the start of the
     * method annotations.
     */
    const u1 *addr = (const u1 *) &pAnnoDir[1];
    addr += pAnnoDir->fieldsSize * sizeof(DexFieldAnnotationsItem);
    return (const DexMethodAnnotationsItem *) addr;
}

/* get method annotation list size */
inline int dexGetMethodAnnotationsSize(const DexFile *pDexFile,
                                       const DexAnnotationsDirectoryItem *pAnnoDir) {
    return pAnnoDir->methodsSize;
}

/* return a pointer to the method's annotation set */
inline const DexAnnotationSetItem *dexGetMethodAnnotationSetItem(
        const DexFile *pDexFile, const DexMethodAnnotationsItem *pItem) {
    return dexGetAnnotationSetItem(pDexFile, pItem->annotationsOff);
}

/* get the class' parameter annotation list */
inline const DexParameterAnnotationsItem *dexGetParameterAnnotations(
        const DexFile *pDexFile, const DexAnnotationsDirectoryItem *pAnnoDir) {
    if (pAnnoDir->parametersSize == 0)
        return NULL;

    /*
     * Skip past the header, field annotations, and method annotations
     * to the start of the parameter annotations.
     */
    const u1 *addr = (const u1 *) &pAnnoDir[1];
    addr += pAnnoDir->fieldsSize * sizeof(DexFieldAnnotationsItem);
    addr += pAnnoDir->methodsSize * sizeof(DexMethodAnnotationsItem);
    return (const DexParameterAnnotationsItem *) addr;
}

/* get method annotation list size */
inline int dexGetParameterAnnotationsSize(const DexFile *pDexFile,
                                          const DexAnnotationsDirectoryItem *pAnnoDir) {
    return pAnnoDir->parametersSize;
}

/* return the parameter annotation ref list */
inline const DexAnnotationSetRefList *dexGetParameterAnnotationSetRefList(
        const DexFile *pDexFile, const DexParameterAnnotationsItem *pItem) {
    if (pItem->annotationsOff == 0) {
        return NULL;
    }
    return (const DexAnnotationSetRefList *) (pDexFile->baseAddr + pItem->annotationsOff);
}

/* get method annotation list size */
inline int dexGetParameterAnnotationSetRefSize(const DexFile *pDexFile,
                                               const DexParameterAnnotationsItem *pItem) {
    if (pItem->annotationsOff == 0) {
        return 0;
    }
    return dexGetParameterAnnotationSetRefList(pDexFile, pItem)->size;
}

/* return the Nth entry from an annotation set ref list */
inline const DexAnnotationSetRefItem *dexGetParameterAnnotationSetRef(
        const DexAnnotationSetRefList *pList, u4 idx) {
    assert(idx < pList->size);
    return &pList->list[idx];
}

/* given a DexAnnotationSetRefItem, return the DexAnnotationSetItem */
inline const DexAnnotationSetItem *dexGetSetRefItemItem(
        const DexFile *pDexFile, const DexAnnotationSetRefItem *pItem) {
    return dexGetAnnotationSetItem(pDexFile, pItem->annotationsOff);
}

/* return the Nth annotation offset from a DexAnnotationSetItem */
inline u4 dexGetAnnotationOff(
        const DexAnnotationSetItem *pAnnoSet, u4 idx) {
    assert(idx < pAnnoSet->size);
    return pAnnoSet->entries[idx];
}

/* return the Nth annotation item from a DexAnnotationSetItem */
inline const DexAnnotationItem *dexGetAnnotationItem(
        const DexFile *pDexFile, const DexAnnotationSetItem *pAnnoSet, u4 idx) {
    u4 offset = dexGetAnnotationOff(pAnnoSet, idx);
    if (offset == 0) {
        return nullptr;
    }
    return (const DexAnnotationItem *) (pDexFile->baseAddr + offset);
}

inline bool dvmIsStaticMethod(const Method *method) {
    return (method->accessFlags & ACC_STATIC) != 0;
}

inline bool dvmIsAbstractMethod(const Method *method) {
    return (method->accessFlags & ACC_ABSTRACT) != 0;
}

#define BYTE_OFFSET(_ptr, _offset)  ((void*) (((u1*)(_ptr)) + (_offset)))

inline s4 dvmGetFieldInt(const Object *obj, int offset) {
    return ((JValue *) BYTE_OFFSET(obj, offset))->i;
}

inline s8 dvmGetFieldLong(const Object *obj, int offset) {
    return ((JValue *) BYTE_OFFSET(obj, offset))->j;
}

inline Object *dvmGetFieldObject(const Object *obj, int offset) {
    return ((JValue *) BYTE_OFFSET(obj, offset))->l;
}

/* Get whether the given method has associated bytecode. This is the
 * case for methods which are neither native nor abstract. */
inline bool dvmIsBytecodeMethod(const Method *method) {
    return (method->accessFlags & (ACC_NATIVE | ACC_ABSTRACT)) == 0;
}

/*
 * Get the associated code struct for a method. This returns NULL
 * for non-bytecode methods.
 */
inline const DexCode *dvmGetMethodCode(const Method *meth) {

    /*
     * The insns field for a bytecode method actually points at
     * &(DexCode.insns), so we can subtract back to get at the
     * DexCode in front.
     */
    return (const DexCode *)
            (((const u1 *) meth->insns) - offsetof(DexCode, insns));
}

#endif //SHELL_UTIL_H
