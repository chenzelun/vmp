//
// Created by dalunlun on 2019/10/21.
//

#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <linux/stat.h>
#include <asm/fcntl.h>
#include <sys/resource.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "NoDebug.h"
#include "Util.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

static char FD5NAME[BUFSIZ];

void protect_ndebug(const char *path) {
    int fd1[2], fd2[2];
    pid_t c1_pid;
    pid_t buf_c2_pid;
    int res;

    memset(FD5NAME, 0, BUFSIZ);
    strcat(FD5NAME, path);
    strcat(FD5NAME, "/fifo5");
    pid_t gpid = getpid();
    if (pipe(fd1) != 0) {
        LOG_D("fd1_err");
        exit(-1);
    }


    if (pipe(fd2) != 0) {
        LOG_D("fd2_err");
        exit(-1);
    }

    if (access(FD5NAME, F_OK) != -1) {
        unlink(FD5NAME);
        LOG_D("access_err_%d", errno);

    }
    res = mkfifo(FD5NAME, S_IRUSR | S_IWUSR);
    if (res != 0) {
        LOG_D("fifo5_err_%d", errno);
        exit(-1);
    }

    c1_pid = fork();

    if (c1_pid < 0) {
        LOG_D("c1_err");
        exit(-1);
    } else if (c1_pid == 0) {//child_1
        pid_t c2_pid;
        int fd3[2];
        char buf1[1];
        int res;

        if (pipe(fd3) != 0) {
            LOG_D("fd3_err");
            exit(-1);
        }

        c2_pid = fork();

        if (c2_pid < 0) {
            LOG_D("c2_err");
            exit(-1);
        } else if (c2_pid == 0) {//child_2
            pthread_t p_moniter_p, p_moniter_c1;
            p_parameter_stop *p_p_s_p, *p_p_s_c1;

            int res;
            fd_set rdst;
            int nfds;
            int fd5 = open(FD5NAME, O_WRONLY);
            if (fd5 < 0) {
                exit(-1);
            }

            p_p_s_p = (p_parameter_stop *) malloc(sizeof(p_parameter_stop));
            p_p_s_c1 = (p_parameter_stop *) malloc(sizeof(p_parameter_stop));

            p_p_s_p->gpid = gpid;
            p_p_s_p->pid = gpid;
            p_p_s_p->ms = 10000;
            p_p_s_p->fd5 = fd5;

            p_p_s_c1->gpid = gpid;
            p_p_s_c1->pid = getppid();
            p_p_s_c1->ms = 10000;
            p_p_s_c1->fd5 = fd5;

            pthread_create(&p_moniter_p, NULL, &p_moniter_stop, p_p_s_p);
            pthread_create(&p_moniter_c1, NULL, &p_moniter_stop, p_p_s_c1);


            nfds = fd5;
            FD_ZERO(&rdst);
            FD_SET(fd5, &rdst);
            if (select(FD_SETSIZE, &rdst, NULL, NULL, NULL) > 0) {
                if (FD_ISSET(nfds, &rdst)) {
                    kill(gpid, SIGKILL);
                    close(fd5);
                    unlink(FD5NAME);
                    pthread_kill(pthread_self(), SIGKILL);
                }

            }

        } else {//child_1
            fd_set rdst;
            int nfds;
            pthread_t p_moniter_p, p_moniter_c2;
            p_parameter_stop *p_p_s_p, *p_p_s_c2;

            setProcessPriority(getpid(), -14);

            p_p_s_p = (p_parameter_stop *) malloc(sizeof(p_parameter_stop));
            p_p_s_c2 = (p_parameter_stop *) malloc(sizeof(p_parameter_stop));

            p_p_s_p->gpid = gpid;
            p_p_s_p->pid = gpid;
            p_p_s_p->ms = 10000;
            p_p_s_p->fd5 = 0;

            p_p_s_c2->gpid = gpid;
            p_p_s_c2->pid = c2_pid;
            p_p_s_c2->ms = 10000;
            p_p_s_c2->fd5 = 0;

            pthread_create(&p_moniter_p, NULL, &p_moniter_stop, p_p_s_p);
            pthread_create(&p_moniter_c2, NULL, &p_moniter_stop, p_p_s_c2);

            close(fd2[0]);
            res = write(fd2[1], &c2_pid, 4);

            close(fd3[0]);

            nfds = fd3[1];
            FD_ZERO(&rdst);
            FD_SET(fd3[1], &rdst);
            if (select(FD_SETSIZE, &rdst, NULL, NULL, NULL) > 0) {
                if (FD_ISSET(nfds, &rdst)) {
                    kill(gpid, SIGKILL);
                    close(fd3[1]);
                    unlink(FD5NAME);
                    pthread_kill(pthread_self(), SIGKILL);
                }

            }

        }
    } else {//father
        pthread_t p_kill, p_moniter_c1, p_moniter_c2;
        f_parameter_p_kill *f_p_p_k;
        p_parameter_stop *p_p_s_c1, *p_p_s_c2;
        int fd5 = open(FD5NAME, O_RDONLY);
        if (fd5 < 0) {
            LOG_D("father_err_%d", errno);
            exit(-1);
        }

        setProcessPriority(getpid(), -15);

        close(fd2[1]);
        close(fd1[0]);

        f_p_p_k = (f_parameter_p_kill *) malloc(sizeof(f_parameter_p_kill));
        p_p_s_c1 = (p_parameter_stop *) malloc(sizeof(p_parameter_stop));
        p_p_s_c2 = (p_parameter_stop *) malloc(sizeof(p_parameter_stop));

        f_p_p_k->fd1 = fd1[1];
        f_p_p_k->fd5 = fd5;
        f_p_p_k->gpid = gpid;

        res = read(fd2[0], &buf_c2_pid, 4);

        //LOG_D("father_%d",f_p_p_k->fd1);
        p_p_s_c1->gpid = gpid;
        p_p_s_c1->pid = c1_pid;
        p_p_s_c1->ms = 50000;
        p_p_s_c1->fd5 = 0;

        p_p_s_c2->gpid = gpid;
        p_p_s_c2->pid = buf_c2_pid;
        p_p_s_c2->ms = 50000;
        p_p_s_c2->fd5 = 0;

//		LOG_D("zhr-test-ndebug father create thread kill c1 c2!!!!");
        pthread_create(&p_kill, NULL, &p_father_kill, f_p_p_k);
        pthread_create(&p_moniter_c1, NULL, &p_moniter_stop, p_p_s_c1);
        pthread_create(&p_moniter_c2, NULL, &p_moniter_stop, p_p_s_c2);

        return;
    }
}

void *p_father_kill(void *arg) {
    f_parameter_p_kill *f_p_p_k;
    fd_set rdst;
    int nfds;

    f_p_p_k = (f_parameter_p_kill *) arg;

    nfds = f_p_p_k->fd1;

    FD_ZERO(&rdst);
    FD_SET(f_p_p_k->fd1, &rdst);

    if (select(FD_SETSIZE, &rdst, NULL, NULL, NULL) > 0) {
        if (FD_ISSET(nfds, &rdst)) {
            kill(f_p_p_k->gpid, SIGKILL);
            close(f_p_p_k->fd1);
            close(f_p_p_k->fd5);
            unlink(FD5NAME);
//			free(f_p_p_k);
            pthread_kill(pthread_self(), SIGKILL);
        }

    }

    return NULL;
}

int readProcessStateFromFile(char *state, const char *filePath) {
    const char sep[] = " ";
    const int sep_size = 1;
    char *strpos = NULL;
    char *target = NULL;
    int cnt;
    int fd = open(filePath, O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    char buffer[100] = {'\0'};
    const int len = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);
    if (len < 0) {
        return 0;
    }

    target = buffer;
    for (cnt = 0; cnt < 2; cnt++) {
        strpos = strstr(target, sep);
        target = strpos + sep_size;
    }
    if (strpos) {
        *state = target[0];
        return 1;
    }

    return 0;

}

void *p_moniter_stop(void *arg) {
    p_parameter_stop *p_p_s;

    char filepathProcStat[100];

    p_p_s = (p_parameter_stop *) arg;
    sprintf(filepathProcStat, "/proc/%d/stat", p_p_s->pid);

    do {
        char state;

        if (readProcessStateFromFile(&state, filepathProcStat)) {

            if (state == 't' || state == 'T' || state == 'z' || state == 'Z') {

                if (p_p_s->fd5 != 0) {
                    unlink(FD5NAME);
                }

                kill(p_p_s->pid, SIGKILL);
                kill(p_p_s->gpid, SIGKILL);

            } else {
                usleep(p_p_s->ms);
            }
        }


    } while (1);
}


int setProcessPriority(int pid, int delta) {
    const int nice = getpriority(PRIO_PROCESS, pid);

    if (setpriority(PRIO_PROCESS, pid, nice + delta)) {
        return 0;
    }

    return 1;
}

void protect_jdebug() {
    pthread_t p;

    pthread_create(&p, NULL, p_protect_jdebug, NULL);
}

void *p_protect_jdebug(void *arg) {

    while (1) {
        int fd;
        int useStdInd = 0;
        int netFlag = 0;
        struct stat fileStat;
        for (fd = 0; fd < FD_SETSIZE; fd++) {
            if (validate(fd)) {//validate_fd
                if (fstat(fd, &fileStat) == 0 && S_ISSOCK(fileStat.st_mode)) {//by_socket
                    LOG_D("jde_socket_%d", fd);
                    LOG_D("jde_st_uid:%d", fileStat.st_uid);
                    if (fileStat.st_uid == 0 || fileStat.st_uid == 2000 ||
                        fileStat.st_uid >= 10000) {//user
                        netFlag = 0;
                        useStdInd = 0;
                        LOG_D("user_%d", fd);
                        processcmdline(fd, &useStdInd);
                        LOG_D("processcmdline_%d", fd);
                        if (1 == useStdInd) {
                            processadbd(fd, &netFlag);
                        }
                        if (1 == useStdInd && 0 == netFlag) {
                            processdebugtool(fd, fileStat);
                        }
                    } else {//sysytem

                    }
                } else {//no_socket

                }
            }
        }
        usleep(1000000);
    }


}

int validate(int fd) {
    if (fd < 3 || fd >= FD_SETSIZE)
        return 0;
    if (fcntl(fd, F_GETFL) == -1 && errno == EBADF)
        return 0;
    return 1;
}

void processcmdline(int fd, int *useStdInd) {
    char fdName[256];
    char newBuf[1024];
    char msg[1024];
    int bytes, pos;
    FILE *fp = NULL;

    if (*useStdInd) {
        return;
    }
    snprintf(fdName, sizeof(fdName), "/proc/%d/cmdline", getpid());

    if ((fp = fopen(fdName, "rb")) == NULL) {
        *useStdInd = 1;
        return;
    }

    if ((bytes = fread(msg, 1, 1024, fp)) == -1) {
        fclose(fp);
        *useStdInd = 1;
        return;
    }
    fclose(fp);

    for (pos = 0; pos < bytes; pos++) {
        if (msg[pos] == '\0' && pos < bytes - 1) {
            newBuf[pos] = ' ';
        } else {
            newBuf[pos] = msg[pos];
        }
    }


    if (strstr(newBuf, "-Xrunjdwp") != NULL || strstr(newBuf, "-agentlib:jdwp")) {
        exit(20);
        return;
    }

    *useStdInd = 1;
}

int processadbd(int fd, int *netFlag) {
    struct sockaddr_un serv_un_addr;
    socklen_t sockLen;
    memset(&serv_un_addr, 0, sizeof(serv_un_addr));
    sockLen = sizeof(serv_un_addr);

    if (getpeername(fd, (struct sockaddr *) &(serv_un_addr), &sockLen) != 0) {
        return 0;
    }

    if (memcmp(serv_un_addr.sun_path, "\0jdwp-control", 13) != 0) {
        return 0;
    }
    *netFlag = 1;
    close(fd);
    return 1;
}

int processdebugtool(int fd, struct stat fs) {
    struct sockaddr_in servAddr;
    char addrStr[UNIX_PATH_MAX];
    char addrStrRemote[UNIX_PATH_MAX];
    unsigned short service;
    unsigned short serviceRemote;
    socklen_t sockLen;

    if (geteuid() == fs.st_uid) {
        return 0;
    }
    memset(&servAddr, 0, sizeof(struct sockaddr_in));
    sockLen = sizeof(servAddr);
    if (getpeername(fd, (struct sockaddr *) &(servAddr), &sockLen) != 0) {
        return 0;
    }
    serviceRemote = ntohs(servAddr.sin_port);
    inet_ntop(AF_INET, &(servAddr.sin_addr), addrStrRemote, UNIX_PATH_MAX);

    memset(&servAddr, 0, sizeof(struct sockaddr_in));
    sockLen = sizeof(servAddr);

    if (getsockname(fd, (struct sockaddr *) &(servAddr), &sockLen) != 0) {
        return 0;
    }

    service = ntohs(servAddr.sin_port);
    inet_ntop(AF_INET, &(servAddr.sin_addr), addrStr, UNIX_PATH_MAX);


    if (servAddr.sin_family == AF_UNIX && service == 0 && serviceRemote == 0 &&
        strcmp(addrStr, "0.0.0.0") == 0 && strcmp(addrStrRemote, "0.0.0.0") == 0) {
        exit(20);
    }
    return 1;
}
