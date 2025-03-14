#include "karma_link.h"

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <threads.h>

#include "tcp_common.h"
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
	bool is_responder;
} KarmaLinkTcpCtx;

static int
karma_link_tcp_start_listen(void *ctx) {
	KarmaLinkTcpCtx *tcp_ctx = ctx;
	KarmaLinkListener kll = tcp_ctx->kll;
	int sock = tcp_ctx->sock;
	bool is_responder = tcp_ctx->is_responder;
	free(ctx);

	while (1) {
		// TODO: reestablishing connection in case of disconnect
		KarmaMessage msg;
		if (recv_karma_message(sock, &msg) == 0) {
			kll.cb(msg, kll.ctx);
			free(msg.payload);
		}
	}
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

	send_karma_message(sock, msg);
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

