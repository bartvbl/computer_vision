package xfish.xraySimulator.ui;

import xfish.xraySimulator.SimulatorSetupWindow;
import xfish.xraySimulator.data.GeometryFile;
import lib.events.Event;
import lib.events.EventDispatcher;
import lib.events.EventHandler;
import lib.events.EventType;

public class GeometryFileEditor implements EventHandler {
	
	private boolean isActive = false;
	private GeometryFile currentSelectedFile = null;
	
	private final SimulatorSetupWindow window;
	private final DensitySliderHandler densitySliderHandler;

	public GeometryFileEditor(SimulatorSetupWindow window, EventDispatcher eventDispatcher) {
		this.window = window;
		
		eventDispatcher.addEventListener(this, EventType.GEOMETRY_SOURCE_SELECTED);
		eventDispatcher.addEventListener(this, EventType.GEOMETRY_SOURCE_DESELECTED);
		
		this.densitySliderHandler = new DensitySliderHandler(window);
		
		update();
	}

	private void update() {
		this.setUIElementsEnabled(isActive);
	}

	private void setUIElementsEnabled(boolean enabled) {
		window.geometryDensity.setEnabled(enabled);
	}

	@Override
	public void handleEvent(Event<?> event) {
		if(event.eventType == EventType.GEOMETRY_SOURCE_SELECTED) {
			isActive = true;
			this.currentSelectedFile = (GeometryFile) event.getEventParameterObject();
			this.densitySliderHandler.setCurrentGeometryFile(currentSelectedFile);
		} else if(event.eventType == EventType.GEOMETRY_SOURCE_DESELECTED) {
			isActive = false;
		}
		update();
	}

}
