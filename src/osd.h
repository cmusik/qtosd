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
#ifndef  OSD_INC
#define  OSD_INC

#include <QTimer>
#include <QPaintEvent>
#include <QSvgRenderer>
#include <QResizeEvent>
#include <QStringList>

#include "ui_osd.h"

class OSD : public QDialog, private Ui::OSD {
	Q_OBJECT

	public:
		OSD(QString, float, int, int, int=0);
		~OSD();

	protected:
		virtual void paintEvent(QPaintEvent*);
		virtual void hideEvent(QHideEvent*);
		virtual void mouseReleaseEvent(QMouseEvent*);

	public slots:
		void setText(QString);
		void fadeOut();
		void fadeIn();

	private:
		void fitText(QLabel*, QString*, int);
		void fitText(QLabel*, QStringList*);

		QTimer *timer;
		QTimer *fadeOutTimer;
		QTimer *fadeInTimer;
		QStringList *text;
		QSvgRenderer *renderer;
		QPixmap cache;
		float timeout;
		QRegExp *progressRegexp;
		QRegExp *fileRegexp;

};

#endif
