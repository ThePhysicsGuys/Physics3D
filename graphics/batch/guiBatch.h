#pragma once

#include "commandBatch.h"
#include "batchConfig.h"
#include "../gui/color.h"
#include "../buffers/bufferLayout.h"
#include "../shader/shaders.h"

namespace P3D::Graphics {

struct GuiCommand {
	// Amount of indices to render
	size_t count;
	// Optional texture ID
	GLID textureID;
	// Offset in index buffer
	size_t indexOffset;
};

struct GuiVertex {
	Vec2f pos;
	Vec2f uv;
	Color col;
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

	void pushCommand(GLID textureID) {
		size_t count = Batch<GuiVertex>::indexBuffer.size() - currentIndexOffset;

		if (count == 0)
			return;

		GuiCommand command = { count, textureID, currentIndexOffset };

		CommandBatch<GuiVertex, GuiCommand>::pushCommand(command);
	}

	void submit() override {
		Shaders::guiShader->bind();

		if (commandBuffer.empty()) 
			pushCommand(0);

		Batch<GuiVertex>::vao->bind();

		Batch<GuiVertex>::vbo->fill((const void*) Batch<GuiVertex>::vertexBuffer.data(), Batch<GuiVertex>::vertexBuffer.size() * sizeof(GuiVertex), Renderer::STREAM_DRAW);
		Batch<GuiVertex>::ibo->fill((const unsigned int*) Batch<GuiVertex>::indexBuffer.data(), Batch<GuiVertex>::indexBuffer.size(), Renderer::STREAM_DRAW);

		GLID lastID = -1;
		size_t lastCount = 0;
		size_t lastIndexOffset = 0;

		for (const GuiCommand& command : commandBuffer) {
			GLID ID = command.textureID;
			size_t count = command.count;
			size_t indexOffset = command.indexOffset;

			if (ID == lastID) {
				// merge calls, no shader update
				lastCount += count;
			} else {
				// render merged calls, shader update, texture bind, render this call
				Renderer::drawElements(Renderer::TRIANGLES, lastCount, Renderer::UINT, (const void*) (intptr_t) (lastIndexOffset * sizeof(unsigned int)));

				// update shader
				if (ID == 0) {
					Shaders::guiShader->setTextured(false);
				} else {
					Renderer::bindTexture2D(ID);
					Shaders::guiShader->setTextured(true);
				}

				lastCount = count;
				lastIndexOffset = indexOffset;
			}

			// update last ID
			lastID = ID;
		}

		Renderer::drawElements(Renderer::TRIANGLES, lastCount, Renderer::UINT, (const void*) (intptr_t) (lastIndexOffset * sizeof(unsigned int)));

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

};