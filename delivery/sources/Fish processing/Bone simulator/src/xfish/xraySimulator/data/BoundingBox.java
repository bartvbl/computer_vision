package xfish.xraySimulator.data;

public class BoundingBox {
	public final float xMin;
	public final float xMax;
	public final float yMin;
	public final float yMax;
	public final float zMin;
	public final float zMax;

	public BoundingBox(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
		this.xMin = xMin;
		this.xMax = xMax;
		this.yMin = yMin;
		this.yMax = yMax;
		this.zMin = zMin;
		this.zMax = zMax;
	}

	public BoundingBox include(BoundingBox additionalBoundingBox) {
		return new BoundingBox(
			Math.min(xMin, additionalBoundingBox.xMin),
			Math.max(xMax, additionalBoundingBox.xMax),
			Math.min(yMin, additionalBoundingBox.yMin),
			Math.max(yMax, additionalBoundingBox.yMax),
			Math.min(zMin, additionalBoundingBox.zMin),
			Math.max(zMax, additionalBoundingBox.zMax)
		);
	}
	
	public String toString() {
		return "BoundingBox [[" + xMin + ", " + xMax + "] [" + yMin + ", " + yMax + "] [" + zMin + ", " + zMax + "]]";
	}

}
