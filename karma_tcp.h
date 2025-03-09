#ifndef KARMA_TCP_H_
#define KARMA_TCP_H_
#include "karma.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <threads.h>

typedef struct {
	Karma *karma;
	int sockfd;
} KarmaTcpCtx;


typedef struct {
	Karma *karma;
	int clientfd;
	mtx_t mutex;
} KarmaTcpCbCtx;

static void
karma_tcp_listener_cb(KarmaMessage msg, void *ctx) {
	// TODO: teardown connection handling
	KarmaTcpCbCtx *tcpctx = (KarmaTcpCbCtx *) ctx;
	Karma *self = tcpctx->karma;
	int clientfd = tcpctx->clientfd;
	mtx_t mutex = tcpctx->mutex;

	mtx_lock(&mutex);
	uint32_t npayload = htonl(msg.payload_size);
	if (send(clientfd, &npayload, sizeof(npayload), 0) == -1) {
		perror("error sending payload size to listener");
		goto end;
	}

	if (send(clientfd, msg.payload, msg.payload_size, 0) == -1) {
		perror("error sending payload to listener");
		goto end;
	}

end:
	mtx_unlock(&mutex);
}

static int
karma_tcp_listen_loop(void *ctx) {
	KarmaTcpCtx *tcpctx = ctx;
	Karma *self = tcpctx->karma;
	int sockfd = tcpctx->sockfd;

	while (1) {
		// TODO: graceful stop
		struct sockaddr_in cliaddr;
		socklen_t clilen = sizeof(sizeof(cliaddr));
		int clientfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clilen);
 
		if (clientfd == -1) {
			perror("error accepting connection");
			continue;
		}

		KarmaTcpConnHeader header;
		int nrecv = recv(clientfd, &header, sizeof(KarmaTcpConnHeader), 0);
		if (nrecv != sizeof(KarmaTcpConnHeader)) {
			printf("disconnected before received header");
			close(clientfd);
			continue;
			
		}
		header.topic_id = ntohs(header.topic_id);

		switch (header.type) {
		case KARMA_TCP_TYPE_POST:
			KarmaMessage msg;
			nrecv = recv(clientfd, &msg.payload_size, sizeof(msg.payload_size), 0);

			if (nrecv != sizeof(msg.payload_size)) {
				perror("error receiving payload size");
				close(clientfd);
				continue;
			}

			msg.payload_size = ntohl(msg.payload_size);
			// TODO: optimization using alloca?
			msg.payload = malloc(msg.payload_size);

			// TODO: adapt; create loop to read very long messages
			nrecv = recv(clientfd, msg.payload, msg.payload_size, 0);
			if (nrecv != msg.payload_size) {
				perror("error reading the whole payload at once");
				close(clientfd);
				continue;
			}
			self->post_message(self, header.topic_id, msg);
			free(msg.payload);
			close(clientfd);
			break;
		case KARMA_TCP_TYPE_LISTEN:
			KarmaTcpCbCtx *cbctx = malloc(sizeof(KarmaTcpCbCtx));
			cbctx->karma = self;
			cbctx->clientfd = clientfd;
			mtx_init(&cbctx->mutex, mtx_plain);
			self->add_listener(self, header.topic_id, karma_tcp_listener_cb, cbctx);
			break;
		}
	}

	free(ctx);

	return 0;
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

	KarmaTcpCtx *ctx = malloc(sizeof(*ctx));
	ctx->karma = self;
	ctx->sockfd = sockfd;

	thrd_t thread;
	thrd_create(&thread, karma_tcp_listen_loop, ctx);
	printf("Karma is started listening TCP traffic on port %d\n", port);
}

#endif