#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define PORT 9610  // default port 9000+(omada=62-1)*10 dld 9610

struct node { //dhmiourgia struct gia diasindedemenh lista pou tha xrisimopoihthei gia apothikeysh ip, port twn clients
		unsigned short port;
		unsigned long ip_addr;
		struct node *next;
	};

void deleteNode(struct node *head, struct node *n) //diagrafh node apo linked list
{
    // When node to be deleted is head node
    if(head == n)
    {
        if(head->next == NULL)
        {
            printf("There is only one node. The list can't be made empty\n");
            return;
        }
 
        /* Copy the data of next node to head */
        head->port = head->next->port;
		head->ip_addr = head->next->ip_addr;
        // store address of next node
        n = head->next;
 
        // Remove the link of next node
        head->next = head->next->next;
 
        // free memory
        free(n);
 
        return;
    }
 
 
    // When not first node, follow the normal deletion process
 
    // find the previous node
    struct node *prev = head;
    while(prev->next != NULL && prev->next != n)
        prev = prev->next;
 
    // Check if node really exists in Linked List
    if(prev->next == NULL)
    {
        printf("\n Given node is not present in Linked List");
        return;
    }
 
    // Remove node from Linked List
    prev->next = prev->next->next;
 
    // Free memory
    free(n);
 
    return; 
}


int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0,read_size,flag=0,size=0;
    int len = sizeof(struct sockaddr);
    struct sockaddr_in serv_addr,cli_addr;
    int bd,portno;
    char sendBuff[1025],client_message[100];
    
	struct node *current_node, *head;
	head = NULL;
    portno = PORT;
    //elegxos gia orthh xrhsh tou programmatos
    if (argc > 2) {
        printf("Xrhsh: server <PORT> ,ksanaprospathiste");
        return -1;
    }
    if (argc == 2) {
        portno = atoi(argv[1]);
        if (portno < 9610 || portno > 9619) {
            printf("To <PORT> prepei na einai metaksi 9610 kai 9619, ksanaprospathiste");
            return -1;
        }
        
    }
        
    else {
        portno = PORT;
    }
    printf("Listening on %d\n", portno);
    


    listenfd = socket(AF_INET, SOCK_STREAM, 0); //Δημιουργία TCP socket από server

    if (listenfd < 0) 
        printf("ERROR opening socket");

    printf("To socket dhmiourgh8hke epityxws!\n");

    memset(&serv_addr, '0', sizeof(serv_addr)); //μηδενισμός των πεδίων της δομής serv_addr
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET; //Καθορισμός του τύπου του socket ως INTERNET socket
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //συνάρτηση μετατροπής IP διεύθυνσης σε long integer. Έτσι ο server μπορεί να στέλνει και να λαμβάνει από οποιαδήποτε IP address
    serv_addr.sin_port = htons(portno); //συνάρτηση μετατροπής του port που ακούει ο server σε short integer

    bd = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); //Σύνδεση του server στο socket
    if ( bd < 0)
    {
        perror("Bind failed:");
        close(listenfd);
        exit(1);
    }
	puts("bind done");

    if (listen(listenfd, 10) < 0) //ο server ορίζει μια ουρά αιτήσεων με μέγεθος 10
        perror("Server listen failure");
	puts("Waiting for incoming connections...");

while( (connfd = accept(listenfd, (struct sockaddr*)&cli_addr, (socklen_t*)&len) ) )	{
 
        //Η συνάρτηση accept εκτελείται αυτόματα κάθε φορά που ο server λαμβάνει 
		//μια αίτηση από τον client και επιστρέφει ένα ακέραιο αριθμό ο οποίος 
		//καταχωρείται στη μεταβλητή connfd και αφορά το socket listenfd
		//Ουσιαστικά ο αριθμός στη connfd είναι ο νέος αριθμός με τον οποίο θα αναφερόμαστε 
		//τώρα στο socket και θα χρησιμοποιηθεί σε όλες τις επόμενες εντολές
		
	
	puts("Connection accepted");
	
	while( (read_size = read(connfd , client_message , sizeof(client_message))) > 0 ) { //diavazoume mesw ths read thn entolh tou xrhsth (addme,removeme,listclients) apo ton client
		client_message[strlen(client_message)-1] = '\0'; //afairoume ton xarakthra \0 pou prostethike mesw ths fgets ston client 
		//ginetai elegxos ths entolhs tou xrhsth 
		if(strcmp(client_message,"addme")==0) {
			
			current_node=head;
			while(current_node) {
				if (current_node->ip_addr==cli_addr.sin_addr.s_addr) { 
						//ginetai elegxos sth diasindedemenh lista gia to an yparxei struct pou na periexei 
						//thn ip tou client pou einai syndedemenos ston server,ayto ginetai gia na mhn yparxei diplh katagrafh kapoiou client
						printf("Client already added in server's list\n");
						flag=1;
				}
				current_node = current_node->next ;
			}
			if (!flag) {	//an den yparxei o client sth lista ,ton prosthetw
				size++; //plithos clients 
				//dimiourgia neou struct pou prostithetai sto linked list
				current_node = (struct node *) malloc (sizeof(struct node));
				//apothikeysh sto struct ths ip kai tou port tou trexontos client
				current_node->ip_addr=cli_addr.sin_addr.s_addr;

				current_node->port=cli_addr.sin_port;

				printf("listening on: ");
				//gia thn ektypwsh ths dieythinshs ,dimiourgia enos in_addr struct ,wste na ginei metatrwph thw ip se string mesw ths inet_ntoa kai ektypwsh
				struct in_addr my_addr;
				my_addr.s_addr=current_node->ip_addr;
				//akoma ginetai metatroph tou port se int me thn ntohs()
				printf("%s:%d\n",inet_ntoa(my_addr),ntohs(current_node->port));
				//h printf ektypwnei thn ip kai to port pou apothikeytike gia logous debug apo merias server
				current_node->next=head;
				head=current_node;
			}	
		}
		else if (strcmp(client_message,"listclients")==0){
			current_node=head;
			uint32_t converted_number=htonl(size); //metatroph tou plhthous twn apothikeymenwn clients sth lista mesw ths htonl gia metafora me ston client
			//mesw ths write xrisimopoiontas socket
			if (write(connfd, &converted_number, sizeof(converted_number))<0) { //apostolh tou converted_number ston client mesw ths write
				printf("\nerror number\n");
			}
			while(current_node) { //gia kathe struct ths listas 
				printf("listening on: ");
				struct in_addr my_addr;
				my_addr.s_addr=current_node->ip_addr;
				//emfanish ths ip kai tou port kathe apothikeymenou client sth lista
				printf("%s:%d\n",inet_ntoa(my_addr),ntohs(current_node->port));
				
				if (write(connfd, &current_node->ip_addr, sizeof(current_node->ip_addr))<0) {	//kai metafora ip tou kathe apothikeymenou client sth lista
																								//ston client pou zita th lista
				printf("\nerror ip\n");
				}
				if (write(connfd, &current_node->port, sizeof(current_node->port))<0) {			//opws epishs kai metafora tou port tou kathe client 
				printf("\nerror port\n");
				}
				
				current_node = current_node->next ;
			}
		}
		else if (strcmp(client_message,"removeme")==0){		//diagrafh tou client apo th lista 
			current_node=head;
			while(current_node) { 				//psaxnw sth lista gia ena node pou na exei apothikeymenh thn ip tou client pou einai syndedemenos ayth th stigmh 
				if (current_node->ip_addr==cli_addr.sin_addr.s_addr) {
								if (size>1) size--;
								deleteNode(head, current_node);			//an to vrw tote kalw thn synarthsh diagrafhs node apo thn linked list
					
				}
				current_node = current_node->next ;
			}
		}
		else {
			printf("Wrong command\n");
		}
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }	
		
}		
    
     exit(0);
}
