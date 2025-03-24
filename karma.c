#include "karma/karma.h"
#include <stdbool.h>

static void
karma_listen_to_channel(struct Karma *self, uint16_t channel, KarmaChannelListener kcl, void *ctx) {
	Array *cs = self->channels;

	// TODO: optimize, RBTree probably
	bool found = false;
	for (size_t i = 0; i < cs->len; ++i) {
		if ()
	}
}

cosnt Karma KARMA_PROTOTYPE = {
.emit =
}

Karma *form_karma(Allocator *a) {

}