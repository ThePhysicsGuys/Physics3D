#include "intersection.h"

#include "../math/vec3.h"
#include "convexShapeBuilder.h"
#include "computationBuffer.h"
#include "../math/utils.h"
#include "../../util/log.h"
#include "../debug.h"
#include "../physicsProfiler.h"
#include "../constants.h"


Vec3 getNormalVec(Triangle t, Vec3* vertices) {
	Vec3 v0 = vertices[t[0]];
	Vec3 v1 = vertices[t[1]];
	Vec3 v2 = vertices[t[2]];

	return (v1 - v0) % (v2 - v0);
}

double getDistanceOfTriangleToOriginSquared(Triangle t, Vec3* vertices) {
	return pointToPlaneDistanceSquared(getNormalVec(t, vertices), vertices[t[0]]);
}

int getNearestSurface(ConvexShapeBuilder& builder, double& distanceSquared) {
	int best = 0;
	double bestDistSq = getDistanceOfTriangleToOriginSquared(builder.triangleBuf[0], builder.vertexBuf);

	for(int i = 1; i < builder.triangleCount; i++) {
		Triangle t = builder.triangleBuf[i];

		double distSq = getDistanceOfTriangleToOriginSquared(t, builder.vertexBuf);

		if(distSq < bestDistSq) {
			best = i;
			bestDistSq = distSq;
		}
	}

	distanceSquared = bestDistSq;
	return best;
}

Simplex runGJK(const Shape& first, const Shape& second, Vec3 initialSearchDirection) {
	physicsMeasure.mark(PhysicsProcess::GJK);

	// Log::debug("Shape::intersects");

	int furthestIndex1 = first.furthestIndexInDirection(initialSearchDirection);
	int furthestIndex2 = second.furthestIndexInDirection(-initialSearchDirection);
	// Vec3 furthest1 = vertices[furthestIndex1];

	// first point
	Simplex s(first.vertices[furthestIndex1] - second.vertices[furthestIndex2], MinkowskiPointIndices{furthestIndex1, furthestIndex2});

	// set new searchdirection to be straight at the origin
	Vec3 searchDirection = -s.A;

	for(int iteration = 0; iteration < GJK_MAX_ITER; iteration++) {
		int furthestIndex1 = first.furthestIndexInDirection(searchDirection);
		int furthestIndex2 = second.furthestIndexInDirection(-searchDirection);

		// Vec3 furthest = this->furthestInDirection(searchDirection);
		Vec3 newPoint = first.vertices[furthestIndex1] - second.vertices[furthestIndex2];// furthest - second.furthestInDirection(-searchDirection);
		if(newPoint * searchDirection < 0) {
			return s; // the best point in the direction does not go past the origin, therefore the entire difference must be on this side of the origin, not containing it, thus no collision
		}

		//simplex[simplexLength] = newPoint;
		//simplexLength++;

		s.insert(newPoint, MinkowskiPointIndices{furthestIndex1, furthestIndex2});

		Vec3 AO = -s.A;
		switch(s.order) {
		case 4: {
			// A is new point, at the top of the tetrahedron
			Vec3 AB = s.B - s.A;
			Vec3 AC = s.C - s.A;
			Vec3 AD = s.D - s.A;
			Vec3 nABC = AB % AC;
			Vec3 nACD = AC % AD;
			Vec3 nADB = AD % AB;

			if(nACD * AO > 0) {
				// remove B and continue with triangle
				s = Simplex(s.A, s.C, s.D, s.At, s.Ct, s.Dt);
			} else {
				if(nABC * AO > 0) {
					// remove D and continue with triangle
					s = Simplex(s.A, s.B, s.C, s.At, s.Bt, s.Ct);
				} else {
					if(nADB * AO > 0) {
						// remove C and continue with triangle
						s = Simplex(s.A, s.D, s.B, s.At, s.Dt, s.Bt);
					} else {
						// GOTCHA! TETRAHEDRON COVERS THE ORIGIN!

						/*
						Will represent our convex polyhedron
						*/

						return s;

					}
				}
			}
		}


		case 3: { // triangle, check if closest to one of the edges, point, or face
			Vec3 AB = s.B - s.A;
			Vec3 AC = s.C - s.A;
			Vec3 normal = AB % AC;
			Vec3 nAB = AB % normal;
			Vec3 nAC = normal % AC;

			if(AO * nAB > 0) {
				if(AO*AB > 0) {
					// edge of AB is closest, searchDirection perpendicular to AB towards O
					s = Simplex(s.A, s.B, s.At, s.Bt);
					searchDirection = (AO % AB) % AB;
				} else {
					if(AO*AC > 0) {
						// edge AC is closest, searchDirection perpendicular to AC towards O
						s = Simplex(s.A, s.C, s.At, s.Ct);
						searchDirection = (AO % AC) % AC;
					} else {
						// Point A is closest, searchDirection is straight towards O
						s = Simplex(s.A, s.At);
						searchDirection = AO;
					}
				}
			} else {
				if(AO*nAC > 0) {
					if(AO*AC > 0) {
						// edge AC is closest, searchDirection perpendicular to AC towards O
						s = Simplex(s.A, s.C, s.At, s.Ct);
						searchDirection = (AO % AC) % AC;
					} else {
						// Point A is closest, searchDirection is straight towards O
						s = Simplex(s.A, s.At);
						searchDirection = AO;
					}
				} else {
					// hurray! best shape is tetrahedron
					// just find which direction to look in
					if(normal * AO > 0) {
						searchDirection = normal;
					} else {
						searchDirection = -normal;
						s = Simplex(s.A, s.C, s.B, s.At, s.Ct, s.Bt); // invert triangle
					}
				}
			}
			break;
		}

		case 2: { // line segment, check if line, or either point closer
					// B can't be closer since we picked a point towards the origin
					// Just one test, to see if the line segment or A is closer
			Vec3 BA = s.A - s.B;
			if(AO*BA > 0) { // AO*BA > 0 means that BA and AO are in the same-ish direction, so O must be closest to A and not the line
				s = Simplex(s.A, s.At);
				searchDirection = AO;
			} else {
				// simplex remains the same
				// new searchdirection perpendicular to the line, towards O as much as possible
				searchDirection = (AO % BA) % BA;
			}
			break;
		}
		case 1: // single point, just go towards the origin
			searchDirection = AO;
			break;
		}
	}
	return s;


}

void initializeBuffer(Simplex s, ComputationBuffers& b) {
	b.vertBuf[0] = s.A;
	b.vertBuf[1] = s.B;
	b.vertBuf[2] = s.C;
	b.vertBuf[3] = s.D;

	b.triangleBuf[0] = {0,1,2};
	b.triangleBuf[1] = {0,2,3};
	b.triangleBuf[2] = {0,3,1};
	b.triangleBuf[3] = {3,2,1};

	b.knownVecs[0] = s.At;
	b.knownVecs[1] = s.Bt;
	b.knownVecs[2] = s.Ct;
	b.knownVecs[3] = s.Dt;
}

bool runEPA(const Shape& first, const Shape& second, Simplex s, Vec3& intersection, Vec3& exitVector, ComputationBuffers& bufs) {
	physicsMeasure.mark(PhysicsProcess::EPA);
	// s.order == 4

	bufs.ensureCapacity(first.vertexCount + second.vertexCount, first.triangleCount + second.triangleCount);

	initializeBuffer(s, bufs);

	ConvexShapeBuilder builder(bufs.vertBuf, bufs.triangleBuf, 4, 4, bufs.neighborBuf, bufs.removalBuf, bufs.edgeBuf);

	for(int i = 0; i < EPA_MAX_ITER; i++) {

		double distSq;
		int closestTriangleIndex = getNearestSurface(builder, distSq);
		Triangle closestTriangle = builder.triangleBuf[closestTriangleIndex];
		Vec3 v0 = builder.vertexBuf[closestTriangle[0]];
		Vec3 v1 = builder.vertexBuf[closestTriangle[1]];
		Vec3 v2 = builder.vertexBuf[closestTriangle[2]];
		Vec3 closestTriangleNormal = getNormalVec(closestTriangle, builder.vertexBuf);

		int furthest1 = first.furthestIndexInDirection(closestTriangleNormal);
		int furthest2 = second.furthestIndexInDirection(-closestTriangleNormal);
		Vec3 delta = first.vertices[furthest1] - second.vertices[furthest2];

		// delta is the new point to be added, check if it's past the current triangle

		double newPointDistSq = pow(delta * closestTriangleNormal, 2) / closestTriangleNormal.lengthSquared();

		// if(newPointDistSq > distSq * 1.001) {
		MinkowskiPointIndices curIndices{furthest1, furthest2};
		//Log::debug("cur: (%d, %d) <-> (%d, %d), (%d, %d), (%d, %d)", curIndices[0], curIndices[1], knownVecs[closestTriangle[0]][0], knownVecs[closestTriangle[0]][1], knownVecs[closestTriangle[1]][0], knownVecs[closestTriangle[1]][1], knownVecs[closestTriangle[2]][0], knownVecs[closestTriangle[2]][1]);
		//Log::debug("%f / %f = %.9f", newPointDistSq, distSq, newPointDistSq/distSq);
		if(!(bufs.knownVecs[closestTriangle[0]] == curIndices || bufs.knownVecs[closestTriangle[1]] == curIndices || bufs.knownVecs[closestTriangle[2]] == curIndices || newPointDistSq <= distSq * 1.000001)) {
			bufs.knownVecs[builder.vertexCount] = curIndices;
			builder.addPoint(delta, closestTriangleIndex);
		} else {
			// closestTriangle is an edge triangle, so our best direction is towards this triangle.

			RayIntersection ri = rayTriangleIntersection(Vec3(), closestTriangleNormal, v0, v1, v2);

			exitVector = ri.d * closestTriangleNormal;

			MinkowskiPointIndices inds[3]{bufs.knownVecs[closestTriangle[0]], bufs.knownVecs[closestTriangle[1]], bufs.knownVecs[closestTriangle[2]]};

			// Log::debug("EPA  %d:%d  %d:%d  %d:%d", inds[0][0], inds[0][1], inds[1][0], inds[1][1], inds[2][0], inds[2][1]);

			// Debug::logShape(builder.toShape());

			if(inds[0][0] == inds[1][0] && inds[0][0] == inds[2][0]) {
				// point-face intersection, with first shape providing the point
				intersection = first.vertices[inds[0][0]] - exitVector / 3;
				// Log::debug("EPA finished after %d iters POINT-FACE", i);
				return true;
			} else if(inds[0][1] == inds[1][1] && inds[0][1] == inds[2][1]) {
				// point-face intersection, with second shape providing the point
				intersection = second.vertices[inds[0][1]] + exitVector / 3;
				// Log::debug("EPA finished after %d iters POINT-FACE", i);
				return true;
			} else {
				for(int i = 0; i < 3; i++) {
					MinkowskiPointIndices A = inds[i];
					MinkowskiPointIndices B = inds[(i + 1) % 3];
					MinkowskiPointIndices C = inds[(i + 2) % 3];

					// if A <-> B is the same point, then check for other points
					if(A[0] == B[0]) {
						Vec3 P0, Q0, U, V;
						if(B[1] == C[1]) {
							// shared point is B
							// B <-> C is edge on first polygon
							// B <-> A is edge on second polygon

							U = first.vertices[C[0]] - first.vertices[B[0]];
							V = second.vertices[A[1]] - second.vertices[B[1]];

							P0 = first.vertices[B[0]];
							Q0 = second.vertices[B[1]];
						} else if(C[1] == A[1]) {
							// shared point is A
							// A <-> C is edge on first polygon
							// A <-> B is edge on second polygon

							U = first.vertices[C[0]] - first.vertices[A[0]];
							V = second.vertices[B[1]] - second.vertices[A[1]];

							P0 = first.vertices[A[0]];
							Q0 = second.vertices[A[1]];
						} else {
							Log::warn("Neither point-face collission, nor edge edge collission! ");
							return false;
						}

						CrossOverPoint cross = getNearestCrossoverOfRays(U, V, P0 - Q0);

						Debug::logVec(P0, U * cross.s, Debug::POSITION);
						Debug::logVec(Q0, V * cross.t, Debug::POSITION);

						intersection = (P0 + U*cross.s + Q0 + V*cross.t) / 2;

						// Log::debug("EPA finished after %d iters EDGE-EDGE 1", i);

						return true;
					}
				}
			}
			return false;
		}
	}

	Log::warn("EPA iteration limit exceeded! ");
	return false;
}
