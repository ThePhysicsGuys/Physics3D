#pragma once

class CPUIDCheck {
	unsigned int available;

	CPUIDCheck();

	static const CPUIDCheck availableCPUHardware; // statically initialized

public:
	static constexpr unsigned int MMX = 1 << 0;
	static constexpr unsigned int SSE = 1 << 1;
	static constexpr unsigned int SSE2 = 1 << 2;
	static constexpr unsigned int SSE3 = 1 << 3;
	static constexpr unsigned int SSSE3 = 1 << 4;
	static constexpr unsigned int SSE4_1 = 1 << 5;
	static constexpr unsigned int SSE4_2 = 1 << 6;
	static constexpr unsigned int AVX = 1 << 7;
	static constexpr unsigned int AVX2 = 1 << 8;
	static constexpr unsigned int FMA = 1 << 9;
	static constexpr unsigned int AVX512_F = 1 << 10;

	// usage: hasTechnology(SSE | SSE2 | AVX | FMA)
	// returns true if all given technologies are available
	inline static bool hasTechnology(unsigned int technologies) {
		return (availableCPUHardware.available & technologies) == technologies;
	}
};
