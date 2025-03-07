#include <stdio.h>
#include <string.h>

#include "karma.h"
#include "karma_link.h"

void karma_listener(KarmaMessage msg, void *ctx) {
	fwrite(msg.payload, sizeof(uint8_t), msg.payload_size, stdout);
}

int main() {
	Karma *karma = form_karma();
	// karma->tcp_listen(karma, 1337);

	KarmaLink *link = form_direct_link(karma);

	link->add_listener(link, 0, karma_listener, NULL);

	char *payload = "If a man speaks or acts with a pure thought, happiness follows him, like a shadow that never leaves him.\n";
	size_t len = strlen(payload);
	KarmaMessage msg;
	msg.payload_size = len;
	msg.payload = payload;

	link->post_message(link, 0, msg);

	link->release(&link);
	karma->release(&karma);
	return 0;
}