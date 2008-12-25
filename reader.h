#include <QObject>
#include <QFile>
#include <QSocketNotifier>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

#ifndef  READER_INC
#define  READER_INC

class Reader : public QThread {
	Q_OBJECT

	public:
		Reader(int);
		~Reader();

	public slots:
		void readSocket();
		void closeConnection();

	signals:
		void receivedText(QString);

	private:
		QTcpServer *srv;
		QTcpSocket *connection;

};

#endif

