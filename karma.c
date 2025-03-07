#include "karma.h"

static void
karma_add_listener(Karma *self, uint16_t topic_id, KarmaListener kl) {
	if (topic_id >= MAX_TOPICS) {
		fprintf(stderr, "topic_id is too high (max=%d)\n", MAX_TOPICS);
		return;
	}

	size_t len = self->topics[topic_id].len;


	if (len == MAX_LISTENERS) {
		fprintf(stderr, "too much listeners\n");
		return;
	}

	self->topics[topic_id].listeners[len] = kl;
	self->topics[topic_id].len++;
}

static void
karma_post_message(Karma *self, uint16_t topic_id, KarmaMessage msg) {
	if (topic_id >= MAX_TOPICS) {
		fprintf(stderr, "topic_id is too high (max=%d)\n", MAX_TOPICS);
		return;
	}

	for (size_t i = 0; i < self->topics[topic_id].len; ++i) {
		self->topics[topic_id].listeners[i](msg);
	}
}

Karma*
form_karma() {
	Karma *karma = malloc(sizeof(*karma));
	for (size_t i = 0; i < MAX_TOPICS; ++i) {
		karma->topics[i].len = 0;
	}

	karma->add_listener = karma_add_listener;
	karma->post_message = karma_post_message;
	return karma;
}

