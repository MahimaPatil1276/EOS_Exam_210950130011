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




size_t getline_fd(int fd,char* buffer,size_t max_bytes)
{
    size_t nread;
    size_t total_read;
    char ch;
    for ( total_read = 0; total_read < max_bytes; total_read++)
    {
        nread=read(fd,&ch,1);
        if(nread>0)
        {
            if(ch == '\n' || ch == '\0')
            {
                buffer[total_read]='\0';
                break;
            }
            buffer[total_read]=ch;
        }
        else if(nread == -1)
        {
            perror("error:");
        }
    }
    printf("buffer read %s\n",buffer);
    return total_read;
    
}

int main()
{
    
    int fdp[2];
    int fdp1[2];
    
    pipe(fdp1);
    pipe(fdp);
    
    
    pid_t id1=fork();
    if(id1==0)   //1st child
    {
        int fd;
        fd=open("dictionary.txt",O_RDONLY);
        close(fdp[1]);
        char word[30];
        size_t len;
        memset(word,'\0',30);
        len=0;
        read(fdp[0],&len,sizeof(long unsigned int));
        read(fdp[0],word,len);

        char buffer[30];
        char ch;
        ssize_t nread=0;
        size_t i;
        memset(buffer,'\0',30);

        while(1)
        {
            
            i=0;
            while((nread=read(fd,&ch,1))>0)
            {
                if( ch=='\0')
                {
                    break;
                }
                else if(ch=='\n')
                {
                    buffer[i]='\0';
                    break;
                }
                else{
                    buffer[i]=ch;
                    i++;
                }
            }
            if(nread == -1)
            {
                perror("error while reading:");
            }
            if(i!=0)
            {
                
                
                /*#if DEBUG
                printf("comparing %s and %s in child 1 of length %ld\n",word,buffer,i);
                #endif*/
                
                if(strncmp(word,buffer,i) == 0)
                {
                    printf("compared %s and %s found by child 1 and i is %ld\n",buffer,word,i);
                    close(fd);
                    return 1;
                }
                memset(buffer,'\0',30);
            }
            
            
        }
        printf("not found in child 1\n");
        close(fd);
        return 1;
    }
    else{
        pid_t id2=fork();

        if(id2 == 0)//2nd child
        {

            int fd;
        
            char buffer[30];
            ssize_t nread;
            
            char word[30];
            size_t len=0;
            memset(word,'\0',30);

            read(fdp1[0],&len,sizeof(long unsigned int));

            read(fdp1[0],word,len);

            char block[1024];
            char ch;

            fd=open("dictionary.txt",O_RDONLY);
            if(fd == -1)
            {
                perror("error:");
                exit(EXIT_FAILURE);
            }

            off_t pos = lseek(fd,0,SEEK_END);
            size_t i;
            
            //while (pos>0)
            //{
                pos = lseek(fd,-1025,SEEK_CUR);
                nread=read(fd,block,1024);
                block[nread]='\0';

                char* str=malloc(sizeof(char)*20);
                memset(str,'\0',20);

                char *temp=block;

                while (*temp!='\0')
                {
                    str=strsep(&temp,"\n");
                    if(strlen(str) == len && strncmp(str,word,len)==0)
                    {
                        printf("FOUND\n");
                        break;
                    }
                }
                
                if(nread >0)
                {
                    i+=nread;
                    if(strncmp(str,word,len)==0)
                    {
                        printf("%s FOUND in child 2\n",str);
                    }
                }
                else{
                    printf("nread is %ld\n",nread);
                    
                }
                
                
            //}
            
            close(fd);

        }
        else{
            
            char ch;
            
            size_t len;
            ssize_t nwrite;
            char buffer[30];

            pid_t wid;
            int status;

            printf("enter a string\n");
            

            fgets(buffer,30,stdin);
            len=strlen(buffer);
            
            buffer[len-1]='\0';
            len-=1;

            printf("received string %s\n",buffer);

            nwrite=write(fdp[1],&len,sizeof(long unsigned int));

            if(nwrite == -1)
            {
                perror("error in write:");
                exit(EXIT_FAILURE);
            }

            nwrite=write(fdp[1],buffer,len);

            if(nwrite == -1)
            {
                perror("error in write:");
                exit(EXIT_FAILURE);
            }

//----------------------------------------------------------------------------

            
            nwrite=write(fdp1[1],&len,sizeof(long unsigned int));
            

            if(nwrite == -1)
            {
                perror("error in write:");
                exit(EXIT_FAILURE);
            }

            nwrite=write(fdp1[1],buffer,len);

            if(nwrite == -1)
            {
                perror("error in write:");
                exit(EXIT_FAILURE);
            }

            

            #if DEBUG
            printf("waiting for childs\n");
            #endif

            while(wait(NULL) > 0);
        }
        


    }
}