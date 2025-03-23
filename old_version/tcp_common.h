#ifndef KARMA_TCP_COMMON_H_
#define KARMA_TCP_COMMON_H_
#include "karma.h"

#include <stdio.h>

static inline int
recv_karma_message(int sockfd, KarmaMessage *msg) {
	uint32_t nrecv = recv(sockfd, &(msg->payload_size), sizeof(msg->payload_size), 0);

	if (nrecv != sizeof(msg->payload_size)) {
		fprintf(stderr, "error getting message payload size\n");
		// TODO: error codes?
		return -1;
	}

	uint32_t npayload = msg->payload_size;
	msg->payload_size = ntohl(msg->payload_size);
	// TODO: custom allocator
	msg->payload = malloc(msg->payload_size);

	// TODO: read in loop
	nrecv = recv(sockfd, msg->payload, msg->payload_size, 0);
	if (nrecv != msg->payload_size) {
		free(msg->payload);
		fprintf(stderr, "error getting message payload\n");
		return -1;
	}

	return 0;
}

static inline int
send_karma_message(int sockfd, KarmaMessage msg) {
	uint32_t npayload = htonl(msg.payload_size);
	if (send(sockfd, &npayload, sizeof(npayload), 0) != sizeof(npayload)) {
		fprintf(stderr, "error sending message payload size\n");
		return -1;
	}

	if (send(sockfd, msg.payload, msg.payload_size, 0) != msg.payload_size) {
		fprintf(stderr, "error sending message payload\n");
		return -1;
	}

	return 0;
}

#endif