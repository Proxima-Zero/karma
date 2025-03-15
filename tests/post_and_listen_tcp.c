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

	KarmaLink *tcp_link = form_tcp_link("127.0.0.1", 1337);
	char *post = "POST: If a man speaks or acts with a pure thought, happiness follows him, like a shadow that never leaves him.\n";

	tcp_link->add_listener(tcp_link, 0, (KarmaListener) {
		.cb = karma_listener, 
		.ctx = NULL
	});

	size_t len = strlen(post);
	KarmaMessage msg;
	msg.payload_size = len + 1;
	msg.payload = post;

	tcp_link->post_message(tcp_link, 0, msg);

	usleep(10000);

	tcp_link->release(&tcp_link);
	karma->release(&karma);
	usleep(10000);
	return 0;
}