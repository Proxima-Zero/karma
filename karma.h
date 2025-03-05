#ifndef KARMA_H_
#define KARMA_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAX_TOPICS 13
#define MAX_LISTENERS 37

typedef struct {
	uint64_t payload_size;
	uint8_t *payload;
} KarmaMessage;

typedef void (*KarmaListener) (KarmaMessage msg);

typedef struct {
	// TODO: prolly should add some flexibility... or probably we good and just need 
	//       to specify these variables at compile time
	struct {
		size_t len;
		KarmaListener listeners[MAX_LISTENERS];
	} topics[MAX_TOPICS];
} Karma;

static Karma
form_karma() {
	Karma karma;
	for (size_t i = 0; i < MAX_TOPICS; ++i) {
		karma.topics[i].len = 0;
	}
	return karma;
}


static void
karma_add_listener(Karma *karma, uint16_t topic_id, KarmaListener kl) {
	if (topic_id >= MAX_TOPICS) {
		fprintf(stderr, "topic_id is too high (max=%d)\n", MAX_TOPICS);
		return;
	}

	size_t len = karma->topics[topic_id].len;


	if (len == MAX_LISTENERS) {
		fprintf(stderr, "too much listeners\n");
		return;
	}

	karma->topics[topic_id].listeners[len] = kl;
	karma->topics[topic_id].len++;
}

static void
karma_post_message(Karma karma, uint16_t topic_id, KarmaMessage msg) {
	if (topic_id >= MAX_TOPICS) {
		fprintf(stderr, "topic_id is too high (max=%d)\n", MAX_TOPICS);
		return;
	}

	for (size_t i = 0; i < karma.topics[topic_id].len; ++i) {
		karma.topics[topic_id].listeners[i](msg);
	}
}

#endif