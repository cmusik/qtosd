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

