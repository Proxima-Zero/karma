#include "karma_link.h"

#include <stdlib.h>

static void
karma_add_listener(KarmaLink *self, uint16_t topic_id, KarmaListener kl) {
	// TODO: impl
}

static void
karma_post_message(KarmaLink *self, uint16_t topic_id, KarmaMessage msg) {
	// TODO: impl
}

static void
release_karma_link(KarmaLink **pself) {
	free(*pself);
	*pself = NULL;
}

KarmaLink *form_tcp_link(Karma *karma) {
	// TODO: impl
	return NULL;
}

