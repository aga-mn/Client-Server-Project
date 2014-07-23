#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_BUFFER		1024
#define MAX_BACKLOG		5
#define MSGSZ     128

int MAXPROC;
/** Zmienna pobierana z pliku cofig.conf i jeśli ma wartość 1 to logowanie odbywa sie na ekran i do pliku, a jeśli 0 to tylko do pliku */
int VDEBUG=1;
/** Zmienna przechowuje nazwe pliku logu*/
char LOG_NAME[150];
/** Port, na którym dziala serwer, zmienna pobierana z pliku config.conf */
int SERV_PORT;

void read_file_and_add_to_list(char *file_name);
int buffer_choper(char *ip_adress,char *in_buff,char *output) ;
void REGISTER(char *file_name, char *out);
void PROCESS_LIST_INIT(char *ip_adress,char arg_list[MAXPROC][300],int count,char * out);
void CHECK_PROCESS(char *ip_adress,char arg_list[MAXPROC][300],int count,char * out);
void HELP(char *ip_adress, char *out);
int delete_from_list(char * ip_adress);
/** Typ zmiennej przechowującej dane o procesie*/
typedef struct
    {
        char  pid[100];
        char command_line[100];
        char full_process_name[100];
    } proc_value;

/** Struktura wykorzystywana jako wezel listy. Zawiera IP klienta oraz tablice z procesami klienta */
  struct client
    {
        char ip_adress[20];
        int proc_count;
        proc_value *proc_tab;

       struct client *next;
    } ;
    
/** Struktura do kolejek */  
typedef struct msgbuf {
         long    mtype;
         char    mtext[MSGSZ];
         } message_buf;    
    

/** Wskaźnik na początkowy element listy */
struct client *head = NULL;
/** Wskaźnik na bieżący element listy */
struct client *curr = NULL;

/**Kolejki obsługa */


void qmessage_get(char out_b[])
{

  int msqid;
    key_t key;
    message_buf  rbuf;

    /*
     * Get the message queue id for the
     * "name" 1234, which was created by
     * the server.
     */
    key = 1234;

    if ((msqid = msgget(key, 0666)) < 0) {
 			strcpy(out_b,"Queueempty");
 //perror("msgget");
       
     //   exit(1);
    }

    
    /*
     * Receive an answer of message type 1.
     */
    if (msgrcv(msqid, &rbuf, MSGSZ, 1, IPC_NOWAIT) < 0) {
          	if(errno == ENOMSG)
			strcpy(out_b,"Queueempty");
			
    } else
    strcpy(out_b,rbuf.mtext);
			



    	
	
}	

void qdelete()
{

  int msqid;
    key_t key;


    /*
     * Get the message queue id for the
     * "name" 1234, which was created by
     * the server.
     */
    key =1234;

    if ((msqid = msgget(key, 0666)) < 0) {
 perror("Queue not exists");
       
    }

msgctl(msqid, IPC_RMID, NULL);
    
  
	
}	



void qmessage_sent(char comm[])
{
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    message_buf sbuf;
    size_t buf_length;

    /*
     * Get the message queue id for the
     * "name" 1234, which was created by
     * the server.
     */
    key = 1234;

    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget error");
        exit(1);
    }

    sbuf.mtype = 1;


 strcpy(sbuf.mtext,comm);
    
 
    
    buf_length = strlen(sbuf.mtext) + 1 ;
    
     /*
     * Send a message.
     */
    if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
    //   printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);

    }
}




void add_cli_from_qq() {
	
char ip_adress[50];
qmessage_get(ip_adress);	
//printf("%s\n",ip_adress);

if(strcmp("Queueempty",ip_adress)!=0)
{
//printf("Pob z kolejki\n");
delete_from_list(ip_adress);
read_file_and_add_to_list(ip_adress);
}

}	




/** Rozdziela na elementy (PID, nazwa, ścieżka procesu) informacje o procesach przyslaną przez klienta */
int split(char* str, char splitstr[MAXPROC][300])
{
   char* p;
   int i=0;
   p = strtok(str,":");
   while(p) {
   strcpy(splitstr[i++], p);
       p = strtok (NULL, ":");
   }
   return i;
}

/** Usuwa ostatni znak z ciągu przekazanego jako parametr*/
char* LastcharDel(char* name)
{
 int i = 0;
    while(name[i] != '\0')
    {
        i++;
    }
    name[i-1] = '\0';

    return name;
}

/** Zapisuje aktualny czas do zmiennej podanej w parametrze */
void current_time(char *ccurrent_time) {

   time_t current_time;
   struct tm *info;

   time( &current_time );

   info = localtime( &current_time );

   strftime(ccurrent_time,80,"%Y%m%d%H%M%S", info);

	}

/** Zapisuje logi do pliku. W momencie uruchomienia serwera tworzy sie plik logu w katalogu log.
* W nazwie zawiera date i czas uruchomienia serwera.
* W pliku zapisywane są wszystkie ważne informacje związane z dzialaniem serwera */

int log_on(char *in_buff,int debug)
{
int all_in_buff_len=0;
char all_in_buff[2*MAX_BUFFER];
char tim[80];
current_time(tim);

    int filedesc = open(LOG_NAME, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
        if(filedesc < 0)
            return 1;

            sprintf(all_in_buff, "%s:%s\n",tim,in_buff);

            all_in_buff_len=strlen(all_in_buff);


        if(write(filedesc,all_in_buff, all_in_buff_len) != all_in_buff_len)
            {
                perror("log_on[write] error");
            }

        if (debug)
		  printf("%s",all_in_buff);

	    close( filedesc );
        return 0;
}

/** Obsluguje polączenie - odbiera i wysyla dane do klienta*/
void doprocessing (int sock ,char *ipadress)
{
    int n;
    char buffer[MAX_BUFFER];
    char debug_tmp[2*MAX_BUFFER];
    char out_send[MAX_BUFFER];

    while (strcmp(out_send,"QUIT")!=0) {

    bzero(out_send,MAX_BUFFER);
    bzero(buffer,MAX_BUFFER);

    n = read(sock,buffer,MAX_BUFFER);

    sprintf(debug_tmp,"doprocessing(%s) read %s [ %d ]",ipadress,buffer,n);
    log_on(debug_tmp,VDEBUG);

if(n>0) {

    buffer_choper(ipadress,buffer,out_send);
    bzero(buffer,MAX_BUFFER);
   // printf("out_send %d\n",strlen(out_send));
    strcpy(buffer,out_send);


    sprintf(debug_tmp,"doprocessing(%s) write %s [ %d ]",ipadress,buffer,strlen(buffer));
    log_on(debug_tmp,VDEBUG);
    n = write(sock,buffer,MAX_BUFFER);

    }

    if (n <= 0)
    {
        sprintf(debug_tmp,"doprocessing(%s) client abort %s [ %d ]",ipadress,buffer,MAX_BUFFER );
        log_on(debug_tmp,VDEBUG);

        perror("ERROR writing to socket");
        exit(1);
    }
}
}

/** Wywolywana w funkcji void doprocessing (int sock, char *ipadress). Dzieli bufor wejściowy (odebrany od klienta) i na podstawie jego zawartości podejmuje odpowiednie dzialania. Przygotowuje ciąg danych do wyslania do klienta */

int buffer_choper(char *ip_adress,char *in_buff,char *output) {

   const char delimiters[] = " ,;";
   char *token;
   char arglist[MAXPROC][300];
   char whattodo[100];
   int count=0;
   int out =0;
   char out_res[MAXPROC];
bzero(out_res,MAXPROC);
   token = strtok(in_buff, delimiters);

	 while( token != NULL )
        {
            if(strcmp(token,"REGISTER")==0)
                {strcpy(whattodo,token);}

	  else if(strcmp(token,"PROCESS_LIST_INIT")==0)
                {
                    printf("PROCESS_LIST_INIT \n");
                    strcpy(whattodo,token);
                }
      else if(strcmp(token,"IP_ADRESS_SYM")==0)
                {  strcpy(whattodo,token); 	  }

      else if(strcmp(token,"QUIT")==0)
                {  strcpy(whattodo,token);    }

      else if(strcmp(token,"CHECK_PROCESS")==0)
                {  strcpy(whattodo,token);     }

      else if(strcmp(token,"HELP")==0)
                {  strcpy(whattodo,token);	  }

      else {  strcpy(arglist[count],token);
                count++;
		 	  }
   token = strtok(NULL,delimiters);

     }

 if(strcmp(whattodo,"REGISTER")==0)
    {REGISTER(arglist[0],out_res); }

 else if(strcmp(whattodo,"PROCESS_LIST_INIT")==0)
    {
	 PROCESS_LIST_INIT(ip_adress,arglist,count,out_res);
     }
 else if(strcmp(whattodo,"IP_ADRESS_SYM")==0)
 {   }

 else if(strcmp(whattodo,"QUIT")==0)
    {   strcpy(out_res,"QUIT");  }

 else if(strcmp(whattodo,"CHECK_PROCESS")==0)
    { 	  CHECK_PROCESS(ip_adress,arglist,count,out_res);
	  }
	  	 else if(strcmp(whattodo,"HELP")==0){
	 HELP(ip_adress,out_res);
 	  }
	   else {
		  strcpy(out_res,"UNKNOWN COMMAND");
		  out=-1;
		  }

    strcpy(output,out_res);

    return out;
	}

/** Akceptuje polączenia i tworzy nowy proces dla przychodzącego polączenie. Wywoluje funkcje void doprocessing (int sock ,char *ipadress)*/
void serv_init_listen(int Port) {


int sockfd, newsockfd ;

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    int pid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    //bzero((char *) &serv_addr, sizeof(serv_addr));
        memset(&serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(Port);


    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }

    listen(sockfd,MAX_BACKLOG);
    clilen = sizeof(cli_addr);
       while (1)
    {
        newsockfd = accept(sockfd,  (struct sockaddr *) &cli_addr,&clilen);
        char *ip = inet_ntoa(cli_addr.sin_addr);

        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }

          pid = fork();
        if (pid < 0)
        {
            perror("ERROR on fork");
	    exit(1);
        }
        if (pid == 0)
        {
            close(sockfd);
            doprocessing(newsockfd,ip);
       
        exit(0);
        }
        else
        {
            close(newsockfd);
            ///sprawdz kolejke
            add_cli_from_qq();
          
        }
    }

}


/** Obsluguje liste. Wywolywana przy utworzeniu pierwszego elementu listy */
struct client* create_list(int proc_count,char *ip_adress,proc_value *proc_tab)
{
    struct client *ptr = (struct client*)malloc(sizeof(struct client));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->proc_count = proc_count;
   strcpy(ptr->ip_adress,ip_adress);
    ptr->proc_tab = proc_tab;
    ptr->next = NULL;

    head = curr = ptr;
    return ptr;
}

/** Obsluguje liste. Dodaje klienta do listy*/
struct client* add_to_list(char *ip_adress,int proc_count,proc_value *proc_tab, bool add_to_end)
{
    if(NULL == head)
    {
        return (create_list(proc_count,ip_adress,proc_tab));
    }

    struct client *ptr = (struct client*)malloc(sizeof(struct client));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
        strcpy(ptr->ip_adress,ip_adress);
        ptr->proc_tab= proc_tab;
        ptr->proc_count = proc_count;
        ptr->next = NULL;

    if(add_to_end)
    {
        curr->next = ptr;
        curr = ptr;
    }
    else
    {
        ptr->next = head;
        head = ptr;
    }
    return ptr;
}

/** Obsluguje liste. Wyszukuje elementu (klienta) na liście po podanym numerze IP */
struct client* search_in_list(char *ip_adress, struct client **prev)
{
    struct client *ptr = head;
    struct client *tmp = NULL;
    bool found = false;

    while(ptr != NULL)
    {
      if(strcmp(ptr->ip_adress,ip_adress)==0)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if(true == found)
    {
        if(prev)
            *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}


/** Obsluguje liste. Usuwa element (klienta) z listy po podanym numerze IP*/
int delete_from_list(char * ip_adress)
{
    struct client *prev = NULL;
    struct client *del = NULL;

   // printf("\n Deleting value [%s] from list\n",ip_adress);

    del = search_in_list(ip_adress,&prev);
    if(del == NULL)
    {
        return -1;
    }
    else
    {
        if(prev != NULL)
            prev->next = del->next;

        if(del == curr)
        {
            curr = prev;
        }
        else if(del == head)
        {
            head = del->next;
        }
    }

    free(del);
    del = NULL;

    return 0;
}

/** Wypisuje liste */
void print_list(void)
{
    struct client *ptr = head;
    int i;

    while(ptr != NULL)
    {
        printf("Client [%s]  %d \n\n",ptr->ip_adress,ptr->proc_count);

  for(i = 0; i<ptr->proc_count; i++){
        printf("%s %s %s ",ptr->proc_tab[i].pid,ptr->proc_tab[i].command_line,ptr->proc_tab[i].full_process_name);
            }
         ptr = ptr->next;
    }
    return;
}

/** Sprawdza czy istnieje plik dla klienta o podanym numerze IP. Zwraca 1 jeśli plik istnieje - dany klient jest już zarejestrowany*/
int is_ip_file_esists 	(char *file_name)
{

    DIR * d;
    int out=0;
    char * dir_name = "./serv_proc";

    d = opendir (dir_name);

    if (! d) {
            fprintf (stderr, "Cannot open directory '%s': %s\n",
            dir_name, strerror (errno));
            exit (EXIT_FAILURE);
            }
    while (1) {

        struct dirent * entry;
        entry = readdir (d);

        if (! entry) { break; }

    if(strcmp(entry->d_name,".")==1 && strcmp(entry->d_name,"..")==1)
        {
        if (strcmp(entry->d_name,file_name)==0) out=1;
        }
            }

    if (closedir (d)) {
        fprintf (stderr, "Could not close '%s': %s\n", dir_name, strerror (errno));
        exit (EXIT_FAILURE);
        }

return out;
}

	/** Rejestruje klienta - tworzy plik o nazwie [IP klienta] w katalogu serv.proc*/

     void REGISTER(char *file_name, char *out)
     {
	 FILE *f;
	 char file_name_path[100];
	 sprintf(file_name_path, "./serv_proc/%s", file_name);

    if(is_ip_file_esists(file_name)==0)
        {
        strcpy(out,"FILE_REGISTRED");
        f = fopen(file_name_path, "w");
        if (f) {    fclose(f); }
        }

    else {	strcpy(out,"FILE_EXISTS");	}

	}

/** Wyświetla pomoc - polecenia protokolu obslugiwane przez serwer*/
void HELP(char *ip_adress, char *out)
{
strcpy(out,"Server accept comunicates: REGISTER <ip_adress>  PROCESS_LIST_INIT <Process list> CHECK_PROCESS <Process list>  HELP help");

}

	/** Wypisuje do pliku liste inicjalizacyjną procesów otrzymaną od klienta. Jeśli istnieje lista dla tego klienta jest ona kasowana i powstaje nowa */
void PROCESS_LIST_INIT(char *ip_adress,char arg_list[MAXPROC][300],int count,char * out)
{
     int i;
	 FILE *f;
	 char file_name_path[100];

	 sprintf(file_name_path, "./serv_proc/%s", ip_adress);

	if(is_ip_file_esists(ip_adress)==0)
        {
		strcpy(out,"CLIENT_NOT_REGISTRED");
	    }
    else
{
	f = fopen(file_name_path, "w");
    if (f) {
	for( i=0; i<count; i++)
        {
        fprintf (f, "%s\n", arg_list[i]);
		}
    fclose(f);
            }
delete_from_list(ip_adress);
read_file_and_add_to_list(ip_adress);
qmessage_sent(ip_adress);
strcpy(out,"PROCESS_LIST_INIT_OK");
}

}

/** Porównuje bieżące procesy klienta z listą inicjalizacyjną.
* Jeśli procesy sie zgadzaja przygotowuje komunikat o poprawności.
* Jeśli na liście są procesy, których nie ma wśród procesów bieżacych to wypisuje je z komunikatem "MISSING".
* Jeśli wśród procesów bieżących są procesy, których nie ma na liście, serwer wypisuje je z komunikatem "UNKNOWN"*/

void CHECK_PROCESS(char *ip_adress,char arg_list[MAXPROC][300],int count,char * out) {
	int j,i,k;

    struct client *prev = NULL;
    struct client *srch = NULL;
	proc_value *data;
    int proc_count;
    int proc_line_nr=-1;

char buf_out[2000];
char buf_out1[2000];

char buf_out_n[2000];
char buf_out_n1[2000];

strcpy(buf_out_n,"");
strcpy(buf_out_n1,"");

char buf_out_tmp[100];
char buf_out_tmp1[100];

    srch = search_in_list(ip_adress,&prev);

	for( j=0; j<count; j++)
        {

	 int i,count1=0;
     char result[MAXPROC][300];
     count1=split (arg_list[j],result);
     for (i=0; i<count1; i++) {
     if(strcmp(result[i],"count")==0){

     proc_count=atoi(result[count1-1]);

    data=malloc(proc_count*sizeof(proc_value));

		}
    else
    {

	if (count1==3)
        {
        if(i==0)
            {
            proc_line_nr++;
            strcpy(data[proc_line_nr].pid,result[i]);
            }
        if(i==1)
	  	   strcpy(data[proc_line_nr].command_line,result[i]);

        if(i==2)
           strcpy(data[proc_line_nr].full_process_name,result[i]);
		}
    }
}


	}

    int serch=0;
    int serch1=0;

    for( k=0; k<proc_count; k++) {
    serch=0;

    for(i = 0; i<srch->proc_count; i++)
        {
    if(strcmp(srch->proc_tab[i].pid,data[k].pid)==0	)
    { serch=1; }
        }

    if (serch==0) {
		sprintf(buf_out_tmp,"%s:%s:%s ",data[k].pid,data[k].command_line,data[k].full_process_name);
		strcat(buf_out,buf_out_tmp) ;
        serch1=1;
	}

}

    serch=0;
    int serch2=0;
    for(i = 0; i<srch->proc_count; i++){
	serch=0;
    for( k=0; k<proc_count; k++)
        {
       if(strcmp(srch->proc_tab[i].pid,data[k].pid)==0	)
        {serch=1;}
        }

    if (serch==0) {
		sprintf(buf_out_tmp1,"%s:%s:%s ",srch->proc_tab[i].pid,srch->proc_tab[i].command_line,srch->proc_tab[i].full_process_name);
		strcat(buf_out1,buf_out_tmp1) ;
        serch2=1;
        }
}

if (serch1) {
	sprintf(buf_out_n,"UNKNOWN( %s )",buf_out);
    }
else strcpy(buf_out_n,"");

if (serch2) {
sprintf(buf_out_n1,"MISSING( %s )",buf_out1);
    }

else strcpy(buf_out_n1,"");

if (serch1||serch2)
    sprintf(out,"CHECK_PROCESS_NOK %s %s",buf_out_n,buf_out_n1);

else
    strcpy(out,"CHECK_PROCESS_OK");

free(data);
}


/** Czyta plik z procesami klienta i dodaje element do listy   */
void read_file_and_add_to_list(char *file_name) {

       FILE * fp;
       char * line = NULL;

       size_t len = 0;
       ssize_t read;
       proc_value *data;
       int proc_line_nr=-1;

    char file_name_path[100];
    int proc_count;

    sprintf(file_name_path, "./serv_proc/%s", file_name);

       fp = fopen(file_name_path, "r");
       if (fp == NULL)
           exit(EXIT_FAILURE);

       while ((read = getline(&line, &len, fp)) != -1)
        {
        int i,count=0;
        char result[MAXPROC][300];
        LastcharDel(line);

        count=split (line,result);
        for (i=0; i<count; i++)
            {
        if(strcmp(result[i],"count")==0)
        {
		proc_count=atoi(result[count-1]);
        data=malloc(proc_count*sizeof(proc_value));
		}
    else
    {

	if (count==3)
        {
	if(i==0){
		proc_line_nr++;
		strcpy(data[proc_line_nr].pid,result[i]);
	       }
     if(i==1)
	  	  strcpy(data[proc_line_nr].command_line,result[i]);
	 if(i==2)
	 strcpy(data[proc_line_nr].full_process_name,result[i]);
        }
    }
    }
     }

add_to_list(file_name,proc_count,data,true);

}

/** Wywolywana przy starcie serwera. Pobiera pliki z danymi wcześniej zarejestrowanych klientów z katalogu ./serv_proc i dodaje je do listy klientów*/
int get_client_process_config(void)
{
  DIR * d;
    char * dir_name = "./serv_proc";
      d = opendir (dir_name);

    if (! d) {
        fprintf (stderr, "Cannot open directory '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
    while (1) {
        struct dirent * entry;

        entry = readdir (d);
        if (! entry) {
            break;
        }

    if(strcmp(entry->d_name,".")==1 && strcmp(entry->d_name,"..")==1){


     log_on(entry->d_name,VDEBUG);
          read_file_and_add_to_list(entry->d_name);
	   }
    }

    if (closedir (d)) {
        fprintf (stderr, "Could not close '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }

    return 0;
}

/**Pobiera wszystkie zmienne wykorzystywane przez serwer z pliku ./conf/config.conf */
void get_server_config() {
       FILE * fp;
       char * line = NULL;
       size_t len = 0;
       ssize_t read;
char log_tmp[50];


       fp = fopen("./conf/config.conf", "r");
       if (fp == NULL)
           exit(EXIT_FAILURE);

       while ((read = getline(&line, &len, fp)) != -1) {

     int i,count=0;
     char result[10][300];
     count=split (line,result);
     for (i=0; i<=count; i++) {

    if(strcmp(result[i],"MAXPROC")==0){

    MAXPROC=atoi(result[i+1]);
	sprintf(log_tmp, "MAXPROC= %d",MAXPROC);
	log_on(log_tmp,VDEBUG);
		} else if (strcmp(result[i],"VDEBUG")==0)

    {

VDEBUG=atoi(result[i+1]);
	 sprintf(log_tmp, "VDEBUG= %d",VDEBUG);
	 log_on(log_tmp,VDEBUG);
	}
    else if (strcmp(result[i],"SERV_PORT")==0)

    {

SERV_PORT=atoi(result[i+1]);
	 sprintf(log_tmp, "SERV_PORT= %d",SERV_PORT);
	 log_on(log_tmp,VDEBUG);
	}
}
}
}

/** Wywolywana przy starcie serwera. Wywoluje funkcje przygotowujące serwer do dzialania. */
int server_initialization() {
char ccurrent_time[50];
current_time(ccurrent_time);

sprintf(LOG_NAME, "./log/my_serv_%s.log",ccurrent_time);

log_on("get_server_config()<START>",VDEBUG);

get_server_config();
log_on("get_server_config()<STOP>",VDEBUG);

log_on("get_client_process_config()<START>",VDEBUG);
get_client_process_config();
log_on("get_client_process_config()<STOP>",VDEBUG);
//qmessage_sent("Start");
qdelete();
return 0;
}


  int main ( int argc, char *argv[])
{
server_initialization();

serv_init_listen(SERV_PORT);

return 0;
}
