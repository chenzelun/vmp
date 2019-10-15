//
// Created by dalunlun on 2019/9/20.
//

#include "So.h"
#include <sys/mman.h>

extern SoFileHelper *gSoFileHelper;

void initSoFileHelper(SoFileHelper **ppSoFileHelper, const ConfigFileProxy *pConfigFileProxy) {
    LOG_D("start, initSoFileHelper(SoFileHelper **ppSoFileHelper, const ConfigFileProxy *pConfigFileProxy)");
    SoFileHelper *soFileHelper = new SoFileHelper();
    soFileHelper->env = pConfigFileProxy->env;
    soFileHelper->soTextBufSize = pConfigFileProxy->srcSoTextSize;
    soFileHelper->soTextBuf = getSoTextBuf();

    string ABI = getCUP_ABI();
    const char *cur = soFileHelper->soTextBuf;
    uint32_t size = *(uint32_t *) cur;
    cur += 4;
    for (int i = 0; i < size; i++) {
        uint32_t nameSize = *(uint32_t *) cur;
        cur += 4;
        string name = cur;
        cur += nameSize;
        if (name.compare(0, ABI.size(), ABI) == 0) {
            string fileName = name.substr(ABI.size() + 1);
            LOG_D("init so text: %s", fileName.data());
            cur += 4;
            uint32_t textSize = *(uint32_t *) cur;
            cur += 4;
            vector<SoText *> *texts = new vector<SoText *>();
            for (int j = 0; j < textSize; j++) {
                auto *text = (SoText *) cur;
                texts->push_back(text);
                cur += 8 + text->size;
                LOG_D("start: %d", text->start);
                LOG_D("size:  %d", text->size);
            }
            LOG_D("init so text, finish: %s, text size: %ld, except size: %d",
                  fileName.data(), texts->size(), textSize);
            fileName = getLibDir(soFileHelper->env) + "/" + fileName;
            soFileHelper->soText[fileName] = texts;
        } else {
            uint32_t textSize = *(uint32_t *) cur;
            cur += 4 + textSize;
        }
    }

    *ppSoFileHelper = soFileHelper;
    LOG_D("finish, initSoFileHelper");
}


void buildSoFile() {
    string ABI = getCUP_ABI();

    const char *buf = getSoBuf();
    const char *cur = buf;
    uint32_t size = *(uint32_t *) cur;
    cur += 4;
    for (int i = 0; i < size; i++) {
        uint32_t nameSize = *(uint32_t *) cur;
        cur += 4;
        string name = cur;
        cur += nameSize;
        uint32_t file_buf_size = *(uint32_t *) cur;
        cur += 4;
        if (name.compare(0, ABI.size(), ABI) == 0) {
            string fileName = name.substr(ABI.size() + 1);
            LOG_D("writing file: %s", fileName.data());
            ofstream writer(getLibDir(gSoFileHelper->env) + "/" + fileName, ios::binary);
            writer.write(cur, file_buf_size);
            writer.flush();
            writer.close();
            LOG_D("write finish.");
        }
        cur += file_buf_size;
    }

    if (size > 0) {
        if (android_get_device_api_level() <= __ANDROID_API_N_MR1__) {
            hookDlopen();
        }
        hookFindLibraries();
    }

    delete[]buf;
}

void updateNativeLibraryDirectories(JNIEnv *env, const string &libDirPath) {
    LOG_D("start, updateNativeLibraryDirectories(JNIEnv *env, const string &libDirPath)");

    jobject oClassLoader = getClassLoader(env);
    jclass cClassLoader = (*env).GetObjectClass(oClassLoader);
    jclass cBaseDexClassLoader = (*env).GetSuperclass(cClassLoader);
    jfieldID fPathList = (*env).GetFieldID(cBaseDexClassLoader, "pathList",
                                           "Ldalvik/system/DexPathList;");
    jobject oPathList = (*env).GetObjectField(oClassLoader, fPathList);
    LOG_D("1");
    jclass cDexPathList = (*env).GetObjectClass(oPathList);
    if (android_get_device_api_level() < __ANDROID_API_M__) {
        jfieldID fNativeLibraryDirectories = (*env).GetFieldID(cDexPathList,
                                                               "nativeLibraryDirectories",
                                                               "[Ljava/io/File;");
        auto oNativeLibraryDirectories = reinterpret_cast<jobjectArray >((*env).GetObjectField(
                oPathList, fNativeLibraryDirectories));
        LOG_D("2");
        jsize oldLenNativeLibraryDirectories = (*env).GetArrayLength(oNativeLibraryDirectories);
        LOG_D("oldLenNativeLibraryDirectories: %d", oldLenNativeLibraryDirectories);
        jclass cFile = (*env).FindClass("java/io/File");
        auto oNativeLibraryDirectoriesNew = (*env).NewObjectArray(
                oldLenNativeLibraryDirectories + 1,
                cFile, nullptr);
        for (int i = 0; i < oldLenNativeLibraryDirectories; i++) {
            auto t = (*env).GetObjectArrayElement(oNativeLibraryDirectories, i);
            (*env).SetObjectArrayElement(oNativeLibraryDirectoriesNew, i, t);
        }
        LOG_D("3");
        jmethodID mFile = (*env).GetMethodID(cFile, "<init>", "(Ljava/lang/String;)V");
        jstring filenameStr = (*env).NewStringUTF(libDirPath.data());
        jobject oFile = (*env).NewObject(cFile, mFile, filenameStr);
        (*env).SetObjectArrayElement(oNativeLibraryDirectoriesNew, oldLenNativeLibraryDirectories,
                                     oFile);
        LOG_D("4");
        (*env).SetObjectField(oPathList, fNativeLibraryDirectories, oNativeLibraryDirectoriesNew);
        (*env).DeleteLocalRef(cFile);
    } else if (android_get_device_api_level() >= __ANDROID_API_M__ &&
               android_get_device_api_level() <= __ANDROID_API_N_MR1__) {
        jfieldID fNativeLibraryPathElements =
                (*env).GetFieldID(cDexPathList, "nativeLibraryPathElements",
                                  "[Ldalvik/system/DexPathList$Element;");
        auto oNativeLibraryPathElements = reinterpret_cast<jobjectArray >((*env).GetObjectField(
                oPathList, fNativeLibraryPathElements));
        LOG_D("2");
        jsize oldLenNativeLibraryPathElements = (*env).GetArrayLength(oNativeLibraryPathElements);
        LOG_D("oldLenNativeLibraryDirectories: %d", oldLenNativeLibraryPathElements);
        jclass cElement = (*env).FindClass("dalvik/system/DexPathList$Element");
        auto oNativeLibraryPathElementsNew = (*env).NewObjectArray(
                oldLenNativeLibraryPathElements + 1, cElement, nullptr);
        for (int i = 0; i < oldLenNativeLibraryPathElements; i++) {
            auto t = (*env).GetObjectArrayElement(oNativeLibraryPathElements, i);
            (*env).SetObjectArrayElement(oNativeLibraryPathElementsNew, i, t);
        }
        LOG_D("3");
        jclass cFile = (*env).FindClass("java/io/File");
        jmethodID mFile = (*env).GetMethodID(cFile, "<init>", "(Ljava/lang/String;)V");
        jstring filenameStr = (*env).NewStringUTF(libDirPath.data());
        jobject oFile = (*env).NewObject(cFile, mFile, filenameStr);
        jmethodID mElement =
                (*env).GetMethodID(cElement,
                                   "<init>",
                                   "(Ljava/io/File;ZLjava/io/File;Ldalvik/system/DexFile;)V");
        jobject oElement = (*env).NewObject(cElement, mElement, oFile, true, nullptr, nullptr);
        (*env).SetObjectArrayElement(oNativeLibraryPathElementsNew,
                                     oldLenNativeLibraryPathElements, oElement);
        (*env).SetObjectField(oPathList, fNativeLibraryPathElements, oNativeLibraryPathElementsNew);
        LOG_D("3.5");
        (*env).DeleteLocalRef(cElement);
        (*env).DeleteLocalRef(cFile);
    } else {
        jfieldID fNativeLibraryPathElements =
                (*env).GetFieldID(cDexPathList, "nativeLibraryPathElements",
                                  "[Ldalvik/system/DexPathList$NativeLibraryElement;");
        auto oNativeLibraryPathElements = reinterpret_cast<jobjectArray >((*env).GetObjectField(
                oPathList, fNativeLibraryPathElements));
        LOG_D("2");
        jsize oldLenNativeLibraryPathElements = (*env).GetArrayLength(oNativeLibraryPathElements);
        LOG_D("oldLenNativeLibraryDirectories: %d", oldLenNativeLibraryPathElements);
        jclass cNativeLibraryElement =
                (*env).FindClass("dalvik/system/DexPathList$NativeLibraryElement");
        auto oNativeLibraryPathElementsNew = (*env).NewObjectArray(
                oldLenNativeLibraryPathElements + 1, cNativeLibraryElement, nullptr);
        for (int i = 0; i < oldLenNativeLibraryPathElements; i++) {
            auto t = (*env).GetObjectArrayElement(oNativeLibraryPathElements, i);
            (*env).SetObjectArrayElement(oNativeLibraryPathElementsNew, i, t);
        }
        LOG_D("3");
        jclass cFile = (*env).FindClass("java/io/File");
        jmethodID mFile = (*env).GetMethodID(cFile, "<init>", "(Ljava/lang/String;)V");
        jstring filenameStr = (*env).NewStringUTF(libDirPath.data());
        jobject oFile = (*env).NewObject(cFile, mFile, filenameStr);
        jmethodID mNativeLibraryElement = (*env).GetMethodID(cNativeLibraryElement, "<init>",
                                                             "(Ljava/io/File;)V");
        jobject oNativeLibraryElement = (*env).NewObject(cNativeLibraryElement,
                                                         mNativeLibraryElement, oFile);
        (*env).SetObjectArrayElement(oNativeLibraryPathElementsNew, oldLenNativeLibraryPathElements,
                                     oNativeLibraryElement);
        (*env).SetObjectField(oPathList, fNativeLibraryPathElements, oNativeLibraryPathElementsNew);
        LOG_D("4");
        (*env).DeleteLocalRef(cNativeLibraryElement);
        (*env).DeleteLocalRef(cFile);
//        jclass cList = (*env).FindClass("java/util/List");
//        jmethodID mAdd = (*env).GetMethodID(cList, "add", "(Ljava/lang/Object;)Z");
//        jfieldID fNativeLibraryDirectories =
//                (*env).GetFieldID(cDexPathList, "nativeLibraryDirectories", "Ljava/util/List;");
//        auto oNativeLibraryDirectories =
//                (*env).GetObjectField(oPathList, fNativeLibraryDirectories);
//        (*env).CallBooleanMethod(oNativeLibraryDirectories, mAdd, oFile);
    }
    LOG_D("5");
    LOG_D("finish, updateNativeLibraryDirectories(JNIEnv *env, const string &libDirPath)");
}

void *(*sysDlopen)(const char *filename, int flag) = nullptr;

void *myDlopen(const char *filename, int flag) {
    size_t dirPathLen = getAppDefaultLibDir(gSoFileHelper->env).size();
    if (strncmp(filename, getAppDefaultLibDir(gSoFileHelper->env).data(), dirPathLen) == 0) {
        string newFileName = getLibDir(gSoFileHelper->env) + (filename + dirPathLen);
        LOG_D("old so path: %s", filename);
        LOG_D("new so path: %s", newFileName.data());
        return sysDlopen(newFileName.data(), flag);
    }
    return sysDlopen(filename, flag);
}

void hookDlopen() {
    static bool hookFlag = false;
    if (!hookFlag) {
        LOG_D("hook");
        hookFlag = HookNativeInline((getSystemLibDir() + "/libdl.so").data(),
                                    "dlopen",
                                    reinterpret_cast<void *>(myDlopen),
                                    reinterpret_cast<void **>(&sysDlopen));
    }
}

SoInfo_19_23 *(*sysFindLibrary_19)(const char *name) = nullptr;

SoInfo_19_23 *myFindLibrary_19(const char *name) {
    SoInfo_19_23 *si = sysFindLibrary_19(name);
    LOG_D("library_names: %s", name);
    if (si == nullptr) {
        return si;
    }
    fillBackSoText(name, (char *) si->base, si->size);
    return si;
}

bool (*sysFindLibraries_21_22)(const char *const library_names[], size_t library_names_size,
                               SoInfo_19_23 **soinfos, void **ld_preloads, size_t ld_preloads_size,
                               int dlflags, const void *extinfo) = nullptr;

bool myFindLibraries_21_22(const char *const *library_names, size_t library_names_size,
                           SoInfo_19_23 **soinfos, void **ld_preloads, size_t ld_preloads_size,
                           int dlflags, const void *extinfo) {
    bool retValue = sysFindLibraries_21_22(library_names, library_names_size, soinfos, ld_preloads,
                                           ld_preloads_size, dlflags, extinfo);
    if (!retValue || soinfos == nullptr || soinfos[0] == nullptr) {
        return retValue;
    }
    fillBackSoText((char *) library_names[0], (char *) soinfos[0]->base, soinfos[0]->size);
    return retValue;
}


bool
(*sysFindLibraries_23)(void *start_with, const char *const *library_names,
                       size_t library_names_count, SoInfo_19_23 **soinfos, void *ld_preloads,
                       size_t ld_preloads_count, int rtld_flags, const void *extinfo) = nullptr;

bool
myFindLibraries_23(void *start_with, const char *const *library_names, size_t library_names_count,
                   SoInfo_19_23 **soinfos, void *ld_preloads, size_t ld_preloads_count,
                   int rtld_flags, const void *extinfo) {
    bool retValue = sysFindLibraries_23(start_with, library_names, library_names_count, soinfos,
                                        ld_preloads, ld_preloads_count, rtld_flags, extinfo);
    LOG_D("library_names: %s", library_names[0]);
    if (!retValue || soinfos == nullptr || soinfos[0] == nullptr) {
        return retValue;
    }
    fillBackSoText((char *) library_names[0], (char *) soinfos[0]->base, soinfos[0]->size);
    return retValue;
}

bool (*sysFindLibraries_26_27)(void *ns, void *start_with, const char *const *library_names,
                               size_t library_names_count, SoInfo_26_27 **soinfos,
                               void *ld_preloads, size_t ld_preloads_count, int rtld_flags,
                               const void *extinfo, bool add_as_children,
                               bool search_linked_namespaces, void *readers_map,
                               void *namespaces) = nullptr;

bool myFindLibraries_26_27(void *ns, void *start_with, const char *const *library_names,
                           size_t library_names_count, SoInfo_26_27 **soinfos, void *ld_preloads,
                           size_t ld_preloads_count, int rtld_flags, const void *extinfo,
                           bool add_as_children, bool search_linked_namespaces, void *readers_map,
                           void *namespaces) {
    bool retValue = sysFindLibraries_26_27(ns, start_with, library_names, library_names_count,
                                           soinfos, ld_preloads, ld_preloads_count, rtld_flags,
                                           extinfo, add_as_children, search_linked_namespaces,
                                           readers_map, namespaces);
    LOG_D("library_names: %s", library_names[0]);
    if (!retValue || soinfos == nullptr || soinfos[0] == nullptr) {
        return retValue;
    }
    fillBackSoText((char *) library_names[0], (char *) soinfos[0]->base, soinfos[0]->size);
    return retValue;
}

bool (*sysFindLibraries_28)(void *ns, void *start_with, const char *const library_names[],
                            size_t library_names_count, SoInfo_28 **soinfos, void *ld_preloads,
                            size_t ld_preloads_count, int rtld_flags, const void *extinfo,
                            bool add_as_children, bool search_linked_namespaces,
                            void *namespaces) = nullptr;

bool myFindLibraries_28(void *ns, void *start_with, const char *const library_names[],
                        size_t library_names_count, SoInfo_28 **soinfos, void *ld_preloads,
                        size_t ld_preloads_count, int rtld_flags, const void *extinfo,
                        bool add_as_children, bool search_linked_namespaces, void *namespaces) {
    bool retValue = sysFindLibraries_28(ns, start_with, library_names, library_names_count, soinfos,
                                        ld_preloads, ld_preloads_count, rtld_flags, extinfo,
                                        add_as_children, search_linked_namespaces, namespaces);
    LOG_D("library_names: %s", library_names[0]);
    if (!retValue || soinfos == nullptr || soinfos[0] == nullptr) {
        return retValue;
    }
    fillBackSoText((char *) library_names[0], (char *) soinfos[0]->base, soinfos[0]->size);
    return retValue;
}

void hookFindLibraries() {
    static bool hookFlag = false;
    if (!hookFlag) {
        LOG_D("hook");
        switch (android_get_device_api_level()) {
            case __ANDROID_API_K__:
                hookFlag = HookNativeInlineAnonymous(getLinkerPath(),
                                                     0x22b4,
                                                     reinterpret_cast<void *>(myFindLibrary_19),
                                                     reinterpret_cast<void **>(&sysFindLibrary_19));
                break;

            case __ANDROID_API_L__:
            case __ANDROID_API_L_MR1__:
                hookFlag = HookNativeInline(getLinkerPath(),
                                            "__dl__ZL14find_librariesPKPKcjPP6soinfoS5_jiPK17android_dlextinfo",
                                            reinterpret_cast<void *>(myFindLibraries_21_22),
                                            reinterpret_cast<void **>(&sysFindLibraries_21_22));
                break;

            case __ANDROID_API_M__:
                hookFlag = HookNativeInline(getLinkerPath(),
                                            "__dl__ZL14find_librariesP6soinfoPKPKcjPS0_PNSt3__16vectorIS0_NS6_9allocatorIS0_EEEEjiPK17android_dlextinfo",
                                            reinterpret_cast<void *>(myFindLibraries_23),
                                            reinterpret_cast<void **>(&sysFindLibraries_23));
                break;

            case __ANDROID_API_O__:
            case __ANDROID_API_O_MR1__:
                hookFlag = HookNativeInline(getLinkerPath(),
                                            "__dl__Z14find_librariesP19android_namespace_tP6soinfoPKPKcmPS2_PNSt3__16vectorIS2_NS8_9allocatorIS2_EEEEmiPK17android_dlextinfobbRNS8_13unordered_mapIPKS1_9ElfReaderNS8_4hashISJ_EENS8_8equal_toISJ_EENSA_INS8_4pairIKSJ_SK_EEEEEEPNS9_IS0_NSA_IS0_EEEE",
                                            reinterpret_cast<void *>(myFindLibraries_26_27),
                                            reinterpret_cast<void **>(&sysFindLibraries_26_27));
                break;

            case __ANDROID_API_P__:
                hookFlag = HookNativeInline(getLinkerPath(),
                                            "__dl__Z14find_librariesP19android_namespace_tP6soinfoPKPKcmPS2_PNSt3__16vectorIS2_NS8_9allocatorIS2_EEEEmiPK17android_dlextinfobbPNS9_IS0_NSA_IS0_EEEE",
                                            reinterpret_cast<void *>(myFindLibraries_28),
                                            reinterpret_cast<void **>(&sysFindLibraries_28));
                break;

            default:
                assert(false);
        }


    }
}

void fillBackSoText(const string &name, char *start, size_t size) {
    auto it = gSoFileHelper->soText.find(name);
    if (it != gSoFileHelper->soText.end()) {
        mprotect(start, size, PROT_READ | PROT_WRITE | PROT_EXEC);
        vector<SoText *> *text = it->second;
        for (auto item: *text) {
            memcpy(start + item->start, item->data, item->size);
        }
        LOG_D("fillBackSoText: %s", name.data());
    }
}
