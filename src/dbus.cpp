#include "dbus.h"

DBusAdaptor::DBusAdaptor(QApplication *a, OSD *g) : QDBusAbstractAdaptor(a), gui(g), app(a) {
}

void DBusAdaptor::showText(QString s) {
	gui->setText(s);
}

void DBusAdaptor::quit() {
	app->quit();
}

void DBusAdaptor::hide() {
	gui->fadeOut();
}
