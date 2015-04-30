package xfish.xraySimulator.ui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.DefaultListModel;
import javax.swing.DefaultListSelectionModel;
import javax.swing.JFileChooser;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import lib.events.Event;
import lib.events.EventDispatcher;
import lib.events.EventHandler;
import lib.events.EventType;
import xfish.xraySimulator.SimulatorSetupWindow;
import xfish.xraySimulator.data.GeometryFile;

public class GeometryListHandler implements ActionListener, ListSelectionListener, EventHandler {

	private final SimulatorSetupWindow window;
	private final DefaultListModel<GeometryFile> listModel;
	private final DefaultListSelectionModel listSelectionModel;
	private final EventDispatcher eventDispatcher;

	public GeometryListHandler(SimulatorSetupWindow window, EventDispatcher eventDispatcher) {
		this.listModel = new DefaultListModel<GeometryFile>();
		this.listSelectionModel = new DefaultListSelectionModel();
		this.window = window;
		this.eventDispatcher = eventDispatcher;
		eventDispatcher.addEventListener(this, EventType.GEOMETRY_SOURCE_ADDED);
		
		window.addGeometrySourceButton.addActionListener(this);
		window.removeSelectedSourceButton.addActionListener(this);
		this.listSelectionModel.addListSelectionListener(this);
		
		window.geometrySourceList.setModel(listModel);
		window.geometrySourceList.setSelectionModel(listSelectionModel);
		
		window.simulateButton.setEnabled(false);
	}

	@Override
	public void actionPerformed(ActionEvent event) {
		if(window.addGeometrySourceButton == event.getSource()) {
			openGeometrySourceSelection();
		}
		if(window.removeSelectedSourceButton == event.getSource()) {
			removeSelectedSource();
		}
	}

	private void openGeometrySourceSelection() {
		JFileChooser chooser = new JFileChooser();
		File workingDirectory = new File(System.getProperty("user.dir"));
		chooser.setCurrentDirectory(workingDirectory);
		int chosenOption = chooser.showOpenDialog(this.window);
		
		if(chosenOption == JFileChooser.APPROVE_OPTION) {
	    	File chosenFile = chooser.getSelectedFile();
	    	GeometryFile addedFile = new GeometryFile(chosenFile, 0);
	    	eventDispatcher.dispatchEvent(new Event<GeometryFile>(EventType.GEOMETRY_SOURCE_ADDED, addedFile)); // use the event handler because that handler is also used by the configuration file code
		}
	}
	
	private void removeSelectedSource() {
		int selectedIndex = this.listSelectionModel.getLeadSelectionIndex();
		if((selectedIndex != -1) && (!this.listModel.isEmpty())) {
			GeometryFile removedFile = this.listModel.get(selectedIndex);
			this.listModel.remove(selectedIndex);
			eventDispatcher.dispatchEvent(new Event<GeometryFile>(EventType.GEOMETRY_SOURCE_REMOVED, removedFile));
			if(listModel.size() == 0) {
				window.simulateButton.setEnabled(false);
			}
		}
	}

	@Override
	public void valueChanged(ListSelectionEvent event) {
		if(listSelectionModel.isSelectionEmpty()) {
			eventDispatcher.dispatchEvent(new Event<Object>(EventType.GEOMETRY_SOURCE_DESELECTED));
		} else {
			int selectedIndex = listSelectionModel.getLeadSelectionIndex();
			GeometryFile selectedFile = this.listModel.get(selectedIndex);
			eventDispatcher.dispatchEvent(new Event<GeometryFile>(EventType.GEOMETRY_SOURCE_SELECTED, selectedFile));
		}
	}

	@Override
	public void handleEvent(Event<?> event) {
		if(event.eventType == EventType.GEOMETRY_SOURCE_ADDED) {
			GeometryFile addedFile = (GeometryFile) event.getEventParameterObject();
			this.listModel.addElement(addedFile);
			window.simulateButton.setEnabled(true);			
		}
	}

}
