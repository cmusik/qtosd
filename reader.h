#include <QObject>
#include <QFile>
#include <QSocketNotifier>
#include <QTcpServer>
#include <QTcpSocket>

#ifndef  READER_INC
#define  READER_INC

class Reader : public QObject {
	Q_OBJECT

	public:
		Reader();
	
	public slots:
		void read(int);
		void handleConnection();
		void readSocket();

	signals:
		void showText(QString);

	private:
		QTcpServer *srv;
		QTcpSocket *connection;
		QFile *m_file;
		QSocketNotifier *m_notify;

};

#endif   /* ----- #ifndef READER_INC  ----- */

