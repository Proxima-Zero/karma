#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../karma.h"
#include "../karma_link.h"

void karma_listener(KarmaMessage msg, void *ctx) {
	fwrite(msg.payload, sizeof(uint8_t), msg.payload_size, stdout);
}


int main() {
	Karma *karma = form_karma();
	karma->start_tcp_listen(karma, 1337);

	KarmaLink *link = form_direct_link(karma);
	char *post = "POST: If a man speaks or acts with a pure thought, happiness follows him, like a shadow that never leaves him.\n";

	link->add_listener(link, 0, (KarmaListener) {
		.cb = karma_listener, 
		.ctx = NULL
	});

	size_t len = strlen(post);
	KarmaMessage msg;
	msg.payload_size = len + 1;
	msg.payload = post;

	link->post_message(link, 0, msg);

	link->release(&link);
	karma->release(&karma);
	return 0;
}