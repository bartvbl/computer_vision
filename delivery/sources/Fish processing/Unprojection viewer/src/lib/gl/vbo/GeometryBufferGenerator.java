package lib.gl.vbo;

import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import lib.geom.VBOContents;

import org.lwjgl.BufferUtils;

import static org.lwjgl.opengl.GL15.*;

import org.lwjgl.opengl.GLContext;

public class GeometryBufferGenerator {
	
	public static GeometryNode generateGeometryBuffer(VBOFormat bufferFormat, VBOContents geometry) {
		DoubleBuffer geometryData = BufferUtils.createDoubleBuffer(geometry.vertexCount * 3 * 2);
		
		FloatBuffer vertices = BufferUtils.createFloatBuffer(geometry.vertexCount * 3);
		FloatBuffer normals = BufferUtils.createFloatBuffer(geometry.vertexCount * 3);
		
		vertices.put(geometry.vertices).rewind();
		normals.put(geometry.normals).rewind();
		
		for(int i = 0; i < geometry.vertexCount; i++) {
			geometryData.put(vertices.get());
			geometryData.put(vertices.get());
			geometryData.put(vertices.get());
			geometryData.put(normals.get());
			geometryData.put(normals.get());
			geometryData.put(normals.get());
		}
		
		geometryData.rewind();
		
		IntBuffer indices = BufferUtils.createIntBuffer(geometry.indices.length);
		indices.put(geometry.indices).rewind();
		
		return storeBuffersInVRAM(geometryData, indices, bufferFormat, geometry.vertexCount, geometry.indices.length);
	}

	private static GeometryNode storeBuffersInVRAM(DoubleBuffer geometryData, IntBuffer indexes, VBOFormat dataFormat, int vertexCount, int indexCount)
	{
		geometryData.rewind();
		indexes.rewind();
		
		int vertexBufferID = createBuffer();
		int indexBufferID = createBuffer();
		
		storeIndexData(indexBufferID, indexes);
		storeVertexData(vertexBufferID, geometryData);
		
		return new GeometryNode(indexBufferID, vertexBufferID, dataFormat, vertexCount, indexCount, DrawingMode.TRIANGLES);
	}
	
	private static int createBuffer() {
		if (supportsBuffers()) 
		{
			IntBuffer buffer = BufferUtils.createIntBuffer(1);
			glGenBuffers(buffer);
			return buffer.get(0);
		}
		return 0;
	}
	
	private static void storeVertexData(int bufferIndex, DoubleBuffer geometryData) {
		if (supportsBuffers()) 
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferIndex);
			glBufferData(GL_ARRAY_BUFFER, geometryData, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
	
	private static void storeIndexData(int bufferIndex, IntBuffer indexes) {
		if (supportsBuffers()) 
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
	
	private static boolean supportsBuffers()
	{
		return GLContext.getCapabilities().GL_ARB_vertex_buffer_object;
	}

	public static GeometryNode generateNormalsGeometryBuffer(VBOFormat dataFormat, DoubleBuffer geometryData, IntBuffer indices) {
		int vertexCount = geometryData.capacity() / dataFormat.elementsPerVertex;
		System.out.println("Building " + vertexCount + " vertices");
		DoubleBuffer vertexBuffer = BufferUtils.createDoubleBuffer(vertexCount * 2 * 3);
		IntBuffer normalIndices = BufferUtils.createIntBuffer(vertexCount * 2 * 2);
		geometryData.rewind();
		for(int i = 0; i < vertexCount; i++) {
			double x = geometryData.get();
			double y = geometryData.get();
			double z = geometryData.get();
			if(dataFormat == VBOFormat.VERTICES_TEXTURES_NORMALS) {
				geometryData.get();
				geometryData.get();
			}
			double nx = geometryData.get();
			double ny = geometryData.get();
			double nz = geometryData.get();
			
			vertexBuffer.put(x);
			vertexBuffer.put(y);
			vertexBuffer.put(z);
			
			vertexBuffer.put(x + nx);
			vertexBuffer.put(y + ny);
			vertexBuffer.put(z + nz);
		}
		
		for(int i = 0; i < vertexCount * 2; i++) {
			normalIndices.put(i);
		}
		
		vertexBuffer.rewind();
		normalIndices.rewind();
		
		int vertexBufferID = createBuffer();
		int indexBufferID = createBuffer();
		
		storeIndexData(indexBufferID, normalIndices);
		storeVertexData(vertexBufferID, vertexBuffer);
		
		return new GeometryNode(indexBufferID, vertexBufferID, VBOFormat.VERTICES, vertexCount*2, vertexCount * 2 * 2, DrawingMode.LINES);
	}
}
