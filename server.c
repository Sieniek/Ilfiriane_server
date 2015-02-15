#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>

#include <string.h>
#include <time.h>
#include <math.h>

#include "data.h"

#define QUEUE_SIZE 5

#define OBJECTS_COUNT 73
#define BUFSIZE 35
char bufor[BUFSIZE];

int main(int argc, char* argv[]){
	int port = 12345;
	if( argc > 1) port = atoi(argv[1]);
	
	
    int nSocket;
	int nClientSocket;
   	int nBind;
	int nListen;
   	int nFoo = 1;
   	socklen_t nTmp;
   	struct sockaddr_in stAddr, stClientAddr;
       
	
   	/* address structure */
   	memset(&stAddr, 0, sizeof(struct sockaddr));
   	stAddr.sin_family = AF_INET;
   	stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   	stAddr.sin_port = htons(port);

   	/* create a socket */
   	nSocket = socket(AF_INET, SOCK_STREAM, 0);
   	if (nSocket < 0){
   	    fprintf(stderr, "%s: Can't create a socket.\n", argv[0]);
   	    exit(1);
   	}
   	setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));
	
   	/* bind a name to a socket */
  	nBind = bind(nSocket, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));
  	if (nBind < 0){
		fprintf(stderr, "%s: Can't bind a name to a socket.\n", argv[0]);
   	    exit(1);
   	}
   	/* specify queue size */
   	nListen = listen(nSocket, QUEUE_SIZE);
  	if (nListen < 0){
  	   fprintf(stderr, "%s: Can't set queue size.\n", argv[0]);
  	}
	time_t prev_time,curr_time;
	int client_A = -1;
	int client_B = -1;
	int client_index = -1;

	int n, sck, odp, client_id, client_x, client_y, code = 12345678;
	
	while( client_A == -1 || client_B == -1 ){//wait for 2 players		
		
		printf("A: %d, B= %d\n", client_A, client_B);
		
		char buffer[10] = {' '};//wait code
		

	   	/* block for connection request */
		printf("Looking for clients...\n", 50);
	   	nTmp = sizeof(struct sockaddr);
	   	nClientSocket = accept(nSocket, (struct sockaddr*)&stClientAddr, &nTmp);
	   	if (nClientSocket < 0){
	   	    fprintf(stderr, "%s: Can't create a connection's socket.\n", argv[0]);
	  	     exit(1);
	   	}
		sprintf(buffer, "%09d", code);	
        write(nClientSocket, buffer, 10);
		

		if ((odp = read (nClientSocket, bufor, 4)) > 0){
			client_id = atoi(strtok(bufor," "));			
			printf("Client_ID: %d \n", client_id);
			//#Register

			if(client_A == -1){//#Client A unregistred
				printf("Client A registred!\n");
				code -= client_id;
				client_A = client_id;
			}else if(client_B == -1 && client_id != client_A){//#client B unregistred and current client is NOT a client_A
				printf("Client B registred!\n");
				code = 12312312;			
				client_B = client_id;
			}
			client_id = -1;
		}	
		
        close(nClientSocket);
		sleep(1);

	}
	client_id = -1;
   	while(1){
		
		time(&prev_time);
    	/* block for connection request */
    	nTmp = sizeof(struct sockaddr);
    	nClientSocket = accept(nSocket, (struct sockaddr*)&stClientAddr, &nTmp);
     	if (nClientSocket < 0){
     	    fprintf(stderr, "%s: Can't create a connection's socket.\n", argv[0]);
     		exit(1);
     	}

        char buffer[71*5*7] = {' '};//71 object * 5 parameters * (3 integers digit + 1 for , + 2 for float part + 1 for space)
		int i,j;		
		for(i = 0; i< 71 * 5; i++){
			char word[7];
			sprintf(word, "%.2f ", array[i]);		
			strcat(buffer, word);
		}
        int n;
        write(nClientSocket, buffer, 72*5*7);
		int sck, odp, client_id, client_x, client_y;
		

		if ((odp = read (nClientSocket, bufor, BUFSIZE)) > 0){
			write (1, bufor, odp);		
			client_id = atoi(strtok(bufor," "));
			client_x = atoi(strtok(NULL," "));
			client_y = atoi(strtok(NULL," "));
			printf("\nX= %d, Y= %d\n", client_x, client_y);
			printf("cX= %f, cY= %f\n", array[5 * client_index], array[5 * client_index + 1]);
		}			
		//#making changes
		if(client_id > 0){
			
			
			if(client_id == client_A){
				printf("Response from A\n");
				client_index = 0;
			}else{
				client_index = 1;
				printf("Response from B\n");
			}
			float c, deltaX, deltaY;
			deltaX = client_x - array[5 * client_index];
			deltaY = client_y - array[5 * client_index + 1];
			printf("DeltaX: %f, DeltaY: %f\n", deltaX, deltaY);

			c = powf(deltaX, 2.0) + powf(deltaY, 2.0);
			
			printf("C**2: %f\n", c);
			c = sqrt(c);
			printf("C: %f\n", c);
			deltaX /= c;
			deltaY /= c;
			printf("\nNormals\nDeltaX: %f, DeltaY: %f\n", deltaX, deltaY);		
			array[5 * client_index + 3] += deltaX;
			array[5 * client_index + 4] += deltaY;
			printf("Vx: %f, Vy: %f\n", array[5 * client_index + 3], array[5 * client_index + 4]);

		}
			
		array[0] -= array[3] * 0.1;
		array[1] -= array[4] * 0.1;
		array[5] -= array[8] * 0.1;
		array[6] -= array[9] * 0.1;
		//colision
		int client;			
		for(client = 0; client < 2; client++){
			//printf("no. %d\n", client);
			float r = array[5 * client + 2];
			if( r > array[5 * client] && array[5 * client + 3] > 0){ //colision on left
				array[5 * client + 3] *= -1;
				//printf("colision on left\n");
			}else if( 800 < r + array[5 * client] && array[5 * client + 3] < 0){ //colision on right
				array[5 * client + 3] *= -1;
				//printf("colision on right\n");
			}
			if( r > array[5 * client + 1] && array[5 * client + 4] > 0){ //colision on up
				array[5 * client + 4] *= -1;
				//printf("colision on up\n");
			}else if( 600 < r + array[5 * client + 1] && array[5 * client + 4] < 0){ //colision on down
				array[5 * client + 4] *= -1;
				//printf("colision on down\n");
			}
		}
		//printf("OUt\n");
		for(client_index = 0; client_index < 2; client_index++)
		for(client = 0; client < 71; client++){
			//printf("Detekcja\n");
			if( client == client_index || array[client * 5 + 2] < 1){
				//printf("cont");
				continue;
			}
			if(	sqrt(powf(array[client_index * 5]- array[client * 5], 2.0)+ powf(array[client_index * 5 + 1]- array[client * 5 + 1],2.0)) < array[client_index * 5 + 2] + array[client * 5 + 2]){//colision
				float scale = 0.1;
				if(array[client_index * 5 + 2] < array[client * 5 + 2]){//client smaller than npc
					float player_v = powf(array[client_index * 5 + 2], 2.0) * M_PI;
					array[client_index * 5 + 2] = sqrt(player_v * (1 - scale) / M_PI) ;
					float client_v = powf(array[client * 5 + 2], 2.0) * M_PI + scale * player_v;
					array[client * 5 + 2] = sqrt(client_v / M_PI);
					
				}else{//npc smaller than player
					float client_v = powf(array[client * 5 + 2], 2.0) * M_PI;
					array[client * 5 + 2] = sqrt(client_v * (1-scale) / M_PI);
					float player_v = powf(array[client_index * 5 + 2], 2.0) * M_PI + client_v * scale;
					array[client_index * 5 + 2] = sqrt(player_v / M_PI);
				}				
			} 				
		}

		code = -1;
		client_id = -1; //mark this msg as readed
        close(nClientSocket);
		time(&curr_time);
		
		prev_time = clock();
		
    }
    close(nSocket);
    exit(0);
}


