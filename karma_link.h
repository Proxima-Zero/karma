#ifndef KARMA_LINK_H_
#define KARMA_LINK_H_
#include "karma.h"

#include <arpa/inet.h>
#include <stdatomic.h>

typedef KarmaResponder KarmaLinkResponder;

typedef KarmaListener KarmaLinkListener;

typedef struct {
	uint16_t topic_id;
	atomic_bool *stop_flag;
} KarmaLinkTcpConnection;

typedef struct KarmaLink {
	union {
		struct {
			Karma *karma;
		} direct;
		struct {
			struct sockaddr_in servaddr;
			Array/*KarmaLinkTcpConnection*/ *connections;
		} tcp;
	} data;
	void                   (*add_listener)   (struct KarmaLink *self, uint16_t channel_id, KarmaLinkListener kl);
	void                   (*post_message)   (struct KarmaLink *self, uint16_t channel_id, KarmaMessage msg);
	// TODO: remove_listener?
	void                   (*add_responder)  (struct KarmaLink *self, uint16_t channel_id, KarmaLinkResponder kr);
	Array*/*KarmaMessage*/ (*make_request)   (struct KarmaLink *self, uint16_t channel_id, KarmaMessage msg);

	// stops both listening and responding for any listeners on a given channel
	void                   (*stop_listening) (struct KarmaLink *self, uint16_t channel_id);

	void                   (*release)        (struct KarmaLink **pself);
} KarmaLink;

KarmaLink *form_direct_link(Karma *karma);
KarmaLink *form_tcp_link(const char *addr, const int port);

#endif