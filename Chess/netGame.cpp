#include "netGame.h"
#include <QTcpServer>
#include <QTcpSocket>
/*
	1)执红方还是何方，这个信息由服务区发出，客户端接收
	第一个字节固定是1，第二个字节是1，或者0，
	1表示接收方走红旗，0表示接收方走黑旗

	2)点击信息
	第一个字节固定是2，第二个字节是row,第三个字节是col，
	第四个字节是点击的棋子id
*/
NetGame::NetGame(bool server)
{
	_server = NULL;
	_socket = NULL;

	if (server)
	{
		_server = new QTcpServer(this);
		_server->listen(QHostAddress::Any,9999);
		connect(_server,SIGNAL(newConnection()),
				this,SLOT(slotNewConnection()));
	}
	else
	{
		_socket = new QTcpSocket(this);
		_socket->connectToHost(QHostAddress("127.0.0.1"),9999);
		connect(_socket,SIGNAL(readyRead()),
			this,SLOT(slotRecv()));
	}
	connect(this,SIGNAL(backOneStep(int, int, int, int)),
		this,SLOT(slotBackOneStep(int, int, int, int)));
}

NetGame::~NetGame()
{

}

void NetGame::click( int id,int row,int col )
{
	if (id != -1)
	{
		if (_selectid != -1)
		{
			if (_s[_selectid]._red != _bSide)
				return;
		}
		else 
		{
			if (_s[id]._red != _bSide)
				return;
		}
	}
	Board::click(id, row, col);
	/*发送给对方*/
	if (_socket)
	{
		char buf[4];
		buf[0] = 2;//固定为2(标志)
		buf[1] = 9 - row;
		buf[2] = 8 - col;
		buf[3] = id;
		_socket->write(buf, 4);
	}
}

void NetGame::slotBackOneStep(int id,int killid,int row,int col)
{
	char buf[5];
	buf[0] = 3;//固定为3(标志)
	buf[1] = 9 - row;
	buf[2] = 8 - col;
	buf[3] = id;
	buf[4] = killid;
	_socket->write(buf, 5);
}

void NetGame::close()
{
	if (_socket)
	{
		char buff[1];
		buff[0] = -1;
		_socket->write(buff);
	}
}

void NetGame::slotNewConnection()
{
	if (_socket && _socket->isValid())
	{
		return;//只接受一个连接
	}
	_socket = _server->nextPendingConnection();//获得客户端的_socket
	connect(_socket,SIGNAL(readyRead()),
		this,SLOT(slotRecv()));
	/*给对方发送数据*/
	char buf[2];
	buf[0] = 1;
	buf[1] = 0;//接收方(客户端)走黑旗，服务端走红旗。
			   //对于服务端，红旗应该在下方。
			   //对客户端黑旗应该在下方。

	_socket->write(buf,2);
	init(buf[1] == 0);//buf[1] == 0，故参数为true
}

void NetGame::slotRecv()
{
	QByteArray acceptBuf = _socket->readAll();
	char cmd = acceptBuf[0];//标志
	if (cmd == 1)//初始化客户端棋盘
	{
		char data = acceptBuf[1];
		init(data == 1);//data == 0，故参数为false
	}
	else if(cmd == 2)//接受对方发来的信息
	{
		int row = acceptBuf[1];
		int col = acceptBuf[2];
		int id = acceptBuf[3];
		Board::click(id,row,col);
	}
	else if (cmd == 3)//悔棋
	{
		int row = acceptBuf[1];
		int col = acceptBuf[2];
		int id = acceptBuf[3];
		int killid = acceptBuf[4];
		reliveStone(killid);
		moveStone(id,row,col);
		removeOneStep();
	}
	else if(cmd == -1)
	{
		_socket =NULL;
	}
}

void NetGame::closeEvent( QCloseEvent * event)
{
	close();
	Board::closeEvent(event);
}


