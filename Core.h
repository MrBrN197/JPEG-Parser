
#include <stdint.h>


#if defined _MSC_VER
	#define ASSERT(x) if(!(x)) { __debugbreak();}
	#define ASSERT_LOG(x, ...) if(!(x)) { printf(__VA_ARGS__); __debugbreak(); }
	#include <intrin.h>
#else
	#define ASSERT(x) if(!(x)) { *((int*)0) = 0; }
#endif

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;