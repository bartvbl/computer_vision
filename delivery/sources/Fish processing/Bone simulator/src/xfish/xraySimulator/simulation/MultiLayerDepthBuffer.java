package xfish.xraySimulator.simulation;

import java.awt.image.BufferedImage;
import java.util.Arrays;

import javax.imageio.ImageIO;

public class MultiLayerDepthBuffer {
	private static final int MAX_LAYERS_PER_PIXEL = 20;
	
	private final double[][][] depthBuffer;
	private final int[][] bufferPointers;
	private final int[][] lastTriangleIndex;

	private final int width;
	private final int height;
	
	public MultiLayerDepthBuffer(int width, int height) {
		this.depthBuffer = new double[width][height][MAX_LAYERS_PER_PIXEL];
		this.bufferPointers = new int[width][height];
		this.lastTriangleIndex = new int[width][height];
		this.width = width;
		this.height = height;
	}
	
	public synchronized void put(int x, int y, double depth, int triangleID) {
		if((x < 0) || (x >= width) || (y < 0) || (y >= height)) {
			return;
		}
		
		try {
			this.depthBuffer[x][y][this.bufferPointers[x][y]] = depth;
			this.bufferPointers[x][y]++;
			this.lastTriangleIndex[x][y] = triangleID;
		} catch(IndexOutOfBoundsException e) {
			System.out.println("Buffer at " + x + ", " + y + " is full. Array: " + Arrays.toString(this.depthBuffer[x][y]));
		}
	}
	
	public synchronized double[][] generateDepthImage() {
		int badPixelCount = 0;
		double[][] depthImage = new double[width][height];
		
		for(int x = 0; x < width; x++) {
			for(int y = 0; y < height; y++) {
				int bufferPointer = bufferPointers[x][y];
				
				if(bufferPointer == 0) {
					continue;
				}
				
				if(bufferPointer % 2 == 1) {
					badPixelCount++;
					continue;
				}
				
				Arrays.sort(depthBuffer[x][y], 0, bufferPointer);
				
				double totalDistance = 0;
				double[] pixelDepthBuffer = depthBuffer[x][y];
				
				for(int i = 0; i < bufferPointer; i += 2) {
					totalDistance += pixelDepthBuffer[i + 1] - pixelDepthBuffer[i];
				}
				
				depthImage[x][y] = totalDistance;
			}
		}
		
		System.out.println("Ignored pixel count: " + badPixelCount);
		
		return depthImage;
	}
	
	public BufferedImage generateTriangleIndexImage() {
		BufferedImage outputImage = new BufferedImage(width, height, BufferedImage.TYPE_4BYTE_ABGR);
		for(int x = 0; x < width; x++) {
			for(int y = 0; y < height; y++) {
				int solidBaseColour = 0xFF000000;
				int triangleID = this.lastTriangleIndex[x][y];
				int pixelColour = solidBaseColour | triangleID;
				outputImage.setRGB(x, height - 1 - y, pixelColour); // flip Y axis
			}
		}
		return outputImage;
	}
}
