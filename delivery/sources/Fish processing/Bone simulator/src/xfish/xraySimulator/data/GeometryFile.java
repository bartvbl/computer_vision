package xfish.xraySimulator.data;

import java.io.File;

public class GeometryFile {
	public final File file;
	private int density;

	public GeometryFile(File file, int density) {
		this.file = file;
		this.density = density;
	}
	
	public String toString() {
		return file.getName();
	}

	public void setDensity(int newDensity) {
		this.density = newDensity;
	}

	public int getDensity() {
		return density;
	}
}
