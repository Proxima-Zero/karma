#include "karma.h"

static void
release_karma(Karma **pself) {
	free(*pself);
	*pself = NULL;
}

Karma*
form_karma() {
	Karma *karma = malloc(sizeof(*karma));
	for (size_t i = 0; i < MAX_TOPICS; ++i) {
		karma->topics[i].len = 0;
	}

	karma->release = release_karma;

	return karma;
}

