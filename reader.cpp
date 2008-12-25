#include <QDebug>
#include <QStringList>
#include "reader.h"


Reader::Reader() : QObject() {
	srv = new QTcpServer();
	srv->listen(QHostAddress::LocalHost, 5000);
	connect(srv, SIGNAL(newConnection()), this, SLOT(handleConnection()));
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
