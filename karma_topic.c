#include "karma.h"

#include <stdlib.h>
#include <string.h>

#define LISTENER_BASE_CAP 5
#define CAP_MULTIPLIER 2

static void 
karma_topic_add_listener(KarmaTopic *self, KarmaListener kl) {
	Array *ls = self->listeners;
	ls->add(ls, &kl);
}

static Array*/*KarmaMessage*/
karma_topic_make_request(KarmaTopic *self, KarmaMessage msg) {
	// TODO: optimize? there can be a lot of responses... allocate each time?
	Array *resps = form_array(sizeof(KarmaMessage));
	Array *rs = self->responders;
	for (size_t i = 0; i < rs->len; ++i) {
		KarmaResponder *kr = rs->get(rs, i);
		KarmaMessage msg = kr->cb(msg, kr->ctx);
		resps->add(resps, &msg);
	}

	return resps;
}

static void 
karma_topic_add_responder(KarmaTopic *self, KarmaResponder kr) {
	Array *rs = self->responders;
	rs->add(rs, &kr);
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
	Array *rs = (*pself)->responders;
	rs->release(&rs);
	free(*pself);

	*pself = NULL;
}

KarmaTopic *form_karma_topic() {
	KarmaTopic *topic = malloc(sizeof(KarmaTopic));
	topic->responders = form_array(sizeof(KarmaResponder));
	topic->listeners = form_array(sizeof(KarmaListener));

	topic->add_listener = karma_topic_add_listener;
	topic->post_message = karma_topic_post_message;
	topic->add_responder = karma_topic_add_responder;
	topic->make_request = karma_topic_make_request;
	topic->release = karma_topic_release;

	return topic;
}
