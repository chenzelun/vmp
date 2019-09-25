//
// Created by ChenDalunlun on 2019/8/27.
//

#ifndef SHELLAPPLICATION_CONFIG_H
#define SHELLAPPLICATION_CONFIG_H


#include "Util.h"
#include <string>
#include <jni.h>
#include <fstream>
#include <cstdint>

using namespace std;

#define SIZE_OF_DOUBLE_UINT32 8


struct ConfigFileProxy {
    JNIEnv *env;
    string configFileName = "config.bin";

    string filePath;
    uint32_t fileSize = 0;

    string srcApkApplicationName;

    uint32_t srcClassesDexSize;
    uint32_t srcClassesDexOff;

    uint32_t srcCodeItemSize;
    uint32_t srcCodeItemOff;

    string fakeClassesDexName;
    uint32_t fakeClassesDexBufSize;
    uint32_t fakeClassesDexBufOff;

    uint32_t srcMethodInsnsSize;
    uint32_t srcMethodInsnsOff;

    uint32_t srcSoBufSize;
    uint32_t srcSoBufOff;

    uint32_t srcSoTextSize;
    uint32_t srcSoTextOff;
};

void initConfigFileProxy(ConfigFileProxy **ppConfigFileProxy, JNIEnv *env);

void initConfigFile();

inline void readStr(ifstream &reader, uint32_t offset, string *pRetString, char *buf);

inline void
readSizeAndOff(ifstream &reader, uint32_t offset, uint32_t *pRetSize, uint32_t *pRetOffset,
               char *buf);

const string &getDataDir(JNIEnv *env);

const string &getBaseFilesDir(JNIEnv *env);

const string &getLibDir(JNIEnv *env);

const string &getAppDefaultLibDir(JNIEnv *env);

const string &getOdexDir(JNIEnv *env);

const string &getDexDir(JNIEnv *env);

void buildFileSystem();

void copyFileFromAssets(JNIEnv *env, const string &configFileName, const string &dstFilePath);

const char *getDataBuf(uint32_t off, uint32_t size);

const char *getCodeItemBuf();

const char *getClassesDexBuf();

const char *getMethodInsnsBuf();

void buildFile(const string &srcPath, const string &desPath, unsigned int off, uint32_t length);

void buildFakeClassesDexFile(const string &path);

const string getSystemLibDir();

const char *getSoBuf();

const char *getSoTextBuf();

#endif //SHELLAPPLICATION_CONFIG_H
