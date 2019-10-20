#pragma once

#include "core.h"

#include "commandBatch.h"
#include "batchConfig.h"

#include "../buffers/bufferLayout.h"
#include "../shader/shaders.h"

struct GuiCommand {
	// Amount of indices to render
	size_t count;
	// Scissor rectangle
	Vec4f clip;
	// Optional texture ID
	int textureID;
	// Offset in index buffer
	size_t indexOffset;
};

struct GuiVertex {
	Vec2f pos;
	Vec2f uv;
	Vec4f col;
};

class GuiBatch : public CommandBatch<GuiVertex, GuiCommand> {
public:
	GuiBatch() : CommandBatch<GuiVertex, GuiCommand>(
		BatchConfig(
			BufferLayout({
				BufferElement("pos", BufferDataType::FLOAT2),
				BufferElement("uv", BufferDataType::FLOAT2),
				BufferElement("col", BufferDataType::FLOAT4)
			}),
			Renderer::TRIANGLES
		)) {

	}

	void pushCommand(int textureID) {
		size_t count = Batch<GuiVertex>::indexBuffer.size() - currentIndexOffset;

		if (count == 0)
			return;

		Vec4f scissor = Vec4f();
		GuiCommand command = { count, Vec4f(), textureID, currentIndexOffset };

		CommandBatch<GuiVertex, GuiCommand>::pushCommand(command);
	}

	void submit() override {
		if (commandBuffer.size() == 0) {
			pushCommand(0);
		}

		Batch<GuiVertex>::vao->bind();

		Batch<GuiVertex>::vbo->fill((const void *) Batch<GuiVertex>::vertexBuffer.data(), Batch<GuiVertex>::vertexBuffer.size() * sizeof(GuiVertex), Renderer::STREAM_DRAW);
		Batch<GuiVertex>::ibo->fill((const unsigned int *) Batch<GuiVertex>::indexBuffer.data(), Batch<GuiVertex>::indexBuffer.size(), Renderer::STREAM_DRAW);

		for (const GuiCommand& command : commandBuffer) {

			if (command.textureID != 0) {
				Renderer::bindTexture2D(command.textureID);
				GraphicsShaders::guiShader.setTextured(true);
			} else {
				GraphicsShaders::guiShader.setTextured(false);
			}

			// TODO project clip rectangle to framebuffer space
			//Renderer::scissor(command.clip.x, command.clip.y, command.clip.z, command.clip.w);

			Renderer::drawElements(Renderer::TRIANGLES, command.count, Renderer::UINT, (const void *)(intptr_t)(command.indexOffset * sizeof(unsigned int)));

		}

		Batch<GuiVertex>::vbo->unbind();
		Batch<GuiVertex>::ibo->unbind();
		Batch<GuiVertex>::vao->unbind();

		clear();
	}

	void clear() {
		currentIndexOffset = 0;
		Batch<GuiVertex>::clear();
		commandBuffer.clear();
	}
};