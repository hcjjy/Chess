#ifndef SIGNALEGAME_H_
#define SIGNALEGAME_H_

#include "board.h"

class Step;
class SingleGame : public Board
{
	Q_OBJECT
public:
	SingleGame();
	virtual void click(int id,int row,int col);
	
	Step* getBestMove();
	void getAllPossibleMove(QVector<Step *> &steps);
	void fakeMove(Step* step);
	void unfakeMove(Step* step);
	int calculateScore();
	int getMinScore(int level, int curMaxScore);
	int getMaxScore(int level, int curMinScore);
public slots:
		void computerMove();
private:
	int _level;
};

#endif //SIGNALEGAME_H_