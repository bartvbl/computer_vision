import javax.swing.ImageIcon;


public class UIController {
	private final MainWindow window;
	private static final ImageIcon[] dieIcons = new ImageIcon[] {
		new ImageIcon("res/dice_1.png"),
		new ImageIcon("res/dice_2.png"),
		new ImageIcon("res/dice_3.png"),
		new ImageIcon("res/dice_4.png"),
		new ImageIcon("res/dice_5.png"),
		new ImageIcon("res/dice_6.png")
	};

	public UIController(MainWindow window) {
		this.window = window;
	}
	
	public void setDiceSectionEnabled(boolean enabled) {
		window.lockDie1Button.setEnabled(enabled);
		window.lockDie2Button.setEnabled(enabled);
		window.lockDie3Button.setEnabled(enabled);
		window.lockDie4Button.setEnabled(enabled);
		window.lockDie5Button.setEnabled(enabled);
		window.analyseRollsButton.setEnabled(enabled);
	}
	
	public void setScoreSectionEnabled(boolean enabled) {
		window.selectOnesButton.setEnabled(enabled);
		window.selectTwosButton.setEnabled(enabled);
		window.selectThreesButton.setEnabled(enabled);
		window.selectFoursButton.setEnabled(enabled);
		window.selectFivesButton.setEnabled(enabled);
		window.selectSixesButton.setEnabled(enabled);
		
		window.selectOnePairButton.setEnabled(enabled);
		window.selectTwoPairButton.setEnabled(enabled);
		window.selectThreeOAKButton.setEnabled(enabled);
		window.SelectFourOAKButton.setEnabled(enabled);
		window.selectSmallStraightButton.setEnabled(enabled);
		window.selectLargeStraightButton.setEnabled(enabled);
		window.selectFullHouseButton.setEnabled(enabled);
		window.selectChanceButton.setEnabled(enabled);
		window.selectYahtzeeButton.setEnabled(enabled);
	}
	
	public void enableScoreSection(Player player) {
		if(!player.hasUsed(ScoreType.ONES)) 	window.selectOnesButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.TWOS)) 	window.selectTwosButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.THREES)) 	window.selectThreesButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.FOURS)) 	window.selectFoursButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.FIVES)) 	window.selectFivesButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.SIXES)) 	window.selectSixesButton.setEnabled(true);
		
		if(!player.hasUsed(ScoreType.ONE_PAIR)) window.selectOnePairButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.TWO_PAIR)) window.selectTwoPairButton.setEnabled(true);
		
		if(!player.hasUsed(ScoreType.THREE_OF_A_KIND)) 	window.selectThreeOAKButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.FOUR_OF_A_KIND)) 	window.SelectFourOAKButton.setEnabled(true);
		
		if(!player.hasUsed(ScoreType.SMALL_STRAIGHT)) 	window.selectSmallStraightButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.LARGE_STRAIGHT)) 	window.selectLargeStraightButton.setEnabled(true);
		
		if(!player.hasUsed(ScoreType.FULL_HOUSE))	window.selectFullHouseButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.CHANCE)) 		window.selectChanceButton.setEnabled(true);
		if(!player.hasUsed(ScoreType.YAHTZEE)) 		window.selectYahtzeeButton.setEnabled(true);
	}

	public void setAnalyseDiesButtonEnabled(boolean enabled) {
		window.analyseRollsButton.setEnabled(enabled);
	}

	public void displayDieRolls(int[] rolls) {
		window.lockDie1Button.setIcon(dieIcons[rolls[0]]);
		window.lockDie2Button.setIcon(dieIcons[rolls[1]]);
		window.lockDie3Button.setIcon(dieIcons[rolls[2]]);
		window.lockDie4Button.setIcon(dieIcons[rolls[3]]);
		window.lockDie5Button.setIcon(dieIcons[rolls[4]]);
	}

	public void setLockButtonsEnabled(boolean enabled) {
		window.lockDie1Button.setEnabled(enabled);
		window.lockDie2Button.setEnabled(enabled);
		window.lockDie3Button.setEnabled(enabled);
		window.lockDie4Button.setEnabled(enabled);
		window.lockDie5Button.setEnabled(enabled);
	}

	public void setLockButtonsUnpressed() {
		window.lockDie1Button.setSelected(false);
		window.lockDie2Button.setSelected(false);
		window.lockDie3Button.setSelected(false);
		window.lockDie4Button.setSelected(false);
		window.lockDie5Button.setSelected(false);
	}

	public void showPlayerScores(Player player) {
		window.selectOnesButton.setText(createScoreString(player, ScoreType.ONES));
		window.selectTwosButton.setText(createScoreString(player, ScoreType.TWOS));
		window.selectThreesButton.setText(createScoreString(player, ScoreType.THREES));
		window.selectFoursButton.setText(createScoreString(player, ScoreType.FOURS));
		window.selectFivesButton.setText(createScoreString(player, ScoreType.FIVES));
		window.selectSixesButton.setText(createScoreString(player, ScoreType.SIXES));
		
		window.selectOnePairButton.setText(createScoreString(player, ScoreType.ONE_PAIR));
		window.selectTwoPairButton.setText(createScoreString(player, ScoreType.TWO_PAIR));
		window.selectThreeOAKButton.setText(createScoreString(player, ScoreType.THREE_OF_A_KIND));
		window.SelectFourOAKButton.setText(createScoreString(player, ScoreType.FOUR_OF_A_KIND));
		
		window.selectSmallStraightButton.setText(createScoreString(player, ScoreType.SMALL_STRAIGHT));
		window.selectLargeStraightButton.setText(createScoreString(player, ScoreType.LARGE_STRAIGHT));
		
		window.selectFullHouseButton.setText(createScoreString(player, ScoreType.FULL_HOUSE));
		window.selectChanceButton.setText(createScoreString(player, ScoreType.CHANCE));
		window.selectYahtzeeButton.setText(createScoreString(player, ScoreType.YAHTZEE));
		
		window.bonusLabel.setText(createScoreString(player, ScoreType.BONUS));
		window.partialSumLabel.setText(""+player.partialSum);
		window.totalSumLabel.setText(""+player.totalSum);
	}

	private String createScoreString(Player player, ScoreType type) {
		int score = 0;
		if(player.hasUsed(type)) {
			score = player.getScore(type);
		}
		return ""+score;
	}

	public void registerScoreHandlers(GameController controller) {
		window.selectOnesButton.addActionListener(new ScoreChoiceListener(ScoreType.ONES, controller));
		window.selectTwosButton.addActionListener(new ScoreChoiceListener(ScoreType.TWOS, controller));
		window.selectThreesButton.addActionListener(new ScoreChoiceListener(ScoreType.THREES, controller));
		window.selectFoursButton.addActionListener(new ScoreChoiceListener(ScoreType.FOURS, controller));
		window.selectFivesButton.addActionListener(new ScoreChoiceListener(ScoreType.FIVES, controller));
		window.selectSixesButton.addActionListener(new ScoreChoiceListener(ScoreType.SIXES, controller));
		
		window.selectOnePairButton.addActionListener(new ScoreChoiceListener(ScoreType.ONE_PAIR, controller));
		window.selectTwoPairButton.addActionListener(new ScoreChoiceListener(ScoreType.TWO_PAIR, controller));
		window.selectThreeOAKButton.addActionListener(new ScoreChoiceListener(ScoreType.THREE_OF_A_KIND, controller));
		window.SelectFourOAKButton.addActionListener(new ScoreChoiceListener(ScoreType.FOUR_OF_A_KIND, controller));
		
		window.selectSmallStraightButton.addActionListener(new ScoreChoiceListener(ScoreType.SMALL_STRAIGHT, controller));
		window.selectLargeStraightButton.addActionListener(new ScoreChoiceListener(ScoreType.LARGE_STRAIGHT, controller));
		
		window.selectFullHouseButton.addActionListener(new ScoreChoiceListener(ScoreType.FULL_HOUSE, controller));
		window.selectChanceButton.addActionListener(new ScoreChoiceListener(ScoreType.CHANCE, controller));
		window.selectYahtzeeButton.addActionListener(new ScoreChoiceListener(ScoreType.YAHTZEE, controller));
	}
}
