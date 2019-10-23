//
// Created by dalunlun on 2019/10/21.
//

#ifndef SHELLAPPLICATION_NODEBUG_H
#define SHELLAPPLICATION_NODEBUG_H

#include <sys/types.h>

using namespace std;

typedef struct f_parameter_p1 {
    int fd1;
    int fd5;
    pid_t gpid;
} f_parameter_p_kill;

typedef struct p_parameter {
    pid_t gpid;
    pid_t pid;
    int ms;
    int fd5;
} p_parameter_stop;

void protect_ndebug(const char *path);

void protect_jdebug();

static int readProcessStateFromFile(char *state, const char *filePath);

void *p_father_kill(void *arg);

void *p_moniter_stop(void *arg);

int setProcessPriority(int pid, int delta);

void *p_protect_jdebug(void *arg);

int validate(int fd);

void processcmdline(int fd, int *useStdInd);

int processadbd(int fd, int *netFlag);

int processdebugtool(int fd, struct stat fs);


#endif //SHELLAPPLICATION_NODEBUG_H
