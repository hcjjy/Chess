#include "netGame.h"
#include "singleGame.h"
#include <QtGui/QApplication>
#include <QtPlugin>
#include <QTextCodec>
#include <QMessageBox>
#include <QDialogButtonBox>

#pragma comment(lib,"qcncodecsd.lib")
Q_IMPORT_PLUGIN(qcncodecs);

int main(int argc, char *argv[])
{
	
	QTextCodec *codec = QTextCodec::codecForName("GBK");//情况1
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);

	QApplication app(argc,argv);
	//Board board;
	//board.show();
	//SingleGame singleGame;
	//singleGame.show();
	int ret;
	ret = QMessageBox::question(NULL,"server or client","作为服务器启动",
		QMessageBox::Yes,QMessageBox::No);
	bool bServer = false;
	if (ret == QMessageBox::Yes)
	{
		bServer = true;
	}
	NetGame netGame(bServer);
	netGame.show();
	return app.exec();
}
