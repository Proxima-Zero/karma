#ifndef KARMA_LINK_H_
#define KARMA_LINK_H_
#include "karma.h"

#include <arpa/inet.h>

typedef KarmaMessage (*KarmaLinkResponder) (KarmaMessage msg, void *ctx);
typedef void         (*KarmaLinkListener)  (KarmaMessage msg, void *ctx);

typedef struct KarmaLink {
	union {
		struct {
			Karma *karma;
		} direct;
		struct {
			struct sockaddr_in servaddr;
		} tcp;
	} data;
	void          (*add_listener)  (struct KarmaLink *self, uint16_t topic_id, KarmaLinkListener kl, void *ctx);
	void          (*post_message)  (struct KarmaLink *self, uint16_t topic_id, KarmaMessage msg);
	// TODO: remove_listener?
	void          (*add_responder) (struct KarmaLink *self, uint16_t topic_id, KarmaLinkResponder kr, void *ctx);
	KarmaMessages (*make_request)  (struct KarmaLink *self, uint16_t topic_id, KarmaMessage msg);

	void          (*release)       (struct KarmaLink **pself);
} KarmaLink;

KarmaLink *form_direct_link(Karma *karma);
KarmaLink *form_tcp_link(const char *addr, const int port);

#endif