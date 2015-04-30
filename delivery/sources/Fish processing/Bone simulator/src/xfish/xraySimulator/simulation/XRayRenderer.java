package xfish.xraySimulator.simulation;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

import xfish.xraySimulator.data.SimulationSettings;
import xfish.xraySimulator.ui.ProgressWindow;
import lib.geom.Rectangle;
import lib.geom.STLFileContents;
import lib.geom.Vector3D;

public class XRayRenderer {

	public static void render(STLFileContents model, SimulationSettings settings, MultiLayerDepthBuffer depthBuffer, ProgressWindow progressWindow, int totalTriangleCount, int triangleLabelstartIndex) {
		StringBuffer buffer = new StringBuffer();
		
		for(int i = 0; i < model.triangleCount; i++) {
			double progress = 1.0 / (double) totalTriangleCount;
			progressWindow.incrementProgress(progress);
			Rectangle screenArea = calculateScreenArea(model.vertices[i], settings);
			for(int x = screenArea.x1; x < screenArea.x2; x++) {
				for(int y = screenArea.y1; y < screenArea.y2; y++) {
					double pixelValue = renderPixel(model.vertices[i], x, y, settings.emitterPosition, buffer);
					if(pixelValue != 0) {
						depthBuffer.put(x, y, pixelValue, triangleLabelstartIndex + i);
					}
				}
			}
		}
		PrintWriter writer = null;
		try {
			writer = new PrintWriter(new FileWriter(new File("output/debugcoords_"+triangleLabelstartIndex+".txt")));
			writer.write(buffer.toString());
			writer.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private static double renderPixel(float[][] triangle, int x, int y, Vector3D emitterPosition, StringBuffer writer) {
		Vector3D detectorPosition = new Vector3D(0, y, 0);
		
		Vector3D vertex1 = new Vector3D(triangle[0][0] - x, triangle[0][1], triangle[0][2]);
		Vector3D vertex2 = new Vector3D(triangle[1][0] - x, triangle[1][1], triangle[1][2]);
		Vector3D vertex3 = new Vector3D(triangle[2][0] - x, triangle[2][1], triangle[2][2]);
		//check both directions
		if(RayTester.intersectLineTriangle(emitterPosition, detectorPosition, vertex1, vertex2, vertex3)) {
			Vector3D intersectionPoint = RayTester.getIntersectionPoint(emitterPosition, detectorPosition, vertex1, vertex2, vertex3);
			Vector3D intersectionRay = intersectionPoint.minus(emitterPosition);
			writer.append((intersectionPoint.x + x) + ", " + intersectionPoint.y + ", " + intersectionPoint.z + "\n");
			double depth = intersectionRay.getLength();
			return depth;
		} else if(RayTester.intersectLineTriangle(detectorPosition, emitterPosition, vertex1, vertex2, vertex3)) {
			Vector3D intersectionPoint = RayTester.getIntersectionPoint(detectorPosition, emitterPosition, vertex1, vertex2, vertex3);
			Vector3D intersectionRay = intersectionPoint.minus(emitterPosition);
			writer.append((intersectionPoint.x + x) + ", " + intersectionPoint.y + ", " + intersectionPoint.z + "\n");
			double depth = intersectionRay.getLength();
			return depth;
		}  
		return 0;
	}
	
	private static Rectangle calculateScreenArea(float[][] triangle, SimulationSettings settings) {
		
		double minX = Math.min(Math.min(triangle[0][0], triangle[1][0]), triangle[2][0]);
		double minY = Math.min(Math.min(triangle[0][1], triangle[1][1]), triangle[2][1]);
		double minZ = Math.min(Math.min(triangle[0][2], triangle[1][2]), triangle[2][2]);
		
		double maxX = Math.max(Math.max(triangle[0][0], triangle[1][0]), triangle[2][0]);
		double maxY = Math.max(Math.max(triangle[0][1], triangle[1][1]), triangle[2][1]);
		double maxZ = Math.max(Math.max(triangle[0][2], triangle[1][2]), triangle[2][2]);
		
		
		//calculate required Y axis screen space
		double areaMinStartY = calculateDetectorYIntersection(settings.emitterPosition, minY, minZ);
		double areaMaxStartY = calculateDetectorYIntersection(settings.emitterPosition, maxY, minZ);
		double areaMinEndY = calculateDetectorYIntersection(settings.emitterPosition, minY, maxZ);
		double areaMaxEndY = calculateDetectorYIntersection(settings.emitterPosition, maxY, maxZ);
		
		double areaStartY = Math.min(Math.min(Math.min(areaMinStartY, areaMaxStartY), areaMinEndY), areaMaxEndY);
		double areaEndY = Math.max(Math.max(Math.max(areaMinStartY, areaMaxStartY), areaMinEndY), areaMaxEndY);
		
		
		double rise = settings.emitterPosition.x; //if x is negative, slope will be negative too and vice versa
		double run = settings.emitterPosition.z; //detector is always at z = 0, emitter z is always positive
		
		double emitterRaySlopeXZ = rise / run; //slope for x as a function of z
		
		double areaMinStartX = minX - (emitterRaySlopeXZ * minZ);
		double areaMaxStartX = minX - (emitterRaySlopeXZ * maxZ);
		double areaMinEndX = maxX - (emitterRaySlopeXZ * minZ);
		double areaMaxEndX = maxX - (emitterRaySlopeXZ * maxZ);
		
		double areaStartX = Math.min(areaMinStartX, areaMaxStartX);
		double areaEndX = Math.max(areaMinEndX, areaMaxEndX);
		
		
		//add a 1 pixel border around screen area to avoid issues with rounding errors
		return new Rectangle(
				(int) Math.floor(areaStartX),
				(int) Math.floor(areaStartY),
				(int) Math.ceil(areaEndX),
				(int) Math.ceil(areaEndY)
			);
	}

	private static double calculateDetectorYIntersection(Vector3D emitterPosition, double y, double z) {
		double dy = y - emitterPosition.y;
		double dz = emitterPosition.z - z;
		
		return emitterPosition.y + ((emitterPosition.z * dy) / dz);
	}
}
