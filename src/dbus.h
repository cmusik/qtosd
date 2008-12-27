#include <QApplication>
#include <QDBusAbstractAdaptor>
#include "osd.h"

#ifndef  DBUS_INC
#define  DBUS_INC

class DBusAdaptor : public QDBusAbstractAdaptor {
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "de.senfdax.osd")

	public:
		DBusAdaptor(QApplication*, OSD*);

	public slots:
		Q_NOREPLY void showText(QString);
		Q_NOREPLY void quit();
		Q_NOREPLY void hide();

	private:
		OSD *gui;
		QApplication *app;
};

#endif
