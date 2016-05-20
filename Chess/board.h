#ifndef BOARD_H_
#define BOARD_H_

#include <QWidget>
#include "stone.h"

class QPushButton;
class Step;
class Board : public QWidget
{
	Q_OBJECT
public:
	explicit Board(QWidget *parent = 0);
	void init(bool bSide);
	void drawStone(QPainter &painter,int id);
	QPoint center(int row, int col);
	QPoint center(int id);
	bool getRowCol(QPoint , int& , int& );
	bool canMove(int moveid, int killid, int row, int col);
	bool canMoveChe(int moveid, int killid, int row, int col);
	bool canMoveMa(int moveid, int killid, int row, int col);
	bool canMoveXiang(int moveid, int killid, int row, int col);
	bool canMoveShi(int moveid, int killid, int row, int col);
	bool canMoveJiang(int moveid, int killid, int row, int col);
	bool canMovePao(int moveid, int killid, int row, int col);
	bool canMoveBing(int moveid, int killid, int row, int col);
	int relation( int id,int row,int col );
	void getRowCol(int id,int& row,int& col);
	int findId(int row ,int col);
	bool isBottomSide(int id);
	int getStoneCountAtLine(int id, int row, int col);
	void click(QPoint pt);
	virtual void click(int id,int row,int col);
	void trySelectStone(int id);
	void tryMoveStone(int killid, int row, int col);
	bool sameColor(int id1, int id2);
	void moveStone(int moveid, int killid, int row, int col);
	void moveStone(int moveid, int row, int col);
	void killStone(int id);
	void reliveStone(int id);
	void saveStep(int moveid, int killid, int row,int col,QVector<Step* > &steps);
	void removeOneStep();
protected:
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void mouseReleaseEvent(QMouseEvent *);

signals:
	void backOneStep(int moveid, int killid, int row, int col);
public slots:
	void slotBack();
private:
	int _r;//棋的半径
protected:
	int _selectid;//是否被选中
	bool _bSide;//棋在哪一方
	Stone _s[32];
	bool _bRedTurn;//轮到哪一方
	QVector<Step*> _steps; //悔棋时候用
	QPushButton* _backButton;
};

#endif
