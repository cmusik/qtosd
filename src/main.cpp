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

#include <QApplication>
#include <unistd.h>

#include "osd.h"
#include "readerserver.h"
#include "mixer.h"

#include <X11/extensions/Xrender.h>


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

	OSD osd;
	MixerThread *t = new MixerThread();
	ReaderServer *s = new ReaderServer(&osd);

	if (!s->listen(QHostAddress::LocalHost, 5000)) {
		qCritical("Couldn't bind on port 5000!");
		return 255;
	}

	QObject::connect(t, SIGNAL(showText(QString)), &osd, SLOT(setText(QString)));

	if (!app.arguments().contains("-n"))
		daemon(1, 1);

	t->start();

	return app.exec();
}