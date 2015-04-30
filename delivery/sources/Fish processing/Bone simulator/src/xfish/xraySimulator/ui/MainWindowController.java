package xfish.xraySimulator.ui;

import xfish.xraySimulator.SimulatorSetupWindow;

public class MainWindowController {

	private final SimulatorSetupWindow window;

	public MainWindowController(SimulatorSetupWindow window) {
		this.window = window;
	}
	
	public void setUIEnabled(boolean enabled) {
		if(!enabled) {
			window.geometrySourceList.getSelectionModel().clearSelection();
		}

		window.geometrySourceList.setEnabled(enabled);
		window.addGeometrySourceButton.setEnabled(enabled);
		window.removeSelectedSourceButton.setEnabled(enabled);
		window.emitterX.setEnabled(enabled);
		window.emitterY.setEnabled(enabled);
		window.emitterZ.setEnabled(enabled);
		window.leftDetector.setEnabled(enabled);
		window.rightDetector.setEnabled(enabled);
		window.geometryDensity.setEnabled(enabled);
		window.rotationSpinner.setEnabled(enabled);
		window.simulateButton.setEnabled(enabled);
		window.imageWidth.setEnabled(enabled);
	}

}
