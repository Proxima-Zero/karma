#ifndef KARMA_TCP_H_
#define KARMA_TCP_H_
#include "karma.h"

#include <arpa/inet.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <threads.h>
#include <unistd.h>

typedef struct {
	Karma *karma;
	int sockfd;
	atomic_bool *stop_flag;
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

static KarmaMessage 
karma_tcp_responder_cb(KarmaMessage msg, void *ctx) {
	// TODO: teardown connection handling
	KarmaTcpCbCtx *tcpctx = (KarmaTcpCbCtx *) ctx;
	Karma *self = tcpctx->karma;
	int clientfd = tcpctx->clientfd;
	mtx_t mutex = tcpctx->mutex;

	mtx_lock(&mutex);

	// 1. send request
	uint32_t npayload = htonl(msg.payload_size);
	if (send(clientfd, &npayload, sizeof(npayload), 0) == -1) {
		perror("error sending payload size to responder");
		goto error;
	}

	if (send(clientfd, msg.payload, msg.payload_size, 0) == -1) {
		perror("error sending payload to responder");
		goto error;
	}

	// 2. get response
	KarmaMessage resp;

	uint32_t nrecv = recv(clientfd, &resp.payload_size, sizeof(resp.payload_size), 0);

	if (nrecv != sizeof(resp.payload_size)) {
		perror("error getting response size from responder");
		goto error;
	}

	resp.payload_size = ntohs(resp.payload_size);

	nrecv = recv(clientfd, resp.payload, resp.payload_size, 0);
	if (nrecv != resp.payload_size) {
		perror("error getting response from responder");
		goto error;
	}

	return resp;
error:
	mtx_unlock(&mutex);
	KarmaMessage errorResp;
	errorResp.payload_size = 0;
	return errorResp;

}

static int
karma_tcp_listen_loop(void *ctx) {
	KarmaTcpCtx *tcpctx = ctx;
	Karma *self = tcpctx->karma;
	int sockfd = tcpctx->sockfd;
	atomic_bool *stop_flag = tcpctx->stop_flag;
	free(ctx);

	while (!atomic_load(stop_flag)) {
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
		case KARMA_MSG_TYPE_POST:
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
		case KARMA_MSG_TYPE_LISTEN:
			KarmaTcpCbCtx *cbctx = malloc(sizeof(KarmaTcpCbCtx));
			cbctx->karma = self;
			cbctx->clientfd = clientfd;
			mtx_init(&cbctx->mutex, mtx_plain);
			self->add_listener(self, header.topic_id, (KarmaListener) {
				.cb = karma_tcp_listener_cb,
				.ctx = cbctx
			});
			break;
		case KARMA_MSG_TYPE_REQUEST:
			// TODO:
			break;
		case KARMA_MSG_TYPE_RESPOND:
			cbctx = malloc(sizeof(KarmaTcpCbCtx));
			cbctx->karma = self;
			cbctx->clientfd = clientfd;
			mtx_init(&cbctx->mutex, mtx_plain);
			self->add_responder(self, header.topic_id, (KarmaResponder) {
				.cb = karma_tcp_responder_cb,
				.ctx = cbctx
			});
			break;
		}
	}
	free(stop_flag);
	close(sockfd);

	return 0;
}

static void 
karma_start_tcp_listen(Karma *self, uint16_t port) {
	// TODO: check whether connection already exists
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	int opt = 1;

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		fprintf(stderr, "setsockopt(SO_REUSEADDR) failed\n");
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


	atomic_bool *stop_flag = malloc(sizeof(atomic_bool));
	*stop_flag = ATOMIC_VAR_INIT(false);

	KarmaTcpConnection conn = {
		.port = port,
		.stop_flag = stop_flag,
	};

	Array *tc = self->tcp_connections;
	tc->add(tc, &conn);

	KarmaTcpCtx *ctx = malloc(sizeof(*ctx));
	ctx->karma = self;
	ctx->sockfd = sockfd;
	ctx->stop_flag = stop_flag;

	thrd_t thread;
	thrd_create(&thread, karma_tcp_listen_loop, ctx);
	printf("Karma is started listening TCP traffic on port %d\n", port);
}

static void
karma_stop_tcp_listen(Karma *self, uint16_t port) {
	Array *tc = self->tcp_connections;
	for (size_t i = 0; i < tc->len; ++i) {
		KarmaTcpConnection *conn = tc->get(tc, i);
		if (conn->port == port) {
			atomic_store(conn->stop_flag, true);
			break;
		}
		printf("Stopped listening for TCP traffic on port %d\n", port);
	}
}

static void
karma_stop_tcp_listen_all(Karma *self) {
	Array *tc = self->tcp_connections;
	for (size_t i = 0; i < tc->len; ++i) {
		KarmaTcpConnection *conn = tc->get(tc, i);
		atomic_store(conn->stop_flag, true);
		printf("Stopped listening TCP traffic\n");
	}
}

#endif