#include "karma.h"

#include <stdlib.h>
#include <string.h>

#define LISTENER_BASE_CAP 5
#define CAP_MULTIPLIER 2

static void 
karma_topic_add_listener(KarmaTopic *self, KarmaListener kl) {
	Array *ls = self->listeners;
	ls->add(ls, &kl);
	self->listeners = ls;
}

static KarmaMessages
karma_topic_make_request(KarmaTopic *self, KarmaMessage msg) {
	KarmaMessages resps;
	resps.msgs = malloc(sizeof(KarmaMessage) * self->responders_len);
	resps.len = 0;
	for (size_t i = 0; i < self->responders_len; ++i) {
		void *ctx = self->rctxs[i];
		resps.msgs[resps.len++] = self->responders[i](msg, ctx);
	}

	return resps;
}

static void 
karma_topic_add_responder(KarmaTopic *self, KarmaResponder kr, void *ctx) {
	if (self->responders_len == self->responders_cap) {
		self->responders_cap *= CAP_MULTIPLIER;
		self->responders = realloc(self->responders, sizeof(KarmaResponder) * self->responders_cap);
		self->rctxs = realloc(self->rctxs, sizeof(void*) * self->responders_cap);
	}

	self->responders[self->responders_len] = kr;
	self->rctxs[self->responders_len] = ctx;
	self->responders_len++;
}

static void
karma_topic_post_message(KarmaTopic *self, KarmaMessage msg) {
	Array *ls = self->listeners; 
	for (size_t i = 0; i < ls->len; ++i) {
		KarmaListener *kl = ls->get(ls, i);
		kl->cb(msg, kl->ctx);
	}
}

static void
karma_topic_release(KarmaTopic **pself) {
	Array *ls = (*pself)->listeners;
	ls->release(&ls);
	free((*pself)->responders);
	free((*pself)->rctxs);
	free(*pself);

	*pself = NULL;
}

KarmaTopic *form_karma_topic() {
	KarmaTopic *topic = malloc(sizeof(KarmaTopic));
	topic->responders_len = 0;
	topic->responders_cap = LISTENER_BASE_CAP;
	topic->responders = malloc(sizeof(KarmaListener) * topic->responders_cap);
	topic->rctxs = malloc(sizeof(void *) * topic->responders_cap);

	topic->listeners = form_array(sizeof(KarmaListener));

	topic->add_listener = karma_topic_add_listener;
	topic->post_message = karma_topic_post_message;
	topic->add_responder = karma_topic_add_responder;
	topic->make_request = karma_topic_make_request;
	topic->release = karma_topic_release;

	return topic;
}
