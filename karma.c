#include "karma.h"

static void
release_karma(Karma **pself) {
	free(*pself);
	*pself = NULL;
}

static void 
karma_tcp_listen(struct Karma *self, uint16_t port) {
	// TODO: impl
}

Karma*
form_karma() {
	Karma *karma = malloc(sizeof(*karma));
	for (size_t i = 0; i < MAX_TOPICS; ++i) {
		karma->topics[i].len = 0;
	}

	karma->release = release_karma;
	karma->tcp_listen = karma_tcp_listen;

	return karma;
}

