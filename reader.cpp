#include <QDebug>
#include <QStringList>
#include "reader.h"
#include <sys/stat.h>
#include <fcntl.h>


Reader::Reader() : QObject() {
	srv = new QTcpServer();
	srv->listen(QHostAddress::LocalHost, 5000);
	connect(srv, SIGNAL(newConnection()), this, SLOT(handleConnection()));

	//m_file = new QFile("/tmp/fifo");
	//m_file->open(QIODevice::ReadWrite|QIODevice::Text);
	//m_file->open(QIODevice::ReadOnly);


	/*
	int fd = ::open ("/tmp/fifo", O_RDWR|O_NDELAY);
	if (fd == -1) {
		qDebug() << "bummer fifo won't open";
		exit (1);
	}
	*/

	//m_notify = new QSocketNotifier (fd, QSocketNotifier::Read);


	//m_notify = new QSocketNotifier(m_file->handle(), QSocketNotifier::Read, this);
	//connect(m_notify, SIGNAL(activated(int)), this, SLOT(read(int)));
}

void Reader::read(int socket) {
	qDebug() << "reading..." << m_file->bytesAvailable() << " " << socket;

	/*
	char    big_buf[8192];          // This is just a demo
	int     len;

	len = ::read (socket, big_buf, 8192);
	if (len == 0)
		return;

	big_buf[len] = 0;

	qDebug() << big_buf;
	*/


	char *buf;
	buf = (char*) calloc(1, 1024);
	qDebug() << m_file->read(buf, 1024);
	//qDebug() << buf;
	emit showText(QString(buf));

	free(buf);

	//if (m_file->bytesAvailable()) {
	
	/*
	QString str = QString(m_file->readLine()).trimmed();
	qDebug() << m_file->canReadLine();

	emit showText(str);
	*/

	/*
	m_notify->setEnabled(false);
	QByteArray ar = m_file->readAll();
	qDebug() << ar;
	m_notify->setEnabled(true);
	*/

		//qint64 lineLength = m_file->readLine(buf, sizeof(buf));
		//qDebug() << buf;
	//}
}

void Reader::handleConnection() {
	connection = srv->nextPendingConnection();
	connect(connection, SIGNAL(readyRead()), this, SLOT(readSocket()));
}

void Reader::readSocket() {
	QString str = QString::fromUtf8(connection->readAll());
	foreach (QString s, str.split('\n')) {
		emit showText(s);
	}
}
