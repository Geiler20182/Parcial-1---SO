#include "protocol.h"                       // Definicion de constantes para shm
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>

// Pedir la memoria compartida para escritura
void *setSHMtoW(int *fd){
    void *ptr;

    *fd = shm_open(NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (*fd<0){
        perror("Error in shm_open()");
        exit(EXIT_FAILURE);
    }

    ftruncate(*fd, SIZE);
    ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
    return ptr;
}

// Pedir la memoria compartida para lectura
void *setSHMtoR(int *fd){
    void *ptr;

    *fd = shm_open(NAME, O_RDONLY, 0400);
    if (*fd<0){
        perror("Error in shm_open()");
        exit(EXIT_FAILURE);
    }

    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, *fd, 0);
    return ptr;
}

// Cerrar la memoria compartida dependiendo si es lector o escritor
int closeSHM(int fd, void *ptr, int isConsumer){
    munmap(ptr, SIZE);
    close(fd);
    if(isConsumer)
        shm_unlink(NAME);
    return EXIT_SUCCESS;
}

// Filtro para ver si el archivo debe ser copiado o no
int filter(char *str){
    char lstch = str[strlen(str)-1];
    int ans = -1;

    if(lstch=='c' || lstch=='h')
        ans = 1;
    else
        ans = 0;
    return ans;
}

// Obtener los nombres de archivos y ponerlos en la memoria compartida
void setFileNames(char (* files)[NUM_FILES][FNAME_LEN], char *dir){
    struct dirent **fNamelist;
    int nFiles, i, j;

    nFiles = scandir(dir, &fNamelist, NULL, alphasort);
    if (nFiles == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }

    j = 0;
    for(i=0; i<nFiles && j<NUM_FILES; i++){
        if(filter(fNamelist[i]->d_name)){
            strcpy((* files)[j], fNamelist[i]->d_name);
            j++;
        }
        free(fNamelist[i]);
    }
    free(fNamelist);
    exit(EXIT_SUCCESS);
}

// Funcion para obtener el mtime un archivo
time_t get_mtime(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        perror(path);
        exit(1);
    }
    return statbuf.st_mtime;
}

// Chequear si un archivo existe o no
int fileCheck(char *originFile, char *destFile){
    int ans;
    if(!access (destFile, F_OK)){
        if(get_mtime(destFile) < get_mtime(originFile))
            ans = 1;
        else
            ans = 0;
    }
    else
        ans = 1;
    return ans;
}

// Funcion para copia de archivos usando comando de terminal cp
int copyFile(char *originFile, char *destFile){
    char *command = (char *)malloc(2*FNAME_LEN*sizeof(char));
    strcpy(command, "cp ");
    strcat(command, originFile);
    strcat(command, " ");
    strcat(command, destFile);
    return system(command);
}

// Funcion de guía para el programa
void usage(char *program){
    printf("Usage: %s <origin path> <destiny path>\n", program);
}

int main(int argc, char *argv[]){
    if(argc!=3){
        usage(argv[0]);
        return -1;
    }

    int mfd, isConsumer;
    char (* files)[NUM_FILES][FNAME_LEN];
    if (fork()==0){
        printf("Waiting for file names...\n");
        sleep(1);

        int i = 0;
        char *originFile, *destFile;

        isConsumer = 1;
        files = (char (*)[NUM_FILES][FNAME_LEN])setSHMtoR(&mfd);
        originFile = (char *)malloc(FNAME_LEN*sizeof(char));
        destFile = (char *)malloc(FNAME_LEN*sizeof(char));

        printf("== COPYING FILES ==\n");
        while(i < NUM_FILES){
            if(strcmp((* files)[i], "")){
                strcpy(originFile, argv[1]);
                strcat(originFile, (* files)[i]);
                strcpy(destFile, argv[2]);
                strcat(destFile, (* files)[i]);
                if(fileCheck(originFile, destFile)){
                    copyFile(originFile, destFile);
                    printf("%s updated!!\n", (* files)[i]);
                }
                else
                    printf("%s passed\n", (* files)[i]);
            }
            else
                i = NUM_FILES;
            i++;
        }
    }
    else{
        isConsumer = 0;
        files = (char (*)[NUM_FILES][FNAME_LEN])setSHMtoW(&mfd);

        printf("Files name array address: %p\n", files);
        printf("== FILE NAMES READY ==\n");
        setFileNames(files, argv[1]);
    }
    return closeSHM(mfd, (void *) files, isConsumer);
}
