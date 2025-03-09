#include "karma.h"

#include <stdlib.h>
#include <string.h>

#define LISTENER_BASE_CAP 5
#define CAP_MULTIPLIER 2

static void 
karma_topic_add_listener(KarmaTopic *self, KarmaListener kl, void *ctx) {
	if (self->listeners_len == self->listeners_cap) {
		self->listeners_cap *= CAP_MULTIPLIER;
		self->listeners = realloc(self->listeners, sizeof(KarmaListener) * self->listeners_cap);
		self->ctxs = realloc(self->ctxs, sizeof(void*) * self->listeners_cap);
	}

	self->listeners[self->listeners_len] = kl;
	self->ctxs[self->listeners_len] = ctx;
	self->listeners_len++;
}

static void
karma_topic_post_message(KarmaTopic *self, KarmaMessage msg) {
	for (size_t i = 0; i < self->listeners_len; ++i) {
		void *ctx = self->ctxs[i];
		self->listeners[i](msg, ctx);
	}
}

static void
karma_topic_release(KarmaTopic **pself) {
	free((*pself)->listeners);
	free((*pself)->ctxs);
	free(*pself);

	*pself = NULL;
}

KarmaTopic *form_karma_topic() {
	KarmaTopic *topic = malloc(sizeof(KarmaTopic));
	topic->listeners_len = 0;
	topic->listeners_cap = LISTENER_BASE_CAP;
	topic->listeners = malloc(sizeof(KarmaListener) * topic->listeners_cap);
	topic->ctxs = malloc(sizeof(void *) * topic->listeners_cap);

	topic->add_listener = karma_topic_add_listener;
	topic->post_message = karma_topic_post_message;
	topic->release = karma_topic_release;

	return topic;
}
