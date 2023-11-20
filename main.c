#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <stdint.h>

struct Header {
    int16_t signature;
    int32_t fileSize;
    int32_t reserved;
    int32_t dataOffset;
};

struct InfoHeader {
    int32_t size;
    int32_t width;
    int32_t height;
    int16_t planes;
    int16_t bitCount;
    int32_t compression;
    int32_t XpixelsPerM;
    int32_t YpixelsPerM;
    int32_t colorsUsed;
    int32_t colorsImportant;
};

struct ColorTable {
    int8_t red;
    int8_t green;
    int8_t blue;
    int8_t reserved;
};

int fOut;
DIR *dir;
char buffer[BUFSIZ];
struct InfoHeader infoHeader;

void verifyArgs(int argc, char **args) {
    if(argc != 2) {
        if(argc == 1) {
            perror("Usage ./program <director_intrare> <director_iesire>");
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
    fstat(*info->d_name, &fileStat);

    snprintf(buffer,sizeof(buffer), "nume director: %s\nidentificatorul utilizatorului: %s\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n", info->d_name,info->d_name, (fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    write(fOut, buffer,strlen(buffer));
}

void generateStatsBmpFile(struct dirent *info) {
    struct Header header;
    struct InfoHeader infoHeader;
    
    read(*info->d_name, &header, sizeof(struct Header));
    read(*info->d_name, &infoHeader, sizeof(struct InfoHeader));

    struct stat fileStat;
    fstat(*info->d_name, &fileStat);

    sprintf(buffer, "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n", info->d_name, infoHeader.height, infoHeader.width, (long long)fileStat.st_size, fileStat.st_uid, ctime(&fileStat.st_mtime), fileStat.st_nlink, (fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    write(fOut,buffer,strlen(buffer));
}

void generateStatsRegFile(struct dirent *info) {
    struct stat fileStat;
    fstat(*info->d_name, &fileStat);

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

            
        }

    }
}


int main(int argc, char* args[]) {
    verifyArgs(argc, args);

    openDir(args[1], argc, args);
    
    close(fOut);
    closedir(dir);
    return 0;
}