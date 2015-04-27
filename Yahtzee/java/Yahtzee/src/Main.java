import javax.swing.JFrame;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;


public class Main {
	public static void main(String[] args) {
		setSwingSettings();
		
		MainWindow window = createWindow();
		GameController controller = new GameController(window);
		controller.startNewGame();
	}

	private static MainWindow createWindow() {
		MainWindow window = new MainWindow();
		window.setTitle("Yahtzee");
		window.setLocation(100, 100);
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		window.setResizable(false);
		window.setVisible(true);
		return window;
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
