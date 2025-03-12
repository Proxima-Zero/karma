#include "karma_link.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <threads.h>

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

typedef struct {
	KarmaLinkListener kll;
	int sock;
} KarmaLinkTcpCtx;

static int
karma_link_tcp_start_listen(void *ctx) {
	KarmaLinkTcpCtx *tcp_ctx = ctx;
	KarmaLinkListener kll = tcp_ctx->kll;
	int sock = tcp_ctx->sock;
	free(ctx);

	uint64_t bufsize = 1024;
	char *buffer = malloc(bufsize);
	while (1) {
		// TODO: reestablishing connection in case of disconnect
		KarmaMessage msg;
		int nrecv = recv(sock, &msg.payload_size, sizeof(msg.payload_size), 0);

		if (nrecv != sizeof(msg.payload_size)) {
			perror("error getting payload size");
			continue;
		}

		msg.payload_size = ntohl(msg.payload_size);

		if (msg.payload_size > bufsize) {
			bufsize = msg.payload_size;
			buffer = realloc(buffer, msg.payload_size);
		}

		// TODO: thread safety considerations for buffer (what if listeners will run threads too?)
		// TODO: change to a loop. it is possible we cannot read message in one read
		nrecv = recv(sock, buffer, msg.payload_size, 0);
		if (nrecv != msg.payload_size) {
			perror("error reading a whole message payload");
			continue;
		}

		msg.payload = buffer;

		kll.cb(msg, kll.ctx);
	}
	free(buffer);
}

static void
karma_link_tcp_add_listener(KarmaLink *self, uint16_t topic_id, KarmaLinkListener kll) {
	int sock;

	if ((sock = establish_connection(self)) == -1) {
		return;
	}

	KarmaTcpConnHeader header = {
		.type = KARMA_MSG_TYPE_LISTEN,
		.topic_id = htons(topic_id),
	};

	if (send(sock, &header, sizeof(KarmaTcpConnHeader), 0) == -1) {
		perror("error sending header");
		close(sock);
		return;
	}

	thrd_t thread;
	KarmaLinkTcpCtx *listen_ctx = malloc(sizeof(KarmaLinkTcpCtx));
	listen_ctx->kll = kll;
	listen_ctx->sock = sock;
	thrd_create(&thread, karma_link_tcp_start_listen, listen_ctx);
}

static void
karma_link_tcp_post_message(KarmaLink *self, uint16_t topic_id, KarmaMessage msg) {
	int sock;

	if ((sock = establish_connection(self)) == -1) {
		return;
	}

	KarmaTcpConnHeader header = {
		.type = KARMA_MSG_TYPE_POST,
		.topic_id = htons(topic_id),
	};

	if (send(sock, &header, sizeof(KarmaTcpConnHeader), 0) == -1) {
		perror("error sending header");
		close(sock);
		return;
	}

	uint32_t npayload_size = htonl(msg.payload_size);
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
karma_link_tcp_create_responder(KarmaLink *self, uint16_t topic_id, KarmaLinkResponder kr) {
	// TODO: impl
}

static Array*/*KarmaMessage*/
karma_link_tcp_make_request(KarmaLink *self, uint16_t topic_id, KarmaMessage msg) {
	// TODO: impl
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
	link->add_responder = karma_link_tcp_create_responder;
	link->make_request = karma_link_tcp_make_request;
	link->release = release_karma_link;

	return link;
}

