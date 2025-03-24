// of course this one is more like PoC and probably not gives much
// but it helps to figure out a good API for Karma and KarmaLink
#ifndef KARMA_DIRECT_LINK_H_
#define KARMA_DIRECT_LINK_H_
#include "karma/karma.h"

#include "codex/allocators/allocator.h"

typedef struct {
	Karma *karma;
} DirectLinkCtx;

static void direct_link_ask(KarmaLink *self, void *payload, size_t payload_size, void *responde) {
	DirectLinkCtx *ctx = self->ctx;	

	// TODO: implement sending message to karma
}

static void direct_link_release(KarmaLink **pself) {
	KarmaLink *self = *pself;
	Allocator *a = self->a;
	a->free(a, self->ctx);
	a->free(a, self);
	*pself = NULL;
}

const KarmaLink DIRECT_LINK_PROTOTYPE = {
	.release = direct_link_release
}

KarmaLink* form_direct_link(Allocator *a, Karma *karma) {
	KarmaLink *kl = a->alloc(a, sizeof(KarmaLink));
	*kl = DIRECT_LINK_PROTOTYPE;
	DirectLinkCtx *ctx = a->alloc(a, sizeof(DirectLinkCtx));
	ctx->karma = karma;

	kl->ctx = ctx;

	return kl;
}
#endif