#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../karma.h"
#include "../karma_link.h"

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

	KarmaMessage msg;
	char *req = "Request: He who lives without looking for pleasures, his senses well controlled, moderate in his food, faithful and strong...\n";
	msg.payload_size = strlen(req) + 1;
	msg.payload = req;

	link->add_responder(link, 1, (KarmaResponder) {
		.cb = karma_responder, 
		.ctx = NULL
	});

	Array *msgs = link->make_request(link, 1, msg);
	KarmaMessage msg2 = *(KarmaMessage *) msgs->get(msgs, 0);
	fwrite(msg2.payload, sizeof(uint8_t), msg2.payload_size, stdout);

	link->release(&link);
	karma->release(&karma);
	return 0;
}