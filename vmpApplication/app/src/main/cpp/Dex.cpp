//
// Created by ChenDalunlun on 2019/8/27.
//

#include "Dex.h"
#include <fcntl.h>
#include <sys/stat.h>

extern DexFileHelper *gDexFileHelper;

int
(*sysDvmRawDexFileOpen)(const char *fileName, const char *odexOutputName, RawDexFile **ppRawDexFile,
                        bool isBootstrap);

int myDvmRawDexFileOpen(const char *fileName, const char *odexOutputName, RawDexFile **ppRawDexFile,
                        bool isBootstrap) {
    LOGD("fileName: %s", fileName);
    if (strcmp(fileName, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        LOGD("fake dex, ha ha ha...");
        *ppRawDexFile = createRawDexFileFromMemoryDalvik();
        LOGD("*ppRawDexFile: %p", *ppRawDexFile);

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
    LOGD("start, loadDexFromMemory()");
    JNIEnv *env = gDexFileHelper->env;
    // update mCookie
    LOGD("0");
    jobject oClassLoader = getClassLoader(env);
    jclass cClassLoader = (*env).GetObjectClass(oClassLoader);
    jclass cBaseDexClassLoader = (*env).GetSuperclass(cClassLoader);
    jfieldID fPathList = (*env).GetFieldID(cBaseDexClassLoader, "pathList",
                                           "Ldalvik/system/DexPathList;");
    jobject oPathList = (*env).GetObjectField(oClassLoader, fPathList);
    LOGD("1");
    jclass cDexPathList = (*env).GetObjectClass(oPathList);
    jfieldID fDexElements = (*env).GetFieldID(cDexPathList, "dexElements",
                                              "[Ldalvik/system/DexPathList$Element;");
    auto oDexElements =
            reinterpret_cast<jobjectArray>((*env).GetObjectField(oPathList, fDexElements));
    LOGD("2");
    if (isArtVm(env)) {
        hookArtOpenDexFiles();
    } else {
        hookDvmRawDexFileOpen();
    }
    LOGD("3");
    jclass cDexFile = (*env).FindClass("dalvik/system/DexFile");
    jmethodID mLoadDex =
            (*env).GetStaticMethodID(cDexFile, "loadDex",
                                     "(Ljava/lang/String;Ljava/lang/String;I)Ldalvik/system/DexFile;");
    jstring fakeClassesDexName = (*env).NewStringUTF(gDexFileHelper->fakeClassesDexName.data());
    jstring odexDir = (*env).NewStringUTF(getOdexDir(env).data());
    jobject oDexFile = (*env).CallStaticObjectMethod(cDexFile, mLoadDex, fakeClassesDexName,
                                                     odexDir, 0);
    LOGD("3.5");
    jfieldID fMCookie = nullptr;
    switch (android_get_device_api_level()){
        case __ANDROID_API_K__:
            fMCookie = (*env).GetFieldID(cDexFile, "mCookie", "I");
            LOGD("new cookie: 0x%08x", (*env).GetIntField(oDexFile, fMCookie));
            break;

        case __ANDROID_API_L__:
        case __ANDROID_API_L_MR1__:
            fMCookie = (*env).GetFieldID(cDexFile, "mCookie", "J");
            LOGD("new cookie: 0x%08lld", (*env).GetLongField(oDexFile, fMCookie));
            break;

        case __ANDROID_API_M__:
            fMCookie = (*env).GetFieldID(cDexFile, "mCookie", "Ljava/lang/Object;");
            LOGD("new cookie: %p", (*env).GetObjectField(oDexFile, fMCookie));
            break;

        default:
            assert(false);
    }
    jclass cElement = (*env).FindClass("dalvik/system/DexPathList$Element");
    jmethodID mElement = (*env).GetMethodID(cElement, "<init>",
                                            "(Ljava/io/File;ZLjava/io/File;Ldalvik/system/DexFile;)V");
    jobject oElement = (*env).NewObject(cElement, mElement, nullptr, JNI_FALSE, nullptr, oDexFile);
    LOGD("5");
    jsize oldLenDexElements = (*env).GetArrayLength(oDexElements);
    LOGD("oldLenDexElements: %d", oldLenDexElements);

    jobjectArray oDexElementsNew = (*env).NewObjectArray(oldLenDexElements + 1, cElement, nullptr);
    for (int i = 0; i < oldLenDexElements; i++) {
        auto t = (*env).GetObjectArrayElement(oDexElements, i);
        (*env).SetObjectArrayElement(oDexElementsNew, i, t);
    }
    (*env).SetObjectArrayElement(oDexElementsNew, oldLenDexElements, oElement);
    LOGD("6");

    (*env).SetObjectField(oPathList, fDexElements, oDexElementsNew);
    LOGD("7");
    (*env).DeleteLocalRef(cClassLoader);
    (*env).DeleteLocalRef(cBaseDexClassLoader);
    (*env).DeleteLocalRef(cDexFile);
    (*env).DeleteLocalRef(cDexPathList);
    (*env).DeleteLocalRef(cElement);
    LOGD("finish, loadDexFromMemory()");
}


RawDexFile *createRawDexFileFromMemoryDalvik() {
    LOGD("start, createRawDexFileFromMemoryDalvik()");
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
        LOGE("ERROR: stored file size (%d) != expected (%d)",
             pDexHeader->fileSize, gDexFileHelper->dexLen);
    }
    LOGD("init pDexFile, finish...");

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
    size_t stringSize = pDexHeader->stringIdsSize * sizeof(struct StringObject *);
    size_t classSize = pDexHeader->typeIdsSize * sizeof(struct ClassObject *);
    size_t methodSize = pDexHeader->methodIdsSize * sizeof(struct Method *);
    size_t fieldSize = pDexHeader->fieldIdsSize * sizeof(struct Field *);
    size_t totalSize = sizeof(DvmDex) + stringSize + classSize + methodSize + fieldSize;
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
    LOGD("+++ DEX %p: allocateAux (%d+%d+%d+%d)*4 = %d bytes",
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
    newCache->entries = (AtomicCacheEntry *)
            (((int) newCache->entryAlloc + CPU_CACHE_WIDTH_1) & ~CPU_CACHE_WIDTH_1);
    pDvmDex->pInterfaceCache = newCache;
    pthread_mutex_init(&pDvmDex->modLock, nullptr);

    // dvmDex memMapping
    pDvmDex->memMap.baseAddr = pDvmDex->memMap.addr = (void *) data;
    pDvmDex->memMap.baseLength = pDvmDex->memMap.length = pDexHeader->fileSize;
    pDvmDex->isMappedReadOnly = false;
    LOGD("init pDvmDex, finish...");

    /*
     * Create the class lookup table.  This will eventually be appended
     * to the end of the .odex.
     *
     * We create a temporary link from the DexFile for the benefit of
     * class loading, below.
     */
    DexClassLookup *pClassLookup = nullptr;
    int numEntries = dexRoundUpPower2(pDexFile->pHeader->classDefsSize * 2);
    LOGD("numEntries: %d", numEntries);
    int allocSize = offsetof(DexClassLookup, table) + numEntries * sizeof(pClassLookup->table[0]);
    LOGD("allocSize: %d, table sizeof: %d", allocSize, sizeof(pClassLookup->table[0]));
    pClassLookup = (DexClassLookup *) new u1[allocSize]();
    LOGD("pClassLookup: %p", pClassLookup);
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
    LOGD("Class lookup: classes=%d slots=%d (%d%% occ) alloc=%d total=%d max=%d",
         pDexFile->pHeader->classDefsSize, numEntries,
         (100 * pDexFile->pHeader->classDefsSize) / numEntries,
         allocSize, totalProbes, maxProbes);
    LOGD("init pClassLookup, finish...");

    pDvmDex->pDexFile->pClassLookup = pClassLookup;

    auto *pRawDexFile = new RawDexFile();
    pRawDexFile->pDvmDex = pDvmDex;
    LOGD("finish, createRawDexFileFromMemoryDalvik()");
    return pRawDexFile;
}


void initDexFileHelper(DexFileHelper **ppDexFileHelper, const ConfigFileProxy *pConfigFileProxy) {
    LOGD("start, initDexFileHelper(DexFucker **ppDexFileHelper, const ConfigFileProxy *pConfigFileProxy)");

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
    size_t size = *(size_t *) cur;
    cur += 4;
    for (int i = 0; i < size; i++) {
        size_t keySize = *(size_t *) cur;
        cur += 4;
        string key = cur;
        cur += keySize;

        size_t valueSize = *(size_t *) cur;
        cur += 4;
        size_t codeBufOff = 0;

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
        LOGD("codeItem[0x%08x]: %s", i, key.data());
        LOGD("     registersSize: %d", pCodeItemData->registersSize);
        LOGD("           insSize: %d", pCodeItemData->insSize);
        LOGD("           outSize: %d", pCodeItemData->outSize);
        LOGD("         triesSize: %d", pCodeItemData->triesSize);
        LOGD("         insnsSize: %d", pCodeItemData->insnsSize);
    }
    LOGD("code item size: %d, map size: %d", size, pDexFileHelper->codeItem.size());
    *ppDexFileHelper = pDexFileHelper;
    LOGD("finish, initDexFileHelper(DexFucker **ppDexFileHelper, const ConfigFileProxy *pConfigFileProxy)");
}

const CodeItemData *getCodeItem(const string &key) {
    auto it = gDexFileHelper->codeItem.find(key);
    if (it != gDexFileHelper->codeItem.end()) {
        return it->second;
    }
    LOGE("can't found code item data by key: %s", key.data());
    return nullptr;
}


void hookArtOpenDexFiles() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = hookOpenDexFilesFromOat() &&
                   hookDexFileOpenFile() &&
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
//    LOGD("fstat file path: %s", linkPath);
    if (strcmp(linkPath, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        LOGD("myFstat");
        st->st_size = gDexFileHelper->dexLen;
    }
    return retValue;
}

bool hookFstat() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = HookNativeInline("/system/lib/libc.so",
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
//    LOGD("mmap file path: %s", linkPath);
    if (strcmp(linkPath, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        LOGD("myMmap");
        LOGD("dexLen:                  0x%08x", gDexFileHelper->dexLen);
        LOGD("page_aligned_byte_count: 0x%08x", size);
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
        LOGD("mmap(%p), success", (void *) retValue);
        return retValue;
    }
    return sysMmap(addr, size, prot, flags, fd, offset);
}

bool hookMmap() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = HookNativeInline("/system/lib/libc.so",
                                    "mmap",
                                    reinterpret_cast<void *>(myMmap),
                                    reinterpret_cast<void **>(&sysMmap));
    }
    return hookFlag;
}

bool (*sysDexFileOpen)(const char *filename, const char *location, string *error_msg,
                       vector<void *> *dex_files) = nullptr;

bool myDexFileOpen(const char *filename, const char *location, string *error_msg,
                   vector<void *> *dex_files) {
    return sysDexFileOpen(filename, location, error_msg, dex_files);
}

bool hookDexFileOpen() {
    static bool hookFlag = false;
    if (!hookFlag) {
        switch (android_get_device_api_level()){
            case __ANDROID_API_L__:
            case __ANDROID_API_L_MR1__:
                hookFlag = HookNativeInline("/system/lib/libart.so",
                                            "_ZN3art7DexFile4OpenEPKcS2_PNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEPNS3_6vectorIPKS0_NS7_ISD_EEEE",
                                            reinterpret_cast<void *>(myDexFileOpen),
                                            reinterpret_cast<void **>(&sysDexFileOpen));
                break;
            case __ANDROID_API_M__:
                hookFlag = HookNativeInline("/system/lib/libart.so",
                                            "_ZN3art7DexFile4OpenEPKcS2_PNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEPNS3_6vectorINS3_10unique_ptrIKS0_NS3_14default_deleteISD_EEEENS7_ISG_EEEE",
                                            reinterpret_cast<void *>(myDexFileOpen),
                                            reinterpret_cast<void **>(&sysDexFileOpen));
                break;
            default:
                assert(false);
        }
    }
    return hookFlag;
}


vector<void *>
(*sysOpenDexFilesFromOat_23)(void *thiz, const char *dex_location, const char *oat_location,
                          vector<string> *error_msgs) = nullptr;

vector<void *>
myOpenDexFilesFromOat_23(void *thiz, const char *dex_location, const char *oat_location,
                      vector<string> *error_msgs) {
    LOGD("dex_location: %s", dex_location);
    LOGD("oat_location: %s", oat_location);
    if (strcmp(dex_location, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        vector<void *> dexFiles;
        string error_msg;
        if (!sysDexFileOpen(dex_location, dex_location, &error_msg, &dexFiles)) {
            LOGE("Failed to open dex files from %s", dex_location);
            LOGE("DexFileOpen error: %s", error_msg.data());
            error_msgs->push_back("Failed to open dex files from " + string(dex_location));
        }
        LOGD("DexFileOpen, success");
        return dexFiles;
    }
    return sysOpenDexFilesFromOat_23(thiz, dex_location, oat_location, error_msgs);
}

bool
(*sysOpenDexFilesFromOat_21_22)(void *thiz, const char *dex_location, const char *oat_location,
                                vector<string> *error_msgs, vector<void *>* dex_files) = nullptr;

bool
myOpenDexFilesFromOat_21_22(void *thiz, const char *dex_location, const char *oat_location,
                            vector<string> *error_msgs, vector<void *>* dex_files){
    LOGD("dex_location: %s", dex_location);
    LOGD("oat_location: %s", oat_location);
    if (strcmp(dex_location, gDexFileHelper->fakeClassesDexName.data()) == 0) {
        string error_msg;
        if (!sysDexFileOpen(dex_location, dex_location, &error_msg, dex_files)) {
            LOGE("Failed to open dex files from %s", dex_location);
            LOGE("DexFileOpen error: %s", error_msg.data());
            error_msgs->push_back("Failed to open dex files from " + string(dex_location));
        }
        LOGD("DexFileOpen, success");
        return true;
    }
    return sysOpenDexFilesFromOat_21_22(thiz, dex_location, oat_location, error_msgs, dex_files);
}

bool hookOpenDexFilesFromOat() {
    static bool hookFlag = false;
    if (!hookFlag) {
        switch (android_get_device_api_level()) {

            case __ANDROID_API_L__:
            case __ANDROID_API_L_MR1__:
                hookFlag = HookNativeInline("/system/lib/libart.so",
                                            "_ZN3art11ClassLinker19OpenDexFilesFromOatEPKcS2_PNSt3__16vectorINS3_12basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEENS8_ISA_EEEEPNS4_IPKNS_7DexFileENS8_ISG_EEEE",
                                            reinterpret_cast<void *>(myOpenDexFilesFromOat_21_22),
                                            reinterpret_cast<void **>(&sysOpenDexFilesFromOat_21_22));
                break;

            case __ANDROID_API_M__:
                hookFlag = HookNativeInline("/system/lib/libart.so",
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

void *
(*sysDexFileOpenFile_23)(void *thiz, int fd, const char *location, bool verify,
                      string *error_msg) = nullptr;

void *myDexFileOpenFile_23(void *thiz, int fd, const char *location, bool verify, string *error_msg) {
    LOGD("location: %s, verify: %s", location, verify ? "true" : "false");
    return sysDexFileOpenFile_23(thiz, fd, location, false, error_msg);
}

void *
(*sysDexFileOpenFile_21_22)(int fd, const char *location, bool verify,
                      string *error_msg) = nullptr;

void *myDexFileOpenFile_21_22(int fd, const char *location, bool verify, string *error_msg) {
    LOGD("location: %s, verify: %s", location, verify ? "true" : "false");
    return sysDexFileOpenFile_21_22(fd, location, false, error_msg);
}

bool hookDexFileOpenFile() {
    static bool hookFlag = false;
    if (!hookFlag) {
        switch (android_get_device_api_level()){
            case __ANDROID_API_L__:
            case __ANDROID_API_L_MR1__:
                hookFlag = HookNativeInline("/system/lib/libart.so",
                                            "_ZN3art7DexFile8OpenFileEiPKcbPNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE",
                                            reinterpret_cast<void *>(myDexFileOpenFile_21_22),
                                            reinterpret_cast<void **>(&sysDexFileOpenFile_21_22));
                break;
            case __ANDROID_API_M__:
                hookFlag = HookNativeInline("/system/lib/libart.so",
                                            "_ZN3art7DexFile8OpenFileEiPKcbPNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE",
                                            reinterpret_cast<void *>(myDexFileOpenFile_23),
                                            reinterpret_cast<void **>(&sysDexFileOpenFile_23));
                break;
            default:
                assert(false);
        }

    }
    return hookFlag;
}









