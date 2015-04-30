package xfish.xraySimulator.simulation;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.Random;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

import javax.imageio.ImageIO;
import javax.swing.JOptionPane;

import org.lwjgl.util.vector.Matrix4f;
import org.lwjgl.util.vector.Vector3f;

import lib.geom.STLFileContents;
import lib.geom.Vector3D;
import lib.loaders.STLLoader;
import xfish.xraySimulator.SimulatorSetupWindow;
import xfish.xraySimulator.data.BoundingBox;
import xfish.xraySimulator.data.GeometryFile;
import xfish.xraySimulator.data.SimulationSettings;
import xfish.xraySimulator.ui.MainWindowController;
import xfish.xraySimulator.ui.ProgressWindow;

public class SimulationThread extends Thread {
	private final SimulationSettings settings;
	private final MainWindowController controller;
	private final SimulatorSetupWindow window;
	private final Random random = new Random(System.nanoTime());

	public SimulationThread(SimulationSettings settings, MainWindowController controller, SimulatorSetupWindow window) {
		this.settings = settings;
		this.controller = controller;
		this.window = window;
	}

	public void run() {
		try {
			simulate();
		} catch (IOException e) {
			e.printStackTrace();
			JOptionPane.showMessageDialog(null, "Error during simulation: " + e.getMessage());
		}
	}

	private void simulate() throws IOException {
		controller.setUIEnabled(false);
		ProgressWindow progressWindow = new ProgressWindow(window);
		
		
//		System.out.println("Image size: " + settings.imageWidth + ", " + settings.imageHeight);
		
		BufferedImage outputImage = new BufferedImage(settings.imageWidth, settings.imageHeight, BufferedImage.TYPE_INT_RGB);
		double[][] outputImageData = new double[settings.imageWidth][settings.imageHeight];
		

		CyclicBarrier mainBarrier = new CyclicBarrier(settings.geometryFiles.length + 1);
		MultiLayerDepthBuffer[] depthBuffers = new MultiLayerDepthBuffer[settings.geometryFiles.length];
		
		int totalTriangleCount = 0;
		
		for(STLFileContents model : settings.models) {
			totalTriangleCount += model.triangleCount;
		}
		
//		System.out.println("Simulating with settings");
//		System.out.println("Destination: " + settings.destinationFile);
//		System.out.println(settings);
		
		int triangleLabelStartIndex = 0;
		
		for(int i = 0; i < settings.geometryFiles.length; i++) {
			depthBuffers[i] = new MultiLayerDepthBuffer(settings.imageWidth, settings.imageHeight);
			
			
			new SingleModelRenderingThread(settings.models[i], settings, depthBuffers[i], progressWindow, mainBarrier, totalTriangleCount, triangleLabelStartIndex).start();
			triangleLabelStartIndex += settings.models[i].triangleCount;
		}

		try {
			mainBarrier.await();
		} catch (InterruptedException | BrokenBarrierException e) {
			e.printStackTrace();
		}
		
		for(int i = 0; i < settings.geometryFiles.length; i++) {
			addBuffer(outputImageData, depthBuffers[i], settings.geometryFiles[i].getDensity());
		}

		normalise(outputImageData);
		
		generateOutputImage(outputImage, outputImageData);

		for(int i = 0; i < depthBuffers.length; i++) {
			BufferedImage idImage = depthBuffers[i].generateTriangleIndexImage();
			if(!settings.triangleIDFiles[i].exists()) {
				settings.triangleIDFiles[i].getParentFile().mkdirs();
				settings.triangleIDFiles[i].createNewFile();
			}
			ImageIO.write(idImage, "png", settings.triangleIDFiles[i]);
		}
		ImageIO.write(outputImage, "png", settings.destinationFile);
		progressWindow.destroy();
		controller.setUIEnabled(true);
	}

	private void addBuffer(double[][] outputImageData, MultiLayerDepthBuffer buffer, int density) {
		double[][] bufferContents = buffer.generateDepthImage();
		for(int x = 0; x < settings.imageWidth; x++) {
			for(int y = 0; y < settings.imageHeight; y++) {
				outputImageData[x][y] += bufferContents[x][y] * (double) density;
			}
		}
	}

	private void generateOutputImage(BufferedImage outputImage, double[][] outputImageData) {
		for(int x = 0; x < settings.imageWidth; x++) {
			for(int y = 0; y < settings.imageHeight; y++) {
				double doubleColour = (outputImageData[x][y] * 235d) + 20;
				//invert colour
				doubleColour = 255 - doubleColour;
				doubleColour = applyNoise(doubleColour);
				int colour = ((int) doubleColour) & 0x000000FF;
				int pixel = 0xFF000000;
				pixel = colour | pixel;
				pixel = (colour << 8) | pixel;
				pixel = (colour << 16) | pixel;
				outputImage.setRGB(x, settings.imageHeight - 1 - y, pixel); // Y-axis is flipped because 
			}
		}
	}

	private double applyNoise(double colour) {
		double standardDeviation = 9.95145 - 0.03174 * colour;
		double newColour = colour + Math.round(random.nextGaussian() * standardDeviation);
		return Math.min(255, Math.max(0, newColour));
	}

	private void normalise(double[][] imageData) {
		double min = Double.POSITIVE_INFINITY;
		double max = Double.NEGATIVE_INFINITY;
		int xmin = 0, xmax = 0, ymin = 0, ymax = 0;

		for(int x = 0; x < settings.imageWidth; x++) {
			for(int y = 0; y < settings.imageHeight; y++) {
				if(imageData[x][y] < min) {
					min = imageData[x][y];
					xmin = x;
					ymin = y;
				}
				if(imageData[x][y] > max) {
					max = imageData[x][y];
					xmax = x;
					ymax = y;
				}
			}
		}
		
		//System.out.println(min + " to " + max);
		//System.out.println("At (" + xmin + ", " + ymin + "), (" + xmax + ", " + ymax + ")");
		
		double delta = max - min;

		for(int x = 0; x < settings.imageWidth; x++) {
			for(int y = 0; y < settings.imageHeight; y++) {
				imageData[x][y] = (imageData[x][y] - min) / delta;
			}
		}
	}
}
