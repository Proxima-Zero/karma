#ifndef KARMA_H_
#define KARMA_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef enum {
	KARMA_TCP_TYPE_POST = 1,
	KARMA_TCP_TYPE_LISTEN = 2
} KarmaTcpConnType;

typedef struct {
	KarmaTcpConnType type;
	uint16_t topic_id;
} KarmaTcpConnHeader;

typedef struct {
	uint32_t payload_size;
	uint8_t *payload;
} KarmaMessage;

typedef void (*KarmaListener) (KarmaMessage msg, void *ctx);

typedef struct KarmaTopic {
	size_t listeners_len;
	size_t listeners_cap;
	KarmaListener *listeners;
	void **ctxs;

	void (*add_listener) (struct KarmaTopic *self, KarmaListener kl, void *ctx);
	void (*post_message) (struct KarmaTopic *self, KarmaMessage msg);
	void (*release) (struct KarmaTopic **pself);
} KarmaTopic;

KarmaTopic *form_karma_topic();

typedef struct Karma {
	// TODO: prolly should add some flexibility... or probably we good and just need 
	//       to specify these variables at compile time
	KarmaTopic **topics;
	size_t topics_len;

	void (*add_listener)(struct Karma *self, uint16_t topic_id, KarmaListener kl, void *ctx);
	void (*post_message)(struct Karma *self, uint16_t topic_id, KarmaMessage msg);

	void (*tcp_listen) (struct Karma *self, uint16_t port);

	void (*release) (struct Karma **pself);
} Karma;

Karma* form_karma();

#endif