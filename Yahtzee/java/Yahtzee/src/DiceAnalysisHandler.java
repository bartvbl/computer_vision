import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.atomic.AtomicReference;

import javax.swing.JOptionPane;


public class DiceAnalysisHandler implements ActionListener {

	private static final int DIES_IN_YAHTZEE_GAME = 5;
	private GameController controller;
	private Random random;
	private boolean[] locks = new boolean[5];
	private int[] rolls = new int[5];
	private Process recognitionProcess;
	private PrintWriter processOut;
	private BufferedReader processIn;
	
	private static boolean enableCam = true;
	private static boolean camUpdatesRunning = true;
	private static AtomicReference<String> currentLine;

	public DiceAnalysisHandler(GameController gameController) {
		this.controller = gameController;
		this.random = new Random(System.nanoTime());
		if(enableCam) {			
			try {
				this.recognitionProcess = Runtime.getRuntime().exec("Yahtzee.exe");
				this.processOut = new PrintWriter(recognitionProcess.getOutputStream());
				this.processIn = new BufferedReader(new InputStreamReader(recognitionProcess.getInputStream()));
				this.currentLine = new AtomicReference<String>();
				
				new Thread(
					new Runnable() {
							
					public void run() {
						while(camUpdatesRunning) {
							processOut.write("R\n");
							processOut.flush();
							try {
								currentLine.set(processIn.readLine());
							} catch (IOException e) {
								e.printStackTrace();
							}
						}
					}
				}).start();
				
				Thread closeChildThread = new Thread() {
				    public void run() {
				        recognitionProcess.destroy();
				    }
				};

				Runtime.getRuntime().addShutdownHook(closeChildThread); 
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		if(enableCam) {
			String line = currentLine.get();
			while(JOptionPane.showConfirmDialog(null, "The interpreted roll was [" + line + "].\nIs this correct?") != JOptionPane.YES_OPTION) {
				line = currentLine.get();
			}
			String[] parts = line.split(", ");
			int lastIndex = 0;
			for(int i = 0; i < DIES_IN_YAHTZEE_GAME; i++) {	
				if(!locks[i]) {
					rolls[i] = Integer.parseInt(parts[lastIndex]) - 1;
					lastIndex++;
				}
			}
		} else {			
			for(int i = 0; i < DIES_IN_YAHTZEE_GAME; i++) {	
				if(!locks[i]) {
					rolls[i] = random.nextInt(6);				
				}
			}
		}
		controller.handleDieRoll(rolls);
	}

	public void setDieLocked(int die, boolean locked) {
		locks[die] = locked;
	}

	public void clearLocks() {
		Arrays.fill(locks, false);
	}
	
	public void finalize() {
		System.out.println("Killing process..");
		recognitionProcess.destroy();
	}

}
