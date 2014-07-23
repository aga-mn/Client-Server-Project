
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <signal.h>
#include <sys/time.h>
#define MAX_BUFFER 1024
#define DAYTIME_SERVER_PORT 13
volatile int mark = 0;

int cli_init_conecctionTCP(int Port, char *IPadress);



int connector(int sock,char *message) {
 char server_reply[2000];
 //while(1)    {
        //printf("Enter message : ");
        //scanf("%s" , message);

        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
          //  break;
        }

        puts("Server reply :");
        puts(server_reply);
   // }

    //close(sock);
return 0;
}

///pobieranie procesow start
 
////listy procesów koniec start
char * get_command_line(char *pid) {
 FILE *f;
 char file[256], cmdline[256] = {0};
 sprintf(file, "/proc/%s/cmdline", pid);
char *out;
 f = fopen(file, "r");
 if (f) {
  //  char *p = cmdline;
    fgets(cmdline, sizeof(cmdline) / sizeof(*cmdline), f);
    fclose(f);

//printf("cmdline %s",p) ;
out=cmdline;
  
}


return out;
}

char * get_full_process_name(char *pid) {
 size_t linknamelen;
 char file[256], cmdline[256] = {0};
char *out;

 sprintf(file, "/proc/%s/exe", pid);
 linknamelen = readlink(file, cmdline, sizeof(cmdline) / sizeof(*cmdline) - 1);
 cmdline[linknamelen + 1] = 0;
out=cmdline;
 //printf("Full name is %s %s\n", file,cmdline);

return out;

}

int getProcessID(char *p_processname,char *out_proc_str) {
        	DIR *dir_p;
        	struct dirent *dir_entry_p;
        	char gfpn[100];
int count=0;
char buff[MAX_BUFFER];
char buff_end[MAX_BUFFER];
char buff1[256];
char c_get_full_process_name[100];
char c_get_command_line[100];
  
        	dir_p = opendir("/proc/"); 															
        	while(NULL != (dir_entry_p = readdir(dir_p))) {											
        		if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) {		
        		
//count:9 5:proc1:proc13.exe 13:proc2:proc13.exe         		
        		
        		strcpy(gfpn,get_full_process_name(dir_entry_p->d_name));
        		
        		if(strstr(gfpn,p_processname))	{
        			count++;
        		
   	strcpy(c_get_full_process_name,get_full_process_name(dir_entry_p->d_name));
	strcpy(c_get_command_line,get_command_line(dir_entry_p->d_name));
 
  sprintf(buff1,"%s:%s:%s ",dir_entry_p->d_name,c_get_full_process_name,c_get_command_line);

			
		
				 strcat(buff, buff1);	
				 
				//	printf("\n2 %s %d\n",buff,strlen(buff)); 
					}
        				
        			}
        		}
        	
        	closedir(dir_p);
   //  printf("%s",buff);
     //strcat(buff," "); 
      sprintf(buff_end,"count:%d %s",count,buff);	
  // printf("%s %d",buff_end,strlen(buff_end));
     	 strcpy(out_proc_str, buff_end);
      printf("%s %d",out_proc_str,strlen(out_proc_str));
      	//printf("inproccess %s",buff);	
      
     return count; 
        }




///pobieranie procesow koniec










void getthisipadress(char *thisip) {
	
 int fd;
 struct ifreq ifr;

 fd = socket(AF_INET, SOCK_DGRAM, 0);

 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

 /* I want IP address attached to "eth0" */
 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

 close(fd);

 /* display result */
 //printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
strcpy(thisip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));




	}







void help()
{
	printf("Usage:\n");
    printf("-a ipadress -a localhost \n");
    printf("-p port -p 8080\n");
	printf("-r rejestracja \n");
	printf("-i inicjalizacja -i python \n");
	printf("-c check  -c python3:\n");
	printf("-s auto check  -s 10:\n");
	
	
	exit(0);
}

int cli_init_conecctionTCP(int Port, char *IPadress) {

int connectionFd;
struct sockaddr_in servaddr;
  
//#Port=DAYTIME_SERVER_PORT;

  connectionFd = socket(PF_INET, SOCK_STREAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(Port);

  servaddr.sin_addr.s_addr = inet_addr(IPadress);

  if((connect(connectionFd, (struct sockaddr *)&servaddr, 
           sizeof(servaddr)))==-1)
  {
    fprintf(stderr,"Polaczenie nie powiodlo sie: %d\n",errno);
    exit(0);
    
  }

	
return connectionFd;	
	}



void check_proc (int i)
{
  struct itimerval tout_val;
  
  tout_val.it_interval.tv_sec = 0;
  tout_val.it_interval.tv_usec = 0;
  tout_val.it_value.tv_sec = i; /* set timer for "INTERVAL (10) seconds */
  tout_val.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &tout_val,0);
 
 if(!mark)
 mark = 1;
 else
 mark = 0;
 
   signal(SIGALRM,check_proc);
 
 
}


int main (int argc, char *argv[] )
{
 		struct itimerval tout_val;
 	char out[MAX_BUFFER];
  int port=0;
  char ip_adress[20];
  char proc_name[20];
  
  		char my_ip[20];
		char my_message[MAX_BUFFER];
   strcpy(ip_adress,"null");
   strcpy(proc_name,"python3");
   int what=0;
char *avalue = NULL;
 int i;
 int connectionFd;
/* Parsujemy argumenty podane w wierszu polecen. */
	while((i = getopt(argc, argv, "p:a:i:c:s:rh")) != -1)
	{
		switch (i)
		{
		
		case 'p': /* Port . */
			port = atoi(optarg);
			break;
		
		case 'a': /* IP. */
		avalue=optarg;
		
		  strcpy(ip_adress,avalue);
		  break;
			case 'r': /* Rejestracja. */
		  what=1;
			
			break;
					case 'i': /* init. */
		  avalue=optarg;

		  strcpy(proc_name,avalue);

		  what=2;
			
			break;	
			
					case 'c': /* check */
			  avalue=optarg;
	
		  strcpy(proc_name,avalue);
		
		  what=3;
			
			break;	
			
						case 's': /* auto pilnowanie. */
								  avalue=optarg;
	
		  strcpy(proc_name,avalue);	
						
		  what=4;
			
			break;
			
			case 'h': /* auto pilnowanie. */
			help();
			
			break;	
					
			
			
		default: /* Wyswietlamy instrukcje obslugi. */
		
			help();
			
			break;
			
		}
	}

if(strcmp(ip_adress,"null")==0)
{
printf("Run without option -a <ipadress>");	
exit(0);
	}

if(port==0)
{
printf("Run without option -p <port>");	
exit(0);
	}



printf("Connection to ip %s on port %d\n\n",ip_adress,port);

connectionFd=cli_init_conecctionTCP(port,ip_adress);




switch (what)
		{
		
		case 1: /* REJESTRACJA */
		;


        getthisipadress(my_ip);
        sprintf(my_message, "REGISTER %s",my_ip); 
        printf("Send message:\n %s\n",my_message);
		connector(connectionFd,my_message); 
		connector(connectionFd,"QUIT "); 
		 close(connectionFd);
		break;
		case 2: /* Inicjalizacja listy procesow do sprawdzania */
		
		if(  getProcessID(proc_name,out)>0) {;
		
        sprintf(my_message, "PROCESS_LIST_INIT %s",out); 
      //  PROCESS_LIST_INIT count:9
        printf("Send message:\n %s\n",my_message);
		connector(connectionFd,my_message); 
				connector(connectionFd,"QUIT "); 
			 close(connectionFd);
	}
	else {
		printf("Brak procesow do rejestracji\n");
		
	}
		break;
        case 3: /* sprawdzanie czy to co jest na serwerz zgadz sie z tym co jest na kliencie. */

			if(  getProcessID(proc_name,out)>0) {;
		
        sprintf(my_message, "CHECK_PROCESS %s",out); 

        printf("Send message:\n %s\n",my_message);
		connector(connectionFd,my_message); 
		connector(connectionFd,"QUIT "); 
		close(connectionFd);
	}
	else {
		printf("Brak procesow do checku\n");
		
	}
					break;
       case 4: /* symulator. */
	
 // #INTERVAL 10
  tout_val.it_interval.tv_sec = 0;
  tout_val.it_interval.tv_usec = 0;
  tout_val.it_value.tv_sec = atoi(proc_name); /* set timer for "INTERVAL (10) seconds */
  tout_val.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &tout_val,0);

  signal(SIGALRM,check_proc); /* set the Alarm signal capture */
  
  while (1)
  {
 
    
 if(!mark) {      
          //      printf("Waiting\n");
            ;
        }
        else
        {

	strcpy(out,"");
		strcpy(my_message,"");
	if(  getProcessID("python",out)>0) {;
		
        sprintf(my_message, "CHECK_PROCESS %s",out); 

        printf("Send message:\n %s\n",my_message);
		connector(connectionFd,my_message); 
			// close(connectionFd);
	
	}
	else {
		printf("Brak procesow do checku\n");
		
	}
	    mark=0;		
		}
        
        
    ;
  }
						break;
				default: /* Wyswietlamy instrukcje obslugi. */
					 printf("Unknown Option\n");
				break;
		}
return 0;
}


