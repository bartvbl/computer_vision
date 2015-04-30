package xfish.xraySimulator.ui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.Arrays;

import javax.swing.DefaultListModel;

import org.lwjgl.util.vector.Matrix4f;
import org.lwjgl.util.vector.Vector3f;

import lib.geom.STLFileContents;
import lib.geom.Vector3D;
import lib.loaders.STLLoader;
import xfish.xraySimulator.SimulatorSetupWindow;
import xfish.xraySimulator.data.BoundingBox;
import xfish.xraySimulator.data.GeometryFile;
import xfish.xraySimulator.data.SimulationSettings;
import xfish.xraySimulator.simulation.SimulationThread;

public class StartSimulationHandler implements ActionListener {

	private final SimulatorSetupWindow window;

	public StartSimulationHandler(SimulatorSetupWindow window) {
		this.window = window;
		
		window.simulateButton.addActionListener(this);
	}

	@Override
	public void actionPerformed(ActionEvent event) {
		try {
		DefaultListModel<GeometryFile> model = (DefaultListModel<GeometryFile>) window.geometrySourceList.getModel();
		GeometryFile[] chosenFiles = new GeometryFile[model.size()];
		model.copyInto(chosenFiles);
		
		int firstDetectorPosition = (int) window.leftDetector.getValue();
		int secondDetectorPosition = (int) window.rightDetector.getValue();
		
		int emitterX_left = (int) window.emitterX.getValue() - firstDetectorPosition;
		int emitterX_right = (int) window.emitterX.getValue() - secondDetectorPosition;
		
		int emitterY = (int) window.emitterY.getValue();
		int emitterZ = (int) window.emitterZ.getValue();
		
		Vector3D emitterPosition_left = new Vector3D(emitterX_left, emitterY, emitterZ);
		Vector3D emitterPosition_right = new Vector3D(emitterX_right, emitterY, emitterZ);
		
		
		int fishRotation = (int) window.rotationSpinner.getModel().getValue();
		

		SimulationSettings settings_left = new SimulationSettings(chosenFiles, emitterPosition_left, fishRotation, "left");
		SimulationSettings settings_right = new SimulationSettings(chosenFiles, emitterPosition_right, fishRotation, "right");

		STLFileContents[] models = prepareModels(chosenFiles, settings_left, settings_right);
		settings_left.models = models;
		settings_right.models = models;
		
		MainWindowController controller = new MainWindowController(window);
		
		System.out.println("Simulating with size (" + settings_left.imageWidth + ", " + settings_left.imageHeight + ")");
		
		new SimulationThread(settings_left, controller, window).start();
		new SimulationThread(settings_right, controller, window).start();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private STLFileContents[] prepareModels(GeometryFile[] geometryFiles, SimulationSettings settings_left, SimulationSettings settings_right) throws IOException {
		STLFileContents[] models = loadModels(geometryFiles);
		rotateModels(models, settings_left);
		BoundingBox modelBounds = calculateBoundingBox(models);
		System.out.println("Bounding box: " + modelBounds);
		updateImageHeight(modelBounds, settings_left);
		updateImageHeight(modelBounds, settings_right);
		Matrix4f geometryTransformation = calculateGeometryTransformation(models, modelBounds);
		for(int i = 0; i < geometryFiles.length; i++) {
			models[i] = models[i].transform(geometryTransformation);
		}
		return models;
	}
	
	private void updateImageHeight(BoundingBox modelBounds, SimulationSettings settings) {
		float scaleFactor = (float) (((double) SimulationSettings.BASE_IMAGE_WIDTH) / (modelBounds.xMax - modelBounds.xMin));
		double boundsY = modelBounds.yMax - modelBounds.yMin;
		settings.imageHeight = (int) (boundsY * scaleFactor);
		settings.updateEmitterPosition(new Vector3D(settings.emitterPosition.x, settings.imageHeight / 2, settings.emitterPosition.z));
//		System.out.println("Updated image height: " + settings.imageHeight);
	}

	private void rotateModels(STLFileContents[] models, SimulationSettings settings) {
		Matrix4f rotationTransform = new Matrix4f();
		System.out.println("-> rotating by " + settings.fishRotation + " around [0, 0, 1]");
		rotationTransform.rotate((float) Math.toRadians(settings.fishRotation), new Vector3f(0, 0, 1));
		//rotate the model (required for different Y/Z axis handling of fish models)
		System.out.println("-> rotating by PI/-2 around [1, 0, 0]");
		rotationTransform.rotate((float) (Math.PI / -2d), new Vector3f(1, 0, 0));
		for(int i = 0; i < models.length; i++) {
			models[i] = models[i].transform(rotationTransform);
		}
	}
	
	private STLFileContents[] loadModels(GeometryFile[] geometryFiles) throws IOException {
		int modelCount = geometryFiles.length;
		STLFileContents[] loadedModels = new STLFileContents[modelCount];

		for(int i = 0; i < modelCount; i++) {
			loadedModels[i] = STLLoader.loadResource(geometryFiles[i].file);
		}

		return loadedModels;
	}
	
	private static Matrix4f calculateGeometryTransformation(STLFileContents[] models, BoundingBox modelBounds) {
		//reminder: transformations are applied in reverse order
		Matrix4f geometryTransformation = new Matrix4f();
		//scale factor to make the model cover the image
		float scaleFactor = (float) (((double) SimulationSettings.BASE_IMAGE_WIDTH) / (modelBounds.xMax - modelBounds.xMin));
		System.out.println("-> Scaling by " + scaleFactor);
		geometryTransformation.scale(new Vector3f(scaleFactor, scaleFactor, scaleFactor));
		//move the model back to the origin
		Vector3f moveToOriginVector = new Vector3f(-modelBounds.xMin, -modelBounds.yMin, -modelBounds.zMin);
		geometryTransformation.translate(moveToOriginVector);
		System.out.println("-> Translating by " + moveToOriginVector);
		return geometryTransformation;
	}

	private static BoundingBox calculateBoundingBox(STLFileContents[] models) {
		BoundingBox totalBoundingBox = models[0].calculateBoundingBox();
		//don't recalculate the first bounding box -> i = 1
		for(int i = 1; i < models.length; i++) {
			BoundingBox modelBounds = models[i].calculateBoundingBox();
			totalBoundingBox = totalBoundingBox.include(modelBounds);
		}
		return totalBoundingBox;
	}

}
