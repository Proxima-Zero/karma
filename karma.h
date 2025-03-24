#ifndef KARMA_H_
#define KARMA_H_
#include <stddef.h>
#include <stdint.h>

#include "karma/link/link.h"
#include "karma/message.h"

#include "codex/allocators/allocator.h"
#include "codex/ds/array.h"

typedef enum {
	// channel is free to usage. every message to it will be discarded because there is no
	// listeners (local of foreign)
	KCHANNEL_MODE_VOID = 0,
	// channel is listened locally and local karma has direct callback to respond to messages
	KCHANNEL_MODE_LOCAL = 1,
	// channel is listened by foreign karma peer. every message will be propagated elsewhere
	KCHANNEL_MODE_FOREIGN = 2
} KarmaChannelMode;

typedef struct KarmaChannel {
	uint16_t channel;

	union {
		struct {
			KarmaChannelListener kcl;
			void *ctx;
		} local;
		struct {
			KarmaLink link;
		} foreign;
	} ctx;
} KarmaChannel;

typedef KarmaMessage (KarmaChannelListener *) (KarmaMessage msg);

typedef struct Karma {
	Allocator *a;

	Array/*KarmaChannel*/ *channels; 
	Array/*KarmaLink*/    *links;

	KarmaMessage (*post_to_channel)   (struct Karma *self, uint16_t channel, KarmaMessage msg);
	void         (*listen_to_channel) (struct Karma *self, uint16_t channel, 
		                               KarmaChannelListener kcl, void *ctx);

	/**
	 * Internal function to send bytes to karma p2p network. Shouldn't be used
	 * outside of karma itself or direct link
	 **/
	size_t (*_emit)    (struct Karma *self, void *payload, uint8_t payload_size, void **answer);
	/**
	 * Process single instance of incoming message
	 **/
	size_t (*_process) (struct Karma *self, AnswerCb cb, void *ctx);
	void (*release) (struct Karma **pself);
} Karma;

Karma *form_karma(Allocator *a);

#endif