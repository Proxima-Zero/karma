// of course this one is more like PoC and probably not gives much
// but it helps to figure out a good API for Karma and KarmaLink
#ifndef KARMA_DIRECT_LINK_H_
#define KARMA_DIRECT_LINK_H_
#include "karma/karma.h"

#include "codex/allocators/allocator.h"

typedef struct {
	// NOTE: it is "remote" karma
	Karma *karma;
} DirectLinkCtx;

static size_t direct_link_ask(KarmaLink *self, void *payload, size_t payload_size, void *response) {
	DirectLinkCtx *ctx = self->ctx;	

	Karma *karma = ctx->karma;

	return karma->_emit(self, payload, payload_size, response);
}

static size_t direct_link_answer(KarmaLink *self, Answer cb, void *ctx) {
	
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