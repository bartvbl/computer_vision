import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JToggleButton;


public class LockHandler implements ActionListener {

	private final int die;
	private final DiceAnalysisHandler diceHandler;

	public LockHandler(int die, DiceAnalysisHandler diceHandler) {
		this.die = die;
		this.diceHandler = diceHandler;
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		JToggleButton button = (JToggleButton) arg0.getSource();
		diceHandler.setDieLocked(die, button.isSelected());
	}

}
