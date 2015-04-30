import java.util.HashMap;


public class Player {
	private HashMap<ScoreType, Integer> scoreMap = new HashMap<ScoreType, Integer>();
	public int partialSum = 0;
	public int totalSum = 0;
	
	public void setScore(ScoreType type, int score) {
		if(hasUsed(type)) {
			return;
		}
		scoreMap.put(type, score);
		totalSum += score;
		switch(type) {
		case ONES:
		case TWOS:
		case THREES:
		case FOURS:
		case FIVES:
		case SIXES:
			partialSum += score;
			if(
					hasUsed(ScoreType.ONES) && 
					hasUsed(ScoreType.TWOS) && 
					hasUsed(ScoreType.THREES) && 
					hasUsed(ScoreType.FOURS) && 
					hasUsed(ScoreType.FIVES) && 
					hasUsed(ScoreType.SIXES)) {
				partialSum = getScore(ScoreType.ONES) +
						getScore(ScoreType.TWOS) + 
						getScore(ScoreType.THREES) + 
						getScore(ScoreType.FOURS) + 
						getScore(ScoreType.FIVES) + 
						getScore(ScoreType.SIXES);
				if(partialSum >= 63) {
					scoreMap.put(ScoreType.BONUS, 50);
				} else {
					scoreMap.put(ScoreType.BONUS, 0);
				}
			}
		default: break;
		}
	}
	
	public boolean hasUsed(ScoreType type) {
		return scoreMap.get(type) != null;
	}
	
	public boolean isFinished() {
		boolean hasUsedAllValues = true;
		for(ScoreType type : ScoreType.values()) {
			hasUsedAllValues = hasUsedAllValues && hasUsed(type);
		}
		return hasUsedAllValues;
	}
	
	public int getScore(ScoreType type) {
		return scoreMap.get(type);
	}
}
