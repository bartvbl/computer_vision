package lib.loaders;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.Arrays;

import lib.geom.STLFileContents;
import lib.geom.VBOContents;

public class STLLoader {
	private static final byte[] header = new byte[80];
	private static final int bytesPerTriangle = 12 + 12 + 12 + 12 + 2;

	public static STLFileContents loadResource(File location) throws IOException {
		FileInputStream inputStream = new FileInputStream(location);
		
		byte[] singleTriangle = new byte[bytesPerTriangle];
		ByteBuffer triangleBuffer = ByteBuffer.allocate(bytesPerTriangle);
		triangleBuffer.order(ByteOrder.LITTLE_ENDIAN);
		
		inputStream.read(header);
		System.out.println("File header: " + new String(header));
		
		int triangleCount = getTriangleCount(inputStream);
		
		float[][] normals = new float[triangleCount][3];
		float[][][] vertices = new float[triangleCount][3][3];
		
		for(int i = 0; i < triangleCount; i++) {
			inputStream.read(singleTriangle);
			triangleBuffer.rewind();
			triangleBuffer.put(singleTriangle);
			triangleBuffer.rewind();
			
			normals[i][0] = -triangleBuffer.getFloat();
			normals[i][1] = triangleBuffer.getFloat();
			normals[i][2] = -triangleBuffer.getFloat();

			for(int j = 0; j < 3; j++) {
				vertices[i][j][0] = triangleBuffer.getFloat();
				vertices[i][j][1] = -triangleBuffer.getFloat();
				vertices[i][j][2] = triangleBuffer.getFloat();
			}
		}
		
		inputStream.close();
		
		STLFileContents geometry = new STLFileContents(normals, vertices);
		
		return geometry;
	}

	private static int getTriangleCount(FileInputStream inputStream) throws IOException {
		ByteBuffer singleIntBuffer = ByteBuffer.allocate(4);
		singleIntBuffer.order(ByteOrder.LITTLE_ENDIAN);
		byte[] intArray = new byte[4];
		
		inputStream.read(intArray);
		singleIntBuffer.put(intArray);
		singleIntBuffer.rewind();
		int triangleCount = singleIntBuffer.getInt();
		
		System.out.println("Triangle count: " + triangleCount);
		return triangleCount;
	}

}
