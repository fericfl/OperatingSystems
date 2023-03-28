#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    struct stat st;
    
    if(argc == 1) {
        printf("We expect at least one argument, which should be a file!\n");
        return -1;
    }
    else {
        lstat(argv[1], &st);
        if(S_ISREG(st.st_mode) == 0) {
            printf("File is not regular file\n");
        }
        
    }
    
    return 0;
}