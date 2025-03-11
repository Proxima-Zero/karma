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

static KarmaMessages
karma_topic_make_request(KarmaTopic *self, KarmaMessage msg) {
	KarmaMessages resps;
	resps.msgs = malloc(sizeof(KarmaMessage) * self->responders_len);
	resps.len = 0;
	for (size_t i = 0; i < self->responders_len; ++i) {
		void *ctx = self->ctxs[i];
		resps.msgs[resps.len++] = self->responders[i](msg, ctx);
	}

	return resps;
}

static void 
karma_topic_add_responder(KarmaTopic *self, KarmaResponder kr, void *ctx) {
	if (self->responders_len == self->responders_cap) {
		self->responders_cap *= CAP_MULTIPLIER;
		self->responders = realloc(self->responders, sizeof(KarmaResponder) * self->responders_cap);
		self->ctxs = realloc(self->ctxs, sizeof(void*) * self->responders_cap);
	}

	self->responders[self->responders_len] = kr;
	self->ctxs[self->responders_len] = ctx;
	self->responders_len++;
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
	topic->responders_len = 0;
	topic->responders_cap = LISTENER_BASE_CAP;
	topic->responders = malloc(sizeof(KarmaListener) * topic->responders_cap);
	topic->rctxs = malloc(sizeof(void *) * topic->responders_cap);

	topic->listeners_len = 0;
	topic->listeners_cap = LISTENER_BASE_CAP;
	topic->listeners = malloc(sizeof(KarmaListener) * topic->listeners_cap);
	topic->ctxs = malloc(sizeof(void *) * topic->listeners_cap);

	topic->add_listener = karma_topic_add_listener;
	topic->post_message = karma_topic_post_message;
	topic->add_responder = karma_topic_add_responder;
	topic->make_request = karma_topic_make_request;
	topic->release = karma_topic_release;

	return topic;
}
