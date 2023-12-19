#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>

#define NUM_THREADS 2

typedef struct {
	int t_id;
	int client_socket;
}client_socket_t;

int server_socket;				/* shared socket */
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;	/* mutex lock for shared var */

void *handleConnection (void *arg);

int main(void) {
	pthread_t connection_threads[NUM_THREADS];	/* connection threads array */
	short server_port;				
	
	printf("Server Port: ");
	scanf("%hd",&server_port);

	/* === SOCKET ====================================================================================== */
	/* Create an unbound socket */
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (server_socket == -1) {
		perror("Failed creating client socket");
		exit(1);
	}

	fcntl(server_socket, F_SETFL, O_NONBLOCK);	/* Set the socket to perform non-block operations

	/* Specify the server addres and port */ 
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = INADDR_ANY;	/* listen all network intephases */

	/* Establish a connection with the server */
	if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
		perror("Failed to binding server socket");
		exit(1);
	}

	/* Listen for incoming connections */
	if (listen(server_socket, 5) == -1) {
		perror("Failed to listen connections");
		exit(1);
	}

	int i = 0;
	client_socket_t t_connection[2];

	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);


	/* Infinite loop to accept more than one connection */
	while (1) {
		/* Accept queued connection */
		do {
			
		t_connection[i].client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
		}while(t_connection[i].client_socket < 0);
		t_connection[i].t_id = i;

		if (t_connection[i].client_socket == -1) {
			perror("Failed to accept the connection");
			exit(1);
		}
		
		/* Create a thread to handle the connection */ 
		pthread_create(&connection_threads[i], NULL, handleConnection, &t_connection[i]);
		i++;
	}
	/* ================================================================================================= */
	
	int k = --i;
	
	for (int i = 0; i < k; i++) {
		pthread_join(connection_threads[i], NULL);
	}

	close(server_socket);
	return 0;
}

void *handleConnection (void *arg) {
	client_socket_t t_connection = *(client_socket_t*)arg;
	char buffer[1024];	/* buffer to storage the incoming message */
	
	//pthread_mutex_lock(&m);
	while (1) {
		pthread_mutex_lock(&m);
		ssize_t bytes_received = recv(t_connection.client_socket, buffer, sizeof(buffer), MSG_DONTWAIT);	/* read the message from connection */
		pthread_mutex_unlock(&m);

		if (bytes_received == 0 ) {
			break;
		}else if (bytes_received > 0) {
			printf("[%d] thread message received: %s\n", t_connection.t_id, buffer);
		}
		usleep(100000);
	}
	//pthread_mutex_unlock(&m);
	printf("Exit connection\n");
	close(t_connection.client_socket);
	pthread_exit(0);
}
