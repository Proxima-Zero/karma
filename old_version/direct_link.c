#include "karma_link.h"

#include <stdlib.h>

static void
karma_link_direct_create_listener(KarmaLink *self, uint16_t channel_id, KarmaLinkListener kl) {
	Karma *karma = self->data.direct.karma;
	karma->add_listener(karma, channel_id, (KarmaListener) kl);
}

static void
karma_link_direct_post_message(KarmaLink *self, uint16_t channel_id, KarmaMessage msg) {
	Karma *karma = self->data.direct.karma;
	karma->post_message(karma, channel_id, msg);
}

static void
karma_link_direct_create_responder(KarmaLink *self, uint16_t channel_id, KarmaLinkResponder kr) {
	Karma *karma = self->data.direct.karma;
	return karma->add_responder(karma, channel_id, (KarmaResponder) kr);
}

static Array*/*KarmaMessage*/
karma_link_direct_make_request(KarmaLink *self, uint16_t channel_id, KarmaMessage msg) {
	Karma *karma = self->data.direct.karma;
	return karma->make_request(karma, channel_id, msg);
}

static void
release_karma_link(KarmaLink **pself) {
	free(*pself);
	*pself = NULL;
}


KarmaLink *form_direct_link(Karma *karma) {
	KarmaLink *link = malloc(sizeof(*link)); 
	link->data.direct.karma = karma;
	link->add_listener = karma_link_direct_create_listener;
	link->post_message = karma_link_direct_post_message;

	link->add_responder = karma_link_direct_create_responder;
	link->make_request = karma_link_direct_make_request;

	link->release = release_karma_link;

	return link;
}

