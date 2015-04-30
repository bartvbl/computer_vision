package xfish.xraySimulator.simulation;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

import lib.geom.STLFileContents;
import xfish.xraySimulator.data.SimulationSettings;
import xfish.xraySimulator.ui.ProgressWindow;

public class SingleModelRenderingThread extends Thread {

	private final STLFileContents model;
	private final SimulationSettings settings;
	private final MultiLayerDepthBuffer depthBuffer;
	private final ProgressWindow progressWindow;
	private final CyclicBarrier mainBarrier;
	private final int totalTriangleCount;
	private final int triangleLabelstartIndex;

	public SingleModelRenderingThread(STLFileContents model,
			SimulationSettings settings, MultiLayerDepthBuffer depthBuffer,
			ProgressWindow progressWindow, CyclicBarrier mainBarrier, int totalTriangleCount, int triangleLabelStartIndex) {
		this.model = model;
		this.settings = settings;
		this.depthBuffer = depthBuffer;
		this.progressWindow = progressWindow;
		this.mainBarrier = mainBarrier;
		this.totalTriangleCount = totalTriangleCount;
		this.triangleLabelstartIndex = triangleLabelStartIndex;
	}
	
	public void run() {
		XRayRenderer.render(model, settings, depthBuffer, progressWindow, totalTriangleCount, triangleLabelstartIndex);
		try {
			mainBarrier.await();
		} catch (InterruptedException e) {
			e.printStackTrace();
		} catch (BrokenBarrierException e) {
			e.printStackTrace();
		}
	}

}
