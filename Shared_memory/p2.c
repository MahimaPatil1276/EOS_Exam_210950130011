#include <unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>  



char *data;
int main(int argc, char const *argv[])
{
    int smd;
    char *data;

    smd = shm_open("/cdacshm", O_RDWR |O_CREAT,S_IRUSR |S_IWUSR); 
    ftruncate(smd,10*sizeof(char));
    data = (char *)mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, smd,0); 

   printf("Data is: %c\n", *data);
    return 0;
}