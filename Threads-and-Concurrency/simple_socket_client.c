#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>

void sendMessage(int);

int main(void) {
	short server_port;				
	
	printf("Server Port: ");
	scanf("%hd",&server_port);
	getchar();	

	/* Create an unbound client_socket */
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (client_socket == -1) {
		perror("Failed creating client client_socket");
		exit(1);
	}

	/* Specify the server ip addres and port */ 
	char ip_str[INET_ADDRSTRLEN];
	printf("\nServer ip: ");
	fgets(ip_str, sizeof(ip_str), stdin);
	
	size_t length = strlen(ip_str);
	if (length > 0 && ip_str[length - 1] == '\n') {
		ip_str[length - 1] = '\0';
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	
	if (inet_pton(AF_INET, ip_str, &(server_address.sin_addr)) <= 0) {
		perror("ERROR");
		exit(1);
	}

	/* Establish a connection with the server */
	if(connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
		perror("Failed to connect with the server");
		exit(1);
	}
	
	sendMessage(client_socket);
	close(client_socket);

	return 0;
}

void sendMessage(int client_socket) {
	while (1) {
		int opt = 0;
		printf("1. Send message\n");
		printf("2. Exit\n");
		printf("What do you want to do? :");
		
		if (scanf("%d", &opt) != 1) {
			printf("\nInvalid input. Please enter a number.\n");
		}
		while(getchar() != '\n');
		
		printf("opt_val : %d\n",opt);
		switch (opt) {
			case 1:
				char message[30];
				printf("\nWrite your message :");
				fgets(message, 20, stdin);
				send(client_socket, message, strlen(message), 0);
				break;
			case 2: 
				return;
			defaut: break;
		}		

	}
}
