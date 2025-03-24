#ifndef KARMA_H_
#define KARMA_H_
#include <stddef.h>

#include "karma/link/link.h"

#include "codex/allocators/allocator.h"


typedef struct Karma {
	Allocator *a;

	/**
	 * Internal function to send bytes to karma p2p network. Shouldn't be used
	 * outside of karma itself or direct link
	 **/
	size_t (*_emit)    (struct Karma *self, void *payload, uint8_t payload_size, void **answer);
	/**
	 * Process single instance of sent message
	 **/
	size_t (*_process) (struct Karma *self, AnswerCb cb, void *ctx);
	void (*release) (struct Karma **pself);
} Karma;

#endif