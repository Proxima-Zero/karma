#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "karma.h"
#include "karma_link.h"

void karma_listener(KarmaMessage msg, void *ctx) {
	fwrite(msg.payload, sizeof(uint8_t), msg.payload_size, stdout);
}

KarmaMessage karma_responder(KarmaMessage msg, void *ctx) {
	char *resp_payload = "Response: him Mara will certainly not overthrow, any more than the wind throws down a rocky mountain.\n";
	KarmaMessage resp;

	fwrite(msg.payload, sizeof(uint8_t), msg.payload_size, stdout);

	resp.payload = resp_payload;
	resp.payload_size = strlen(resp.payload) + 1;

	return resp;
}


int main() {
	Karma *karma = form_karma();
	karma->start_tcp_listen(karma, 1337);

	KarmaLink *link = form_direct_link(karma);
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

	sleep(1);
	KarmaMessage msg2;
	char *req = "Request: He who lives without looking for pleasures, his senses well controlled, moderate in his food, faithful and strong...\n";
	msg2.payload_size = strlen(req) + 1;
	msg2.payload = req;

	link->add_responder(link, 1, (KarmaResponder) {
		.cb = karma_responder, 
		.ctx = NULL
	});

	Array *msgs = link->make_request(link, 1, msg2);
	KarmaMessage msg3 = *(KarmaMessage *) msgs->get(msgs, 0);
	fwrite(msg3.payload, sizeof(uint8_t), msg3.payload_size, stdout);

	link->release(&link);
	karma->release(&karma);
	sleep(1);
	return 0;
}