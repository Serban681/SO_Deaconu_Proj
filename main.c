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
#include <sys/wait.h>
#include <ctype.h>

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
DIR *outDir;
char buffer[3000];
struct InfoHeader infoHeader;
int procesFiu, status;
int pid[100];

int pipeFF[100][2];

void verif_pipes(int i) {
    if(pipe(pipeFF[i]) < 0) {
        perror("Eroare la pipe-uri.\n");
        exit(-1);
    }
}

void createFile(char *buffer, char *fileName, char *dirName)
{
    char outFileName[500] = "";
    char *p = strtok(fileName, ".");
    strcat(outFileName, p);
    strcat(outFileName, "_statistica");
    strcat(outFileName, ".txt");

    char pathName[500] = "./";
    strcat(pathName, dirName);
    strcat(pathName, "/");
    strcat(pathName, outFileName);

    int fp;

    fp = open(pathName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    
    write(fp,buffer,strlen(buffer));

    close(fp);
}

void verifyArgs(int argc, char **args) {
    if(argc != 4) {
        if(argc == 3 || argc == 2 || argc == 1) {
            perror("Usage ./program <director_intrare> <director_iesire> <c>");
        }
        else {
            char *err = strcat("Usage ./program ", args[1]);
            perror(err);
        }

        exit(-1);
    }
}

void verifyCorrectSentences(char* text, char* letter) {
    const char *scriptName = "script.sh";

    char command[100];
    snprintf(command, sizeof(command), 
        "bash %s %s\n", 
        scriptName, 
        letter
    );

    FILE *scriptInput = popen(command, "w");
    if (scriptInput == NULL) {
        perror("Error opening script");
        exit(EXIT_FAILURE);
    }

    fprintf(scriptInput, text);

    if (pclose(scriptInput) == -1) {
        perror("Error closing script");
        exit(EXIT_FAILURE);
    }
}

void generateStatsDir(struct dirent *info, char* dirName) {
    struct stat fileStat;
    fstat(*info->d_name, &fileStat);

    snprintf(buffer,sizeof(buffer), "nume director: %s\nidentificatorul utilizatorului: %s\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n", info->d_name,info->d_name, (fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    
    createFile(buffer, info->d_name, dirName);
}

void generateStatsBmpFile(struct dirent *info, char* dirName) {
    struct Header header;
    struct InfoHeader infoHeader;
    struct ColorTable colorTable;
    
    read(*info->d_name, &header, sizeof(struct Header));
    read(*info->d_name, &infoHeader, sizeof(struct InfoHeader));
    read(*info->d_name, &colorTable, sizeof(struct ColorTable));

    colorTable.red = colorTable.red * 0.299;
    colorTable.blue = colorTable.blue * 0.114;
    colorTable.green = colorTable.green * 0.587;

    struct stat fileStat;
    fstat(*info->d_name, &fileStat);

    sprintf(buffer,
        "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n", 
        info->d_name, 
        infoHeader.height, 
        infoHeader.width, 
        (long long)fileStat.st_size, 
        fileStat.st_uid, 
        ctime(&fileStat.st_mtime), 
        fileStat.st_nlink, 
        (fileStat.st_mode & S_IRUSR) ? "R" : "-", 
        (fileStat.st_mode & S_IWUSR) ? "W" : "-", 
        (fileStat.st_mode & S_IXUSR) ? "X" : "-", 
        (fileStat.st_mode & S_IRGRP) ? "R" : "-", 
        (fileStat.st_mode & S_IWGRP) ? "W" : "-", 
        (fileStat.st_mode & S_IXGRP) ? "X" : "-", 
        (fileStat.st_mode & S_IROTH) ? "R" : "-", 
        (fileStat.st_mode & S_IWOTH) ? "W" : "-", 
        (fileStat.st_mode & S_IXOTH) ? "X" : "-");


    createFile(buffer, info->d_name, dirName);    
}

void secondProccess(int i, char* letter) {
    if (pid[i + 50] == 0) {
        close(pipeFF[1]);
    
        char buffer[1024];
        ssize_t bytesRead = read(pipeFF[i+50][0], buffer, sizeof(buffer));
        if (bytesRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        buffer[bytesRead] = '\0';

        close(pipeFF[0]);

        verifyCorrectSentences(buffer, letter);

        exit(1);
    }
}

void generateStatsRegFile(struct dirent *info, char* dirName, char* inDir, int i, char* letter) {
    struct stat fileStat;
    fstat(*info->d_name, &fileStat);

    sprintf(buffer,"nume fisier: %s\ndimensiune: %lld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n", info->d_name, (long long)fileStat.st_size, fileStat.st_uid, ctime(&fileStat.st_mtime), fileStat.st_nlink, (fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    createFile(buffer, info->d_name, dirName);

    char filePath[100] = "";
    strcat(filePath, inDir);
    strcat(filePath, "/");
    strcat(filePath, info->d_name);
    strcat(filePath, ".txt");

    int fileDescriptor = open(filePath, O_RDONLY);
    if (fileDescriptor == -1) {
        perror("Error opening file");
        exit(-1);
    }

    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) { }

    verifyCorrectSentences(buffer, letter);

    if (pid[i] == 0) {
        close(pipeFF[i][0]);

        const char *message = "Hello, child process!";

        ssize_t bytesWritten = write(pipeFF[i][1], message, strlen(message));
        if (bytesWritten == -1) {
            perror("write");
            exit(-1);
        }

        close(pipeFF[i][1]);

        exit(1);
    }
    
}

void generateStatsLnk(struct dirent *info, char* dirName) {
    struct stat fileStat;
    stat(info->d_name, &fileStat);

    sprintf(buffer, "nume legatura: %s\ndimensiune: %lld\ndimensiune fisier: %lld \n \ndrepturi de acces user: %s%s%s\ndrepturi de acces grup: %s%s%s\ndrepturi de acces altii: %s%s%s\n\n",
                     info->d_name, (long long)fileStat.st_size, (long long)fileStat.st_size,(fileStat.st_mode & S_IRUSR) ? "R" : "-", (fileStat.st_mode & S_IWUSR) ? "W" : "-", (fileStat.st_mode & S_IXUSR) ? "X" : "-", (fileStat.st_mode & S_IRGRP) ? "R" : "-", (fileStat.st_mode & S_IWGRP) ? "W" : "-", (fileStat.st_mode & S_IXGRP) ? "X" : "-", (fileStat.st_mode & S_IROTH) ? "R" : "-", (fileStat.st_mode & S_IWOTH) ? "W" : "-", (fileStat.st_mode & S_IXOTH) ? "X" : "-");
    createFile(buffer, info->d_name, dirName); 
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

    int i = 0;

    while ((info = readdir(dir)) != NULL) {    
        struct stat fileInfo;
        stat(info->d_name, &fileInfo);
        if(S_ISREG(fileInfo.st_mode) == 0)
        {
            int value = 0;
            if((pid[i] = fork()) < 0){
                perror("fork error\n");
                exit(-1);
            }     
            if(pid[i] == 0){
                if (info->d_type == DT_REG) {
                    const char *extension = strrchr(info->d_name, '.');
                    if (extension != NULL && strcmp(extension, ".bmp") == 0) {
                        generateStatsBmpFile(info, argv[2]);
                        value = 10;
                    } else if(S_ISLNK(fileInfo.st_mode)){
                        generateStatsLnk(info, argv[2]);
                    }
                    else {
                        verif_pipes(i);

                        generateStatsRegFile(info, argv[2], argv[1], i, argv[3]);

                        if ((pid[i+50] = fork()) == -1) {
                            perror("fork");
                            exit(EXIT_FAILURE);
                        }
                        
                        secondProccess(i, argv[3]);
                        
                        value = 8;
                    }
                }
                else if (info->d_type == DT_DIR) {
                    generateStatsDir(info, argv[2]);
                    value=5;
                } else {
                    continue;
                }

                exit(value);
            } 
        }

        

        close(pipeFF[i][0]);
        close(pipeFF[i][1]);

        wait(NULL);
        wait(NULL);

        i++;
    }

    for(int j=0; j<i; j++) {
        int fiu = wait(&status);
        sprintf(buffer, "Procesul cu pid-ul %d a returnat %d.\n", fiu, WEXITSTATUS(status));
        write(fOut, buffer, sizeof(buffer));
        sprintf(buffer, "");
    }
}


int main(int argc, char* args[]) {
    verifyArgs(argc, args);

    openDir(args[1], argc, args);
    
    close(fOut);
    closedir(dir);

    return 0;
}