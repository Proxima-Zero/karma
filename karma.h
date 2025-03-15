#ifndef KARMA_H_
#define KARMA_H_

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: dep management
#include "deps/codex/array.h"

typedef enum {
	// just post a message don't caring if anyone respond to it
	KARMA_MSG_TYPE_POST    = 1,
	// setup listener for a channel
	KARMA_MSG_TYPE_LISTEN  = 2,
	// post a message that need immediate response
	KARMA_MSG_TYPE_REQUEST = 3,
	// setup listener to respond for requests
	KARMA_MSG_TYPE_RESPOND = 4
} KarmaTcpConnType;

typedef struct {
	KarmaTcpConnType type;
	uint16_t         channel_id;
} KarmaTcpConnHeader;

typedef struct {
	uint32_t payload_size;
	uint8_t  *payload;
} KarmaMessage;

typedef struct KarmaMessages {
	KarmaMessage *msgs;
	size_t       len;
} KarmaMessages;

typedef struct {
	void *ctx;

	KarmaMessage (*cb) (KarmaMessage msg, void *ctx);
} KarmaResponder;

typedef struct {
	void *ctx;

	void (*cb) (KarmaMessage msg, void *ctx);
} KarmaListener;

typedef struct KarmaChannel {
	Array/*KarmaResponder*/ *responders;
	Array/*KarmaListener*/  *listeners;

	void                   (*add_listener)     (struct KarmaChannel *self, KarmaListener kl);
	void                   (*remove_listener)  (struct KarmaChannel *self, KarmaListener kl);
	void                   (*add_responder)    (struct KarmaChannel *self, KarmaResponder kr);
	void                   (*remove_responder) (struct KarmaChannel *self, KarmaResponder kr);
	void                   (*post_message)     (struct KarmaChannel *self, KarmaMessage msg);
	Array*/*KarmaMessage*/ (*make_request)     (struct KarmaChannel *self, KarmaMessage msg);

	void                   (*release) (struct KarmaChannel **pself);
} KarmaChannel;

KarmaChannel *form_karma_channel();

typedef struct {
	uint16_t port;
	atomic_bool *stop_flag;
} KarmaTcpConnection;

typedef struct Karma {
	KarmaChannel                **channels;
	size_t                      channels_len;
	Array/*KarmaTcpConnection*/ *tcp_connections;

	void                   (*add_listener)     (struct Karma *self, uint16_t channel_id, KarmaListener kl);
	void                   (*remove_listener)  (struct Karma *self, uint16_t channel_id, KarmaListener kl);
	void                   (*add_responder)    (struct Karma *self, uint16_t channel_id, KarmaResponder kr);
	void                   (*remove_responder) (struct Karma *self, uint16_t channel_id, KarmaResponder kr);
	void                   (*post_message)     (struct Karma *self, uint16_t channel_id, KarmaMessage msg);
	Array*/*KarmaMessage*/ (*make_request)     (struct Karma *self, uint16_t channel_id, KarmaMessage msg);

	void                   (*start_tcp_listen)    (struct Karma *self, uint16_t port);
	void                   (*stop_tcp_listen)     (struct Karma *self, uint16_t port);
	void                   (*stop_tcp_listen_all) (struct Karma *self);

	void                   (*release) (struct Karma **pself);
} Karma;

Karma* form_karma();

#endif