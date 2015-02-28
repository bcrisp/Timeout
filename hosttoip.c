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
int hostname_to_ip(char *  ,  char *addrs[]);
void sig_handler(int signum);
void printRules();

// IP in global scope, needed for SIGINT cleanup
//char ip[100];
char *addrs[5];

// Entry 
int main(int argc , char *argv[])
{
    printf("program entry");

    if (signal(SIGINT, sig_handler) == SIG_ERR)
	printf("\nCan't catch SIGINT\n");

    if(argc <2)
    {
        printf("Please provide a hostname to resolve");
	exit(1);
    }
     
    char *hostname = argv[1];
    printf("\nCalling hostname to ip");
    hostname_to_ip(hostname , addrs);
    int i = 0;
    for (i = 0; addrs[i] != NULL; i++)
{
    char *buf;
    size_t sz;
    sz = snprintf(NULL, 0, "iptables -A INPUT -p tcp -s %s -m state --state ESTABLISHED,RELATED -j DROP", addrs[i]);
    buf = (char *)malloc(sz+1);
    snprintf(buf,sz+1,"iptables -A INPUT -p tcp -s %s -m state --state ESTABLISHED,RELATED -j DROP", addrs[i]);
 
    printf("%s resolved to %s\n" , hostname , addrs[i]);
    system(buf);

}
    //printf(buf);
   
    printRules(); 
    while(1)
      sleep(1);
    
    return 0;
}

void sig_handler(int signum)
{
  char *rule = "iptables -D INPUT -p tcp -s %s -m state --state ESTABLISHED,RELATED -j DROP";
  if (signum == SIGINT)
  {
    //printf("SIGINT is happening!");
    int i;
    for (i = 0; addrs[i] != NULL; i++)
{

    char *buf;
    size_t sz;
    sz = snprintf(NULL, 0, "iptables -D INPUT -p tcp -s %s -m state --state ESTABLISHED,RELATED -j DROP", addrs[i]);
    buf = (char *)malloc(sz+1);
    snprintf(buf,sz+1,"iptables -D INPUT -p tcp -s %s -m state --state ESTABLISHED,RELATED -j DROP", addrs[i]);

    //printf("\nThe following system call is about to occur: %s\n", buf);
    system(buf);
}   

    printRules(); 
    // exit gracefully
    exit(0);
  }  
}   

// Below code courtesy of http://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/

/*
    Get ip from domain name
 */
 
int hostname_to_ip(char * hostname , char *addrs[])
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
    //char* addresses[2];
     
    for(i = 0; addr_list[i] != NULL; i++) 
    {
	addrs[i] = malloc(100);
	//printf("Raw address list entry: %s", *addr_list[0]);
        //Return the first one;
        //strcpy(ip , inet_ntoa(*addr_list[i]) );
	strcpy(addrs[i], inet_ntoa(*addr_list[i]));
	//printf("addresses %s", addresses[i]);
	//addrs = addresses;
        //return 0;
    }
     
    return 0;
}

void printRules()
{

  FILE *fp;
  char path[1035];

  fp = popen("iptables -L", "r");

  while (fgets(path, sizeof(path)-1,fp) != NULL){
    printf("%s", path);
  }

  pclose(fp);
}
