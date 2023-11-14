#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

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

int fOut;
DIR *dir;
char buffer[BUFSIZ];
struct BMPInfoHeader infoHeader;


void verifyArgs(int argc, char **args) {
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
}

void generateStatsDir(struct dirent *info) {
    struct stat fileStat;
    fstat(info->d_name, &fileStat);

    snprintf(buffer,sizeof(buffer),"nume director: %s\nidentificatorul utilizatorului: %s\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n", info->d_name,info->d_name, (fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    write(fOut,buffer,strlen(buffer));
}

void generateStatsBmpFile(struct dirent *info) {
    struct BMPInfoHeader infoHeader;
    ssize_t bytesRead = read(info->d_name, &infoHeader, sizeof(struct BMPInfoHeader));
    
    if (bytesRead == -1) {
        perror("Error reading info header");
        close(info->d_name);
        exit(-1);
    }

    struct stat fileStat;
    fstat(info->d_name, &fileStat);

    sprintf(buffer, "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s", info->d_name, infoHeader.height, infoHeader.width, (long long)fileStat.st_size, fileStat.st_uid, ctime(&fileStat.st_mtime), fileStat.st_nlink, (fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    write(fOut,buffer,strlen(buffer));
}

void generateStatsRegFile(struct dirent *info) {
    struct stat fileStat;
    fstat(info->d_name, &fileStat);

    sprintf(buffer,"nume fisier: %s\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n", info->d_name, (long long)fileStat.st_size, fileStat.st_uid, ctime(&fileStat.st_mtime), fileStat.st_nlink, (fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    write(fOut,buffer,strlen(buffer));
}

void generateStatsLnk(struct dirent *info) {
    struct stat fileStat;
    stat(info->d_name, &fileStat);

    snprintf(buffer, sizeof(buffer), "nume legatura: %s\ndimensiune: %lld\ndimensiune fisier: %lld \n \ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n",
                     info->d_name, (long long)fileStat.st_size, (long long)fileStat.st_size,(fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    write(fOut,buffer,strlen(buffer));
}

void openDir(const char *dirName,int argc, char *argv[]) {
    DIR *dir;
    struct dirent *info;
    
    dir = opendir(dirName);

    
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    
     if( (fOut = open("statistica.txt",O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0 )
    {
      perror("output file error \n");
      exit(-1);
    }

    while ((info = readdir(dir)) != NULL) {
        struct stat fileInfo;
        stat(info->d_name, &fileInfo);
        if(S_ISREG(fileInfo.st_mode) == 0)
        {
            if (info->d_type == DT_REG) {
                const char *extension = strrchr(info->d_name, '.');
                if (extension != NULL && strcmp(extension, ".bmp") == 0) {
                    generateStatsBmpFile(info);
                } else if(S_ISLNK(fileInfo.st_mode)){
                    generateStatsLnk(info);
                }
                else {
                    generateStatsRegFile(info);
                } 
            }
        
            else if (info->d_type == DT_DIR) {
                generateStatsDir(info);
                printf("Directory: %s\n", info->d_name);
            } else {
                continue;
            }

            close(fOut);
            closedir(dir);
        }

    }
}


int main(int argc, char* args[]) {
    verifyArgs(argc, args);

    openDir(args[1], argc, args);
    // generatestatisticsFile(args);

    
    return 0;
}