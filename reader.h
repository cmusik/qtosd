#include <QObject>
#include <QFile>
#include <QSocketNotifier>

#ifndef  READER_INC
#define  READER_INC

class Reader : public QObject {
	Q_OBJECT

	public:
		Reader();
	
	public slots:
		void read(int);

	signals:
		void showText(QString);

	private:
		QFile *m_file;
		QSocketNotifier *m_notify;

};

#endif   /* ----- #ifndef READER_INC  ----- */

