//
// Created by dalunlun on 2019/9/20.
//

#ifndef SHELLAPPLICATION_SO_H
#define SHELLAPPLICATION_SO_H

#include <cstdint>
#include <jni.h>
#include <map>
#include <string>
#include <vector>
#include "Config.h"
#include <link.h>
#include <unordered_map>


using namespace std;

struct SoText {
    uint32_t start;
    uint32_t size;
    uint8_t data[1];
};

struct SoFileHelper {
    JNIEnv *env;

    const char *soTextBuf;
    uint32_t soTextBufSize;
    map<string, vector<SoText *> *> soText;
};

#define SOINFO_NAME_LEN 128
struct SoInfo_19_23 {
    char name[SOINFO_NAME_LEN];
    const ElfW(Phdr) *phdr;
    size_t phnum;
    ElfW(Addr) entry;
    ElfW(Addr) base;
    unsigned size;
};


struct SoInfo_26_27 {
    const ElfW(Phdr) *phdr;
    size_t phnum;
    ElfW(Addr) base;
    size_t size;
};

struct SoInfo_28 {
    char old_name_[SOINFO_NAME_LEN];
    const ElfW(Phdr) *phdr;
    size_t phnum;
    ElfW(Addr) base;
    size_t size;
};

void initSoFileHelper(SoFileHelper **ppSoFileHelper, const ConfigFileProxy *pConfigFileProxy);

void buildSoFile();

void updateNativeLibraryDirectories(JNIEnv *env, const string &libDirPath);

void *myDlopen(const char *filename, int flag);

void hookDlopen();

SoInfo_19_23 *myFindLibrary_19(const char *name);

bool myFindLibraries_21_22(const char *const *library_names, size_t library_names_size,
                           SoInfo_19_23 **soinfos, void **ld_preloads, size_t ld_preloads_size,
                           int dlflags, const void *extinfo);

bool
myFindLibraries_23(void *start_with, const char *const *library_names, size_t library_names_count,
                   SoInfo_19_23 **soinfos, void *ld_preloads, size_t ld_preloads_count,
                   int rtld_flags, const void *extinfo);

bool
myFindLibraries_26_27(void *ns, void *start_with, const char *const library_names[],
                      size_t library_names_count, SoInfo_26_27 *soinfos[], void *ld_preloads,
                      size_t ld_preloads_count, int rtld_flags, const void *extinfo,
                      bool add_as_children,
                      bool search_linked_namespaces, void *readers_map, void *namespaces);

void hookFindLibraries();

void fillBackSoText(const string &name, char *start, size_t size);

#endif //SHELLAPPLICATION_SO_H
