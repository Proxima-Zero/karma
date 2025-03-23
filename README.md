# Karma - simple P2P communication abstraction layer protocol (WIP)

## Vision 

Karma is envisioned to be simple library that enables communication through abstract channels between network of karma peers. Karma is strives to abstract both:

1. How far (how many hops) peer can be of each other - that is effectively done by P2P schema in each all peers propagate data to each other (though some QoS and duplicate mitigation mechanism should be implemented too)

2. How karma peer communicates with other hosts? While TCP is probably the most probable option (and the only which is currently developed), there can be plenty of other options - IPC, Bluetooth, Websocket. It is envisioned that Karma can enable to write solutions completely agnostic of media type.

Karma will provide following communication patterns:
1. request/response communication
2. pub/sub without responses
3. direct data streaming

### Rework from old to new version

The main difference between old and new version is that old version was not seen as P2P utility and more like centralized message broker. Thus, I found it much simpler to rewrite it completely - probably using just some parts of all code.

Another reason also that I modified Proxima Codex a lot (most notable change - is explicit passing of memory allocator) and it will be easier probably to start out clean because of that too (though of course - if not the P2P thing - I'd rather just adapted it).