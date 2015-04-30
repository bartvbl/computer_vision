package lib.geom;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import org.lwjgl.util.vector.Matrix4f;
import org.lwjgl.util.vector.Vector4f;

import xfish.xraySimulator.data.BoundingBox;

public class STLFileContents {
	public final float[][] normals;
	public final float[][][] vertices;
	public final int triangleCount;

	public STLFileContents(float[][] normals, float[][][] vertices) {
		this.normals = normals;
		this.vertices = vertices;
		this.triangleCount = normals.length;
	}

	public VBOContents asVBOContents() {
		FloatBuffer normalBuffer = FloatBuffer.allocate(triangleCount * 9);
		FloatBuffer vertexBuffer = FloatBuffer.allocate(triangleCount * 9);
		IntBuffer indexBuffer = IntBuffer.allocate(triangleCount * 3);
		
		for(int i = 0; i < triangleCount; i++) {
			for(int j = 0; j < 3; j++) {
				normalBuffer.put(normals[i]);
				vertexBuffer.put(vertices[i][j]);
				indexBuffer.put(3*i + j);
			}
		}
		
		return new VBOContents(vertexBuffer.array(), normalBuffer.array(), indexBuffer.array());
	}

	public BoundingBox calculateBoundingBox() {
		float xMin = Float.POSITIVE_INFINITY;
		float xMax = Float.NEGATIVE_INFINITY;
		float yMin = Float.POSITIVE_INFINITY;
		float yMax = Float.NEGATIVE_INFINITY;
		float zMin = Float.POSITIVE_INFINITY;
		float zMax = Float.NEGATIVE_INFINITY;
		
		for(int triangle = 0; triangle < triangleCount; triangle++) {
			for(int vertex = 0; vertex < 3; vertex++) {
				xMin = Math.min(vertices[triangle][vertex][0], xMin);
				xMax = Math.max(vertices[triangle][vertex][0], xMax);
				yMin = Math.min(vertices[triangle][vertex][1], yMin);
				yMax = Math.max(vertices[triangle][vertex][1], yMax);
				zMin = Math.min(vertices[triangle][vertex][2], zMin);
				zMax = Math.max(vertices[triangle][vertex][2], zMax);
			}
		}
		
		return new BoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);
	}

	public STLFileContents transform(Matrix4f transformation) {
		Vector4f point = new Vector4f();
		
		Matrix4f normalMatrix = new Matrix4f();
		normalMatrix.load(transformation);
		//get rid of translation transformations
		normalMatrix.m30 = 0;
		normalMatrix.m31 = 0;
		normalMatrix.m32 = 0;
		
		float[][] transformedNormals = new float[triangleCount][3];
		float[][][] transformedVertices = new float[triangleCount][3][3];
		
		for(int triangle = 0; triangle < triangleCount; triangle++) {
			load(normals[triangle], point);
			Matrix4f.transform(normalMatrix, point, point);
			store(point, transformedNormals[triangle]);
			
			for(int vertex = 0; vertex < 3; vertex++) {
				load(vertices[triangle][vertex], point);
				Matrix4f.transform(transformation, point, point);
				store(point, transformedVertices[triangle][vertex]);
			}
		}
		
		return new STLFileContents(transformedNormals, transformedVertices);
	}

	private void load(float[] vector, Vector4f point) {
		point.setX(vector[0]);
		point.setY(vector[1]);
		point.setZ(vector[2]);
		point.setW(1);
	}
	
	private void store(Vector4f point, float[] destination) {
		destination[0] = point.x;
		destination[1] = point.y;
		destination[2] = point.z;
	}
}
