#ifndef KARMA_BEACON_H_
#define KARMA_BEACON_H_
#include "karma/link/link.h"

typedef void (*IncomingKarmaLinkCb) (KarmaLink *link);

typedef struct KarmaBeacon {
	Allocator *allocator;
	void *ctx;

	void (*enable) (struct KarmaBeacon *self, IncomingKarmaLinkCb cb);
	void (*disable) (struct KarmaBeacon *self);

	void (*release) (struct KarmaBeacon **pself);

} KarmaBeacon;

#endif