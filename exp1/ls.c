/*
-a 列出目录下的所有文件，包含以.开头的隐藏文件。
-d 显示目录的自身属性，而不显示目录下的文件。
-l 列出目录或者文件的详细信息
-i 显示文件或者目录的inode信息，即索引信息
-r 与其他参数一起使用，使之反向排序
*/
#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
#include<unistd.h>
#include<string.h>

#define LS_NONE 0
#define LS_L 101
#define LS_R 102
#define LS_D 103
#define LS_I 104
#define LS_A 200

#define LS_AL (LS_A+LS_L)
#define LS_AI (LS_A+LS_I)

//展示单个文件的详细信息
void show_file_info(char *filename, struct stat *info_p) {
    char *uid_to_name(), *ctime(), *gid_to_name(), *filemode();

    void mode_to_letters();
    char modestr[11];

    mode_to_letters(info_p->st_mode, modestr);//权限信息

    printf("%s", modestr);
    printf(" %4d", (int) info_p->st_nlink); //硬连接数
    printf(" %-8s", uid_to_name(info_p->st_uid));//用户的名字
    printf(" %-8s", gid_to_name(info_p->st_gid));//用户组的名字
    printf(" %8ld", (long) info_p->st_size);//文件大小
    printf(" %.12s", 4 + ctime(&info_p->st_mtime));//文件创建时间
    printf(" %s\n", filename);//文件名
}

void mode_to_letters(int mode, char str[]) {
    strcpy(str, "----------");

    if (S_ISDIR(mode)) //是否是一个目录
        str[0] = 'd';

    if (S_ISCHR(mode)) //是否是一个字符设备
        str[0] = 'c';

    if (S_ISBLK(mode)) //是否是一个块设备
        str[0] = 'b';

    if ((mode & S_IRUSR)) //用户读
        str[1] = 'r';

    if ((mode & S_IWUSR)) //用户写
        str[2] = 'w';

    if ((mode & S_IXUSR))
        str[3] = 'x';

    if ((mode & S_IRGRP)) //用户组读
        str[4] = 'r';

    if ((mode & S_IWGRP)) //用户组写
        str[5] = 'w';

    if ((mode & S_IXGRP))
        str[6] = 'x';

    if ((mode & S_IROTH))
        str[7] = 'r';

    if ((mode & S_IWOTH))
        str[8] = 'w';

    if ((mode & S_IXOTH))
        str[9] = 'x';

}

char *uid_to_name(uid_t uid) {
    struct passwd *getpwuid(), *pw_ptr;
    static char numstr[10];

    if ((pw_ptr == getpwuid(uid)) == NULL)//出错返回用户id
    {
        sprintf(numstr, "%d", uid);
        return numstr;
    } else
        return pw_ptr->pw_name; //用户名字
}

char *gid_to_name(gid_t gid) {
    struct group *getgrgid(), *grp_ptr;
    static char numstr[10];
    if ((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    } else
        return grp_ptr->gr_name;
}

void do_ls(char dirname[], int mode) {
    DIR *dir_ptr;
    struct dirent *direntp;

    if ((dir_ptr = opendir(dirname)) == NULL) //返回dirname子目录下所有文件和目录的列表，如果是文件，返回值为NULL
        fprintf(stderr, "ls2: cannot open %s \n", dirname);
    else {
        if (mode == LS_D) //-d 命令，显示目录
            printf("%s\n", dirname);
        else {
            char dirs[20][100];
            int dir_count = 0;

            while ((direntp = readdir(dir_ptr)) != NULL)//循环读取dir_ptr
            {
                if (mode < 200 && direntp->d_name[0] == '.')
                    continue;

                char complete_d_name[200]; //文件的完整路径
                strcpy(complete_d_name, dirname);
                strcat(complete_d_name, "/");
                strcat(complete_d_name, direntp->d_name);

                struct stat info;
                if (stat(complete_d_name, &info) == -1) //获取指定路径和文件夹的信息
                    perror(complete_d_name);
                else {
                    if (mode == LS_L || mode == LS_AL)//列出目录或文件的详细信息
                        show_file_info(direntp->d_name, &info);
                    else if (mode == LS_A || mode == LS_NONE || mode == LS_I || mode == LS_AI) {
                        if (mode == LS_I || mode == LS_AI)//列出文件或目录的inode信息
                            printf("%llu ", direntp->d_ino);

                        printf("%s\n", direntp->d_name);
                    } else if (mode == LS_R)//与其他参数一起使用，使其反向排序
                    {
                        if (S_ISDIR(info.st_mode))//是目录
                        {
                            printf("%s\n", direntp->d_name);

                            strcpy(dirs[dir_count], complete_d_name);
                            dir_count++;
                        } else
                            printf("%s\n", direntp->d_name);
                    }
                }
            }
            if (mode == LS_R)//将目录下的所有子目录文件都显示出来，递归实现
            {
                int i = 0;
                printf("\n");
                for (; i < dir_count; i++) {
                    printf("%s:\n", dirs[i]);
                    do_ls(dirs[i], LS_R);
                    printf("\n");
                }
            }

        }

        closedir(dir_ptr);
    }
}

int analyzeParam(char *input) {
    if (strlen(input) == 2) {
        if (input[1] == 'l')
            return LS_L;
        else if (input[1] == 'a')
            return LS_A;
        else if (input[1] == 'd')
            return LS_D;
        else if (input[1] == 'R')
            return LS_R;
        else if (input[1] == 'i')
            return LS_I;
    } else if (strlen(input) == 3) {
        if (input[1] == 'a' && input[2] == 'l')
            return LS_AL;
        if (input[1] == 'a' && input[2] == 'i')
            return LS_AI;
    }
    return -1;
}

int main(int arc, char *argv[]) {
    //printf("%d\n",arc);
    if (arc == 1) //如果没有参数，打印当前目录下的文件信息
        do_ls(".", LS_NONE);
    else {
        int mode = LS_NONE; //默认为无参数ls
        int have_file_param = 0; //是否有输入文件参数

        while (arc > 1) {
            arc--;
            argv++;
            int calMode = analyzeParam(*argv);
            if (calMode != -1)
                mode += calMode;
            else {
                have_file_param = 1;
                do {
                    do_ls(*argv, mode);//mode表示命令参数
                    printf("\n");

                    arc--;
                    argv++;
                } while (arc >= 1);

            }
        }
        if (!have_file_param)
            do_ls(".", mode);
    }
}