#include "cpuid.h"



#ifdef _WIN32
#include <intrin.h>
#endif

#include <stdint.h>

namespace Util {

class CPUID {
	uint32_t regs[4];

public:
	explicit CPUID(unsigned i, unsigned ecx) {
#ifdef _WIN32
		__cpuidex(( int*) regs, ( int) i, ( int) ecx);

#else
		asm volatile
			("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
			 : "a" (i), "c" (ecx));
#endif
	}

	const uint32_t& EAX() const { return regs[0]; }
	const uint32_t& EBX() const { return regs[1]; }
	const uint32_t& ECX() const { return regs[2]; }
	const uint32_t& EDX() const { return regs[3]; }
};

CPUIDCheck::CPUIDCheck() : available(0) {
	CPUID cpuid0(0, 0);
	if(cpuid0.EAX() >= 1) {
		CPUID cpuid1(1, 0);

		if(cpuid1.EDX() & (1 << 23)) this->available |= MMX;
		if(cpuid1.EDX() & (1 << 25)) this->available |= SSE;
		if(cpuid1.EDX() & (1 << 26)) this->available |= SSE2;
		if(cpuid1.ECX() & (1 << 0)) this->available |= SSE3;
		if(cpuid1.ECX() & (1 << 9)) this->available |= SSSE3;
		if(cpuid1.ECX() & (1 << 19)) this->available |= SSE4_1;
		if(cpuid1.ECX() & (1 << 20)) this->available |= SSE4_2;
		if(cpuid1.ECX() & (1 << 28)) this->available |= AVX;
		if(cpuid1.ECX() & (1 << 12)) this->available |= FMA;
	}

	if(cpuid0.EAX() >= 7) {
		CPUID cpuid7(7, 0);

		if(cpuid7.EBX() & (1 << 5)) this->available |= AVX2;
		if(cpuid7.EBX() & (1 << 16)) this->available |= AVX512_F;
	}
}

CPUIDCheck CPUIDCheck::availableCPUHardware;
};
