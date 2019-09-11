//
// Created by ChenDalunlun on 2019/8/27.
//

#include "Config.h"
#include <unistd.h>
#include <sys/stat.h>
#include <android/asset_manager_jni.h>
#include <vector>


extern ConfigFileProxy *gConfigFileProxy;

void initConfigFile() {
    LOGD("start, initConfigFile()");
    ifstream reader(gConfigFileProxy->filePath, ios::binary);

    // get config file's size
    reader.seekg(0, ios::end);
    gConfigFileProxy->fileSize = static_cast<size_t>(reader.tellg());
    LOGD("filePath: %s, fileSize: %d", gConfigFileProxy->filePath.data(),
         gConfigFileProxy->fileSize);
    char buf[BUFSIZ];

    reader.seekg(0, ios::beg);
    size_t offset = 0;

    // read srcApkApplicationName
    readStr(reader, offset, &gConfigFileProxy->srcApkApplicationName, buf);
    offset += SIZE_OF_DOUBLE_UINT32;
    LOGD("srcApkApplicationName: %s", gConfigFileProxy->srcApkApplicationName.data());

    // read fakeClassesDexName
    readStr(reader, offset, &gConfigFileProxy->fakeClassesDexName, buf);
    offset += SIZE_OF_DOUBLE_UINT32;
    LOGD("fakeClassesDexName: %s", gConfigFileProxy->fakeClassesDexName.data());

    // read fakeClassesDexBuf
    readSizeAndOff(reader, offset, &gConfigFileProxy->fakeClassesDexBufSize,
                   &gConfigFileProxy->fakeClassesDexBufOff, buf);
    offset += SIZE_OF_DOUBLE_UINT32;
    LOGD("fakeClassesDexBufSize: %d, fakeClassesDexBufOff: %d",
         gConfigFileProxy->fakeClassesDexBufSize, gConfigFileProxy->fakeClassesDexBufOff);

    // read classes dex
    readSizeAndOff(reader, offset, &gConfigFileProxy->srcClassesDexSize,
                   &gConfigFileProxy->srcClassesDexOff, buf);
    offset += SIZE_OF_DOUBLE_UINT32;
    LOGD("srcClassesDexSize: %d, srcClassesDexOff: %d", gConfigFileProxy->srcClassesDexSize,
         gConfigFileProxy->srcClassesDexOff);

    // read code item
    readSizeAndOff(reader, offset, &gConfigFileProxy->srcCodeItemSize,
                   &gConfigFileProxy->srcCodeItemOff, buf);
    LOGD("srcCodeItemSize: %d, srcCodeItemOff: %d", gConfigFileProxy->srcCodeItemSize,
         gConfigFileProxy->srcCodeItemOff);

    reader.close();
    LOGD("finish, initConfigFile()");
}

void readStr(ifstream &reader, size_t offset, string *pRetString, char *buf) {
    size_t size;
    size_t off;
    readSizeAndOff(reader, offset, &size, &off, buf);
    reader.seekg(off, ios::beg);
    reader.read(buf, size);
    *pRetString = buf;
}

void readSizeAndOff(ifstream &reader, size_t offset, size_t *pRetSize, size_t *pRetOffset,
                    char *buf) {
    reader.seekg(offset, ios::beg);
    reader.read(buf, SIZE_OF_DOUBLE_UINT32);
    *pRetSize = *(size_t *) buf;
    *pRetOffset = *(size_t *) (buf + 4);
    LOGD("readSizeAndOff, offset: %d, ret_size: %d, ret_offset: %d",
         offset, *pRetSize, *pRetOffset);
}

const string &getBaseFilesDir(JNIEnv *env) {
    static string baseDir;
    if (baseDir.empty()) {
        jobject oContext = getAppContext(env);
        jclass cContext = (*env).GetObjectClass(oContext);
        jmethodID mGetFilesDir = (*env).GetMethodID(cContext, "getFilesDir", "()Ljava/io/File;");
        jobject oFile = (*env).CallObjectMethod(oContext, mGetFilesDir);
        assert(oFile != nullptr);
        jclass cFile = (*env).GetObjectClass(oFile);
        jmethodID mGetCanonicalPath = (*env).GetMethodID(cFile, "getCanonicalPath",
                                                         "()Ljava/lang/String;");
        auto oPath = reinterpret_cast<jstring >((*env).CallObjectMethod(oFile, mGetCanonicalPath));
        assert(oPath != nullptr);
        baseDir = (*env).GetStringUTFChars(oPath, JNI_FALSE);

        LOGD("getBaseFilesDir success first....");
        LOGD("baseDir: %s", baseDir.data());
        (*env).DeleteLocalRef(cContext);
        (*env).DeleteLocalRef(cFile);
    }
    return baseDir;
}

const string &getDataDir(JNIEnv *env) {
    static string data;
    if (data.empty()) {
        data = getBaseFilesDir(env) + "/data";
        LOGD("getDataDir success first....");
        LOGD("dataDir: %s", data.data());
    }
    return data;
}

const string &getLibDir(JNIEnv *env) {
    static string lib;
    if (lib.empty()) {
        lib = getBaseFilesDir(env) + "/lib";
        LOGD("getLibDir success first....");
        LOGD("libDir: %s", lib.data());
    }
    return lib;
}


const string &getOdexDir(JNIEnv *env) {
    static string odex;
    if (odex.empty()) {
        odex = getBaseFilesDir(env) + "/odex";
        LOGD("getOdexDir success first....");
        LOGD("odexDir: %s", odex.data());
    }
    return odex;
}

const string &getDexDir(JNIEnv *env) {
    static string dex;
    if (dex.empty()) {
        dex = getBaseFilesDir(env) + "/dex";
        LOGD("getDexDir success first....");
        LOGD("dexDir: %s", dex.data());
    }
    return dex;
}

void buildFileSystem() {
    LOGD("start, buildFileSystem()");
    vector<string> path = {
            getBaseFilesDir(gConfigFileProxy->env),
            getDataDir(gConfigFileProxy->env),
            getOdexDir(gConfigFileProxy->env),
            getDexDir(gConfigFileProxy->env),
    };

    for (auto &p : path) {
        if (0 != access(p.data(), R_OK) &&
            0 != mkdir(p.data(), S_IRWXU | S_IRWXG | S_IRWXO)) { // NOLINT(hicpp-signed-bitwise)
            // if this folder not exist, create a new one.
            LOGE("create dir failed: %s", p.data());
            throw runtime_error("create dir failed");
        }
    }

    // copy file from assets
    copyFileFromAssets(gConfigFileProxy->env, gConfigFileProxy->configFileName,
                       gConfigFileProxy->filePath);
    LOGD("finish, buildFileSystem()");
}

void copyFileFromAssets(JNIEnv *env, const string &configFileName, const string &dstFilePath) {
    LOGD("start, copyFileFromAssets(JNIEnv *env, const string &configFileName, const string &dstFilePath)");
    LOGD("copyFileFromAssets start, write %s to %s", configFileName.data(), dstFilePath.data());
    jobject oAssetsManager = getAssetsManager(env);
    AAssetManager *assetManager = AAssetManager_fromJava(env, oAssetsManager);
    assert(assetManager != nullptr);
    LOGD("Get AssetManager success...");

    AAsset *asset = AAssetManager_open(assetManager, configFileName.data(), AASSET_MODE_BUFFER);
    auto fileSize = AAsset_getLength(asset);
    const char *fileBuf = reinterpret_cast<const char *>(AAsset_getBuffer(asset));
    LOGD("srcPath: %s, fileSize: %ld", configFileName.data(), fileSize);

    ofstream writer(dstFilePath.data(), ios::binary);
    writer.write(fileBuf, fileSize);
    writer.close();
    LOGD("finish, copyFileFromAssets(JNIEnv *env, const string &configFileName, const string &dstFilePath)");
}

void initConfigFileProxy(ConfigFileProxy **ppConfigFileProxy, JNIEnv *env) {
    LOGD("start, initConfigFileProxy(ConfigFileProxy **ppConfigFileProxy, JNIEnv *env)");
    auto *proxy = new ConfigFileProxy();
    proxy->env = env;
    proxy->filePath = getDataDir(env) + "/" + proxy->configFileName;
    *ppConfigFileProxy = proxy;
    LOGD("finish, initConfigFileProxy(ConfigFileProxy **ppConfigFileProxy, JNIEnv *env)");
}

const char *getDataBuf(size_t off, size_t size) {
    char *buf = new char[size]();
    ifstream reader(gConfigFileProxy->filePath, ios::binary);
    reader.seekg(off, ios::beg);
    reader.read(buf, size);
    reader.close();
    LOGD("getDataBuf(size_t off: %d, size_t size: %d)", off, size);
    return buf;
}

const char *getCodeItemBuf() {
    LOGD("getCodeItemBuf()");
    return getDataBuf(gConfigFileProxy->srcCodeItemOff, gConfigFileProxy->srcCodeItemSize);
}

const char *getClassesDexBuf() {
    LOGD("getClassesDexBuf()");
    return getDataBuf(gConfigFileProxy->srcClassesDexOff, gConfigFileProxy->srcClassesDexSize);
}

void buildFile(const string &srcPath, const string &desPath, unsigned int off, size_t length) {
    ofstream writer(desPath, ios::binary);
    ifstream reader(srcPath, ios::binary);
    char buf[BUFSIZ];

    LOGD("start write files...   %s", desPath.data());
    reader.seekg(off, ios::beg);
    while (length > 0) {
        size_t realReadSize = length >= BUFSIZ ? BUFSIZ : length;
        length -= realReadSize;
        reader.read(buf, realReadSize);
        writer.write(buf, realReadSize);
    }

    LOGD("finish write files...   %s", desPath.data());
    reader.close();
    writer.flush();
    writer.close();
}


void buildFakeClassesDexFile(const string &path) {
    buildFile(gConfigFileProxy->filePath, path, gConfigFileProxy->fakeClassesDexBufOff,
              gConfigFileProxy->fakeClassesDexBufSize);
}

