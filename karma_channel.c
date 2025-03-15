#include "karma.h"

#include <stdlib.h>
#include <string.h>

#define LISTENER_BASE_CAP 5
#define CAP_MULTIPLIER 2

static void 
karma_channel_add_listener(KarmaChannel *self, KarmaListener kl) {
	Array *ls = self->listeners;
	ls->add(ls, &kl);
}

static void
karma_channel_remove_listener(KarmaChannel *self, KarmaListener kl) {
	Array *ls = self->listeners;
	for (size_t i = 0; i < ls->len; ++i) {
		KarmaListener *kl2 = ls->get(ls, i);
		if (kl.cb == kl2->cb && kl.ctx == kl2->ctx) {
			ls->fremove(ls, i--);
		}
	}
}

static Array*/*KarmaMessage*/
karma_channel_make_request(KarmaChannel *self, KarmaMessage msg) {
	// TODO: optimize? there can be a lot of responses... allocate each time?
	Array *resps = form_array(sizeof(KarmaMessage));
	Array *rs = self->responders;
	for (size_t i = 0; i < rs->len; ++i) {
		KarmaResponder *kr = rs->get(rs, i);
		KarmaMessage respMsg = kr->cb(msg, kr->ctx);
		resps->add(resps, &respMsg);
	}

	return resps;
}

static void 
karma_channel_add_responder(KarmaChannel *self, KarmaResponder kr) {
	Array *rs = self->responders;
	rs->add(rs, &kr);
}

static void
karma_channel_remove_responder(KarmaChannel *self, KarmaResponder kr) {
	Array *rs = self->responders;
	for (size_t i = 0; i < rs->len; ++i) {
		KarmaResponder *kr2 = rs->get(rs, i);
		if (kr.cb == kr2->cb && kr.ctx == kr2->ctx) {
			rs->fremove(rs, i--);
		}
	}
}

static void
karma_channel_post_message(KarmaChannel *self, KarmaMessage msg) {
	Array *ls = self->listeners; 
	for (size_t i = 0; i < ls->len; ++i) {
		KarmaListener *kl = ls->get(ls, i);
		kl->cb(msg, kl->ctx);
	}
}

static void
karma_channel_release(KarmaChannel **pself) {
	Array *ls = (*pself)->listeners;
	ls->release(&ls);
	Array *rs = (*pself)->responders;
	rs->release(&rs);
	free(*pself);

	*pself = NULL;
}

KarmaChannel *form_karma_channel() {
	KarmaChannel *channel = malloc(sizeof(KarmaChannel));
	channel->responders = form_array(sizeof(KarmaResponder));
	channel->listeners = form_array(sizeof(KarmaListener));

	channel->add_listener = karma_channel_add_listener;
	channel->remove_listener = karma_channel_remove_listener;
	channel->post_message = karma_channel_post_message;
	channel->add_responder = karma_channel_add_responder;
	channel->remove_responder = karma_channel_remove_responder;
	channel->make_request = karma_channel_make_request;
	channel->release = karma_channel_release;

	return channel;
}
