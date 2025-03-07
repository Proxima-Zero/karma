#ifndef KARMA_H_
#define KARMA_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_TOPICS 13
#define MAX_LISTENERS 37

typedef struct {
	uint64_t payload_size;
	uint8_t *payload;
} KarmaMessage;

typedef void (*KarmaListener) (KarmaMessage msg);

typedef struct Karma {
	// TODO: prolly should add some flexibility... or probably we good and just need 
	//       to specify these variables at compile time
	struct {
		size_t len;
		KarmaListener listeners[MAX_LISTENERS];
	} topics[MAX_TOPICS];

	void (*tcp_listen) (struct Karma *self, uint16_t port);

	void (*release) (struct Karma **pself);
} Karma;

Karma* form_karma();

#endif