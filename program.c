#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    struct stat st;
    
    if(argc == 1) {
        printf("We expect at least one argument, which should be a file or directory\n");
        return -1;
    }
    else {
        for(int i = 1; argv[i] != NULL; i++) {
            lstat(argv[i], &st);
            if(S_ISREG(st.st_mode) != 0) {
                char* extension = strrchr(argv[i], '.');
                //printf("%s", extension);
                if(strcmp(extension, ".c") != 0) {
                    printf("The current argument you passed is a regular file.\nWhat do you want to do with it?\n[-n] - file name\n[-d] - Dimension / size\n[-h] - Number of hard links\n[-m] - Time of last modification\n[-a] - Access rights\n[-l] - Create a symbolic link (provide the link's name)\n[-q] - Exit Program\n");
                    printf("Options: ");
                    int option = getchar();
                    while((option = getchar()) != EOF) {
                        if(option == 'q')
                            return 0;
                        if(option == 'n') {
                            printf("File name is: %s\n", argv[1]);
                        }
                        if(option == 'd') {
                            printf("Total size of file is: %ld bytes\n", st.st_size);
                        }
                        if(option == 'h') {
                            printf("Total number of hard links is: %ld\n", st.st_nlink);
                        }
                        if(option == 'm') {
                            printf("Time of last modification is: %ld\n", st.st_mtime);
                        }
                        if(option == 'a') {
                            printf("Access rights are: %d\n", st.st_mode);
                        }
                        if(option == 'l') {
                            char link[26];
                            printf("Please provide a link name (25 chars max): ");
                            scanf("%s", &link);
                            int sl = symlink(argv[1], link);
                            if (sl == 0){
                                printf("\nCreated Symbolic link!\n");
                            } else
                                printf("Couldn't create symbolic link\n");
                        }
                        if(option == '\n') {
                            printf("Options: ");
                        }
                    } 
                } else {
                    pid_t pid=fork();
                    if (pid == 0) {
                        system("./compile.sh");
                        exit(0);
                    } else {
                        waitpid(pid,0,0);
                        printf("C compile is done");
                    }
                }
            }   
            if(S_ISDIR(st.st_mode) != 0) {
                
            }
        }
    }
    
    return 0;
}