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
                if(lstat(argv[i], &st) == 0) {
                    pid_t menu = fork();
                    if(menu == 0) {
                        if(S_ISREG(st.st_mode) != 0) {
                            //printf("%s", extension);
                            printf("The current argument you passed is a regular file.\nWhat do you want to do with it?\n[-n] - file name\n[-d] - Dimension / size\n[-h] - Number of hard links\n[-m] - Time of last modification\n[-a] - Access rights\n[-l] - Create a symbolic link (provide the link's name)\n[-q] - Exit Program\n");
                            printf("Options: ");
                            int option;
                            int quit=1;
                            while((option = getchar()) != EOF && quit) {
                                switch(option) {
                                    case '-': break;
                                    case 'n':
                                        printf("File name is: %s\n", argv[1]);
                                        break;
                                    case 'd':
                                        printf("Total size of file is: %ld bytes\n", st.st_size);
                                        break;
                                    case 'h':
                                        printf("Total number of hard links is: %ld\n", st.st_nlink);
                                        break;
                                    case 'm':
                                        printf("Time of last modification is: %ld\n", st.st_mtime);
                                        break;
                                    case 'a':
                                        printf("Access rights are: %d\n", st.st_mode);
                                        break;
                                    case 'l':
                                        char link[26];
                                        printf("Please provide a link name (25 chars max): ");
                                        scanf("%25s", &link);
                                        int sl = symlink(argv[1], link);
                                        if (sl == 0){
                                            printf("\nCreated Symbolic link!\n");
                                        } else {
                                            printf("Couldn't create symbolic link\n");
                                        }
                                        break;
                                    case '\n':
                                        printf("Options: ");
                                        break;
                                    case 'q':
                                        quit = 0;
                                        break;
                                    default:
                                        printf("Invalid option\n");
                                        break;
                                }
                            }
                            
                        }
                        
                    } else {
                        char* extension = strstr( argv[i], ".c");
                        if(extension != NULL) { 
                                int pfd[2];
                                if(pipe(pfd) < 0) {
                                    perror("Pipe creation error\n");
                                    exit(1);
                                }
                                pid_t pid=fork();
                                if(pid < 0) {
                                    perror("\nFork failed.\n");
                                    return -1;
                                }
                                else if (pid == 0) {
                                    close(pfd[0]);
                                    dup2(pfd[1],1);
                                    execlp("bash", "bash", "compile.sh", argv[i], NULL);
                                    close(pfd[1]);
                                    perror("\nExec failed.\n");
                                    return -2;
                                } else {
                                    close(pfd[1]);
                                    FILE* stream=fdopen(pfd[0],"r");
                                    
                                    close(pfd[0]);
                                    int status;
                                    wait(&status);
                                }
                        int status;
                        wait(&status);
                        
                    }
                    
                    }   
                // if argument is a directory create new file using fork()
                if(S_ISDIR(st.st_mode) != 0) {
                    printf("\nI see a folder\n");
                    pid_t create_txt = fork();
                    if(create_txt == 0) {
                        execlp("bash", "bash", "create_file.sh", argv[i], NULL);
                    }
                }
            } else {
                int status;
                wait(&status);
            }
            }
            
    }
    return 0;
}