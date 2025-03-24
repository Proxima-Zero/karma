#ifndef KARMA_LINK_H_
#define KARMA_LINK_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Result should be written to response with returning number of bytes.
 **/
typedef size_t (*AnswerCb) (void *payload, uint8_t payload_size, void **response, void *ctx);

typedef struct KarmaLink {
	Allocator *a;
	void      *ctx;

	/**
	 * Send outbound message to karma peer through link which is meant to have a response
	 * Blocking only when karma peer is ready to read and answer 
	 **/
	size_t (*ask)    (struct KarmaLink *self, void* payload, uint8_t payload_size, void *response);

	/**
	 * Process single inbound message if available. from karma peer with callback
	 * Blcoking only if message is available. Otherwise cb will be ignored
	 **/
	void   (*answer)  (struct KarmaLink *self, Answer cb, void *ctx);

	void   (*release) (struct KarmaLink **pself);
} KarmaLink;

#endif