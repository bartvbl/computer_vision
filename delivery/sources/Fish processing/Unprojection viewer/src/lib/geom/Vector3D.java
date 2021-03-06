package lib.geom;

public class Vector3D {
	public final double x;
	public final double y;
	public final double z;
	
	public Vector3D(double x, double y, double z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public Vector3D(double[] vector) {
		this.x = vector[0];
		this.y = vector[1];
		this.z = vector[2];
	}
	
	public double scalarProduct(Vector3D vector) {
		return this.x*vector.x + this.y*vector.y + this.z*vector.z;
	}
	
	public Vector3D vectorProduct(Vector3D vector) {
		double x = (this.y*vector.z) - (this.z*vector.y);
		double y = (this.z*vector.x) - (this.x*vector.z);
		double z = (this.x*vector.y) - (this.y*vector.x);
		return new Vector3D(x, y, z);
	}
	
	public double getLength() {
		return Math.sqrt(x*x + y*y + z*z);
	}
	
	public Vector3D normalize() {
		double length = this.getLength();
		double x = this.x/length;
		double y = this.y/length;
		double z = this.z/length;
		return new Vector3D(x, y, z);
	}
	
	@Override
	public String toString() {
		return "vert3D ["+x+", "+y+", "+z+"]";
	}

	public Vector3D inverse() {
		return new Vector3D(-x, -y, -z);
	}

	public Vector3D minus(Vector3D other) {
		return new Vector3D(this.x - other.x, this.y - other.y, this.z - other.z);
	}

	public Vector3D plus(Vector3D other) {
		return new Vector3D(x + other.x, y + other.y, z + other.z);
	}

	public Vector3D times(double scalar) {
		return new Vector3D(x*scalar, y*scalar, z*scalar);
	}

	public Vector3D scale(double scaleFactor) {
		return new Vector3D(x * scaleFactor, y * scaleFactor, z * scaleFactor);
	}
}
