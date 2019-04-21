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

inline int furthestIndexInDirection(Vec3* vertices, int vertexCount, Vec3 direction) {
	double bestDot = vertices[0] * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		double newD = vertices[i] * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;
}

inline MinkPoint getSupport(const Shape& first, const Shape& second, Vec3 searchDirection) {
	int furthestIndex1 = first.furthestIndexInDirection(searchDirection);
	int furthestIndex2 = second.furthestIndexInDirection(-searchDirection);
	return MinkPoint{first.vertices[furthestIndex1] - second.vertices[furthestIndex2], furthestIndex1, furthestIndex2 };
	/*if(newPoint * searchDirection < 0)
		return false;
	if(--iteration == 0) {
		Log::warn("GJK iteration limit reached!");
		return false;
	}*/
}

bool runGJKBool(const Shape& first, const Shape& second, Vec3 initialSearchDirection, Tetrahedron& simplex) {
	physicsMeasure.mark(PhysicsProcess::GJK);

	MinkPoint A(getSupport(first, second, initialSearchDirection));
	MinkPoint B, C, D;

	// set new searchdirection to be straight at the origin
	Vec3 searchDirection = -A.p;
	Vec3 newPoint;

	// GJK 2
	// s.A is B.p
	// s.B is A.p
	// line segment, check if line, or either point closer
	// B can't be closer since we picked a point towards the origin
	// Just one test, to see if the line segment or A is closer
	B = getSupport(first, second, searchDirection); 
	if(B.p * searchDirection < 0)
		return false;

	Vec3 AO = -B.p;
	Vec3 AB = A.p - B.p;
	searchDirection = -(AO % AB) % AB;
	
	C = getSupport(first, second, searchDirection);
	if(C.p * searchDirection < 0)
		return false;
	// s.A is C.p  newest
	// s.B is B.p
	// s.C is A.p
	// triangle, check if closest to one of the edges, point, or face
	for(int iter = 0; iter < GJK_MAX_ITER; iter++){
		Vec3 AO = -C.p;
		Vec3 AB = B.p - C.p;
		Vec3 AC = A.p - C.p;
		Vec3 normal = AB % AC;
		Vec3 nAB = AB % normal;
		Vec3 nAC = normal % AC;

		if(AO * nAB > 0) {
			// edge of AB is closest, searchDirection perpendicular to AB towards O
			A = B;
			B = C;
			searchDirection = -(AO % AB) % AB;
			C = getSupport(first, second, searchDirection);
			if(C.p * searchDirection < 0)
				return false;
		} else {
			if(AO*nAC > 0) {
				// edge of AC is closest, searchDirection perpendicular to AC towards O
				B = C;
				searchDirection = -(AO % AC) % AC;
				C = getSupport(first, second, searchDirection);
				if(C.p * searchDirection < 0)
					return false;
			} else {
				// hurray! best shape is tetrahedron
				// just find which direction to look in
				if(normal * AO > 0) {
					searchDirection = normal;
				} else {
					searchDirection = -normal;
					// invert triangle
					MinkPoint tmp(A);
					A = B;
					B = tmp;
				}

				// GJK 4
				// s.A is D.p
				// s.B is C.p
				// s.C is B.p
				// s.D is A.p
				D = getSupport(first, second, searchDirection);
				if(D.p * searchDirection < 0)
					return false;
				Vec3 AO = -D.p;
				// A is new point, at the top of the tetrahedron
				Vec3 AB = C.p - D.p;
				Vec3 AC = B.p - D.p;
				Vec3 AD = A.p - D.p;
				Vec3 nABC = AB % AC;
				Vec3 nACD = AC % AD;
				Vec3 nADB = AD % AB;

				if(nACD * AO > 0) {
					// remove B and continue with triangle
					// s = Simplex(s.A, s.C, s.D, s.At, s.Ct, s.Dt);
					C = D;
				} else {
					if(nABC * AO > 0) {
						// remove D and continue with triangle
						// s = Simplex(s.A, s.B, s.C, s.At, s.Bt, s.Ct);
						A = B;
						B = C;
						C = D;
					} else {
						if(nADB * AO > 0) {
							// remove C and continue with triangle
							//s = Simplex(s.A, s.D, s.B, s.At, s.Dt, s.Bt);
							B = C;
							C = D;
						} else {
							// GOTCHA! TETRAHEDRON COVERS THE ORIGIN!

							simplex = Tetrahedron{D, C, B, A};
							return true;
						}
					}
				}
			}
		}
	}
	
	Log::warn("GJK iteration limit reached!");
	return false;
}

void initializeBuffer(Tetrahedron& s, ComputationBuffers& b) {
	b.vertBuf[0] = s.A.p;
	b.vertBuf[1] = s.B.p;
	b.vertBuf[2] = s.C.p;
	b.vertBuf[3] = s.D.p;

	b.triangleBuf[0] = {0,1,2};
	b.triangleBuf[1] = {0,2,3};
	b.triangleBuf[2] = {0,3,1};
	b.triangleBuf[3] = {3,2,1};

	b.knownVecs[0] = MinkowskiPointIndices{s.A.originFirst, s.A.originSecond};
	b.knownVecs[1] = MinkowskiPointIndices{s.B.originFirst, s.B.originSecond};
	b.knownVecs[2] = MinkowskiPointIndices{s.C.originFirst, s.C.originSecond};
	b.knownVecs[3] = MinkowskiPointIndices{s.D.originFirst, s.D.originSecond};
}

bool runEPA(const Shape& first, const Shape& second, Tetrahedron& s, Vec3& intersection, Vec3& exitVector, ComputationBuffers& bufs) {
	physicsMeasure.mark(PhysicsProcess::EPA);
	// s.order == 4

	bufs.ensureCapacity(first.vertexCount + second.vertexCount + EPA_MAX_ITER, first.triangleCount + second.triangleCount + EPA_MAX_ITER*2);

	initializeBuffer(s, bufs);

	ConvexShapeBuilder builder(bufs.vertBuf, bufs.triangleBuf, 4, 4, bufs.neighborBuf, bufs.removalBuf, bufs.edgeBuf);

	for(int i = 0; i < EPA_MAX_ITER; i++) {

		double distSq;
		int closestTriangleIndex = getNearestSurface(builder, distSq);
		Triangle closestTriangle = builder.triangleBuf[closestTriangleIndex];
		Vec3 a = builder.vertexBuf[closestTriangle[0]];
		Vec3 b = builder.vertexBuf[closestTriangle[1]];
		Vec3 c = builder.vertexBuf[closestTriangle[2]];
		Vec3 closestTriangleNormal = getNormalVec(closestTriangle, builder.vertexBuf);

		int furthest1 = first.furthestIndexInDirection(closestTriangleNormal);
		int furthest2 = second.furthestIndexInDirection(-closestTriangleNormal);
		Vec3 delta = first.vertices[furthest1] - second.vertices[furthest2];

		// delta is the new point to be added, check if it's past the current triangle

		double newPointDistSq = pow(delta * closestTriangleNormal, 2) / closestTriangleNormal.lengthSquared();

		MinkowskiPointIndices curIndices{furthest1, furthest2};

		if(!(newPointDistSq <= distSq * 1.01)) {
			bufs.knownVecs[builder.vertexCount] = curIndices;
			builder.addPoint(delta, closestTriangleIndex);
		} else {
			// closestTriangle is an edge triangle, so our best direction is towards this triangle.

			RayIntersection ri = rayTriangleIntersection(Vec3(), closestTriangleNormal, a, b, c);

			exitVector = ri.d * closestTriangleNormal;

			MinkowskiPointIndices inds[3]{bufs.knownVecs[closestTriangle[0]], bufs.knownVecs[closestTriangle[1]], bufs.knownVecs[closestTriangle[2]]};
			
			Vec3 v0 = b - a, v1 = c - a, v2 = exitVector - a;

			double d00 = v0 * v0;
			double d01 = v0 * v1;
			double d11 = v1 * v1;
			double d20 = v2 * v0;
			double d21 = v2 * v1;
			double denom = d00 * d11 - d01 * d01;
			double v = (d11 * d20 - d01 * d21) / denom;
			double w = (d00 * d21 - d01 * d20) / denom;
			double u = 1.0 - v - w;

			Vec3 A0 = first.vertices[inds[0][0]], A1 = first.vertices[inds[1][0]], A2 = first.vertices[inds[2][0]];
			Vec3 B0 = second.vertices[inds[0][1]], B1 = second.vertices[inds[1][1]], B2 = second.vertices[inds[2][1]];

			Vec3 avgFirst = A0 * u + A1 * v + A2 * w;
			Vec3 avgSecond = B0 * u + B1 * v + B2 * w;

			intersection = (avgFirst + avgSecond) / 2;
			return true;
		}
	}

	Log::warn("EPA iteration limit exceeded! ");
	return false;
}
