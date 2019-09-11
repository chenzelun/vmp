//
// Created by ChenDalunlun on 2019/8/27.
//

#ifndef SHELLAPPLICATION_CONFIG_H
#define SHELLAPPLICATION_CONFIG_H


#include "Util.h"
#include <string>
#include <jni.h>
#include <fstream>

using namespace std;

#define SIZE_OF_DOUBLE_UINT32 8


struct ConfigFileProxy {
    JNIEnv *env;
    string configFileName = "config.bin";

    string filePath;
    size_t fileSize = 0;

    string srcApkApplicationName;

    size_t srcClassesDexSize;
    size_t srcClassesDexOff;

    size_t srcCodeItemSize;
    size_t srcCodeItemOff;

    string fakeClassesDexName;
    size_t fakeClassesDexBufSize;
    size_t fakeClassesDexBufOff;
};

void initConfigFileProxy(ConfigFileProxy **ppConfigFileProxy, JNIEnv *env);

void initConfigFile();

inline void readStr(ifstream &reader, size_t offset, string *pRetString, char *buf);

inline void
readSizeAndOff(ifstream &reader, size_t offset, size_t *pRetSize, size_t *pRetOffset,
               char *buf);

const string &getDataDir(JNIEnv *env);

const string &getBaseFilesDir(JNIEnv *env);

const string &getLibDir(JNIEnv *env);

const string &getOdexDir(JNIEnv *env);

const string &getDexDir(JNIEnv *env);

void buildFileSystem();

void copyFileFromAssets(JNIEnv *env, const string &configFileName, const string &dstFilePath);


const char *getDataBuf(size_t off, size_t size);

const char *getCodeItemBuf();

const char *getClassesDexBuf();

void buildFile(const string &srcPath, const string &desPath, unsigned int off, size_t length);

void buildFakeClassesDexFile(const string &path);

#endif //SHELLAPPLICATION_CONFIG_H
