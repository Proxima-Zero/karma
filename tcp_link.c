#include "karma_link.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"

static int
establish_connection(KarmaLink *self) {
	struct sockaddr_in servaddr = self->data.tcp.servaddr;
	int sock;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket creation failed");
		return -1;
	}

	if (connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
		perror("connection failed");
		return -1;
	}

	return sock;
}

static void
karma_link_tcp_add_listener(KarmaLink *self, uint16_t topic_id, KarmaListener kl, void *ctx) {
	// TODO: impl
}

static void
karma_link_tcp_post_message(KarmaLink *self, uint16_t topic_id, KarmaMessage msg) {
	int sock;

	if ((sock = establish_connection(self)) == -1) {
		return;
	}

	KarmaTcpConnHeader header = {
		.type = KARMA_TCP_TYPE_POST,
		.topic_id = htons(topic_id),
	};

	if (send(sock, &header, sizeof(KarmaTcpConnHeader), 0) == -1) {
		perror("error sending header");
		close(sock);
		return;
	}

	uint64_t npayload_size = htonll(msg.payload_size);
	if (send(sock, &npayload_size, sizeof(npayload_size), 0) == -1) {
		perror("error sending payload size");
		close(sock);
		return;
	}

	if (send(sock, msg.payload, msg.payload_size, 0) == -1) {
		perror("error sending payload");
	}

	close(sock);
	return;
}

static void
release_karma_link(KarmaLink **pself) {
	free(*pself);
	*pself = NULL;
}

KarmaLink *form_tcp_link(const char *addr, const int port) {
	struct sockaddr_in servaddr;	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if (inet_pton(AF_INET, addr, &servaddr.sin_addr) != 1) {
		perror("Invalid address");
		return NULL;
	}

	KarmaLink *link = malloc(sizeof(*link));
	link->data.tcp.servaddr = servaddr;

	link->add_listener = karma_link_tcp_add_listener;
	link->post_message = karma_link_tcp_post_message;
	link->release = release_karma_link;

	return link;
}

