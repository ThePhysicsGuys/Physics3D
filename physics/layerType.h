#pragma once

enum class LayerType : int {
	OBJECT = 0,
	TERRAIN = 1,
	
};

struct LayerIndex {
	int index;

	constexpr LayerIndex(int layerIndex, LayerType type) : index(layerIndex << 2 | static_cast<int>(type)) {}

	inline constexpr LayerType getType() const noexcept {
		return static_cast<LayerType>(index & 0b00000011);
	}
};

inline constexpr bool operator==(LayerIndex a, LayerIndex b) noexcept { return a.index == b.index; }
inline constexpr bool operator!=(LayerIndex a, LayerIndex b) noexcept { return a.index != b.index; }
