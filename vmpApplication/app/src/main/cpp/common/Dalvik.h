//
// Created by ChenDalunlun on 2019/6/25.
//

#ifndef SHELLAPPLICATION_DALVIK_H
#define SHELLAPPLICATION_DALVIK_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <jni.h>
#include <vector>
#include <string>
#include <atomic>


#include <jni.h>
#include <cstring>

using namespace std;

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

#define PACKED(x) __attribute__ ((__aligned__(x), __packed__))
#define MANAGED PACKED(4)

template<typename T>
class PACKED(sizeof(T)) Atomic : public atomic<T> {

};

class ArtDexFile {
public:
    const void *virtualMethod;

    // The base address of the memory mapping.
    const uint8_t *const begin;

    // The size of the underlying memory allocation in bytes.
    const size_t size;

    // Typically the dex file name when available, alternatively some identifying string.
    //
    // The ClassLinker will use this to match DexFiles the boot class
    // path to DexCache::GetLocation when loading from an image.
    const std::string location;

    const uint32_t location_checksum;

    // Manages the underlying memory allocation.
    void *mem_map;

    // Points to the header section.
    const void *const header;

    // Points to the base of the string identifier list.
    const void *const string_ids;

    // Points to the base of the type identifier list.
    const void *const type_ids;

    // Points to the base of the field identifier list.
    const void *const field_ids;

    // Points to the base of the method identifier list.
    const void *const method_ids;

    // Points to the base of the prototype identifier list.
    const void *const proto_ids;

    // Points to the base of the class definition list.
    const void *const class_defs;

    // Number of misses finding a class def from a descriptor.
    mutable Atomic<uint32_t> find_class_def_misses;

    mutable Atomic<void *> class_def_index;

    // If this dex file was loaded from an oat file, oat_dex_file_ contains a
    // pointer to the OatDexFile it was loaded from. Otherwise oat_dex_file_ is
    // null.
    const void *oat_dex_file;
};


// C++ mirror of java.lang.Object
class MANAGED ArtObject {
public:
    // The Class representing the type of the object.
    uint32_t klass;
    // Monitor and hash code information.
    uint32_t monitor;

};

// C++ mirror of java.lang.DexCache.
class MANAGED ArtDexCache : public ArtObject {
public:
    uint32_t dex;
    uint32_t location;
    // Either an int array or long array based on runtime ISA since these arrays hold pointers.
    uint32_t resolved_fields;
    uint32_t resolved_methods;
    uint32_t resolved_types;
    uint32_t strings;
    uint64_t dex_file;
};


// C++ mirror of java.lang.Class
class MANAGED ArtClass : public ArtObject {
public:
    // Defining class loader, or null for the "bootstrap" system loader.
    uint32_t class_loader;

    // For array classes, the component class object for instanceof/checkcast
    // (for String[][][], this will be String[][]). null for non-array classes.
    uint32_t component_type;

    // DexCache of resolved constant pool entries (will be null for classes generated by the
    // runtime such as arrays and primitive classes).
    uint32_t dex_cache;

    // Short cuts to dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_strings;

    // The interface table (iftable_) contains pairs of a interface class and an array of the
    // interface methods. There is one pair per interface supported by this class.  That means one
    // pair for each interface we support directly, indirectly via superclass, or indirectly via a
    // superinterface.  This will be null if neither we nor our superclass implement any interfaces.
    //
    // Why we need this: given "class Foo implements Face", declare "Face faceObj = new Foo()".
    // Invoke faceObj.blah(), where "blah" is part of the Face interface.  We can't easily use a
    // single vtable.
    //
    // For every interface a concrete class implements, we create an array of the concrete vtable_
    // methods for the methods in the interface.
    uint32_t iftable;

    // Descriptor for the class such as "java.lang.Class" or "[C". Lazily initialized by ComputeName
    uint32_t name;

    // The superclass, or null if this is java.lang.Object, an interface or primitive type.
    uint32_t super_class;

    // If class verify fails, we must return same error on subsequent tries.
    uint32_t verify_error_class;

    // Virtual method table (vtable), for use by "invoke-virtual".  The vtable from the superclass is
    // copied in, and virtual methods from our class either replace those from the super or are
    // appended. For abstract classes, methods may be created in the vtable that aren't in
    // virtual_ methods_ for miranda methods.
    uint32_t vtable;

    // Access flags; low 16 bits are defined by VM spec.
    // Note: Shuffled back.
    uint32_t access_flags;

    // static, private, and <init> methods. Pointer to an ArtMethod array.
    uint64_t direct_methods;

    // instance fields
    //
    // These describe the layout of the contents of an Object.
    // Note that only the fields directly declared by this class are
    // listed in ifields; fields declared by a superclass are listed in
    // the superclass's Class.ifields.
    //
    // ArtField arrays are allocated as an array of fields, and not an array of fields pointers.
    uint64_t ifields;

    // Static fields
    uint64_t sfields;

    // Virtual methods defined in this class; invoked through vtable. Pointer to an ArtMethod array.
    uint64_t virtual_methods;

    // Total size of the Class instance; used when allocating storage on gc heap.
    // See also object_size_.
    uint32_t class_size;

    // Tid used to check for recursive <clinit> invocation.
    pid_t clinit_thread_id;

    // ClassDef index in dex file, -1 if no class definition such as an array.
    int32_t dex_class_def_idx;

    // Type index in dex file.
    int32_t dex_type_idx;

    // Number of direct fields.
    uint32_t num_direct_methods;

    // Number of instance fields.
    uint32_t num_instance_fields;

    // Number of instance fields that are object refs.
    uint32_t num_reference_instance_fields;

    // Number of static fields that are object refs,
    uint32_t num_reference_static_fields;

    // Number of static fields.
    uint32_t num_static_fields;

    // Number of virtual methods.
    uint32_t num_virtual_methods;

    // Total object size; used when allocating storage on gc heap.
    // (For interfaces and abstract classes this will be zero.)
    // See also class_size_.
    uint32_t object_size;

    // The lower 16 bits contains a Primitive::Type value. The upper 16
    // bits contains the size shift of the primitive type.
    uint32_t primitive_type;

    // Bitmap of offsets of ifields.
    uint32_t reference_instance_offsets;
};

class MANAGED ArtMethod_21_22 :public ArtObject{
public:
    // Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
    // The class we are a part of.
    uint32_t declaring_class;

    // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_resolved_methods;

    // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_resolved_types;

    // Access flags; low 16 bits are defined by spec.
    uint32_t access_flags;

    /* Dex file fields. The defining dex file is available via declaring_class_->dex_cache_ */

    // Offset to the CodeItem.
    uint32_t dex_code_item_offset;

    // Index into method_ids of the dex file associated with this method.
    uint32_t dex_method_index;

    /* End of dex file fields. */

    // Entry within a dispatch table for this method. For static/direct methods the index is into
    // the declaringClass.directMethods, for virtual methods the vtable and for interface methods the
    // ifTable.
    uint32_t method_index;
};


class ArtMethod_23 {
public:
    // Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
    // The class we are a part of.
    uint32_t declaring_class;

    // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_resolved_methods;

    // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_resolved_types;

    // Access flags; low 16 bits are defined by spec.
    uint32_t access_flags;

    /* Dex file fields. The defining dex file is available via declaring_class_->dex_cache_ */

    // Offset to the CodeItem.
    uint32_t dex_code_item_offset;

    // Index into method_ids of the dex file associated with this method.
    uint32_t dex_method_index;

    /* End of dex file fields. */

    // Entry within a dispatch table for this method. For static/direct methods the index is into
    // the declaringClass.directMethods, for virtual methods the vtable and for interface methods the
    // ifTable.
    uint32_t method_index;
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

    IndirectRefSlot *table;
    size_t i;
    size_t capacity;
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
    IndirectRefSlot *table;
    /* bit mask, ORed into all irefs */
    IndirectRefKind kind;
    /* #of entries we have space for */
    size_t alloc_entries;
    /* max #of entries allowed */
    size_t max_entries;

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
    VERIFY_ERROR_REF_CLASS = 0,
    VERIFY_ERROR_REF_FIELD = 1,
    VERIFY_ERROR_REF_METHOD = 2,
};

#define kThrowShow_accessFromClass     1


class ArtField {
public:

    uint32_t declaring_class;

    uint32_t access_flags;

    // Dex cache index of field id
    uint32_t field_dex_idx;

    // Offset of field within an instance or in the Class' static fields
    uint32_t offset;
};


#endif //SHELLAPPLICATION_HEADERINNEED_H
