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
		KarmaTcpConnHeader header;
		int nrecv = recv(clientfd, &header, sizeof(KarmaTcpConnHeader), 0);
		if (nrecv != sizeof(KarmaTcpConnHeader)) {
			printf("disconnected before received header");
			continue;
			
		}
		header.topic_id = ntohs(header.topic_id);

		switch (header.type) {
		case KARMA_TCP_TYPE_POST:
			// TODO: implement
			break;
		case KARMA_TCP_TYPE_LISTEN:
			// TODO: implement
			break;
		}
		// TODO: change this logic to actual Karma message handling
		nrecv = recv(clientfd, buffer, TCP_BUFFER_SIZE, 0);
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

static void
karma_add_listener(Karma *self, uint16_t topic_id, KarmaListener kl, void *ctx) {
	if (topic_id >= MAX_TOPICS) {
		fprintf(stderr, "topic_id is too high (max=%d)\n", MAX_TOPICS);
		return;
	}

	size_t len = self->topics[topic_id].len;


	if (len == MAX_LISTENERS) {
		fprintf(stderr, "too much listeners\n");
		return;
	}

	self->topics[topic_id].listeners[len] = kl;
	self->topics[topic_id].ctxs[len] = ctx;
	self->topics[topic_id].len++;
}

static void
karma_post_message(Karma *self, uint16_t topic_id, KarmaMessage msg) {
	if (topic_id >= MAX_TOPICS) {
		fprintf(stderr, "topic_id is too high (max=%d)\n", MAX_TOPICS);
		return;
	}

	for (size_t i = 0; i < self->topics[topic_id].len; ++i) {
		void *ctx = self->topics[topic_id].ctxs[i];
		self->topics[topic_id].listeners[i](msg, ctx);
	}
}

Karma*
form_karma() {
	Karma *karma = malloc(sizeof(*karma));
	for (size_t i = 0; i < MAX_TOPICS; ++i) {
		karma->topics[i].len = 0;
	}

	karma->add_listener = karma_add_listener;
	karma->post_message = karma_post_message;

	karma->release = release_karma;
	karma->tcp_listen = karma_tcp_listen;

	return karma;
}

