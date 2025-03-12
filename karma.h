#ifndef KARMA_H_
#define KARMA_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// TODO: dep management
#include "../codex/array.h"

typedef enum {
	// just post a message don't caring if anyone respond to it
	KARMA_MSG_TYPE_POST    = 1,
	// setup listener for a topic
	KARMA_MSG_TYPE_LISTEN  = 2,
	// post a message that need immediate response
	KARMA_MSG_TYPE_REQUEST = 3,
	// setup listener to respond for requests
	KARMA_MSG_TYPE_RESPOND = 4
} KarmaTcpConnType;

typedef struct {
	KarmaTcpConnType type;
	uint16_t         topic_id;
} KarmaTcpConnHeader;

typedef struct {
	uint32_t payload_size;
	uint8_t  *payload;
} KarmaMessage;

typedef struct KarmaMessages {
	KarmaMessage *msgs;
	size_t       len;
} KarmaMessages;

typedef KarmaMessage (*KarmaResponder) (KarmaMessage msg, void *ctx);

typedef struct {
	void *ctx;

	void (*cb) (KarmaMessage msg, void *ctx);
} KarmaListener;

typedef struct KarmaTopic {
	size_t         responders_len;
	size_t         responders_cap;
	KarmaResponder *responders;
	void           **rctxs;

	Array/*KarmaListener*/ *listeners;

	void          (*add_listener)  (struct KarmaTopic *self, KarmaListener kl);
	void          (*add_responder) (struct KarmaTopic *self, KarmaResponder kr, void *ctx);
	void          (*post_message)  (struct KarmaTopic *self, KarmaMessage msg);
	KarmaMessages (*make_request)  (struct KarmaTopic *self, KarmaMessage msg);

	void          (*release)       (struct KarmaTopic **pself);
} KarmaTopic;

KarmaTopic *form_karma_topic();

typedef struct Karma {
	KarmaTopic **topics;
	size_t topics_len;

	void          (*add_listener)  (struct Karma *self, uint16_t topic_id, KarmaListener kl);
	void          (*add_responder) (struct Karma *self, uint16_t topic_id, KarmaResponder kr, void *ctx);
	void          (*post_message)  (struct Karma *self, uint16_t topic_id, KarmaMessage msg);
	KarmaMessages (*make_request)  (struct Karma *self, uint16_t topic_id, KarmaMessage msg);

	void          (*tcp_listen)    (struct Karma *self, uint16_t port);

	void          (*release)       (struct Karma **pself);
} Karma;

Karma* form_karma();

#endif