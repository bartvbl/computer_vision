package xfish.xraySimulator.ui;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import xfish.xraySimulator.SimulatorSetupWindow;
import xfish.xraySimulator.data.GeometryFile;

public class DensitySliderHandler implements ChangeListener {

	private final SimulatorSetupWindow window;
	
	private GeometryFile currentSelectedFile;

	public DensitySliderHandler(SimulatorSetupWindow window) {
		this.window = window;
		this.window.geometryDensity.getModel().addChangeListener(this);
	}

	public void setCurrentGeometryFile(GeometryFile currentSelectedFile) {
		this.currentSelectedFile = currentSelectedFile;
		this.window.geometryDensity.setValue(currentSelectedFile.getDensity());
	}

	@Override
	public void stateChanged(ChangeEvent event) {
		if(currentSelectedFile != null) {
			int newDensity = (int) window.geometryDensity.getValue();
			currentSelectedFile.setDensity(newDensity);		
		}
	}

}
