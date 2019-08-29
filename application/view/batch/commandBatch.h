#pragma once
#include "batch.h"

#include "../application/debug.h"
#include "../engine/math/vec.h"

#include "../shaderProgram.h"

#include <vector>

struct DrawCommand {
	// Amount of indices to render
	size_t count;
	// Scissor rectangle
	Vec4f clip;
	// Optional texture ID
	int textureID;
	// Offset in index buffer
	size_t indexOffset;
};

template<typename Vertex>
class CommandBatch : public Batch<Vertex> {
protected:
	std::vector<DrawCommand> commandBuffer;

	size_t currentIndexOffset;
public:
	CommandBatch(BatchConfig config) : Batch<Vertex>(config), currentIndexOffset(0) {

	}

	void pushCommand(int textureID) {
		size_t count = Batch<Vertex>::indexBuffer.size() - currentIndexOffset;

		if (count == 0)
			return;

		Vec4f scissor = Vec4f();
		DrawCommand command = { count, Vec4f(), textureID, currentIndexOffset };

		commandBuffer.push_back(command);
		currentIndexOffset += count;
	}

	void submit() override {
		if (commandBuffer.size() == 0) {
			pushCommand(0);
		}

		Batch<Vertex>::vao->bind();
		 
		Batch<Vertex>::vbo->fill((const void *) Batch<Vertex>::vertexBuffer.data(), Batch<Vertex>::vertexBuffer.size() * sizeof(Vertex), Renderer::STREAM_DRAW);
		Batch<Vertex>::ibo->fill((const unsigned int *) Batch<Vertex>::indexBuffer.data(), Batch<Vertex>::indexBuffer.size(), Renderer::STREAM_DRAW);

		for (const DrawCommand& command : commandBuffer) {

			if (command.textureID != 0) {
				glCall(Renderer::bindTexture2D(command.textureID));
				glCall(Shaders::guiShader.setTextured(true));
			} else {
				glCall(Shaders::guiShader.setTextured(false));
			}

			// TODO project clip rectangle to framebuffer space
			//Renderer::scissor(command.clip.x, command.clip.y, command.clip.z, command.clip.w);
			
			glCall(Renderer::drawElements(Renderer::TRIANGLES, command.count, Renderer::UINT, (const void *) (intptr_t) (command.indexOffset * sizeof(unsigned int))));
			
		}

		Batch<Vertex>::vbo->unbind();
		Batch<Vertex>::ibo->unbind();
		Batch<Vertex>::vao->unbind();
		
		clear();
	}

	void clear() {
		currentIndexOffset = 0;
		Batch<Vertex>::clear();
		commandBuffer.clear();
	}
};