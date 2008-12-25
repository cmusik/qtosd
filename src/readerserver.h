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
#include <QTcpServer>
#include "osd.h"

#ifndef  READERSERVER_INC
class ReaderServer : public QTcpServer {
	Q_OBJECT

	public:
		ReaderServer(OSD *, QObject* = NULL);

	private:
		virtual void incomingConnection(int);

		OSD *gui;
};
#define  READERSERVER_INC

#endif   /* ----- #ifndef READERSERVER_INC  ----- */

