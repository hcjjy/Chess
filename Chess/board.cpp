#include "board.h"
#include "step.h"
#include<QPainter>
#include <QPaintEvent>
#include <QPushButton>

Board::Board(QWidget *parent) : QWidget(parent)
{
	_r = 20;
	resize(600,600);
	init(true);
	_backButton = new QPushButton("悔棋",this);
	connect(_backButton, SIGNAL(clicked()),this,SLOT(slotBack()));
}

void Board::init(bool bSide)
{
	while(_steps.count())
	{
		Step* step = _steps.back();
		delete step;
		_steps.pop_back();
	}
	for(int i = 0; i < 32; ++i)
	{
		_s[i].init(i);
	}
	if (bSide)//若为true则红旗在下方
	{
		for (int i = 0; i<32; i++)
		{
			_s[i].rotate();
		}
	}
	_selectid = -1;
	_bRedTurn = true;
	_bSide = bSide;
	update();
}

void Board::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int d = 40;
    _r = d/2;
    //画10条横线
    for(int i = 1;i <= 10; ++i)
    {
        painter.drawLine(QPoint(d,i*d),QPoint(9*d,i*d));
    }
    //画9条竖线
    for(int i = 1;i <=9; ++i)
    {
        if(i == 1 || i == 9)
        {
            painter.drawLine(QPoint(i*d,d),QPoint(i*d,10*d));
        }
        else
        {
            painter.drawLine(QPoint(i*d,d),QPoint(i*d,5*d));
            painter.drawLine(QPoint(i*d,6*d),QPoint(i*d,10*d));
        }
    }
    //九宫格
    painter.drawLine(QPoint(4*d,1*d),QPoint(6*d,3*d));
    painter.drawLine(QPoint(6*d,1*d),QPoint(4*d,3*d));

    painter.drawLine(QPoint(4*d,8*d),QPoint(6*d,10*d));
    painter.drawLine(QPoint(6*d,8*d),QPoint(4*d,10*d));

    //绘制32个棋子
    for(int i = 0; i < 32; ++i)
    {
		if (!_s[i]._dead)
			drawStone(painter,i);
    }

	_backButton->setFont(QFont("Helvetica [Cronyx]", 14));
}

QPoint Board::center(int row, int col)
{  
    QPoint ret;
    ret.rx() = (col+1) * _r * 2;
    ret.ry() = (row+1) * _r * 2;
    return ret;
}

QPoint Board::center(int id)
{
    return center(_s[id]._row,_s[id]._col);
}

void Board::drawStone(QPainter &painter, int id)
{
    QPoint c = center(id);
    QRect rect = QRect(c.x()-_r,c.y()-_r,_r*2,_r*2);

	if (id == _selectid)
		painter.setBrush(QBrush(Qt::gray));
	else
		painter.setBrush(QBrush(Qt::yellow));

	painter.setPen(Qt::black);

	painter.drawEllipse(c,_r,_r);
	if(_s[id]._red)
		painter.setPen(Qt::red);
	painter.setFont(QFont("system",_r,75));
    painter.drawText(rect,_s[id].getText(),QTextOption(Qt::AlignCenter));
}

void Board::resizeEvent(QResizeEvent *)
{
	_backButton->setGeometry(_r*19, _r*11,_backButton->width(),_backButton->height());
}

void Board::mouseReleaseEvent( QMouseEvent * event)
{
	if (event->button() != Qt::LeftButton)
	{
		return ;
	}
	click(event->pos());
}

bool Board::getRowCol( QPoint pt, int&row , int&col )
{
	int colBgn = pt.x()/(2*_r) - 1;
	int rowBgn = pt.y()/(2*_r) - 1;
	for (int i = 0; i < 2; ++i)
	{
		row = rowBgn + i;
		for(int j = 0; j <2 ;++j)
		{
			col = colBgn + j;
			if (row >= 0 && row<= 9 && col >= 0 && col <= 8)
			{
				QPoint c = center(row ,col);
				int dx = c.x() - pt.x();
				int dy = c.y() - pt.y();
				int dist = dx * dx + dy * dy;
				if (dist < _r* _r)
					return true;
			}
		}
	}
	return false;
}

void Board::getRowCol( int id,int& row,int& col )
{
	row = _s[id]._row;
	col = _s[id]._col;
}

bool Board::canMove(int moveid, int killid, int row, int col)
{
	if(sameColor(killid, _selectid))//moveid和killid颜色相同
	{
		return false;
	}

	switch(_s[moveid]._type)
	{
	case Stone::CHE:
		return canMoveChe(moveid,killid,row,col);
	case Stone::MA:
		return canMoveMa(moveid,killid,row,col);
	case Stone::XIANG:
		return canMoveXiang(moveid,killid,row,col);
	case Stone::SHI:
		return canMoveShi(moveid,killid,row,col);
	case Stone::JIANG:
		return canMoveJiang(moveid,killid,row,col);
	case Stone::PAO:
		return canMovePao(moveid,killid,row,col);
	case Stone::BING:
		return canMoveBing(moveid,killid,row,col);
	}
	return false;
}

bool Board::canMoveChe(int moveid, int killid, int row, int col)
{
	int ret = getStoneCountAtLine(moveid, row, col);
	if (ret == 0)
		return true;
	return false;
}
bool Board::canMoveMa(int moveid, int killid, int row, int col)
{
	int d = relation(moveid, row, col);
	if (d != 12 && d != 21)
	{
		return false;
	}
	if (d == 12)
	{
		if (findId(_s[moveid]._row,(_s[moveid]._col + col)/2) != -1)
		{
			return false;
		}
	}
	else
	{
		if (findId( (_s[moveid]._row + row)/2, _s[moveid]._col) != -1)
		{
			return false;
		}
	}
	return true;
}
bool Board::canMoveXiang(int moveid, int killid, int row, int col)
{
	int d = relation(moveid, row, col);
	if (d != 22)
		return false;
	int rEye = (_s[moveid]._row + row)/2;
	int cEye = (_s[moveid]._col + col)/2;
	if (findId(rEye, cEye) != -1)
		return false;
	if (isBottomSide(moveid))
	{
		if (row < 4)
		{
			return false;
		}
	}
	else
	{
		if (row > 5)
		{
			return false;
		}
	}
	return true;
}
bool Board::canMoveShi(int moveid, int killid, int row, int col)
{

	int d = relation(moveid,row,col);
	if(d != 11)
		return false;

	if (isBottomSide(moveid))
	{	if (row < 7)
			return false;
	}
	else
	{
		if(row > 2)
			return false;
	}
	if (col < 3 || col > 5)
		return false;

	return true;
}
bool Board::canMoveJiang(int moveid, int killid, int row, int col)
{
	/*
	1.首先目标位置在九宫格内
	2.移动的步长是一个格子
	*/
	//特殊情况，当“将”和“将”面对面的时候可以吃掉对方。
	//走棋规则其实和车是一样的，所以才调用了canMoveChe。
	if (killid != -1 && _s[killid]._type == Stone::JIANG)
	{
		return canMoveChe(moveid, killid, row, col);
	}
	int d = relation(moveid,row,col);
	if(d != 1 && d != 10)
		return false;

	if (isBottomSide(moveid))
	{	
		if (row < 2)
			return false;
	}
	else
	{
		if(row > 2)
			return false;
	}
	if (col < 3 || col > 5)
		return false;
	return true;
}
bool Board::canMovePao(int moveid, int killid, int row, int col)
{
	int ret = getStoneCountAtLine(moveid, row, col);
	if (killid != -1)
	{
		if (ret == 1)
		{
			return true;
		}
	}
	else
	{
		if (ret == 0)
		{
			return true;
		}
	}
	return false;
}
bool Board::canMoveBing(int moveid, int killid, int row, int col)
{
	int d = relation(moveid, row, col);
	if (d != 1 && d != 10)
	{
		return false;
	}
	if (isBottomSide(moveid))
	{
		if (row > _s[moveid]._row)//后退
		{
			return false;
		}
		if (_s[moveid]._row == row && row >=5)//本地盘平行走一步
		{
			return false;
		}
	}
	else
	{
		if (row < _s[moveid]._row)
		{
			return false;
		}
		if (_s[moveid]._row == row && _s[moveid]._row <=4)
		{
			return false;
		}
	}
	return true;
}

int Board::relation( int id,int row,int col )
{
	int dr = _s[id]._row - row; 
	int dc = _s[id]._col - col;
	return abs(dr)*10 + abs(dc);
}

int Board::findId( int row ,int col)
{
	for (int i = 0;i < 32; ++i)
	{
		if(_s[i]._row == row && _s[i]._col == col && _s[i]._dead == false)
		{
			return i;
		}
	}
	return -1;
}

bool Board::isBottomSide( int id )//红旗默认在下方
{
	return _s[id]._red == _bSide;
}

int Board::getStoneCountAtLine( int id, int row, int col )
{
	int rowSrc = _s[id]._row;
	int colSrc = _s[id]._col;
	if (rowSrc != row && colSrc != col)
		return -1;
	if (rowSrc == row && colSrc == col)
		return -1;
	int ret =0,min,max;
	if(rowSrc == row)
	{
		 min = colSrc < col ? colSrc : col;
		 max = colSrc < col ? col : colSrc;
		for(int i = min+1 ;i < max; ++i)
		{
			if(findId(row, i) != -1)
			{
				++ret;
			}
		}
	}
	else
	{
		 min = rowSrc < row ? rowSrc : row;
		 max = rowSrc < row ? row : rowSrc;
		for(int i = min+1 ;i < max; ++i)
		{
			if(findId(i, col) != -1)
			{
				++ret;
			}
		}
	}
	return ret;
}

void Board::click( QPoint pt )
{
	//将pt转化成想起的行列值
	int row,col;
	bool bRet = getRowCol(pt,row,col);
	if(bRet == false)//点到棋盘外
		return;
	//点到了某个棋子上
	int id = findId(row, col);
	click(id, row, col);
}

void Board::click( int id,int row,int col )
{
	if (_selectid == -1)
	{
		trySelectStone(id);
	}
	else
	{
		tryMoveStone(id, row, col);
	}
}

void Board::trySelectStone( int id )
{
	if(id == -1)
		return;
	if (_bRedTurn == _s[id]._red)//轮到该方走时，点击才有效
	{
		_selectid = id;
		update();
	}	
}

void Board::tryMoveStone( int killid, int row, int col )
{
	if (sameColor(killid, _selectid))//第二次选中的棋子和原来棋子不同才move，否则视为select
	{
		trySelectStone(killid);
		return ;
	}
	if (canMove(_selectid,killid,row,col))
	{
		moveStone(_selectid, killid, row, col);
	}
}

bool Board::sameColor( int id1, int id2 )
{
	if (id1 == -1 || id2 == -1)
	{
		return false;
	}
	return _s[id1]._red == _s[id2]._red;
}

void Board::moveStone( int moveid, int killid, int row, int col )
{
	saveStep(moveid, killid, row, col, _steps);//悔棋时候用的
	killStone(killid);
	moveStone(moveid, row, col);
}

void Board::moveStone( int moveid, int row, int col )
{
	_s[moveid]._row = row;
	_s[moveid]._col = col;
	_selectid = -1;
	_bRedTurn = !_bRedTurn;
	update();
}

void Board::killStone( int id )
{
	if (id == -1)
	{
		return;
	}
	_s[id]._dead = true;
}

void Board::reliveStone(int id)
{
	if (id == -1)
	{
		return;
	}
	_s[id]._dead = false;
}
void Board::saveStep( int moveid, int killid, int row,int col,QVector<Step* > &steps )
{
	Step* step = new Step;
	step->_rowFrom = _s[moveid]._row;
	step->_colFrom = _s[moveid]._col;
	step->_rowTo = row;
	step->_colTo = col;
	step->_moveid = moveid;
	step->_killid = killid;
	steps.append(step);
}

void Board::slotBack()
{
	if (_steps.count())
	{
		Step* step = _steps.back();
		reliveStone(step->_killid);
		moveStone(step->_moveid,step->_rowFrom,step->_colFrom);
		emit backOneStep(step->_moveid,step->_killid,step->_rowFrom,step->_colFrom);
		removeOneStep();
		step = NULL;
		update();
	}

}

void Board::removeOneStep()
{
	if (_steps.count())
	{
		Step* step = _steps.back();
		delete step;
		_steps.pop_back();
		step = NULL;
	}
}
