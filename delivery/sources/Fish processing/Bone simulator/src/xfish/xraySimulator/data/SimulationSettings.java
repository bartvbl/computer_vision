package xfish.xraySimulator.data;

import java.io.File;

import lib.geom.STLFileContents;
import lib.geom.Vector3D;

public class SimulationSettings {
	
	public static final int BASE_IMAGE_WIDTH = 2050;
	public static final int BASE_IMAGE_HEIGHT = 960;

	public final GeometryFile[] geometryFiles;
	public final int imageWidth;
	public int imageHeight = BASE_IMAGE_HEIGHT;
	public final File destinationFile;
	public final File[] triangleIDFiles;
	public final int fishRotation;
	public STLFileContents[] models;
	
	public Vector3D emitterPosition;

	public SimulationSettings(GeometryFile[] geometryFiles, Vector3D emitterPosition, int fishRotation, String name) {
		this.geometryFiles = geometryFiles;
		this.fishRotation = fishRotation;
		this.destinationFile = new File("output/render_" + name + ".png");
		this.triangleIDFiles = new File[geometryFiles.length];
		for(int i = 0; i < geometryFiles.length; i++) {
			triangleIDFiles[i] = new File("output/render_triangles_" + name + "_" + i + ".png");
		}
		
		//deduced values
		this.imageWidth = (int) (BASE_IMAGE_WIDTH);
		
		this.emitterPosition = emitterPosition;
	}
	
	public void updateEmitterPosition(Vector3D position) {
		System.out.println("Emitter position: " + position);
		this.emitterPosition = position;
	}
	
	public String toString() {
		return 	"Simulation settings:\n" +
				"Emitter position: " + emitterPosition;
	}

}
