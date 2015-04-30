package xfish.xraySimulator.ui;

import java.awt.Dimension;

import javax.swing.JDialog;
import javax.swing.JProgressBar;

import xfish.xraySimulator.SimulatorSetupWindow;

public class ProgressWindow {
	
	private final JDialog frame;
	private final JProgressBar progressBar;
	private double progress = 0;

	public ProgressWindow(SimulatorSetupWindow window) {
		this.frame = new JDialog(window);
		this.progressBar = new JProgressBar();
		
		frame.setMinimumSize(new Dimension(300, 50));
		frame.setLocation(100, 100);
		frame.setResizable(false);
		frame.add(progressBar);
		frame.setVisible(true);
	}

	public void destroy() {
		frame.setVisible(false);
	}

	public synchronized void incrementProgress(double amount) {
		progress += amount;
		this.progressBar.setValue((int) (progress * 100));
	}

}
