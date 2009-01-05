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
#include <QApplication>
#include <QDBusConnection>
#include <iostream>
#include <unistd.h>

#include "osd.h"
#include "dbus.h"
#include "readerserver.h"
#include "mixer.h"

#include <X11/extensions/Xrender.h>

void usage() {
	std::cout << "Usage: $0 [ -p port | -n | -b background.svg ]" << std::endl;
	exit(0);
}

int main (int argc, char *argv[]) {
	bool  argbVisual=false;
	Display *dpy = XOpenDisplay(0); // open default display
	if (!dpy) {
		qWarning("Cannot connect to the X server");
		exit(1);
	}

	int screen = DefaultScreen(dpy);
	Colormap colormap = 0;
	Visual *visual = 0;
	int eventBase, errorBase;

	if (XRenderQueryExtension(dpy, &eventBase, &errorBase)) {
		int nvi;
		XVisualInfo templ;
		templ.screen  = screen;
		templ.depth   = 32;
		templ.c_class = TrueColor;
		XVisualInfo *xvi = XGetVisualInfo(dpy, VisualScreenMask |
				VisualDepthMask |
				VisualClassMask, &templ, &nvi);

		for (int i = 0; i < nvi; ++i) {
			XRenderPictFormat *format = XRenderFindVisualFormat(dpy,
					xvi[i].visual);
			if (format->type == PictTypeDirect && format->direct.alphaMask) {
				visual = xvi[i].visual;
				colormap = XCreateColormap(dpy, RootWindow(dpy, screen),
						visual, AllocNone);
				argbVisual=true;
				break;
			}
		}
	}

	QApplication app(dpy, argc, argv,
			Qt::HANDLE(visual), Qt::HANDLE(colormap));

	QStringList args = app.arguments();

	bool daemonize = true;
	QString background;
	int port = 5000;
	float timeout = 4;
	int width = 800;
	int height = 130;

	for (int i = 1; i < args.count(); ++i) {
		if (args[i] == "-n")
			daemonize = false;
		if (args[i] == "-b")
			background = args[i+1];
		if (args[i] == "-p")
			port = args[i+1].toInt();
		if (args[i] == "-t")
			timeout = args[i+1].toFloat();
		if (args[i] == "-w")
			width = args[i+1].toInt();
		if (args[i] == "-h")
			height = args[i+1].toInt();
		if (args[i] == "-help")
			usage();
	}

	OSD osd(background, timeout, width, height);
	MixerThread *t = new MixerThread();

	new DBusAdaptor(&app, &osd);
	QDBusConnection dbuscon = QDBusConnection::connectToBus(QDBusConnection::SessionBus, "qtosd");
	dbuscon.registerService("de.senfdax.qtosd");
	dbuscon.registerObject("/osd", &app);

	if (port > 0) {
		ReaderServer *s = new ReaderServer(&osd);

		if (!s->listen(QHostAddress::LocalHost, port)) {
			qCritical("Couldn't bind on port %d!", port);
			return 255;
		}
	}

	QObject::connect(t, SIGNAL(showText(QString)), &osd, SLOT(setText(QString)));

	if (daemonize)
		daemon(1, 1);

	t->start();

	return app.exec();
}
