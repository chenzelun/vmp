//
// Created by dalunlun on 2019/10/22.
//

#ifndef SHELLAPPLICATION_FILE_H
#define SHELLAPPLICATION_FILE_H

void hookFileIO();

bool hookOpen19();

bool hookOpen();

bool hookClose();

bool hookRead();

bool hookWrite();

static inline bool needs_mode(int flags);

#endif //SHELLAPPLICATION_FILE_H
