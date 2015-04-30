package lib.geom;

public class Rectangle {
	public final int x1;
	public final int y1;
	public final int x2;
	public final int y2;
	public final int width;
	public final int height;
	
	public Rectangle(int x1, int y1, int x2, int y2) {
		this.x1 = x1;
		this.x2 = x2;
		this.y1 = y1;
		this.y2 = y2;
		this.width = x2 - x1;
		this.height = y2 - y1;
	}
	
	public String toString() {
		return "Rectangle (" + x1 + ", " + y1 + ", " + x2 + ", " + y2 + ")";
	}
}
