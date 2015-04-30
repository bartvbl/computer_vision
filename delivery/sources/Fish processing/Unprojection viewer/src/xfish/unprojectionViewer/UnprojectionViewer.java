package xfish.unprojectionViewer;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import static org.lwjgl.opengl.GL11.*;

import org.lwjgl.input.Controller;
import org.lwjgl.input.Controllers;
import org.lwjgl.input.Keyboard;
import org.lwjgl.input.Mouse;
import org.lwjgl.opengl.Display;
import org.lwjgl.util.vector.Matrix4f;
import org.lwjgl.util.vector.Vector3f;

import lib.geom.Point3D;
import lib.geom.STLFileContents;
import lib.geom.VBOContents;
import lib.gl.GraphicsController;
import lib.gl.RenderUtils;
import lib.gl.vbo.GeometryBufferGenerator;
import lib.gl.vbo.GeometryNode;
import lib.gl.vbo.VBOFormat;
import lib.loaders.STLLoader;

public class UnprojectionViewer {
	private static double x;
	private static double y; 
	private static double z;
	
	private static double rotationX;
	private static double rotationY;
	private static double rotationZ;
	
	private static final double moveSpeed = 5;
	private static final double rotationSpeed = 0.6;
	
	private static boolean isControllerActive = false;
	private static Controller controller;

	public static void main(String[] args) {
		try {
			runViewer();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private static void runViewer() throws Exception {
		RenderUtils.initOpenGL("STL Viewer");
		Mouse.create();
		Keyboard.create();
		Controllers.create();
		
		for(int i = 0; i < Controllers.getControllerCount(); i++) {
			controller = Controllers.getController(i);
			if((controller.getAxisCount() == 5) && (controller.getButtonCount() == 10)) {
				isControllerActive = true;
				System.out.println("Controller detected!");
				break;
			}
		}
		
		ColouredPoint3D[] correctPoints = loadPoints("res/coordinates_correct.txt", false);
		ColouredPoint3D[] reconstructedPoints = loadPoints("res/coordinates_unprojected.txt", true);

		int correctPointsList = renderPoints(correctPoints);
		int reconstructedList = renderPoints(reconstructedPoints);
		
		GeometryNode boneNode = loadFishModel(new File("res/boneLarge.stl"));
		GeometryNode fishNode = loadFishModel(new File("res/fishLarge.stl"));
		
		
		while(!Display.isCloseRequested()) {
			RenderUtils.newFrame();
			RenderUtils.set3DMode();
			glEnable(GL_NORMALIZE);
			glDisable(GL_TEXTURE_2D);
			
			handleUserInput();
			
			glPushMatrix();

			glDisable(GL_LIGHTING);
			
			glColor4d(0, 1, 0, 1);
			glPointSize(1);
			glCallList(correctPointsList);
			
			glPointSize(5);
			glColor4d(1, 0, 0, 1);
			glCallList(reconstructedList);
			
			glEnable(GL_LIGHTING);
			
			glColor4d(1, 1, 1, 0.2);
			boneNode.render();
			if(Keyboard.isKeyDown(Keyboard.KEY_SPACE)) {
				fishNode.render();				
			}
			
			glDisable(GL_LIGHTING);
			
			glPopMatrix();
			
			glColor4d(1, 0, 0, 1);
			drawDetectorLines();
			
			glColor4d(0, 0, 1, 1);
			drawBoundingBox();

			drawOrigin();
			
			RenderUtils.set2DMode();
			//GraphicsController.drawString(10, 35, "Bart van Blokland (NTNU)");
			//GraphicsController.drawString(10, 10, "Preliminary results stereoscopic bone reconstruction");
			
			if(Keyboard.isKeyDown(Keyboard.KEY_R)) {
				correctPoints = loadPoints("res/coordinates_correct.txt", false);
				reconstructedPoints = loadPoints("res/coordinates_unprojected.txt", true);
				
				glDeleteLists(correctPointsList, 2);

				correctPointsList = renderPoints(correctPoints);
				reconstructedList = renderPoints(reconstructedPoints);
			}
		}
	}

	private static void drawBoundingBox() {
		glBegin(GL_LINES);
		double[] xCoordinates = {0, (300.51648 + 0.014505532) * 6.8212605};
		double[] yCoordinates = {0, (53.302094 + 47.669716) * 6.8212605};
		double[] zCoordinates = {0, (16.081057 + 1.3891844) * 6.8212605};
		
		glVertex3d(xCoordinates[0], yCoordinates[0], zCoordinates[0]);
		glVertex3d(xCoordinates[1], yCoordinates[0], zCoordinates[0]);
		glVertex3d(xCoordinates[1], yCoordinates[0], zCoordinates[0]);
		glVertex3d(xCoordinates[1], yCoordinates[1], zCoordinates[0]);
		glVertex3d(xCoordinates[1], yCoordinates[1], zCoordinates[0]);
		glVertex3d(xCoordinates[0], yCoordinates[1], zCoordinates[0]);
		glVertex3d(xCoordinates[0], yCoordinates[1], zCoordinates[0]);
		glVertex3d(xCoordinates[0], yCoordinates[0], zCoordinates[0]);
		
		glVertex3d(xCoordinates[0], yCoordinates[0], zCoordinates[1]);
		glVertex3d(xCoordinates[1], yCoordinates[0], zCoordinates[1]);
		glVertex3d(xCoordinates[1], yCoordinates[0], zCoordinates[1]);
		glVertex3d(xCoordinates[1], yCoordinates[1], zCoordinates[1]);
		glVertex3d(xCoordinates[1], yCoordinates[1], zCoordinates[1]);
		glVertex3d(xCoordinates[0], yCoordinates[1], zCoordinates[1]);
		glVertex3d(xCoordinates[0], yCoordinates[1], zCoordinates[1]);
		glVertex3d(xCoordinates[0], yCoordinates[0], zCoordinates[1]);
		
		glVertex3d(xCoordinates[0], yCoordinates[0], zCoordinates[0]);
		glVertex3d(xCoordinates[0], yCoordinates[0], zCoordinates[1]);
		glVertex3d(xCoordinates[0], yCoordinates[1], zCoordinates[0]);
		glVertex3d(xCoordinates[0], yCoordinates[1], zCoordinates[1]);
		glVertex3d(xCoordinates[1], yCoordinates[0], zCoordinates[0]);
		glVertex3d(xCoordinates[1], yCoordinates[0], zCoordinates[1]);
		glVertex3d(xCoordinates[1], yCoordinates[1], zCoordinates[0]);
		glVertex3d(xCoordinates[1], yCoordinates[1], zCoordinates[1]);
		glEnd();
	}

	private static void drawDetectorLines() {
		glBegin(GL_LINES);
		//first detector
		glVertex3d(125, 0, 0);
		glVertex3d(125, 688, 0);
		glVertex3d(125, 0, 0);
		glVertex3d(259, 344, 600);
		glVertex3d(125, 688, 0);
		glVertex3d(259, 344, 600);
		
		//second detector
		glVertex3d(430, 0, 0);
		glVertex3d(430, 688, 0);
		glVertex3d(430, 0, 0);
		glVertex3d(259, 344, 600);
		glVertex3d(430, 688, 0);
		glVertex3d(259, 344, 600);
		glEnd();
	}

	private static void drawOrigin() {
		glBegin(GL_LINES);
		glColor4d(1, 0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(100, 0, 0);
		
		glColor4d(0, 1, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 100, 0);
		
		glColor4d(0, 0, 1, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 100);
		glEnd();
	}

	private static int renderPoints(Point3D[] points) {
		int pointsList = glGenLists(1);
		glDisable(GL_LIGHTING);
		glNewList(pointsList, GL_COMPILE);

		glBegin(GL_POINTS);
		for(Point3D point : points) {
			glVertex3d(point.x, point.y, point.z);
		}
		glEnd();
		glEndList();
		glEnable(GL_LIGHTING);
		return pointsList;
	}

	private static void handleUserInput() {
		float deadZone = 0.2f;
		
		double deltaX = (Keyboard.isKeyDown(Keyboard.KEY_D) ? 1 : 0) - (Keyboard.isKeyDown(Keyboard.KEY_A) ? 1 : 0);
		double deltaY = (Keyboard.isKeyDown(Keyboard.KEY_W) ? 1 : 0) - (Keyboard.isKeyDown(Keyboard.KEY_S) ? 1 : 0);
		double deltaZ = (Keyboard.isKeyDown(Keyboard.KEY_Q) ? 1 : 0) - (Keyboard.isKeyDown(Keyboard.KEY_E) ? 1 : 0);

		double deltaRotationY = -((Keyboard.isKeyDown(Keyboard.KEY_LEFT) ? 1 : 0) - (Keyboard.isKeyDown(Keyboard.KEY_RIGHT) ? 1 : 0));
		double deltaRotationX = -((Keyboard.isKeyDown(Keyboard.KEY_UP) ? 1 : 0) - (Keyboard.isKeyDown(Keyboard.KEY_DOWN) ? 1 : 0));
		
		if(isControllerActive) {
			controller.setXAxisDeadZone(deadZone);
			controller.setYAxisDeadZone(deadZone);
			
			controller.setRXAxisDeadZone(deadZone);
			controller.setRYAxisDeadZone(deadZone);

			deltaX += controller.getAxisValue(1) * moveSpeed;
			deltaY += -1 * controller.getAxisValue(0) * moveSpeed;
			deltaZ += ((controller.isButtonPressed(4) ? 1 : 0) + (controller.isButtonPressed(5) ? -1 : 0)) * moveSpeed;

			deltaRotationX += controller.getAxisValue(2);
			deltaRotationY += controller.getAxisValue(3);
		}
		
		rotationY += deltaRotationY * 2;
		rotationX += deltaRotationX * 2;
		
		// forward - backward
		x -= deltaY * Math.sin(Math.toRadians(rotationY));
	    z += deltaY * Math.cos(Math.toRadians(rotationY));
	    
	    // left - right
	    x -= deltaX * Math.sin(Math.toRadians(rotationY + 90));
	    z += deltaX * Math.cos(Math.toRadians(rotationY + 90));
	    
	    y += deltaZ;
		
		glRotated(rotationX, 1, 0, 0);
		glRotated(rotationY, 0, 1, 0);
		glRotated(rotationZ, 0, 0, 1);
		glTranslated(x, y, z);
		
	}
	
	private static ColouredPoint3D[] loadPoints(String src, boolean enableColours) {
		ArrayList<ColouredPoint3D> points = new ArrayList<ColouredPoint3D>();
		try {
			BufferedReader reader = new BufferedReader(new FileReader(src));
			int lineNumber = 0;
			while(reader.ready()) {
				lineNumber++;
				String line = reader.readLine();
				int colour = 0xFF;
				if(enableColours) {
					String[] colourAndCoordinate = line.split(": ");
					colour = (int) Long.parseLong(colourAndCoordinate[0].substring(2).toUpperCase(), 16);
					line = colourAndCoordinate[1];
				}
				String[] parts = line.split(", ");
				if(parts.length < 3) {
					System.out.println("Incorrect point format on line " + lineNumber);
				} else {					
					try {
						double x = Double.parseDouble(parts[0]);
						double y = Double.parseDouble(parts[1]);
						double z = Double.parseDouble(parts[2]);
						ColouredPoint3D point = new ColouredPoint3D(x, y, z, colour);
						points.add(point);
					} catch(NumberFormatException e) {
						System.out.println("Incorrect number format on line " + lineNumber);
					}
				}
			}
			reader.close();
			ColouredPoint3D[] pointArray = new ColouredPoint3D[points.size()];
			points.toArray(pointArray);
			return pointArray;
		} catch (IOException e) {
			e.printStackTrace();
		}
		return new ColouredPoint3D[0];
		
	}

	private static GeometryNode loadFishModel(File src) throws IOException {
		VBOContents fishGeometry = STLLoader.loadResource(src).asVBOContents();
		fishGeometry = transformModel(fishGeometry);
		return GeometryBufferGenerator.generateGeometryBuffer(VBOFormat.VERTICES_AND_NORMALS, fishGeometry);
	}
	
	private static VBOContents transformModel(VBOContents fishGeometry) {
		Matrix4f geometryTransform = new Matrix4f();
		geometryTransform.rotate((float) (Math.PI / -2d), new Vector3f(1, 0, 0));
		fishGeometry = fishGeometry.transform(geometryTransform);
		geometryTransform = new Matrix4f();
		geometryTransform.scale(new Vector3f(6.8212605f, 6.8212605f, 6.8212605f));
		geometryTransform.translate(new Vector3f(0.014505532f, 47.669716f, 1.3891844f));
		return fishGeometry.transform(geometryTransform);
	}
}
