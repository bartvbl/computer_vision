import javax.swing.JButton;


public class ScoreCalculator {

	public static void calculateScore(int[] rolls, MainWindow window, Player player) {
		rolls = rolls.clone();
		for(int i = 0; i < 5; i++) {
			rolls[i]++; //avoid offset by one errors
		}
		
		if(!player.hasUsed(ScoreType.ONES)) 	calculateSum(rolls, 1, window.selectOnesButton);
		if(!player.hasUsed(ScoreType.TWOS)) 	calculateSum(rolls, 2, window.selectTwosButton);
		if(!player.hasUsed(ScoreType.THREES))	calculateSum(rolls, 3, window.selectThreesButton);
		if(!player.hasUsed(ScoreType.FOURS)) 	calculateSum(rolls, 4, window.selectFoursButton);
		if(!player.hasUsed(ScoreType.FIVES))	calculateSum(rolls, 5, window.selectFivesButton);
		if(!player.hasUsed(ScoreType.SIXES))	calculateSum(rolls, 6, window.selectSixesButton);
		
		if(!player.hasUsed(ScoreType.ONE_PAIR))	calculatePairScore(rolls, 1, window.selectOnePairButton);
		if(!player.hasUsed(ScoreType.TWO_PAIR))	calculatePairScore(rolls, 2, window.selectTwoPairButton);
		
		if(!player.hasUsed(ScoreType.THREE_OF_A_KIND)) 	calculateOfAKind(rolls, 3, window.selectThreeOAKButton);
		if(!player.hasUsed(ScoreType.FOUR_OF_A_KIND)) 	calculateOfAKind(rolls, 4, window.SelectFourOAKButton);
		
		if(!player.hasUsed(ScoreType.SMALL_STRAIGHT)) 	calculateStraight(rolls, 1, window.selectSmallStraightButton);
		if(!player.hasUsed(ScoreType.LARGE_STRAIGHT)) 	calculateStraight(rolls, 2, window.selectLargeStraightButton);
		
		if(!player.hasUsed(ScoreType.FULL_HOUSE)) 	calculateFullHouse(rolls, window.selectFullHouseButton);
		if(!player.hasUsed(ScoreType.CHANCE))	 	calculateChance(rolls, window.selectChanceButton);
		if(!player.hasUsed(ScoreType.YAHTZEE)) 		calculateYahtzee(rolls, window.selectYahtzeeButton);
		
	}



	private static void calculateYahtzee(int[] rolls, JButton button) {
		if( (rolls[0] == rolls[1]) &&
			(rolls[1] == rolls[2]) &&
			(rolls[2] == rolls[3]) &&
			(rolls[3] == rolls[4])) {
			button.setText("50");
		} else {
			button.setText("0");
		}
	}


	private static void calculateSum(int[] rolls, int number, JButton button) {
		int sum = 0;
		for(int i = 0; i < 5; i++) {
			if(rolls[i] == number) {
				sum += number;
			}
		}
		button.setText(""+sum);
	}
	
	
	private static void calculatePairScore(int[] rolls, int numPairs, JButton button) {
		int pairCount = 0;
		int pairScore = 0;
		for(int i = 6; i > 0; i--) {
			if(count(rolls, i) >= 2) {
				pairCount++;
				pairScore += i * 2;
				if(pairCount == numPairs) {
					break;
				}
			}		
		}
		if(pairCount < numPairs) {
			pairScore = 0;
		}
		button.setText("" + pairScore);
	}
	
	private static void calculateOfAKind(int[] rolls, int minimumCount, JButton button) {
		for(int i = 6; i > 0; i--) {
			if(count(rolls, i) >= minimumCount) {
				button.setText(""+(minimumCount * i));
				return;
			}
		}
		button.setText("0");
	}
	
	private static void calculateStraight(int[] rolls, int start, JButton button) {
		int sum = 0;
		for(int i = 0; i < 5; i++) {
			if(!contains(rolls, start + i)) {
				button.setText("0");
				return;
			}
			sum += start + i;
		}
		button.setText(""+sum);
	}
	
	private static void calculateFullHouse(int[] rolls, JButton button) {
		boolean containsPair = false;
		boolean containsTriple = false;
		for(int i = 1; i <= 6; i++) {
			if(count(rolls, i) == 2) {
				containsPair = true;
			}
			if(count(rolls, i) == 3) {
				containsTriple = true;
			}
		}
		if(containsPair && containsTriple) {
			button.setText("" + sum(rolls));
		} else {
			button.setText("0");
		}
	}
	
	private static void calculateChance(int[] rolls, JButton selectChanceButton) {
		int score = 0;
		for(int i = 0; i < 5; i++) {
			score += rolls[i];
		}
		selectChanceButton.setText(""+score);
	}
	
	private static int sum(int[] rolls) {
		int pointSum = 0;
		for(int i = 0; i < 5; i++) {
			pointSum += rolls[i];
		}
		return pointSum;
	}

	private static boolean contains(int[] rolls, int number) {
		for(int i = 0; i < 5; i++) {
			if(rolls[i] == number) {
				return true;
			}
		}
		return false;
	}
	
	private static int count(int[] rolls, int number) {
		int count = 0;
		for(int i = 0; i < 5; i++) {
			if(rolls[i] == number) {
				count++;
			}
		}
		return count;
	}

}
