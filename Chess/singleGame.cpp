#include "step.h"
#include "singleGame.h"
#include <QDebug>
#include <QTimer>
//enum TYPE{CHE,MA,XIANG,SHI,JIANG,PAO,BING};
static int chessScore[] = {100, 50, 10, 6, 1500, 51, 20};

SingleGame::SingleGame()
{
	_level = 4;
}

void SingleGame::click( int id,int row,int col )
{
	if(!_bRedTurn)//红旗先走
		return;
	Board::click(id, row, col);//若是move,将会改变bRedTurn的值,即bRedTurn由true变为false;
	if(!_bRedTurn)
	{
		QTimer::singleShot(100,this,SLOT(computerMove()));
		//computerMove();
	}
}

void SingleGame::computerMove()
{
	Step* step = getBestMove();//电脑走完后，将会改变bRedTurn的值,即bRedTurn由false变为true;
	moveStone(step->_moveid,step->_killid,step->_rowTo,step->_colTo);
	delete step; 
	step = NULL;
	update();
}

void SingleGame::getAllPossibleMove( QVector<Step *> &steps )
{
	int min = 16, max = 32;
	if (this->_bRedTurn)
	{
		min = 0, max = 16;
	}
	for (int i= min; i<max ;++i)
	{
		if (_s[i]._dead)
		{
			continue;
		}
		for(int row = 0;row <= 9; ++row)
		{
			for(int col = 0; col <= 8; ++col)
			{
				int killid = this->findId(row, col);
				if (sameColor(i,killid))
				{
					continue;
				}
				if (canMove(i, killid, row, col))
				{
					saveStep(i,killid,row,col,steps);
				}
			}

		}
	}
}

void SingleGame::fakeMove( Step* step )
{
	killStone(step->_killid);
	moveStone(step->_moveid,step->_rowTo,step->_colTo);
}

void SingleGame::unfakeMove( Step* step )
{
	reliveStone(step->_killid);
	moveStone(step->_moveid,step->_rowFrom,step->_colFrom);	
}

/*评估局面分*/
int SingleGame::calculateScore()
{
	int redTotalScore = 0;
	int blackTotalScore = 0;
	//黑旗分的总数 - 红旗分的总数
	for (int i = 0; i < 16; ++i)
	{
		if(_s[i]._dead) continue;
		redTotalScore += chessScore[_s[i]._type];
	}
	for (int i = 16; i < 32; ++i)
	{
		if(_s[i]._dead) continue;
		blackTotalScore += chessScore[_s[i]._type];
	}
	return blackTotalScore - redTotalScore;
}

int SingleGame::getMinScore(int level, int curMaxScore)
{
	if (level == 0)
	{
		return calculateScore(); 
	}
	//1.看看那些步可以走
	QVector<Step* >steps;
	getAllPossibleMove(steps);//是红旗(人)的possibleMove
	//2.试着走一下
	//3.评估走的结果
	int minScore = INT_MAX;
	QVector<Step* >::iterator iter = steps.begin();
	for (;iter != steps.end();)
	{
		Step* step = *iter;
		iter = steps.erase(iter);
		fakeMove(step);
		int score = getMaxScore(level - 1,minScore);
		unfakeMove(step);
		delete step;
		step = NULL;
		if (score <= curMaxScore)//减枝
		{
			while(steps.count())
			{
				step = steps.back();
				steps.pop_back();
				delete step;
			}
			return score; 
		}
		if (score < minScore)
		{
			minScore = score;
		}
	}
	return minScore;
}

int SingleGame::getMaxScore(int level, int curMinScore)
{
	if (level == 0)
	{
		return calculateScore();
	}
	//1.看看那些步可以走
	QVector<Step* >steps;
	getAllPossibleMove(steps);//是黑旗(电脑)的possibleMove
	//2.试着走一下
	//3.评估走的结果
	int maxScore = INT_MIN;
	QVector<Step* >::iterator iter = steps.begin();
	for (;iter != steps.end();)
	{
		Step* step = *iter;
		iter = steps.erase(iter);
		fakeMove(step);
		int score = getMinScore(level - 1,maxScore);
		unfakeMove(step);
		delete step;
		step = NULL;
		if (score >= curMinScore)//减枝
		{
			while(steps.count())
			{
				step = steps.back();
				steps.pop_back();
				delete step;
			}
			return score;
		}
		if (score > maxScore)
		{
			maxScore = score;
		}
	}
	return maxScore;
}

Step* SingleGame::getBestMove()
{
	//1.看看有哪些步骤可以走
	QVector<Step* > steps;
	getAllPossibleMove(steps);//是黑旗(电脑)的PossibleMove
	//2.试着走一下
	//3.评估走的结果
	int cnt = 0;//_test
	int maxScore = INT_MIN;
	Step* ret =nullptr;
	QVector<Step* >::iterator iter = steps.begin();
	for (;iter != steps.end();)
	{
		Step* step = *iter;
		fakeMove(step);
		int score = getMinScore(_level, maxScore);
		unfakeMove(step);
		if (score > maxScore)
		{
			maxScore = score;
			if(ret)
			{
				delete ret;
			}
			ret = step;
			++iter;
		}
		else
		{
			iter = steps.erase(iter);
			delete step;
			step = NULL;
		}
		cnt++;
	}
	qDebug()<<cnt<<endl;
	//4.选最好的走
	return ret;
}
