#include "karma.h"


Karma*
form_karma() {
	Karma *karma = malloc(sizeof(*karma));
	for (size_t i = 0; i < MAX_TOPICS; ++i) {
		karma->topics[i].len = 0;
	}

	return karma;
}

