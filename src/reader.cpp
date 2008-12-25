/*
  Copyright 2008 Christof Musik <christof at senfdax.de>

  This file is part of osdmixer.

  Foobar is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Foobar is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/
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
