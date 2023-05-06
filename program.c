#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct stat st;
    
    if(argc == 1) {
        printf("We expect at least one argument, which should be a file or directory\n");
        return -1;
    }
    else {
        for(int i = 0; i < argc; i++) {     
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
                    switch (option)
                    {
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

                        case 'a':   printf("Access rights are: %d\n", st.st_mode);
                                    break;

                        case 'l':   char link[26];
                                    printf("Please provide a link name (25 chars max): ");
                                    scanf("%25s", &link);
                                    int sl = symlink(argv[1], link);
                                    if (sl == 0){
                                        printf("\nCreated Symbolic link!\n");
                                    } else
                                        printf("Couldn't create symbolic link\n");
                                    break;

                        default:    break;
                    }
                }
                char* extension = srtrchr(argv[i],  '.c');
                if(strcmp(extension, argv[i]) != 0) {
                    pid_t compile = fork();
                }
                    
                
            }
                
        }
        
    }
    
    return 0;
}