#pragma once

#include "boundsTreeOld.h"
#include "boundsTree2.h"

// USE_NEW_BOUNDSTREE defined in bounds.h

class Part;

#ifdef USE_NEW_BOUNDSTREE
using ChosenBoundsTree = P3D::NewBoundsTree::BoundsTree<Part>;
#else
using ChosenBoundsTree = P3D::OldBoundsTree::BoundsTree<Part>;
#endif