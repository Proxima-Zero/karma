#include "karma.h"
#include "karma_tcp.h"

static void
release_karma(Karma **pself) {
	Karma *self = *pself;
	for (ssize_t i = 0; i < self->topics_len; ++i) {
		KarmaTopic *topic = self->topics[i];
		if (NULL != topic) {
			topic->release(&topic);
		}
	}

	self->stop_tcp_listen_all(self);
	self->tcp_connections->release(&self->tcp_connections);
	free(*pself);
	*pself = NULL;
}

static KarmaTopic*
karma_get_topic(Karma *karma, uint16_t topic_id) {
	if (topic_id >= karma->topics_len) {
		karma->topics = realloc(karma->topics, sizeof(KarmaTopic *) * (topic_id + 1));
		bzero(karma->topics + karma->topics_len, sizeof(KarmaTopic *) * (topic_id + 1 - karma->topics_len));
		karma->topics_len = topic_id + 1;
	}

	if (NULL == karma->topics[topic_id]) {
		karma->topics[topic_id] = form_karma_topic();
	}

	return karma->topics[topic_id];
}

static void
karma_add_listener(Karma *self, uint16_t topic_id, KarmaListener kl) {
	KarmaTopic *topic = karma_get_topic(self, topic_id);
	topic->add_listener(topic, kl);
}

static void
karma_post_message(Karma *self, uint16_t topic_id, KarmaMessage msg) {
	KarmaTopic *topic = karma_get_topic(self, topic_id);
	topic->post_message(topic, msg);
}

static void
karma_add_responder(Karma *self, uint16_t topic_id, KarmaResponder kr) {
	KarmaTopic *topic = karma_get_topic(self, topic_id);
	topic->add_responder(topic, kr);
}

static Array*/*KarmaMessage*/
karma_make_request(Karma *self, uint16_t topic_id, KarmaMessage msg) {
	KarmaTopic *topic = karma_get_topic(self, topic_id);
	return topic->make_request(topic, msg);
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

