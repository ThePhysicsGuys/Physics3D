#pragma once

#define FOR_ALL(type, name) type name = forAll<type>(#name)

void dothing() {
	FOR_ALL(Vec3, v1);
}