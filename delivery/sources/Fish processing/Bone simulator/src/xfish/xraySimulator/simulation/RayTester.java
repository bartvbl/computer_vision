package xfish.xraySimulator.simulation;

import lib.geom.Vector3D;

public class RayTester {
	//ADAPTED FROM "REAL-TIME COLLISSION DETECTION". See reference in report.
	
	//=== Section 5.3.4: =============================================================

	// Given line pq and ccw triangle abc, return whether line pierces triangle. If
	// so, also return the barycentric coordinates (u,v,w) of the intersection point
	public static boolean intersectLineTriangle(Vector3D p, Vector3D q, Vector3D a, Vector3D b, Vector3D c)
	{
		Vector3D pq = q.minus(p);
		Vector3D pa = a.minus(p);
		Vector3D pb = b.minus(p);
		Vector3D pc = c.minus(p);
		
		// Test if pq is inside the edges bc, ca and ab. Done by testing
		// that the signed tetrahedral volumes, computed using scalar triple
		// products, are all positive
		double u = scalarTriple(pq, pc, pb);
		if (u < 0.0) return false;
		double v = scalarTriple(pq, pa, pc);
		if (v < 0.0) return false;
		double w = scalarTriple(pq, pb, pa);
		if (w < 0.0) return false;
		
		return true;
	}

	private static double scalarTriple(Vector3D a, Vector3D b, Vector3D c) {
		return a.scalarProduct(b.vectorProduct(c));
	}

	public static Vector3D getIntersectionPoint(Vector3D p, Vector3D q, Vector3D a, Vector3D b, Vector3D c) {
		Vector3D pq = q.minus(p);
		Vector3D pa = a.minus(p);
		Vector3D pb = b.minus(p);
		Vector3D pc = c.minus(p);
		
		// Test if pq is inside the edges bc, ca and ab. Done by testing
		// that the signed tetrahedral volumes, computed using scalar triple
		// products, are all positive
		double u = scalarTriple(pq, pc, pb);
		double v = scalarTriple(pq, pa, pc);
		double w = scalarTriple(pq, pb, pa);
		
		// Compute the barycentric coordinates (u, v, w) determining the
	    // intersection point r, r = u*a + v*b + w*c
	    double denom = 1.0 / (u + v + w);
	    u *= denom;
	    v *= denom;
	    w *= denom; // w = 1.0f - u - v;
		
		return a.times(u).plus(b.times(v)).plus(c.times(w));
	}
}
