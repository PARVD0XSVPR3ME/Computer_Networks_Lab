#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int sockfd = 0, readsize;
    char recvBuff[1024],input[100];
    struct sockaddr_in serv_addr;
	int portno;
	int received_int = 0,i;
	unsigned long received_ip;
	unsigned short received_port;
	//elegxos gia orthh xrysh tou programmatos 
    if(argc != 3)
    {
        printf("\n Usage: %s <ip of server> <port of server> \n",argv[0]);
        return 1;
    }
	portno = atoi(argv[2]);
	if (portno < 9610 || portno > 9619) {
        printf("To <PORT> prepei na einai metaksi 9610 kai 9619, ksanaprospathiste");
        return -1;
    }	
	while(1) {

		printf("Give command: ");		
		fgets(input,100,stdin);		//eisodos entolhs xrhsth
		memset(recvBuff, '0',sizeof(recvBuff));
    		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //Δημιουργία TCP socket από client
    		{
        		printf("\n Error : Could not create socket \n");
        		return 1;
    		}

    		memset(&serv_addr, '0', sizeof(serv_addr)); //μηδενισμός των πεδίων της δομής cliaddr

    		serv_addr.sin_family = AF_INET; //Καθορισμός του τύπου του socket ως INTERNET socket
    		serv_addr.sin_port = htons(portno); //συνάρτηση μετατροπής του port που ακούει ο client σε short integer

    		if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    		{
        	printf("\n inet_pton error occured\n");
        	return 1;
    		}

			if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) //Σύνδεση του client στο socket
    		{
       			perror("Error : Connect Failed");
       		return 1;
    		}

			if( write(sockfd , input , strlen(input)+1) < 0)		//apostolh entolhs xrhsth ston server mesw ths write 
        	{
            		puts("Send failed");
            		return 1;
       		 }
			input[strlen(input)-1] = '\0';
		if (strcmp(input,"listclients")==0) {		//an h entolh tou xrhsth einai h listclients tote 
			shutdown(sockfd, SHUT_WR);				//termatismos ths egrafhs write apo ton client ston server wste h read ston server na stamatisei 
													//wste na mporesoume pleon na diavasoume apo ton server mesw ths read kati pou tha steilei mesw ths write 
			readsize = read(sockfd, &received_int, sizeof(received_int));	//anagnwsh ths received_int apo ton server poy periexei to plithos twn 
																			//apothikeymenwn clients sth lista
			
			for (i=1;i<=ntohl(received_int);i++) {		//metatroph tou received_int se int mesw ths ntohl kai epanalipsh gia to plithos twn clients 
				readsize = read(sockfd, &received_ip, sizeof(received_ip));	//anagnwsh ths ip tou kathe client sth lista apo ton server
				if (readsize>0) {
					struct in_addr my_addr;
					my_addr.s_addr=received_ip;
					printf("%d. %s ",i,inet_ntoa(my_addr));		//kai metatroph ths ip se string mesw ths inet_ntoa kai ektypwsh ths ip
				}

				readsize = read(sockfd, &received_port, sizeof(received_port));	//anagnwsh tou port tou kathe client sth lista apo ton server 
				if (readsize>0) {
					printf("%d\n",ntohs(received_port));		//kai metatroph tou port se short mesw ths ntohs kai ektypwsh tou port 
				}
				else printf("\nerror receiving port ...");
				
			}
			
			
		}
		shutdown(sockfd, 2); 	
	}
    return 0;
}
