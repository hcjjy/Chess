#include "netGame.h"
#include <QTcpServer>
#include <QTcpSocket>
/*
	1)ִ�췽���Ǻη��������Ϣ�ɷ������������ͻ��˽���
	��һ���ֽڹ̶���1���ڶ����ֽ���1������0��
	1��ʾ���շ��ߺ��죬0��ʾ���շ��ߺ���

	2)�����Ϣ
	��һ���ֽڹ̶���2���ڶ����ֽ���row,�������ֽ���col��
	���ĸ��ֽ��ǵ��������id
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
	/*���͸��Է�*/
	if (_socket)
	{
		char buf[4];
		buf[0] = 2;//�̶�Ϊ2(��־)
		buf[1] = 9 - row;
		buf[2] = 8 - col;
		buf[3] = id;
		_socket->write(buf, 4);
	}
}

void NetGame::slotBackOneStep(int id,int killid,int row,int col)
{
	char buf[5];
	buf[0] = 3;//�̶�Ϊ3(��־)
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
		return;//ֻ����һ������
	}
	_socket = _server->nextPendingConnection();//��ÿͻ��˵�_socket
	connect(_socket,SIGNAL(readyRead()),
		this,SLOT(slotRecv()));
	/*���Է���������*/
	char buf[2];
	buf[0] = 1;
	buf[1] = 0;//���շ�(�ͻ���)�ߺ��죬������ߺ��졣
			   //���ڷ���ˣ�����Ӧ�����·���
			   //�Կͻ��˺���Ӧ�����·���

	_socket->write(buf,2);
	init(buf[1] == 0);//buf[1] == 0���ʲ���Ϊtrue
}

void NetGame::slotRecv()
{
	QByteArray acceptBuf = _socket->readAll();
	char cmd = acceptBuf[0];//��־
	if (cmd == 1)//��ʼ���ͻ�������
	{
		char data = acceptBuf[1];
		init(data == 1);//data == 0���ʲ���Ϊfalse
	}
	else if(cmd == 2)//���ܶԷ���������Ϣ
	{
		int row = acceptBuf[1];
		int col = acceptBuf[2];
		int id = acceptBuf[3];
		Board::click(id,row,col);
	}
	else if (cmd == 3)//����
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


