#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //for exit(0);
#include<sys/socket.h>
#include<errno.h> //For errno - the error number
#include<netdb.h> //hostent
#include<arpa/inet.h>
#include<signal.h>
#include<unistd.h>
 
// Function declarations
int hostname_to_ip(char *  , char *);
void sig_handler(int signum);

// IP in global scope, needed for SIGINT cleanup
char ip[100];

// Entry 
int main(int argc , char *argv[])
{

    if (signal(SIGINT, sig_handler) == SIG_ERR)
	printf("\nCan't catch SIGINT\n");

    if(argc <2)
    {
        printf("Please provide a hostname to resolve");
	exit(1);
    }
     
    char *hostname = argv[1];
    hostname_to_ip(hostname , ip);

    char *buf;
    size_t sz;
    sz = snprintf(NULL, 0, "iptables -A INPUT -p tcp -s %s -m state --state ESTABLISHED -j DROP", ip);
    buf = (char *)malloc(sz+1);
    snprintf(buf,sz+1,"iptables -A INPUT -p tcp -s %s -m state --state ESTABLISHED -j DROP", ip);
 
    printf("%s resolved to %s\n" , hostname , ip);
    system(buf); 
    //printf(buf);
   
    while(1)
      sleep(1);
    
    return 0;
}

void sig_handler(int signum)
{
  if (signum == SIGINT)
  {
    //printf("SIGINT is happening!");

    char *buf;
    size_t sz;
    sz = snprintf(NULL, 0, "iptables -D INPUT -p tcp -s %s -m state --state ESTABLISHED -j DROP", ip);
    buf = (char *)malloc(sz+1);
    snprintf(buf,sz+1,"iptables -D INPUT -p tcp -s %s -m state --state ESTABLISHED -j DROP", ip);

    //printf("\nThe following system call is about to occur: %s\n", buf);
    system(buf);
    
    // exit gracefully
    exit(0);
  }  
}   

// Below code courtesy of http://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/

/*
    Get ip from domain name
 */
 
int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
}
