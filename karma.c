#include "karma.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define TCP_BUFFER_SIZE 1024

static void
release_karma(Karma **pself) {
	free(*pself);
	*pself = NULL;
}

static void 
karma_tcp_listen(Karma *self, uint16_t port) {
	// TODO: impl
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in servaddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(port)
	};

	if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
		perror("error binding socket");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 5) == -1) {
		perror("error listening socket");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	printf("Karma is started listening TCP traffic on port %d\n", port);

	// TODO: threading
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(sizeof(cliaddr));
	int clientfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clilen);

	if (clientfd == -1) {
		perror("error accepting connection");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	char buffer[TCP_BUFFER_SIZE];

	while (1) {
		// TODO: change this logic to actual Karma message handling
		int nrecv = recv(clientfd, buffer, TCP_BUFFER_SIZE, 0);
		if (nrecv == 0) {
			printf("disconnected\n");
			break;
		}
		buffer[nrecv] = '\0';
		printf("Received: %s\n", buffer);
	}

	close(clientfd);
	close(sockfd);
}

Karma*
form_karma() {
	Karma *karma = malloc(sizeof(*karma));
	for (size_t i = 0; i < MAX_TOPICS; ++i) {
		karma->topics[i].len = 0;
	}

	karma->release = release_karma;
	karma->tcp_listen = karma_tcp_listen;

	return karma;
}

