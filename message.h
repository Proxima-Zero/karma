#ifndef KARMA_MESSAGE_H_
#define KARMA_MESSAGE_H_

#include <stdint.h>

typedef struct {
	uint16_t channel;
	uint32_t payload_size;
	void     *payload;
} KarmaMessage;
#endif