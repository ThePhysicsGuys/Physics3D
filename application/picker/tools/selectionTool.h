#pragma once

#include "../graphics/glfwUtils.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/tool/stateTool.h"
#include "../engine/ecs/registry.h"
#include <Physics3D/math/ray.h>
#include <Physics3D/math/linalg/vec.h>
#include "../../ecs/components.h"
#include "../selection.h"
#include <optional>

struct ExtendedPart;

namespace P3D::Application {

class SelectionTool : public Engine::Tool {
public:
	DEFINE_TOOL("Select", "Selects one or multiple entities.", Graphics::GLFW::Cursor::ARROW);
	
	static Ray ray;
	static Vec2 mouse;
	static Selection selection;
	static std::optional<Position> selectedPoint;
	static std::optional<Position> intersectedPoint;

	~SelectionTool() override = default;	

	void onRegister() override;
	void onDeregister() override;
	void onRender() override;
	void onEvent(Engine::Event& event) override;

	static bool onMousePress(Engine::MousePressEvent& event);

	static void clear();
	static void select(const Engine::Registry64::entity_type& entity);
	static void toggle(const Engine::Registry64::entity_type& entity);
	static void single(const Engine::Registry64::entity_type& entity);

	static std::optional<std::pair<Engine::Registry64::entity_type, Position>> getIntersectedEntity();
	static std::optional<std::pair<Engine::Registry64::entity_type, Position>> getIntersectedCollider();
	static std::optional<double> intersect(const GlobalCFrame& cframe, IRef<Comp::Hitbox> hitbox);
	static std::optional<double> intersect(const GlobalCFrame& cframe, const Shape& shape);
	static std::optional<double> intersect(const GlobalCFrame& cframe, const Graphics::ExtendedTriangleMesh& shape);
};

};