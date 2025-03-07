#include "karma_link.h"

#include <stdlib.h>

static void
karma_add_listener(KarmaLink *self, uint16_t topic_id, KarmaListener kl) {
	Karma *karma = self->data.direct.karma;
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
karma_post_message(KarmaLink *self, uint16_t topic_id, KarmaMessage msg) {
	Karma *karma = self->data.direct.karma;
	if (topic_id >= MAX_TOPICS) {
		fprintf(stderr, "topic_id is too high (max=%d)\n", MAX_TOPICS);
		return;
	}

	for (size_t i = 0; i < karma->topics[topic_id].len; ++i) {
		karma->topics[topic_id].listeners[i](msg);
	}
}

KarmaLink *form_direct_link(Karma *karma) {
	KarmaLink *link = malloc(sizeof(*link)); 
	link->data.direct.karma = karma;
	link->add_listener = karma_add_listener;
	link->post_message = karma_post_message;

	return link;
}

