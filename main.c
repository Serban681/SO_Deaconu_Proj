#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

struct BMPInfoHeader {
    int headerSize;     
    int width;          
    int height;         
    int planes;         
    int bitCount;       
    int compression;    
    int imageSize;      
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    int colorsUsed;     
    int colorsImportant;
};

int main(int argc, char* args[]) {
    int fIn, fOut;
    char buffer[BUFSIZ];

    if(argc != 2) {
        if(argc == 1) {
            perror("Usage ./program <fisier_intrare>");
        }
        else {
            char *err = strcat("Usage ./program ", args[1]);
            perror(err);
        }

        exit(-1);
    }

    if( (fIn = open(args[1],O_RDONLY)) < 0 )
    {
      perror("input file error \n");
    }

    if (read(fIn, buffer, BUFSIZ) == -1) {
        perror("Error reading file header");
        close(fIn);
        return -1;  // Error reading file header
    }

    struct BMPInfoHeader infoHeader;

    ssize_t bytesRead = read(fIn, &infoHeader, sizeof(struct BMPInfoHeader));

    if (bytesRead == -1) {
        perror("Error reading info header");
        close(fIn);
        exit(-1);
    }

    struct stat fileStat;
    fstat(fIn,&fileStat);


    if( (fOut = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0 )
    {
        perror("output file error \n");
    }

    char buffer2[BUFSIZ];
    sprintf(buffer2,"nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s", args[1],infoHeader.height,infoHeader.width, (long long)fileStat.st_size, fileStat.st_uid, ctime(&fileStat.st_mtime), fileStat.st_nlink, (fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");


    if(write(fOut,buffer2,strlen(buffer2))==-1)
    {
        perror("write in file output error");
    }

    close(fOut);
    close(fIn);

    return 0;
}