#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define NO_COMMOND "noCommond"
#define DIRNAME "."

char com[2][100];
int flag[7] = {0};

/**
 * 解析参数
 * @param argc 参数个数
 * @param argv 参数
 */
void resolveArgs(int argc, char *argv[]) {
    strcpy(com[0], DIRNAME);
    strcpy(com[1], NO_COMMOND);
    if (argc == 1) {
    }
    if (argc == 2) {
        if (argv[1][0] != '-') {
            strcpy(com[0], argv[1]);
        } else {
            strcpy(com[1], argv[1]);
        }
    }
    if (argc == 3) {
        if ((argv[1][0] == '-' && argv[2][0] == '-') || (argv[1][0] != '-' && argv[2][0] != '-')) {
            perror("no match commond !");
        }
        if (argv[1][0] != '-') {

            strcpy(com[0], argv[1]);
            strcpy(com[1], argv[2]);
        } else {

            strcpy(com[1], argv[1]);
            strcpy(com[0], argv[2]);
        }
    }
}

/**
 * 打印 -l
 * @param dp 当前目录
 */
void lPrint(struct dirent *dp) {
    struct stat *buf = malloc(sizeof(struct stat));
    char filepath[100] = {};
    strcat(filepath, com[0]);
    strcat(filepath, "/");
    strcat(filepath, dp->d_name);
    if (stat(filepath, buf) != 0) {
        perror("stat error!");
        exit(EXIT_FAILURE);
    }
    for (int i = 8; i >= 0; i--) {
        if (buf->st_mode & 1 << i) {
            switch (i % 3) {
                case (2):
                    printf("r");
                    break;
                case (1):
                    printf("w");
                    break;
                case (0):
                    printf("x");
                    break;
                default:
                    break;
            }
        } else {
            printf("-");
        }
    }
    printf("\t");
    switch (buf->st_mode & S_IFMT) {
        case S_IFREG:
            printf("-\t");
            break;
        case S_IFDIR:
            printf("d\t");
            break;
        case S_IFLNK:
            printf("l\t");
            break;
        case S_IFBLK:
            printf("b\t");
            break;
        case S_IFCHR:
            printf("c\t");
            break;
        case S_IFIFO:
            printf("p\t");
            break;
        case S_IFSOCK:
            printf("s\t");
            break;
    }

    printf("%ld\t", buf->st_size);
    struct tm *ftm = localtime(&buf->st_ctime);
    printf(" %04d-%02d-%02d %02d:%02d\t", ftm->tm_year + 1900, ftm->tm_mon + 1, ftm->tm_mday, ftm->tm_hour,
           ftm->tm_min);
    printf("%-15s\t", dp->d_name);
    printf("\n");
}

int main(int argc, char *argv[]) {
    resolveArgs(argc, argv);
    if (com[1] != NO_COMMOND) {
        for (int i = 1; com[1][i] != '\0'; i++) {
            switch (com[1][i]) {
                case ('a'):
                    flag[0] = 1;
                    break;
                case ('l'):
                    flag[1] = 1;
                    break;
                case ('m'):
                    flag[2] = 1;
                    break;
                case ('t'):
                    flag[3] = 1;
                    break;
                default:
                    break;
            }
        }
    }
    DIR *dir;
    struct dirent *dp_move = malloc(sizeof(struct dirent));
    struct dirent *dp = malloc(sizeof(struct dirent));
    int i = 0;
    dir = opendir(com[0]);
    while (1) {
        if ((readdir_r(dir, dp_move, &dp)) != 0) {
            perror("readdir_r error!");
            exit(EXIT_FAILURE);
        }
        if (dp == NULL) {
            printf("\n");
            break;
        }
        if (dp->d_name[0] == '.' && flag[0] == 0) {
            continue;
        }
        if (flag[1] == 1) {
            lPrint(dp);
        } else {
            if (flag[2] == 1) {
                printf("%s ", dp->d_name);
            } else {
                printf("%s\t", dp->d_name);
            }
        }
    }
    return 0;
}
