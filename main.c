#include <stdio.h>
#include <string.h>

#include "karma.h"

void karma_listener(KarmaMessage msg) {
	fwrite(msg.payload, sizeof(uint8_t), msg.payload_size, stdout);
}

int main() {
	Karma karma = form_karma();

	karma_add_listener(&karma, 0, karma_listener);

	char *payload = "If a man speaks or acts with a pure thought, happiness follows him, like a shadow that never leaves him.\n";
	size_t len = strlen(payload);
	KarmaMessage msg;
	msg.payload_size = len;
	msg.payload = payload;

	karma_post_message(karma, 0, msg);

	return 0;
}