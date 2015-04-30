package xfish.unprojectionViewer;

import java.nio.ByteBuffer;

import lib.geom.Point3D;

public class ColouredPoint3D extends Point3D {

	public final int colour_packed;
	public final byte[] colour;

	public ColouredPoint3D(double x, double y, double z, int colour) {
		super(x, y, z);
		this.colour_packed = colour;
		this.colour = ByteBuffer.allocate(4).putInt(colour).array();
	}

}
