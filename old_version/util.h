#ifndef KARMA_UTIL_H_
#define KARMA_UTIL_H_

static inline uint64_t 
htonll(uint64_t val) {
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return ((uint64_t) htonl(val & 0xffffffff) << 32) | htonl(val >> 32);
	#else
		return val;
	#endif
}

static inline uint64_t
ntohll(uint64_t val) {
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return ((uint64_t) ntohl(val & 0xffffffff) << 32) | ntohl(val >> 32);
	#else
		return val;
	#endif
}

#endif