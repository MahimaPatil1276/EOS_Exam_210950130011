#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<mqueue.h>

mqd_t cdacmq;
struct mq_attr cdacmq_attr;
char buff[128];
int msg_prio;

int main(int argc, char const *argv[])
{
    cdacmq_attr.mq_flags = 0;  //message quene flags
    cdacmq_attr.mq_maxmsg = 4;  //maximum number of message
    cdacmq_attr.mq_msgsize = 128;  //maximum message size
    cdacmq_attr.mq_curmsgs = 0;   //number of message currently queued

    cdacmq = mq_open("/cdacmq", O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, &cdacmq_attr);

    mq_receive(cdacmq, buff, 128, &msg_prio);
    printf("Received msg: %s\nMsg prio: %d\n", buff, msg_prio);

    mq_close(cdacmq);
    return 0;
}