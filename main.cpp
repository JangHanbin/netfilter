#include <iostream>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <errno.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

using namespace std;

void getError(string error);
static int callback(struct nfq_q_handle *qhandle, struct nfgenmsg *nfmsg,struct nfq_data *nfa, void *data);

int main(int argc, char *argv[])
{
    struct nfq_handle* handle=nfq_open();

    /*open lib handle*/
    if(!handle)
        getError("error during nfq_open()");

    /*unbinding existing nf_queue handler for AF_INET*/
    if(nfq_unbind_pf(handle,AF_INET)<0)
        getError("error during nfq_unbind_pf()");

    /*binding nfnetlink_queue as nf_queue handler for AF_INET*/
    if(nfq_bind_pf(handle,AF_INET)<0)
        getError("error during nfq_bind_pf()");

    /*binding this socket to queue '0'*/
    struct nfq_q_handle* qhandle=nfq_create_queue(handle,0,&callback,0); //you can give user defined parameter at last parameter. (e.g., nfq_create_queue(handle,0,&callback,&userClass);)
    if(!qhandle)
        getError("error during nfq_create_queue()");

    /*setting copy_packet mode*/

    if(nfq_set_mode(qhandle,NFQNL_COPY_PACKET,0xffff)<0)
        getError("can't set packet_copy mode");

    int fd=nfq_fd(handle);
    int rv=0;
    char buf[4096] __attribute__ ((aligned));


    while (true)
    {
        if((rv=recv(fd,buf,sizeof(buf),0))>=0) //if recv success
            nfq_handle_packet(handle,buf,rv); //call callback method

    }
    return 0;
}

void getError(string error)
{
    perror(error.c_str());
    exit(1);
}

static u_int32_t checkPacket(nfq_data *tb, int &flag)
{
    int id = 0;
    struct nfqnl_msg_packet_hdr *ph;

    ph = nfq_get_msg_packet_hdr(tb);
    if (ph)
        id = ntohl(ph->packet_id);

    uint8_t* data;
    int ret=nfq_get_payload(tb,&data);
    if(ret<=0) //no ip packet
    {
        return id;
    }


// FLAG can set as NF_DROP or NF_ACCEPT
//    flag=NF_DROP;
//    flag=NF_ACCEPT;

    return id;
}

static int callback(nfq_q_handle *qhandle, nfgenmsg *nfmsg, nfq_data *nfa, void *data)
{
    (void)nfmsg;

    int flag=0;
    u_int32_t id = checkPacket(nfa,flag); //call another method
    return nfq_set_verdict(qhandle, id, flag, 0, NULL); //decide Drop or Accept
}

