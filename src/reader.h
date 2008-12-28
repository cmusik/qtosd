/*
  Copyright 2008 Christof Musik <christof at senfdax.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
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
		QTcpSocket *connection;

};

#endif

