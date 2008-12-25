#include <QDebug>
#include <QStringList>
#include "reader.h"


Reader::Reader(int socket) : QThread() {
	connection = new QTcpSocket(this);
	connection->setSocketDescriptor(socket);
	connect(connection, SIGNAL(readyRead()), this, SLOT(readSocket()));
	connect(connection, SIGNAL(disconnected()), this, SLOT(closeConnection()));
}

Reader::~Reader() {
	delete connection;
}

void Reader::readSocket() {
	QString str = QString::fromUtf8(connection->readAll());

	foreach (QString s, str.split('\n')) {
		s = s.simplified();
		if (s == "quit") {
			closeConnection();
			break;
		}
		if (!s.isEmpty()) {
			emit receivedText(s);
		}
	}
}

void Reader::closeConnection() {
	connection->close();
	terminate();
}
