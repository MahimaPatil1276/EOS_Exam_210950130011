#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<errno.h>

#define DEBUG 1

#define MAX 31

#define MAX_MSGS   500000




int main()
{
    
    
    int fdp[2];
    int fdp1[2];
    
    pipe(fdp1);
    pipe(fdp);
    
    int fd;
    char **arr;

    FILE* fptr=fopen("dictionary.txt","r");

    size_t len=0;
    size_t total_strings=0;
    
    arr=malloc(MAX_MSGS*sizeof(char*));

    FILE* fptr1=fopen("test.txt","w+");
    
    while(1)
    {
        arr[total_strings]=(char*)malloc(sizeof(char)*21);
        if(fgets(arr[total_strings],20,fptr)!=NULL)
        {
            len=strlen(arr[total_strings]);
            arr[total_strings][len-1]='\0';
            total_strings++;
        }
        else{
            break;
        }
    }

    pid_t id1=fork();

    if(id1==0)   //1st child
    {
        close(fdp[1]);
        char search_key[30];
        size_t search_key_len;
        memset(search_key,'\0',30);
        search_key_len=0;
        read(fdp[0],&search_key_len,sizeof(long unsigned int));
        read(fdp[0],search_key,search_key_len);

        close(fdp[0]);

        char buffer[30];
        ssize_t nread=0;
        size_t j=0;
        memset(buffer,'\0',30);
        

        for (size_t i = 0; i < total_strings; i++)
        {
            fprintf(fptr1,"comparing %s and %s in child 1\n",search_key,arr[i]);
            if(strncmp(arr[i],search_key,search_key_len) == 0)
            {
                fprintf(stdout,"FOUND by child1\n");
                return 1;
            }
        }
        
        fprintf(stdout,"NOT FOUND in child1\n");
        
        return 1;
    }
    else{

        pid_t id2=fork();

        if(id2 == 0)//2nd child
        {

            int fd;
        
            char buffer[30];
            ssize_t nread;
            
            char search_key[30];
            size_t search_key_len=0;
            memset(search_key,'\0',30);

            read(fdp1[0],&search_key_len,sizeof(long unsigned int));

            read(fdp1[0],search_key,search_key_len);

            close(fdp1[0]);

            for (size_t i = total_strings-1 ; i > 0 ; i--)
            {
                fprintf(fptr1,"comparing %s and %s in child 2\n",search_key,arr[i]);
                if(strncmp(arr[i],search_key,search_key_len) == 0)
                {

                    fprintf(stdout,"FOUND by child2\n");
                    return 1;
                }
            }
            fprintf(stdout,"NOT FOUND in child2\n");

        }
        else{//parent
            
            char ch;
            
            size_t search_key_len;
            ssize_t nwrite;
            char buffer[30];

            pid_t wid;
            int wstatus;
            

            printf("enter a string\n");
            

            fgets(buffer,30,stdin);
            search_key_len=strlen(buffer);
            
            buffer[search_key_len-1]='\0';
            search_key_len-=1;

            nwrite=write(fdp[1],&search_key_len,sizeof(long unsigned int));

            if(nwrite == -1)
            {
                perror("error in write:");
                exit(EXIT_FAILURE);
            }

            nwrite=write(fdp[1],buffer,search_key_len);

            if(nwrite == -1)
            {
                perror("error in write:");
                exit(EXIT_FAILURE);
            }

//----------------------------------------------------------------------------

            
            nwrite=write(fdp1[1],&search_key_len,sizeof(long unsigned int));
            

            if(nwrite == -1)
            {
                perror("error in write:");
                exit(EXIT_FAILURE);
            }

            nwrite=write(fdp1[1],buffer,search_key_len);

            if(nwrite == -1)
            {
                perror("error in write:");
                exit(EXIT_FAILURE);
            }


            

            #if DEBUG
            printf("waiting for childs\n");
            #endif

            while(wait(NULL)>0);
        
            printf("closing file descriptor\n");
            close(fd);
            for (size_t i = 0; i < MAX_MSGS; i++)
            {
                free(arr[i]);
            }
            
        }
        


    }
}