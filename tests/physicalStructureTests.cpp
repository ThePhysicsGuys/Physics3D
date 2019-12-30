#include "testsMain.h"

#include "compare.h"
#include "../physics/misc/toString.h"

#include "estimateMotion.h"

#include "../physics/geometry/basicShapes.h"
#include "../physics/part.h"
#include "../physics/physical.h"
#include "../physics/constraints/fixedConstraint.h"

static Part* createPart() {
	return new Part(Box(1.0, 1.0, 1.0), GlobalCFrame(), {1.0, 1.0, 1.0});
}

TEST_CASE(testBasicCreateDestroy) {
	Part* p = createPart();
	p->ensureHasParent();
	delete p;
}

TEST_CASE(testManyAttachBasic) {
	Part* a = createPart();
	Part* b = createPart();
	Part* c = createPart();
	Part* d = createPart();

	a->attach(b, CFrame());
	a->attach(c, CFrame());
	c->attach(d, CFrame());
		
	// a should be mainPart
	ASSERT_TRUE(a->isMainPart());
	ASSERT_FALSE(b->isMainPart());
	ASSERT_FALSE(c->isMainPart());
	ASSERT_FALSE(d->isMainPart());

	delete b;
	delete a;
	delete c;
	delete d;
}

TEST_CASE(testManualDetach) {
	Part* a = createPart();
	Part* b = createPart();
	Part* c = createPart();
	Part* d = createPart();

	a->attach(b, CFrame());
	a->attach(c, CFrame());
	c->attach(d, CFrame());

	// a should be mainPart
	ASSERT_TRUE(a->isMainPart());
	ASSERT_FALSE(b->isMainPart());
	ASSERT_FALSE(c->isMainPart());
	ASSERT_FALSE(d->isMainPart());

	b->detach();
	ASSERT_TRUE(b->isMainPart());

	a->detach();
	ASSERT_TRUE(c->parent->rigidBody.mainPart != a);

	delete b;
	delete a;
	delete c;
	delete d;
}

TEST_CASE(testManyAttachComplex) {
	Part* a = createPart();
	Part* b = createPart();
	Part* c = createPart();
	Part* d = createPart();

	Part* e1 = createPart();
	Part* e2 = createPart();
	e1->attach(e2, CFrame());

	Part* f1 = createPart();
	Part* f2 = createPart();
	f1->attach(f2, CFrame());

	Part* g1 = createPart();
	Part* g2 = createPart();
	g1->attach(g2, CFrame());

	// single new part attachments
	a->attach(b, CFrame());
	b->attach(c, CFrame());
	d->attach(b, CFrame());

	a->attach(e1, CFrame());
	a->attach(f2, CFrame());
	g1->attach(a, CFrame());
		
	ASSERT_TRUE(a->parent->rigidBody.getPartCount() == 10);

	Part* parts[10]{a,b,c,d,e1,e2,f1,f2,g1,g2};
	Physical* parent = a->parent;
	for(Part* p : parts) {
		ASSERT_TRUE(p->parent == parent);
		delete p;
	}

	throw "TODO Memory leak in Part::attach(Part*, CFrame)";
}
