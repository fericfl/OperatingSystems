#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

float getScore(float score, int errors, int warnings) {
    if(errors != 0)
        score = 1;
    else if (warnings == 0)
        score = 10;
    else if (warnings > 10)
        score = 2;
    else score = 2 + 8*(10-warnings)/10.0;
    return score;
}

int main(int argc, char *argv[]) {
    struct stat st;
    
    if(argc == 1) {
        printf("We expect at least one argument, which should be a file or directory\n");
        return -1;
    }
    else {
        for(int i = 1; i < argc; i++) {     
            lstat(argv[i], &st);
            if(S_ISREG(st.st_mode) != 0) {
                printf("The current argument you passed is a regular file.\nWhat do you want to do with it?\n[-n] - file name\n[-d] - Dimension / size\n[-h] - Number of hard links\n[-m] - Time of last modification\n[-a] - Access rights\n[-l] - Create a symbolic link (provide the link's name)\n[-q] - Exit Program\n");
                printf("Options: ");
                int option = getchar();
                int running = 1;
                int inSwitch = 0;
                pid_t menu = fork();
                if(menu == 0) {
                    while((option = getchar()) != EOF && running) {
                        switch (option) {
                            case 'q':   running = 0;
                                        break;

                            case 'n':   printf("File name is: %s\n", argv[i]);
                                        break;

                            case 'd':   printf("Total size of file is: %ld bytes\n", st.st_size);
                                        break;

                            case 'h':   printf("Total number of hard links is: %ld\n", st.st_nlink);
                                        break;

                            case 'm':   printf("Time of last modification is: %ld\n", st.st_mtime);
                                        break;

                            case 'a':   printf("User:\n");
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
                                        
                            default:    break;
                        }
                    }
                    exit(0);
                } else if(menu > 0) {
                    int status;
                    wait(&status);
                }
                } else {
                    perror("Error with menu process");
                    exit(-2);
                }
                if(strstr(argv[i], ".c") != NULL) {
                    int pfd[2]; //[0] - read, [1] - write
                    char buffer[30];
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
                        //write(pfd[1], buffer, sizeof(buffer)/sizeof(buffer[0]));
                        close(pfd[1]);
                        exit(1);
                    }
                    close(pfd[1]); /* close the writting end of the pipe */
                    FILE* stream = fdopen(pfd[0],"r");
                    char string[100];
                    fscanf(stream, "%s", string);
                    int errors = atoi(string);
                    //fprintf(stdout, "There are %d errors\n", errors);
                    fscanf(stream, "%s", string);
                    int warnings = atoi(string);
                    //fprintf(stdout, "There are %d warnings\n", warnings);

                    float score = 0;
                    printf("Total score is: %.2f\n", getScore(score, errors, warnings));
                    close(pfd[0]);
            }
        }
    }
    
    return 0;
}