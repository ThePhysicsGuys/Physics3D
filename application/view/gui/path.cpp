#include "path.h"

#include <vector>

#include "gui.h"
#include "../screen.h"

namespace Path {

	std::vector<Vec2f> path;

	void lineTo(const Vec2f& vertex) {
		path.push_back(vertex);
	}

	void clear() {
		path.clear();
	}

	
	void stroke(Vec4f color, float thickness, bool closed) {
		
		if (path.size() == 0) {
			return;
		}

		if (path.size() == 1) {
			// point not supported
			clear();
			return;
		}

		if (closed)
			path.push_back(Vec2f(path[0]));

		int vertexCount = 4 * (path.size() - 1);
		int indexCount = 6 * (path.size() - 1);

		GUI::batch->reserve(vertexCount, indexCount);

		float size = 1.5 * thickness / GUI::screen->dimension.y;

		for (int i = 0; i < path.size() - 1; i++) {
			Vec2f a = path[i];
			Vec2f b = path[i + 1];
			
			Vec2f dxy = normalize(Vec2f(b.y - a.y, a.x - b.x)) * size;

			GUI::batch->pushVertex({ a + dxy, Vec2f(1), color });
			GUI::batch->pushVertex({ a - dxy, Vec2f(1), color });
			GUI::batch->pushVertex({ b + dxy, Vec2f(1), color });
			GUI::batch->pushVertex({ b - dxy, Vec2f(1), color });

			GUI::batch->pushIndex(0);
			GUI::batch->pushIndex(1);
			GUI::batch->pushIndex(2);
			GUI::batch->pushIndex(2);
			GUI::batch->pushIndex(1);
			GUI::batch->pushIndex(3);

			GUI::batch->endIndex();
		}

		clear();
	}

	void fill(Vec4f color) {
		if (path.size() == 0) {
			return;
		}

		if (path.size() == 1) {
			// point not supported
			clear();
			return;
		}

		if (path.size() == 2) {
			stroke(color);
			return;
		}

		int vertexCount = path.size();
		int indexCount = 3 * (path.size() - 2);

		GUI::batch->reserve(vertexCount, indexCount);

		for (int i = 0; i < path.size(); i++)
			GUI::batch->pushVertex({ path[i], Vec2f(1), color });
			   
		for (int i = 0; i < path.size() - 2; i++) {
			GUI::batch->pushIndex(0);
			GUI::batch->pushIndex(i + 1);
			GUI::batch->pushIndex(i + 2);
		}

		GUI::batch->endIndex();

		clear();
	}

	/*
		Primitives
	*/
	void line(const Vec2f& a, const Vec2f& b, float thickness, const Vec4f& colorA, const Vec4f& colorB) {
		Vec2f dxy = normalize(Vec2f(b.y - a.y, a.x - b.x)) / GUI::screen->dimension.y * 1.5 * thickness;

		GUI::batch->reserve(4, 6);

		GUI::batch->pushVertex({ Vec2f(a + dxy), Vec2f(1), colorA });
		GUI::batch->pushVertex({ Vec2f(a - dxy), Vec2f(1), colorA });
		GUI::batch->pushVertex({ Vec2f(b + dxy), Vec2f(1), colorB });
		GUI::batch->pushVertex({ Vec2f(b - dxy), Vec2f(1), colorB });

		GUI::batch->pushIndex(0);
		GUI::batch->pushIndex(1);
		GUI::batch->pushIndex(2);
		GUI::batch->pushIndex(2);
		GUI::batch->pushIndex(1);
		GUI::batch->pushIndex(3);

		GUI::batch->endIndex();
	}
}