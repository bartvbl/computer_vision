package xfish.xraySimulator;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import lib.events.Event;
import lib.events.EventDispatcher;
import lib.events.EventType;
import xfish.xraySimulator.data.GeometryFile;
import xfish.xraySimulator.ui.GeometryFileEditor;
import xfish.xraySimulator.ui.GeometryListHandler;
import xfish.xraySimulator.ui.StartSimulationHandler;

public class XRaySimulator {

	public static void main(String[] args) {
		setSwingSettings();
		initWindow(args);
	}

	private static void initWindow(String[] args) {
		SimulatorSetupWindow window = new SimulatorSetupWindow();
		configureWindow(window);
		EventDispatcher eventDispatcher = new EventDispatcher();
		new GeometryListHandler(window, eventDispatcher);
		new GeometryFileEditor(window, eventDispatcher);
		new StartSimulationHandler(window);
		
		if((args.length > 1) && (args[0].equals("-load"))) {
			try {
				loadSettingsFile(args, window, eventDispatcher);
			} catch (IOException e) {
				System.out.println("Settings file was not found.");
				e.printStackTrace();
			}
		}
	}
	
	private static void loadSettingsFile(String[] args, SimulatorSetupWindow window, EventDispatcher eventDispatcher) throws IOException {
		File settingsFile = new File(args[1]);
		BufferedReader reader = new BufferedReader(new FileReader(settingsFile));
		while(reader.ready()) {
			String line = reader.readLine();
			
			if(line.startsWith("addModel")) {
				String[] parts = line.split(" ");
				String path = parts[1];
				int density = Integer.parseInt(parts[2]);
				
				File chosenFile = new File(path);
		    	GeometryFile addedFile = new GeometryFile(chosenFile, density);
		    	eventDispatcher.dispatchEvent(new Event<GeometryFile>(EventType.GEOMETRY_SOURCE_ADDED, addedFile));
				
				continue;
			}
			
			String[] settingParts = line.split(" = ");
			
			if(settingParts.length < 2) {
				continue;
			}
			
			int settingValue = Integer.parseInt(settingParts[1]);
			if(settingParts[0].equals("emitter.x")) {
				window.emitterX.setValue(settingValue);
			} else if(settingParts[0].equals("emitter.y")) {
				window.emitterY.setValue(settingValue);
			} else if(settingParts[0].equals("emitter.z")) {
				window.emitterZ.setValue(settingValue);
			} else if(settingParts[0].equals("detector1.x")) {
				window.leftDetector.setValue(settingValue);
			} else if(settingParts[0].equals("detector2.x")) {
				window.rightDetector.setValue(settingValue);
			} else if(settingParts[0].equals("image.width")) {
				window.imageWidth.setValue(settingValue);
			} else if(settingParts[0].equals("fish.rotation")) {
				window.rotationSpinner.setValue(settingValue);
			}
		}
		reader.close();
	}

	private static void configureWindow(SimulatorSetupWindow window) {
		window.setResizable(false);
		window.setTitle("X-Ray Simulator");
		window.setVisible(true);
		window.setLocation(150, 150);
	}

	private static void setSwingSettings() {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (UnsupportedLookAndFeelException e) {
			e.printStackTrace();
		}
	}

}
