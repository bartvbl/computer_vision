import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;


public class ScoreChoiceListener implements ActionListener {

	private final ScoreType type;
	private final GameController controller;

	public ScoreChoiceListener(ScoreType type, GameController controller) {
		this.type = type;
		this.controller = controller;
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		JButton button = (JButton) arg0.getSource();
		int score = Integer.parseInt(button.getText());
		controller.handleScoreChoice(score, type);
	}

}
