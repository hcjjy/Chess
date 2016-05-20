#ifndef NETGAME_H_
#define NETGAME_H_

#include "board.h"

class QTcpServer;
class QTcpSocket;
class NetGame: public Board
{
	Q_OBJECT
public:
	NetGame(bool server);
	~NetGame();
	QTcpServer* _server;
	QTcpSocket* _socket;
	void click(int id,int row,int col);
	void close();
public slots:
	void slotNewConnection();
	void slotRecv();
	void slotBackOneStep(int id,int killid,int row,int col);
protected:
	void closeEvent(QCloseEvent *);
};


#endif	//NetGame.h