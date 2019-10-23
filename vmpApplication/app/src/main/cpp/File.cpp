//
// Created by dalunlun on 2019/10/22.
//

#include <sys/types.h>
#include <cstdarg>
#include <asm/fcntl.h>
#include "File.h"
#include "Util.h"
#include "Config.h"

void hookFileIO() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = hookOpen() && hookClose() && hookRead() && hookWrite();
    }
}

bool hookOpen() {
    switch (android_get_device_api_level()){
        case __ANDROID_API_K__:
            return hookOpen19();

        default:
            LOG_E("error api for open hook");
            assert(false);
            return false;
    }
}

bool hookClose() {
    return false;
}

bool hookRead() {
    return false;
}

bool hookWrite() {
    return false;
}

int (*sysOpen19)(const char *pathname, int flags, ...) = nullptr;

int myOpen19(const char *pathname, int flags, ...) {
    mode_t mode = 0;
    int fd = -1;

    if (needs_mode(flags)) {
        va_list args;
        va_start(args, flags);
        mode = static_cast<mode_t>(va_arg(args, int));
        va_end(args);
        fd = sysOpen19(pathname, flags, mode);
    } else {
        fd = sysOpen19(pathname, flags);
    }

    LOG_E("open");
}

bool hookOpen19() {
    static bool hookFlag = false;
    if (!hookFlag) {
        hookFlag = HookNativeInline((getSystemLibDir() + "/libjavacore.so").data(),
                                    "open",
                                    reinterpret_cast<void *>(myOpen19),
                                    reinterpret_cast<void **>(&sysOpen19));
    }
    return hookFlag;
}

bool needs_mode(int flags) {
    return ((flags & O_CREAT) == O_CREAT) || ((flags & O_TMPFILE) == O_TMPFILE);
}
