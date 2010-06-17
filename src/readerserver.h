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
#include <QTcpServer>
#include "osd.h"

#ifndef  READERSERVER_INC
#define  READERSERVER_INC

class ReaderServer : public QTcpServer {
    Q_OBJECT

    public:
        ReaderServer(OSD *, QObject* = NULL);

    private:
        virtual void incomingConnection(int);

        OSD *gui;
};

#endif

