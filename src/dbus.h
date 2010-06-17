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
