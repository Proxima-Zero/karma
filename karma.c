#include "karma.h"
#include "karma_tcp.h"

static void
release_karma(Karma **pself) {
	Karma *self = *pself;
	for (ssize_t i = 0; i < self->channels_len; ++i) {
		KarmaChannel *channel = self->channels[i];
		if (NULL != channel) {
			channel->release(&channel);
		}
	}

	self->stop_tcp_listen_all(self);
	self->tcp_connections->release(&self->tcp_connections);
	free(*pself);
	*pself = NULL;
}

static KarmaChannel*
karma_get_channel(Karma *karma, uint16_t channel_id) {
	if (channel_id >= karma->channels_len) {
		karma->channels = realloc(karma->channels, sizeof(KarmaChannel *) * (channel_id + 1));
		bzero(karma->channels + karma->channels_len, sizeof(KarmaChannel *) * (channel_id + 1 - karma->channels_len));
		karma->channels_len = channel_id + 1;
	}

	if (NULL == karma->channels[channel_id]) {
		karma->channels[channel_id] = form_karma_channel();
	}

	return karma->channels[channel_id];
}

static void
karma_add_listener(Karma *self, uint16_t channel_id, KarmaListener kl) {
	KarmaChannel *channel = karma_get_channel(self, channel_id);
	channel->add_listener(channel, kl);
}

static void
karma_post_message(Karma *self, uint16_t channel_id, KarmaMessage msg) {
	KarmaChannel *channel = karma_get_channel(self, channel_id);
	channel->post_message(channel, msg);
}

static void
karma_add_responder(Karma *self, uint16_t channel_id, KarmaResponder kr) {
	KarmaChannel *channel = karma_get_channel(self, channel_id);
	channel->add_responder(channel, kr);
}

static Array*/*KarmaMessage*/
karma_make_request(Karma *self, uint16_t channel_id, KarmaMessage msg) {
	KarmaChannel *channel = karma_get_channel(self, channel_id);
	return channel->make_request(channel, msg);
}

Karma*
form_karma() {
	Karma *karma = malloc(sizeof(*karma));

	karma->tcp_connections = form_array(sizeof(KarmaTcpConnection));

	karma->add_listener = karma_add_listener;
	karma->post_message = karma_post_message;
	karma->add_responder = karma_add_responder;
	karma->make_request = karma_make_request;

	// karma_tcp.h
	karma->start_tcp_listen = karma_start_tcp_listen; 
	karma->stop_tcp_listen = karma_stop_tcp_listen;
	karma->stop_tcp_listen_all = karma_stop_tcp_listen_all;

	karma->release = release_karma;

	return karma;
}

