#pragma once

#include "../math/linalg/vec.h"
#include "../rigidBody.h"
#include "../part.h"

namespace P3D {

	class SoftLink {
	public:
		AttachedPart attachedPartA;
		AttachedPart attachedPartB;

		SoftLink(const SoftLink& other) = delete;
		SoftLink& operator=(const SoftLink& other) = delete;
		SoftLink(SoftLink&& other) = delete;
		SoftLink& operator=(SoftLink&& other) = delete;

		virtual ~SoftLink();
		virtual void update() = 0;

		SoftLink(const AttachedPart& attachedPartA, const AttachedPart& attachedPartB);

		GlobalCFrame getGlobalCFrameOfAttachmentA() const;
		GlobalCFrame getGlobalCFrameOfAttachmentB() const;

		CFrame getLocalCFrameOfAttachmentA() const;
		CFrame getLocalCFrameOfAttachmentB() const;

		CFrame getRelativeOfAttachmentA() const;
		CFrame getRelativeOfAttachmentB() const;

		Position getGlobalPositionOfAttachmentA() const;
		Position getGlobalPositionOfAttachmentB() const;

		Vec3 getLocalPositionOfAttachmentA() const;
		Vec3 getLocalPositionOfAttachmentB() const;

		Vec3 getRelativePositionOfAttachmentA() const;
		Vec3 getRelativePositionOfAttachmentB() const;
	};
};
