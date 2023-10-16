#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define NUM_THREADS 5

int shared_client_socket;			/* shared socket */
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;	/* mutex lock for shared var */

void *sendMessage(void *arg);

int main(void) {
	pthread_t connection_threads[NUM_THREADS];	/* connection threads array */
	short server_port;				
	int t_id[NUM_THREADS];
	
	printf("Server Port: ");
	scanf("%hd",&server_port);

	/* === SOCKET ====================================================================================== */
	/* Create an unbound socket */
	shared_client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (shared_client_socket == -1) {
		perror("Failed creating client socket");
		exit(1);
	}

	/* Specify the server addres and port */ 
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");	/* loopback ip */

	/* Esrablish a connection with the server */
	if(connect(shared_client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
		perror("Failed to connect with the server");
		exit(1);
	}
	/* ================================================================================================= */
	
	
	/* create connection threads */
	for (int i = 0; i < NUM_THREADS; i++) {
		t_id[i] = i;
		pthread_create(&connection_threads[i], NULL, sendMessage, &t_id[i]);
	}

	/* wait/join connections threads */
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(connection_threads[i], NULL);
	}

	close(shared_client_socket);
	return 0;
}

void *sendMessage(void *arg) {
	int t_id = *(int*)arg;
	char message[10];

	for (int i = 0; i < 5; i++) {
		/* Enter critical section */
		pthread_mutex_lock(&m);
		sprintf(message, "%d", i);
		write(shared_client_socket, message, sizeof(message));
		printf("[%d] thread message: %s\n", t_id, message);
		pthread_mutex_unlock(&m);
		/* Exit critical section */
	}

	pthread_exit(0);
}
