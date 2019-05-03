#include "layout.h"
#include "container.h"

Vec2 FlowLayout::resize(Container* container) {
	// Resulting width of the container
	double contentWidth = 0;
	// Resulting height of the container
	double contentHeight = 0;
	// Width of the current row of components
	double rowWidth = 0;
	// Height of the current row of components
	double rowHeight = 0;

	for (int i = 0; i < container->children.size(); i++) {
		auto child = container->children[i];
		Component* component = child.first;
		Align alignment = child.second;
		Vec2 componentSize = component->resize();

		// NO HEIGHT CHECK YET
		if (alignment == Align::FILL) {
			//Log::debug("fill %f, %f", componentSize.x, componentSize.y);
			double newRowWidth = rowWidth + componentSize.x;
			if (newRowWidth <= container->width || container->resizing) {
				// Set component position relative to parent
				component->position = container->position + Vec2(rowWidth, -contentHeight);

				// End of the current row, component keeps dimension
				rowWidth = newRowWidth;
				rowHeight = fmax(rowHeight, componentSize.y);

				// Resize the container so the component fits in
				contentWidth = fmax(contentWidth, rowWidth);

				// Next line
				contentHeight += rowHeight;

				// Reset row size
				rowWidth = 0;
				rowHeight = 0;
			} else {
				// NO CHECK IF COMPONENT WIDTH IS GREATER THAN CONTENTWIDTH
				// Component does not fit in the current row, advance to the next row
				contentHeight += rowHeight;

				// Set component position relative to parent
				component->position = container->position + Vec2(0, -contentHeight);

				// Advance position
				contentHeight += componentSize.y;

				// Reset row size
				rowWidth = 0;
				rowHeight = 0;
			}
		} else if (alignment == Align::RELATIVE) {
			//Log::debug("relative %f, %f", componentSize.x, componentSize.y);
			double newRowWidth = rowWidth + componentSize.x;
			if (newRowWidth <= container->width || container->resizing) {
				// Set component position relative to parent
				component->position = container->position + Vec2(rowWidth, -contentHeight);

				// Add component to current row, resize row height
				rowWidth = newRowWidth;
				rowHeight = fmax(rowHeight, componentSize.y);

				// Resize the container
				contentWidth = fmax(contentWidth, rowWidth);
			} else {
				// Component does not fit in the current row, advance to the next row
				contentHeight += rowHeight;

				// Set component position relative to parent
				component->position = container->position + Vec2(0, -contentHeight);

				// Set new row size
				rowWidth = componentSize.x;
				rowHeight = componentSize.y;
			}
		}
	}

	// Add height of last row
	contentHeight += rowHeight;

	if (container->resizing) {
		container->dimension = Vec2(contentWidth, contentHeight);
	}

	return container->dimension;
}
