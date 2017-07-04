/*
 * Eran Peled
 *
 * Made on MacBook Pro 15'
 * Made in Unix-apple operating system
 *
 *
 * */


#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>

#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define RESET "\x1B[0m"

void printPermissions(struct stat itemStats)
{
    /*This function responsible for printing the file mode in unix
     *
     * */
    mode_t mode = itemStats.st_mode;

    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");


    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");


    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");


}

void printMonthDayYear(struct stat itemStats)
{
    /*This function is responsible for printing the file last access date
     *
     * */
    char buffer [80];
    struct tm * timeinfo;
    time_t timeStamp = itemStats.st_atime;
    timeinfo = localtime (&timeStamp);
    strftime (buffer,80,"\t%h\t%e\t%Y\t",timeinfo);
    printf("\t%s",buffer);

}

void printFileType(struct stat itemStats)
{
/*This function prints in a "digit" form the type of the file
 *
 * */
    printf( (S_ISDIR(itemStats.st_mode)) ? "d" : "");
    printf( (S_ISREG(itemStats.st_mode)) ? "-" : "");
    printf( (S_ISLNK(itemStats.st_mode)) ? "l" : "");

}

void printFileNames(char *name,struct stat *itemStats){
    /*
     * This function is responsible for printing the file name, and if the file is a link, print the orginal file also.
     * */

    char buff[100];

    if(S_ISREG(itemStats->st_mode))
    {
        if(itemStats->st_mode & S_IXOTH)
        {
            printf(GRN "\t%s", name);
            printf(RESET);
            return;

        }
        printf(RESET "\t%s", name);



    }

    if(S_ISDIR(itemStats->st_mode))
    {
        printf(BLU "\t%s", name);
        printf(RESET);


    }


    if(S_ISLNK(itemStats->st_mode)){
                printf(MAG "\t%s", name);
        printf(RESET);

    }

    readlink(name,buff,100);

    //check if the file is a symbolic Link
    //if it is a SL print the name
    if(S_ISLNK(itemStats->st_mode))
    {
            /* Print it. */
            printf (" -> %s", buff);

    }

}

char* getAbsolutePath(char *path)
{
    /*This file will handle the case on local path and convert it to absolute path
     *
     *
     * */
    char *newPath  = malloc( sizeof(char) * ( 255 ) ); //no need to free at the end of the program, using the gcc standard it is free automaticlly
    char buff[255];
    if(path[0]=='/')
    {
        return path;
    }
    newPath = getcwd(buff,255);
    strcat(newPath,"/");
    strcat(newPath,path);
    return newPath;

}

int main(int argc, char*argv[]) {

    int i = 0;
    int argsSize = argc;

    DIR *currentDir;

    struct dirent *myCurrentArg;
    struct stat myCurrentStat;

    //handle the case of zero arguments to the program
    getcwd(argv[0],255);

    char *path = argv[0];
    char backupStr[1024];


    //nice "trick" because we put the currentWorkingDirectory at argv[0] if the user enters args it will make the loop start from 1, meaning these args
    if(argc > 1)
    {
        i=1;
    }

    //loop over all args

    for(;i<argsSize;i++) {
        path = getAbsolutePath(argv[i]);
        strcpy(backupStr,path);
        //put check error condition file not found and such.
        char buf[1000] = {0};

        if(argc > 1)
        {

          printf("\nThe Output of %s is:\n\n",backupStr);

        }

        currentDir = opendir(path);

        //print folders contents and of her files content
        if(currentDir!=NULL)
        {
            while ((myCurrentArg = readdir(currentDir)) != NULL) {
                sprintf(buf, "%s/%s", argv[i], myCurrentArg->d_name);
                lstat(buf, &myCurrentStat);
                printFileType(myCurrentStat);
                printPermissions(myCurrentStat);
                printf("\t%d", myCurrentStat.st_nlink);
                printf("\t%s", getpwuid(myCurrentStat.st_uid)->pw_name);
                printf("\t%s", getgrgid(myCurrentStat.st_gid)->gr_name);
                printf("\t%lld ", myCurrentStat.st_size);
                printMonthDayYear(myCurrentStat);
                printFileNames(myCurrentArg->d_name, &myCurrentStat);
                printf("\n");
            }
            closedir(currentDir);
        }
        else //its a file and not a folder! print file statics and name
        {

            int file=0;
            if((file= open(backupStr,O_RDONLY)) <= -1)
            {
                perror("Sorry cannot open file!\n");
                exit(0);
            }


            struct stat fileStat;
            if(fstat(file,&fileStat) < 0)
            {
                perror("Sorry cannot open file status!\n");
                exit(0);
            }


            fstat(file, &myCurrentStat);
            printFileType(myCurrentStat);
            printPermissions(myCurrentStat);
            printf("\t%d", myCurrentStat.st_nlink);
            printf("\t%s", getpwuid(myCurrentStat.st_uid)->pw_name);
            printf("\t%s", getgrgid(myCurrentStat.st_gid)->gr_name);
            printf("\t%lld ", myCurrentStat.st_size);
            printMonthDayYear(myCurrentStat);
            printFileNames(argv[1], &myCurrentStat);
            printf("\n");
        }
    }
    return 0;
}