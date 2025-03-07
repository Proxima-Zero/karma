#include "karma_link.h"

#include <stdlib.h>

static void
karma_link_direct_create_listener(KarmaLink *self, uint16_t topic_id, KarmaLinkListener kl, void *ctx) {
	Karma *karma = self->data.direct.karma;
	// for direct link KarmaLinkListener and KarmaListener are the same
	karma->add_listener(karma, topic_id, (KarmaListener) kl, ctx);
}

static void
karma_link_direct_post_message(KarmaLink *self, uint16_t topic_id, KarmaMessage msg) {
	Karma *karma = self->data.direct.karma;
	karma->post_message(karma, topic_id, msg);
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

	link->release = release_karma_link;

	return link;
}

