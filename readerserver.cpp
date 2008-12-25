#include "readerserver.h"
#include "reader.h"

ReaderServer::ReaderServer(OSD *g, QObject *parent) : QTcpServer(parent) {
	listen(QHostAddress::LocalHost, 5000);
	gui = g;
}

void ReaderServer::incomingConnection(int socket) {
	Reader *r = new Reader(socket);
	connect(r, SIGNAL(receivedText(QString)), gui, SLOT(setText(QString)));
	r->start();
}

