#include "core.h"

#include "layout.h"

#include "container.h"

namespace Graphics {

Vec2 FlowLayout::resize(Container* container, Vec2 minDimension) {
	// Resulting width of the container
	double contentWidth = minDimension.x;
	// Resulting height of the container
	double contentHeight = minDimension.y;
	// Width of the current row of components
	double rowWidth = 0;
	// Height of the current row of components
	double rowHeight = 0;

	// Components to be centered
	std::vector<Component*> centeredComponents;

	for (int i = 0; i < container->children.size(); i++) {
		auto child = container->children[i];
		Component* component = child.first;
		Align alignment = child.second;
		Vec2 componentSize = component->resize();

		// NO HEIGHT CHECK YET
		if (alignment == Align::FILL || alignment == Align::CENTER) {
			// Calculate new row width, component margin included
			double newRowWidth = rowWidth + component->margin + componentSize.x + component->margin;
			if (newRowWidth <= container->width || container->resizing) {
				// Set component position relative to parent with old rowWidth and contentHeight, component margin included
				component->position = container->position + Vec2(rowWidth, -contentHeight) + Vec2(component->margin, -component->margin);

				// End of the current row, component keeps dimension
				// Update row width with calculated value
				rowWidth = newRowWidth;
				// Update row height, component margin included
				rowHeight = fmax(rowHeight, component->margin + componentSize.y + component->margin);

				// Resize the container so the component fits in
				contentWidth = fmax(contentWidth, rowWidth);

				// Next line
				contentHeight += rowHeight;

				// Reset row size
				rowWidth = 0;
				rowHeight = 0;

				if (alignment == Align::CENTER) {
					centeredComponents.push_back(component);
				}
			} else {
				// NO CHECK IF COMPONENT WIDTH IS GREATER THAN CONTENTWIDTH
				// Component does not fit in the current row, advance to the next row
				contentHeight += rowHeight;

				// Set component position relative to parent, component margin included
				component->position = container->position + Vec2(0, -contentHeight) + Vec2(component->margin, -component->margin);

				// Advance content height, component margin included
				contentHeight += component->margin + componentSize.y + component->margin;

				// Reset row size
				rowWidth = 0;
				rowHeight = 0;
			}
		} else if (alignment == Align::RELATIVE) {
			// Calculate new row width, component margin included
			double newRowWidth = rowWidth + component->margin + componentSize.x + component->margin;
			if (newRowWidth <= container->width || container->resizing) {
				// Set component position relative to parent
				component->position = container->position + Vec2(rowWidth, -contentHeight) + Vec2(component->margin, -component->margin);

				// Update row width with calculated value
				rowWidth = newRowWidth;
				// Update row height, component margin included
				rowHeight = fmax(rowHeight, component->margin + componentSize.y + component->margin);

				// Resize the container
				contentWidth = fmax(contentWidth, rowWidth);
			} else {
				// Component does not fit in the current row, advance to the next row
				contentHeight += rowHeight;

				// Set component position relative to parent
				component->position = container->position + Vec2(0, -contentHeight) + Vec2(component->margin, -component->margin);

				// Set new row size
				rowWidth = component->margin + componentSize.x + component->margin;
				rowHeight = component->margin + componentSize.y + component->margin;
			}
		}
	}

	// Center remaining components
	for (Component* component : centeredComponents) {
		// Distance from component position to the end of the container minus the component margin
		double remainingWidth = container->x + contentWidth - component->x - component->margin;
		// Component width
		double componentWidth = component->width;

		// Move component to the center
		component->x += (remainingWidth - componentWidth) / 2;
	}

	// Add height of last row
	contentHeight += rowHeight;

	if (container->resizing) {
		container->dimension = Vec2f(contentWidth, contentHeight);
	}

	return container->dimension;
}

};