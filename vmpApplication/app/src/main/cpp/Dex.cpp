//
// Created by ChenDalunlun on 2019/8/27.
//

#include "Dex.h"
#include <sys/stat.h>
#include <unistd.h>

extern DexFileHelper *gDexFileHelper;

int
(*sysDvmRawDexFileOpen)(const char *fileName, const char *odexOutputName, RawDexFile **ppRawDexFile,
                        bool isBootstrap);

int myDvmRawDexFileOpen(const char *fileName, const char *odexOutputName, RawDexFile **ppRawDexFile,
                        bool isBootstrap) {
    LOG_D("fileName: %s", fileName);
    if (strcmp(fileName, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        LOG_D("fake dex, ha ha ha...");
        *ppRawDexFile = createRawDexFileFromMemoryDalvik();
        LOG_D("*ppRawDexFile: %p", *ppRawDexFile);

        return *ppRawDexFile == nullptr ? -1 : 0;
    }
    return sysDvmRawDexFileOpen(fileName, odexOutputName, ppRawDexFile, isBootstrap);
}

void hookDvmRawDexFileOpen() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = HookNativeInline("/system/lib/libdvm.so",
                                    "_Z17dvmRawDexFileOpenPKcS0_PP10RawDexFileb",
                                    reinterpret_cast<void *>(myDvmRawDexFileOpen),
                                    reinterpret_cast<void **>(&sysDvmRawDexFileOpen));
    }
}

void loadDexFromMemory() {
    LOG_D("start, loadDexFromMemory()");
    JNIEnv *env = gDexFileHelper->env;
    // update mCookie
    LOG_D("0");
    jobject oClassLoader = getClassLoader(env);
    jclass cClassLoader = (*env).GetObjectClass(oClassLoader);
    jclass cBaseDexClassLoader = (*env).GetSuperclass(cClassLoader);
    jfieldID fPathList = (*env).GetFieldID(cBaseDexClassLoader, "pathList",
                                           "Ldalvik/system/DexPathList;");
    jobject oPathList = (*env).GetObjectField(oClassLoader, fPathList);
    LOG_D("1");
    jclass cDexPathList = (*env).GetObjectClass(oPathList);
    jfieldID fDexElements = (*env).GetFieldID(cDexPathList, "dexElements",
                                              "[Ldalvik/system/DexPathList$Element;");
    auto oDexElements =
            reinterpret_cast<jobjectArray>((*env).GetObjectField(oPathList, fDexElements));
    LOG_D("2");
    jclass cDexFile = (*env).FindClass("dalvik/system/DexFile");
    jobject oDexFile = nullptr;
    if (android_get_device_api_level() >= __ANDROID_API_O__) {
        jbyteArray byteArray = (*env).NewByteArray(gDexFileHelper->dexLen);
        (env)->SetByteArrayRegion(byteArray, 0, gDexFileHelper->dexLen,
                                  reinterpret_cast<const jbyte *>(gDexFileHelper->dexBuf));
        jclass cByteBuffer = (*env).FindClass("java/nio/ByteBuffer");
        jmethodID mWrap = (*env).GetStaticMethodID(cByteBuffer, "wrap",
                                                   "([B)Ljava/nio/ByteBuffer;");
        jobject oBuf = (*env).CallStaticObjectMethod(cByteBuffer, mWrap, byteArray);
        jmethodID mDexFileInit = (*env).GetMethodID(cDexFile, "<init>", "(Ljava/nio/ByteBuffer;)V");
        oDexFile = (*env).NewObject(cDexFile, mDexFileInit, oBuf);
    } else {
        if (android_get_device_api_level() >= __ANDROID_API_L__ &&
            android_get_device_api_level() <= __ANDROID_API_N_MR1__) {
            hookArtOpenDexFiles();
        } else if (!isArtVm(env)) {
            hookDvmRawDexFileOpen();
        }
        LOG_D("3");
        jmethodID mLoadDex =
                (*env).GetStaticMethodID(cDexFile, "loadDex",
                                         "(Ljava/lang/String;Ljava/lang/String;I)Ldalvik/system/DexFile;");
        jstring fakeClassesDexName = (*env).NewStringUTF(gDexFileHelper->fakeClassesDexName.data());
        jstring odexDir = (*env).NewStringUTF(getOdexDir(env).data());
        oDexFile = (*env).CallStaticObjectMethod(cDexFile, mLoadDex, fakeClassesDexName,
                                                 odexDir, 0);
    }


    LOG_D("3.5");
    jfieldID fMCookie = nullptr;
    switch (android_get_device_api_level()) {
        case __ANDROID_API_K__:
            fMCookie = (*env).GetFieldID(cDexFile, "mCookie", "I");
            LOG_D("new cookie: 0x%08x", (*env).GetIntField(oDexFile, fMCookie));
            break;

        case __ANDROID_API_L__:
        case __ANDROID_API_L_MR1__:
            fMCookie = (*env).GetFieldID(cDexFile, "mCookie", "J");
            LOG_D("new cookie: 0x%08ld", (*env).GetLongField(oDexFile, fMCookie));
            break;

        case __ANDROID_API_M__:
        case __ANDROID_API_O__:
        case __ANDROID_API_O_MR1__:
        case __ANDROID_API_P__:
            fMCookie = (*env).GetFieldID(cDexFile, "mCookie", "Ljava/lang/Object;");
            LOG_D("new cookie: %p", (*env).GetObjectField(oDexFile, fMCookie));
            break;

        default:
            assert(false);
    }
    jclass cElement = (*env).FindClass("dalvik/system/DexPathList$Element");
    jmethodID mElement = (*env).GetMethodID(cElement, "<init>",
                                            "(Ljava/io/File;ZLjava/io/File;Ldalvik/system/DexFile;)V");
    jobject oElement = (*env).NewObject(cElement, mElement, nullptr, JNI_FALSE, nullptr, oDexFile);
    LOG_D("5");
    jsize oldLenDexElements = (*env).GetArrayLength(oDexElements);
    LOG_D("oldLenDexElements: %d", oldLenDexElements);

    jobjectArray oDexElementsNew = (*env).NewObjectArray(oldLenDexElements + 1, cElement, nullptr);
    for (int i = 0; i < oldLenDexElements; i++) {
        auto t = (*env).GetObjectArrayElement(oDexElements, i);
        (*env).SetObjectArrayElement(oDexElementsNew, i, t);
    }
    (*env).SetObjectArrayElement(oDexElementsNew, oldLenDexElements, oElement);
    LOG_D("6");

    (*env).SetObjectField(oPathList, fDexElements, oDexElementsNew);
    LOG_D("7");
    (*env).DeleteLocalRef(cClassLoader);
    (*env).DeleteLocalRef(cBaseDexClassLoader);
    (*env).DeleteLocalRef(cDexFile);
    (*env).DeleteLocalRef(cDexPathList);
    (*env).DeleteLocalRef(cElement);
    LOG_D("finish, loadDexFromMemory()");
}


RawDexFile *createRawDexFileFromMemoryDalvik() {
    LOG_D("start, createRawDexFileFromMemoryDalvik()");
    /*
     * Set up the basic raw data pointers of a DexFile. This function isn't
     * meant for general use.
     */
    const u1 *data = (const u1 *) gDexFileHelper->dexBuf;
    auto *pDexHeader = (DexHeader *) data;
    auto *pDexFile = new DexFile();
    pDexFile->baseAddr = data;
    pDexFile->pHeader = pDexHeader;
    pDexFile->pStringIds = (const DexStringId *) (data + pDexHeader->stringIdsOff);
    pDexFile->pTypeIds = (const DexTypeId *) (data + pDexHeader->typeIdsOff);
    pDexFile->pFieldIds = (const DexFieldId *) (data + pDexHeader->fieldIdsOff);
    pDexFile->pMethodIds = (const DexMethodId *) (data + pDexHeader->methodIdsOff);
    pDexFile->pProtoIds = (const DexProtoId *) (data + pDexHeader->protoIdsOff);
    pDexFile->pClassDefs = (const DexClassDef *) (data + pDexHeader->classDefsOff);
    pDexFile->pLinkData = (const DexLink *) (data + pDexHeader->linkOff);

    // check dex file size
    if (pDexHeader->fileSize != gDexFileHelper->dexLen) {
        LOG_E("ERROR: stored file size (%d) != expected (%d)",
              pDexHeader->fileSize, gDexFileHelper->dexLen);
    }
    LOG_D("init pDexFile, finish...");

    /*
     * Create auxillary data structures.
     *
     * We need a 4-byte pointer for every reference to a class, method, field,
     * or string constant.  Summed up over all loaded DEX files (including the
     * whoppers in the boostrap class path), this adds up to be quite a bit
     * of native memory.
     *
     * For more traditional VMs these values could be stuffed into the loaded
     * class file constant pool area, but we don't have that luxury since our
     * classes are memory-mapped read-only.
     *
     * The DEX optimizer will remove the need for some of these (e.g. we won't
     * use the entry for virtual methods that are only called through
     * invoke-virtual-quick), creating the possibility of some space reduction
     * at dexopt time.
     */
    uint32_t stringSize = pDexHeader->stringIdsSize * sizeof(struct StringObject *);
    uint32_t classSize = pDexHeader->typeIdsSize * sizeof(struct ClassObject *);
    uint32_t methodSize = pDexHeader->methodIdsSize * sizeof(struct Method *);
    uint32_t fieldSize = pDexHeader->fieldIdsSize * sizeof(struct Field *);
    uint32_t totalSize = sizeof(DvmDex) + stringSize + classSize + methodSize + fieldSize;
    u1 *blob = new u1[totalSize]();
    auto *pDvmDex = (DvmDex *) blob;
    blob += sizeof(DvmDex);

    pDvmDex->pDexFile = pDexFile;
    pDvmDex->pHeader = pDexHeader;

    pDvmDex->pResStrings = (struct StringObject **) blob;
    blob += stringSize;
    pDvmDex->pResClasses = (struct ClassObject **) blob;
    blob += classSize;
    pDvmDex->pResMethods = (struct Method **) blob;
    blob += methodSize;
    pDvmDex->pResFields = (struct Field **) blob;
    LOG_D("+++ DEX %p: allocateAux (%d+%d+%d+%d)*4 = %d bytes",
          pDvmDex, stringSize / 4, classSize / 4, methodSize / 4, fieldSize / 4,
          stringSize + classSize + methodSize + fieldSize);

    auto *newCache = new AtomicCache();
    newCache->numEntries = DEX_INTERFACE_CACHE_SIZE;
    newCache->entryAlloc = new u1[sizeof(AtomicCacheEntry) * newCache->numEntries +
                                  CPU_CACHE_WIDTH];
    /*
     * Adjust storage to align on a 32-byte boundary.  Each entry is 16 bytes
     * wide.  This ensures that each cache entry sits on a single CPU cache
     * line.
     */
    static_assert(sizeof(AtomicCacheEntry) == 16, "");
#if defined(__aarch64__)
    newCache->entries = (AtomicCacheEntry *)
            (((long) newCache->entryAlloc + CPU_CACHE_WIDTH_1) & ~CPU_CACHE_WIDTH_1);
#else
    newCache->entries = (AtomicCacheEntry *)
            (((int) newCache->entryAlloc + CPU_CACHE_WIDTH_1) & ~CPU_CACHE_WIDTH_1);
#endif
    pDvmDex->pInterfaceCache = newCache;
    pthread_mutex_init(&pDvmDex->modLock, nullptr);

    // dvmDex memMapping
    pDvmDex->memMap.baseAddr = pDvmDex->memMap.addr = (void *) data;
    pDvmDex->memMap.baseLength = pDvmDex->memMap.length = pDexHeader->fileSize;
    pDvmDex->isMappedReadOnly = false;
    LOG_D("init pDvmDex, finish...");

    /*
     * Create the class lookup table.  This will eventually be appended
     * to the end of the .odex.
     *
     * We create a temporary link from the DexFile for the benefit of
     * class loading, below.
     */
    DexClassLookup *pClassLookup = nullptr;
    uint32_t numEntries = dexRoundUpPower2(pDexFile->pHeader->classDefsSize * 2);
    LOG_D("numEntries: %d", numEntries);
    uint32_t allocSize = (uint32_t) offsetof(DexClassLookup, table) +
                         numEntries * sizeof(pClassLookup->table[0]);
    LOG_D("allocSize: %d, table sizeof: %ld", allocSize, sizeof(pClassLookup->table[0]));
    pClassLookup = (DexClassLookup *) new u1[allocSize]();
    LOG_D("pClassLookup: %p", pClassLookup);
    pClassLookup->size = allocSize;
    pClassLookup->numEntries = numEntries;
    int maxProbes = 0;
    int totalProbes = 0;
    for (int i = 0; i < pDexFile->pHeader->classDefsSize; i++) {
        const DexClassDef *pClassDef = dexGetClassDef(pDexFile, (u4) i);
        const char *classDescriptor = dexStringByTypeIdx(pDexFile, pClassDef->classIdx);
        u4 hash = classDescriptorHash(classDescriptor);
        int mask = pClassLookup->numEntries - 1;
        int idx = hash & mask;
        /*
         * Find the first empty slot.  We oversized the table, so this is
         * guaranteed to finish.
         */
        int numProbes = 0;
        while (pClassLookup->table[idx].classDescriptorOffset != 0) {
            idx = (idx + 1) & mask;
            numProbes++;
        }
        pClassLookup->table[idx].classDescriptorHash = hash;
        pClassLookup->table[idx].classDescriptorOffset =
                static_cast<int>((u1 *) classDescriptor - pDexFile->baseAddr);
        pClassLookup->table[idx].classDefOffset =
                static_cast<int>((u1 *) pClassDef - pDexFile->baseAddr);
        if (numProbes > maxProbes) {
            maxProbes = numProbes;
        }
        totalProbes += numProbes;
    }
    LOG_D("Class lookup: classes=%d slots=%d (%d%% occ) alloc=%d total=%d max=%d",
          pDexFile->pHeader->classDefsSize, numEntries,
          (100 * pDexFile->pHeader->classDefsSize) / numEntries,
          allocSize, totalProbes, maxProbes);
    LOG_D("init pClassLookup, finish...");

    pDvmDex->pDexFile->pClassLookup = pClassLookup;

    auto *pRawDexFile = new RawDexFile();
    pRawDexFile->pDvmDex = pDvmDex;
    LOG_D("finish, createRawDexFileFromMemoryDalvik()");
    return pRawDexFile;
}


void initDexFileHelper(DexFileHelper **ppDexFileHelper, const ConfigFileProxy *pConfigFileProxy) {
    LOG_D("start, initDexFileHelper(DexFucker **ppDexFileHelper, const ConfigFileProxy *pConfigFileProxy)");

    auto *pDexFileHelper = new DexFileHelper();
    pDexFileHelper->env = pConfigFileProxy->env;
    pDexFileHelper->fakeClassesDexName =
            getDexDir(pConfigFileProxy->env) + pConfigFileProxy->fakeClassesDexName;
    pDexFileHelper->dexBuf = const_cast<char *>(getClassesDexBuf());
    pDexFileHelper->dexLen = pConfigFileProxy->srcClassesDexSize;
    pDexFileHelper->codeItemBuf = getCodeItemBuf();
    pDexFileHelper->codeItemLen = pConfigFileProxy->srcCodeItemSize;

    buildFakeClassesDexFile(pDexFileHelper->fakeClassesDexName);

    const char *cur = pDexFileHelper->codeItemBuf;
    uint32_t size = *(uint32_t *) cur;
    cur += 4;
    for (int i = 0; i < size; i++) {
        uint32_t keySize = *(uint32_t *) cur;
        cur += 4;
        string key = cur;
        cur += keySize;

        uint32_t valueSize = *(uint32_t *) cur;
        cur += 4;
        uint32_t codeBufOff = 0;

        auto *pCodeItemData = (CodeItemData *) (cur + codeBufOff);
        codeBufOff += 0x20; // data: 0x10B, two pointers: 0x10B (64 bit operating system)
        pCodeItemData->insns = (u2 *) (cur + codeBufOff);
        codeBufOff += pCodeItemData->insnsSize * sizeof(u2);
        if (pCodeItemData->triesSize > 0 && (pCodeItemData->insnsSize & 0x01)) {
            codeBufOff += 2;
        }
        if (pCodeItemData->triesSize > 0) {
            pCodeItemData->triesAndHandlersBuf = (u1 *) (cur + codeBufOff);
        } else {
            pCodeItemData->triesAndHandlersBuf = nullptr;
        }
        cur += valueSize;
        pDexFileHelper->codeItem[key] = pCodeItemData;
        LOG_D("codeItem[0x%08x]: %s", i, key.data());
        LOG_D("     registersSize: %d", pCodeItemData->registersSize);
        LOG_D("           insSize: %d", pCodeItemData->insSize);
        LOG_D("           outSize: %d", pCodeItemData->outSize);
        LOG_D("         triesSize: %d", pCodeItemData->triesSize);
        LOG_D("         insnsSize: %d", pCodeItemData->insnsSize);
    }
    LOG_D("code item size: %d, map size: %lu", size, pDexFileHelper->codeItem.size());
    *ppDexFileHelper = pDexFileHelper;
    LOG_D("finish, initDexFileHelper(DexFucker **ppDexFileHelper, const ConfigFileProxy *pConfigFileProxy)");
}

const CodeItemData *getCodeItem(const string &key) {
    auto it = gDexFileHelper->codeItem.find(key);
    if (it != gDexFileHelper->codeItem.end()) {
        return it->second;
    }
    LOG_E("can't found code item data by key: %s", key.data());
    return nullptr;
}


void hookArtOpenDexFiles() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = hookOpenDexFilesFromOat() &&
                   hookDexFileOpen() &&
                   hookFstat() &&
                   hookMmap();
    }
}

int (*sysFstat)(int fp, struct stat *st) = nullptr;

int myFstat(int fp, struct stat *st) {
    int retValue = sysFstat(fp, st);

    char fdlinkstr[256];
    char linkPath[256];
    memset(fdlinkstr, 0, 256);
    memset(linkPath, 0, 256);
    pid_t pid = getpid();
    sprintf(fdlinkstr, "/proc/%d/fd/%d", pid, fp);
    readlink(fdlinkstr, linkPath, 256);
//    LOG_D("fstat file path: %s", linkPath);
    if (strcmp(linkPath, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        st->st_size = gDexFileHelper->dexLen;
        LOG_D("myFstat");
    }
    return retValue;
}

bool hookFstat() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = HookNativeInline((getSystemLibDir() + "/libc.so").data(),
                                    "fstat",
                                    reinterpret_cast<void *>(myFstat),
                                    reinterpret_cast<void **>(&sysFstat));
    }
    return hookFlag;
}

void *(*sysMmap)(void *addr, size_t size, int prot, int flags, int fd, off_t offset) = nullptr;

void *myMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) {
    char fdlinkstr[256];
    char linkPath[256];
    memset(fdlinkstr, 0, 256);
    memset(linkPath, 0, 256);
    pid_t pid = getpid();
    sprintf(fdlinkstr, "/proc/%d/fd/%d", pid, fd);
    readlink(fdlinkstr, linkPath, 256);
//    LOG_D("mmap file path: %s", linkPath);
    if (strcmp(linkPath, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        LOG_D("myMmap");
        LOG_D("dexLen:                  0x%08x", gDexFileHelper->dexLen);
        LOG_D("page_aligned_byte_count: 0x%08lx", size);
        auto *retValue = (uint8_t *) (sysMmap(addr, size,
                                              PROT_READ | PROT_WRITE,
                                              MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
        assert(retValue != MAP_FAILED);
        memcpy(retValue, gDexFileHelper->dexBuf + offset, gDexFileHelper->dexLen);
        size_t off = size - gDexFileHelper->dexLen - offset;
        if (off > 0) {
            memset(retValue + gDexFileHelper->dexLen, 0, off);
        }
        mprotect(retValue, size, prot);
        LOG_D("mmap(%p), success", (void *) retValue);
        return retValue;
    }
    return sysMmap(addr, size, prot, flags, fd, offset);
}

bool hookMmap() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = HookNativeInline((getSystemLibDir() + "/libc.so").data(),
                                    "mmap",
                                    reinterpret_cast<void *>(myMmap),
                                    reinterpret_cast<void **>(&sysMmap));
    }
    return hookFlag;
}

bool (*sysDexFileOpen_21_23)(const char *filename, const char *location, string *error_msg,
                             vector<void *> *dex_files) = nullptr;

bool myDexFileOpen_21_23(const char *filename, const char *location, string *error_msg,
                         vector<void *> *dex_files) {
    return sysDexFileOpen_21_23(filename, location, error_msg, dex_files);
}

bool hookDexFileOpen() {
    static bool hookFlag = false;
    if (!hookFlag) {
        switch (android_get_device_api_level()) {
            case __ANDROID_API_L__:
            case __ANDROID_API_L_MR1__:
                hookFlag = HookNativeInline((getSystemLibDir() + "/libart.so").data(),
                                            "_ZN3art7DexFile4OpenEPKcS2_PNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEPNS3_6vectorIPKS0_NS7_ISD_EEEE",
                                            reinterpret_cast<void *>(myDexFileOpen_21_23),
                                            reinterpret_cast<void **>(&sysDexFileOpen_21_23));
                break;
            case __ANDROID_API_M__:
                hookFlag = HookNativeInline((getSystemLibDir() + "/libart.so").data(),
                                            "_ZN3art7DexFile4OpenEPKcS2_PNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEPNS3_6vectorINS3_10unique_ptrIKS0_NS3_14default_deleteISD_EEEENS7_ISG_EEEE",
                                            reinterpret_cast<void *>(myDexFileOpen_21_23),
                                            reinterpret_cast<void **>(&sysDexFileOpen_21_23));
                break;


            default:
                assert(false);
        }
    }
    return hookFlag;
}

bool
(*sysOpenDexFilesFromOat_21_22)(void *thiz, const char *dex_location, const char *oat_location,
                                vector<string> *error_msgs, vector<void *> *dex_files) = nullptr;

bool
myOpenDexFilesFromOat_21_22(void *thiz, const char *dex_location, const char *oat_location,
                            vector<string> *error_msgs, vector<void *> *dex_files) {
    LOG_D("dex_location: %s", dex_location);
    if (strcmp(dex_location, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        string error_msg;
        if (!sysDexFileOpen_21_23(dex_location, dex_location, &error_msg, dex_files)) {
            LOG_E("Failed to open dex files from %s", dex_location);
            LOG_E("DexFileOpen error: %s", error_msg.data());
            error_msgs->push_back("Failed to open dex files from " + string(dex_location));
        }
        LOG_D("DexFileOpen, success");
        return true;
    }
    return sysOpenDexFilesFromOat_21_22(thiz, dex_location, oat_location, error_msgs, dex_files);
}

vector<void *>
(*sysOpenDexFilesFromOat_23)(void *thiz, const char *dex_location, const char *oat_location,
                             vector<string> *error_msgs) = nullptr;

vector<void *>
myOpenDexFilesFromOat_23(void *thiz, const char *dex_location, const char *oat_location,
                         vector<string> *error_msgs) {
    LOG_D("dex_location: %s", dex_location);
    if (strcmp(dex_location, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        vector<void *> dexFiles;
        string error_msg;
        if (!sysDexFileOpen_21_23(dex_location, dex_location, &error_msg, &dexFiles)) {
            LOG_E("Failed to open dex files from %s", dex_location);
            LOG_E("DexFileOpen error: %s", error_msg.data());
            error_msgs->push_back("Failed to open dex files from " + string(dex_location));
        }
        LOG_D("DexFileOpen, success");
        return dexFiles;
    }
    return sysOpenDexFilesFromOat_23(thiz, dex_location, oat_location, error_msgs);
}


bool hookOpenDexFilesFromOat() {
    static bool hookFlag = false;
    if (!hookFlag) {
        switch (android_get_device_api_level()) {

            case __ANDROID_API_L__:
            case __ANDROID_API_L_MR1__:
                hookFlag = HookNativeInline((getSystemLibDir() + "/libart.so").data(),
                                            "_ZN3art11ClassLinker19OpenDexFilesFromOatEPKcS2_PNSt3__16vectorINS3_12basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEENS8_ISA_EEEEPNS4_IPKNS_7DexFileENS8_ISG_EEEE",
                                            reinterpret_cast<void *>(myOpenDexFilesFromOat_21_22),
                                            reinterpret_cast<void **>(&sysOpenDexFilesFromOat_21_22));
                break;

            case __ANDROID_API_M__:
                hookFlag = HookNativeInline((getSystemLibDir() + "/libart.so").data(),
                                            "_ZN3art11ClassLinker19OpenDexFilesFromOatEPKcS2_PNSt3__16vectorINS3_12basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEENS8_ISA_EEEE",
                                            reinterpret_cast<void *>(myOpenDexFilesFromOat_23),
                                            reinterpret_cast<void **>(&sysOpenDexFilesFromOat_23));
                break;

            default:
                assert(false);
        }

    }
    return hookFlag;
}

