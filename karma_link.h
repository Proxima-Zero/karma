#ifndef KARMA_LINK_H_
#define KARMA_LINK_H_
#include "karma.h"

#include <arpa/inet.h>

typedef void (*KarmaLinkListener) (KarmaMessage msg, void *ctx);

typedef struct KarmaLink {
	union {
		struct {
			Karma *karma;
		} direct;
		struct {
			struct sockaddr_in servaddr;
		} tcp;
	} data;
	void (*add_listener)(struct KarmaLink *self, uint16_t topic_id, KarmaLinkListener kl, void *ctx);
	void (*post_message)(struct KarmaLink *self, uint16_t topic_id, KarmaMessage msg);
	// TODO: remove_listener?
	// TODO: request/response

	void (*release) (struct KarmaLink **pself);
} KarmaLink;

KarmaLink *form_direct_link(Karma *karma);
KarmaLink *form_tcp_link(const char *addr, const int port);

#endif