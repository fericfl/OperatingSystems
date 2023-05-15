#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

//COUNT C FILES INSIDE DIRECTORY
void countCFiles(char* arg) {
    DIR* dir;
    struct dirent* readd;
    int count = 0;
    dir = opendir(arg);
    if(dir != NULL) {
        while((readd = readdir(dir)) != NULL) {
            if(strstr(readd->d_name, ".c") != NULL)
                count++;
        }
        closedir(dir);
    } else {
        printf("Couldn't open directory");
    }
    printf("The total number of C files is: %d\n", count);
}

//GET SCORE FUNCTION
void getScore(int errors, int warnings, char* arg) {
    float score = 0;
    if(errors != 0)
        score = 1;
    else if (warnings == 0)
        score = 10;
    else if (warnings > 10)
        score = 2;
    else score = 2 + 8*(10-warnings)/10.0;
    FILE* out = fopen("grades.txt", "a");
    if(!out) {
        perror("Could not open file");
        exit(1);
    }
    fprintf(out,"%s:%.2f\n",arg, score);
    fclose(out);
}

//FUNCTION TO PRINT ACCESS RIGHT ACCORDING TO PROJECT SPECIFICATION
void access_rights(char* arg) {
    struct stat st;
    lstat(arg, &st);
    printf("User:\n");
    if(S_IRUSR & st.st_mode){
        printf("Read: Yes\n");
    } else printf("Read: No\n");
    if(S_IWUSR & st.st_mode) {
        printf("Write: Yes\n");
    } else printf("Write: No\n");
    if(S_IXUSR & st.st_mode) {
        printf("Exec: Yes\n");
    } else printf("Exec: No\n");
    printf("Group:\n");
    if(S_IRGRP & st.st_mode) {
        printf("Read: Yes\n");
    } else printf("Read: No\n");
    if(S_IWGRP & st.st_mode) {
        printf("Write: Yes\n");
    } else printf("Write: No\n");
    if(S_IXGRP & st.st_mode) {
        printf("Exec: Yes\n");
    } else printf("Exec: No\n");
    printf("Other:\n");
    if(S_IROTH & st.st_mode) {
        printf("Read: Yes\n");
    } else printf("Read: No\n");
    if(S_IWOTH & st.st_mode) {
        printf("Write: Yes\n");
    } else printf("Write: No\n");
    if(S_IXOTH & st.st_mode) {
        printf("Exec: Yes\n");
    } else printf("Exec: No\n");
}

int main(int argc, char *argv[]) {
    struct stat st;
    
    if(argc == 1) {
        printf("We expect at least one argument, which should be a file or directory\n");
        return -1;
    }
    else {
        for(int i = 1; i < argc; i++) {     
            if(lstat(argv[i], &st) == -1) {
                perror("lstat");
                continue;
            }
            pid_t menu = fork();
            if(menu == 0) {
                //REGULAR FILE MENU
                if(S_ISREG(st.st_mode) != 0) {
                    fflush(stdin);
                    printf("The current argument you passed is a regular file.\nWhat do you want to do with it?\n[-n] - file name\n[-d] - Dimension / size\n[-h] - Number of hard links\n[-m] - Time of last modification\n[-a] - Access rights\n[-l] - Create a symbolic link (provide the link's name)\n[-q] - Quit the program / Go to the next argument\n");
                    printf("Options: ");
                    int option = getchar();
                    int running = 1;
                    while(running && (option = getchar()) != EOF) {
                        switch (option) {
                            case 'q':   running = 0;
                                        break;
                            case 'n':   printf("\nFile name is: %s", argv[i]);
                                        break;

                            case 'd':   printf("\nTotal size of file is: %ld bytes", st.st_size);
                                        break;

                            case 'h':   printf("\nTotal number of hard links is: %ld", st.st_nlink);
                                        break;

                            case 'm':   time_t modificationTime = st.st_mtime;
                                        char* modifiedTime = ctime(&modificationTime);
                                        printf("\nTime of last modification is: %s", modifiedTime);
                                        break;

                            case 'a':   access_rights(argv[i]);
                                        break;

                            case 'l':   char link[26];
                                        printf("Please provide a link name (25 chars max): ");
                                        scanf("%25s", link);
                                        int sl = symlink(argv[1], link);
                                        if (sl == 0){
                                            printf("\nCreated Symbolic link!\n");
                                        } else
                                            printf("Couldn't create symbolic link\n");
                                        break;
                            case '-':   break;
                            case '\n':  printf("\n\nWhat do you want to do with it?\n[-n] - file name\n[-d] - Dimension / size\n[-h] - Number of hard links\n[-m] - Time of last modification\n[-a] - Access rights\n[-l] - Create a symbolic link (provide the link's name)\n[-q] - Quit the program / Go to next argument\nOptions:");
                                        break;   
                            default:    printf("'%c' is not a valid option in the menu!\n", option);
                                        break;
                        }
                    }
                }
                //SYMBOLIC LINK MENU
                if(S_ISLNK(st.st_mode) != 0) {
                    fflush(stdin);
                    printf("This is a link file\nWhat do you want to do with it?\n[-n] - File name\n[-l] - Delete symbolic link\n[-d] - Size of symbolic link\n[-t] - Size of target file\n[-a] - Access rights\n[-q] - Quit the program / Go to next argument\nOptions:");
                    int option = getchar();
                    int running = 1;
                    while(running && (option = getchar()) != EOF) {
                        switch(option) {
                            case 'q':   running = 0;
                                        break;
                            case 'n':   printf("Symbolic link name is: %s\n", argv[i]);
                                        break;
                            case 'l':   printf("Deleting symbolic link...\n");
                                        unlink(argv[i]);
                                        exit(0);
                                        break;
                            case 'd':   printf("Size of the symbolic link is: %ld\n", st.st_size);
                                        break;
                            case 't':   struct stat target;
                                        stat(argv[i], &target);
                                        printf("Size of the target file is: %ld\n", target.st_size);
                            case 'a':   access_rights(argv[i]);
                                        break;
                            case '-':   break;
                            case '\n':  printf("\nWhat do you want to do with it?\n[-n] - File name\n[-l] - Delete symbolic link\n[-d] - Size of symbolic link\n[-t] - Size of target file\n[-a] - Access rights\n[-q] - Quit the program / Go to next argument\nOptions:");
                                        break;
                            default:    printf("'%c' is not a valid option in the menu!\n", option);
                                        break;
                        }
                    }
                }
                //DIRECTORY MENU
                if(S_ISDIR(st.st_mode) != 0) {
                    fflush(stdin);
                    printf("This is a directory!\n");
                    printf("What do you want to do with it?\n[-n] - Directory name\n[-d] - Size of directory\n[-a] - Access rights\n[-c] - Total number of files with the .c extension\n[-q] - Quit program / Go to next argument\nOptions:");
                    int running = 1;
                    int option;
                    while(running && (option = getchar()) != EOF) {
                        switch (option)
                        {
                        case 'n':
                            printf("\nDirectory name is: %s", argv[i]);
                            break;
                        case 'd':
                            printf("\nSize of the directory is: %ld", st.st_size);
                            break;
                        case 'a':
                            access_rights(argv[i]);
                            break;
                        case 'c':
                            countCFiles(argv[i]);
                            break;
                        case 'q':
                            running = 0;
                            break;
                        case '-': break;
                        case '\n': 
                            printf("\nWhat do you want to do with it?\n[-n] - Directory name\n[-d] - Size of directory\n[-a] - Access rights\n[-c] - Total number of files with the .c extension\n[-q] - Quit program / Go to next argument\nOptions:");
                            break;
                        default:
                            printf("\n'%c' is not a valid option in the menu!", option);
                            break;
                        }
                    }
                }
            } else if(menu > 0) {
                pid_t status = waitpid(menu, &status, 0);
                if(WIFEXITED(status)) {
                    int exitCode = WEXITSTATUS(status);
                    printf("\nThe process with PID %d has ended with the exit code %d\n",status, exitCode);
                } else {
                    printf("The process with PID %d has terminated abnormally\n", status);
                }
                pid_t s_child = fork();
                if(s_child < 0) {
                    perror("Second child process could not be created");
                    exit(1);
                } else if(s_child == 0) {
                    if((strstr(argv[i], ".c") != NULL)) {
                        int pfd[2]; //[0] - read, [1] - write
                        int newfd;
                        if(pipe(pfd) < 0) {
                            perror("Could not create pipe");
                            exit(-3);
                        }

                        int p2 = fork();
                        if(p2 < 0) {
                            perror("Could not create new process");
                            exit(-4);
                        } else if(p2 == 0) {
                            close(pfd[0]);
                            if((newfd = dup2(pfd[1],1))<0) {
                                perror("Could not duplicate");
                                exit(1);
                            }
                            execlp("bash", "bash", "compile.sh", argv[i], NULL);
                            close(pfd[1]);
                            exit(1);
                        }
                        close(pfd[1]); /* close the writting end of the pipe */
                        FILE* stream = fdopen(pfd[0],"r");
                        char string[100];
                        fscanf(stream, "%s", string);
                        int errors = atoi(string);
                        fscanf(stream, "%s", string);
                        int warnings = atoi(string);
                        getScore(errors, warnings, argv[i]);
                        close(pfd[0]);
                    }
                    if(S_ISREG(st.st_mode) != 0) {
                        int pfd[2]; //[0] - read, [1] - write
                        int newfd;
                        if(pipe(pfd) < 0) {
                            perror("Could not create pipe");
                            exit(-3);
                        }

                        int p2 = fork();
                        if(p2 < 0) {
                            perror("Could not create new process");
                            exit(-4);
                        } else if(p2 == 0) {
                            close(pfd[0]);
                            if((newfd = dup2(pfd[1],1))<0) {
                                perror("Could not duplicate");
                                exit(1);
                            }
                            execlp("bash", "bash", "word_count.sh", argv[i], NULL);
                            close(pfd[1]);
                            exit(1);
                        }
                        close(pfd[1]); /* close the writting end of the pipe */
                        FILE* stream = fdopen(pfd[0],"r");
                        char string[100];
                        fscanf(stream, "%s", string);
                        int word_count = atoi(string);
                        printf("Total lines of text: %d\n", word_count);
                        close(pfd[0]);
                    } 
                    if(S_ISLNK(st.st_mode) != 0) {
                        printf("Changing permissions for symbolic link %s\n", argv[i]);
                        execlp("chmod", "chmod", "u=rwx,g=rw,o=", argv[i], NULL);
                        perror("execlp");
                        exit(1);   
    
                    }
                    if(S_ISDIR(st.st_mode) != 0) {
                        char file_name[strlen(argv[i]) + 11];
                        if(strrchr(argv[i], '/') != 0) {
                            char* dir_name = strrchr(argv[i], '/') + 1;
                            sprintf(file_name, "%s/%s_file.txt", argv[i], dir_name);
                        } else {
                            strcpy(file_name, argv[i]);
                            strcat(file_name, "/");
                            strcat(file_name, argv[i]);
                            strcat(file_name, "_file.txt");
                        }
                        int fd = open(file_name, O_WRONLY | O_CREAT, 0644);
                        if(fd == -1) {
                            perror("open");
                            exit(EXIT_FAILURE);
                        }
                        close(fd);
                        exit(EXIT_SUCCESS);
                    }
                } else {
                    pid_t status = waitpid(s_child, &status, 0);
                    if(WIFEXITED(status)) {
                        int exitCode = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n",status, exitCode);
                    } else {
                        printf("The process with PID %d has terminated abnormally\n", status);
                    }
                }
                    
            } else {
                perror("Error with menu process");
                exit(-2);
            } 
                
                 
        }
    }
    return 0;
}