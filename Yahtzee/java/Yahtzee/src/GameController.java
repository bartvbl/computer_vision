import javax.swing.JOptionPane;


public class GameController {

	private final UIController controller;
	private final MainWindow window;
	
	private Player[] players;
	private int playerCount;
	private int currentPlayer = -1;
	private int remainingRollsInTurn = 0;
	private DiceAnalysisHandler diceHandler;
	private int[] currentRoll;
	
	private static final int TURNS_IN_YAHTZEE_GAME = ScoreType.values().length - 1; //being too lazy to count

	public GameController(MainWindow window) {
		this.window = window;
		this.controller = new UIController(window);
	}

	public void startNewGame() {
		initialise();
		
		handleTurnStart();
	}

	private void handleTurnStart() {
		remainingRollsInTurn = 3;
		currentPlayer++;
		currentPlayer = currentPlayer % playerCount;
		
		JOptionPane.showMessageDialog(window, "It's player " + currentPlayer + "'s turn.");
		controller.setAnalyseDiesButtonEnabled(true);
		controller.setLockButtonsUnpressed();
		diceHandler.clearLocks();
		controller.showPlayerScores(players[currentPlayer]);
	}
	
	public void handleDieRoll(int[] rolls) {
		remainingRollsInTurn--;
		controller.displayDieRolls(rolls);
		this.currentRoll = rolls;
		if(remainingRollsInTurn > 0) {
			controller.setLockButtonsEnabled(true);			
		} else {
			controller.setDiceSectionEnabled(false);
			ScoreCalculator.calculateScore(rolls, window, players[currentPlayer]);
			controller.enableScoreSection(players[currentPlayer]);
		}
	}

	private void initialise() {
		controller.setScoreSectionEnabled(false);
		controller.setDiceSectionEnabled(false);
		
		playerCount = requestPlayerCount();
		players = new Player[playerCount];
		for(int i = 0; i < playerCount; i++) {
			players[i] = new Player();
		}
		
		this.diceHandler = new DiceAnalysisHandler(this);
		window.analyseRollsButton.addActionListener(diceHandler);
		window.lockDie1Button.addActionListener(new LockHandler(0, diceHandler));
		window.lockDie2Button.addActionListener(new LockHandler(1, diceHandler));
		window.lockDie3Button.addActionListener(new LockHandler(2, diceHandler));
		window.lockDie4Button.addActionListener(new LockHandler(3, diceHandler));
		window.lockDie5Button.addActionListener(new LockHandler(4, diceHandler));
		controller.registerScoreHandlers(this);
	}

	private int requestPlayerCount() {
		boolean receivedAcceptableAnswer = false;
		int answer = -1;
		
		while(!receivedAcceptableAnswer) {
			String input = JOptionPane.showInputDialog(window, "How many players?");
			try {
				answer = Integer.parseInt(input);
				receivedAcceptableAnswer = true;
			} catch (NumberFormatException e) {}
		}
		
		return answer;
	}

	public void handleScoreChoice(int score, ScoreType type) {
		players[currentPlayer].setScore(type, score);
		controller.setScoreSectionEnabled(false);
		if(players[currentPlayer].isFinished() && currentPlayer == playerCount - 1) {
			String gameoverMessage = "Game finished! Final scores:\n";
			for(int i = 0; i < playerCount; i++) {
				gameoverMessage += "Player " + (i + 1) + ": " + players[i].totalSum;
			}
			JOptionPane.showMessageDialog(window, gameoverMessage);
			System.exit(0);
		} else {
			handleTurnStart();
		}
	}

}
