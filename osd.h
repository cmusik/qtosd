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
/*
 * =====================================================================================
 * 
 *       Filename:  OSD.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  15.06.2008 14:12:50 CEST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:   (), 
 *        Company:  
 * 
 * =====================================================================================
 */

#ifndef  OSD_INC
#define  OSD_INC

#include "ui_osd.h"
#include <QTimer>

#include <QPaintEvent>
#include <QSvgRenderer>
#include <QResizeEvent>
#include <QStringList>


class OSD : public QDialog, private Ui::OSD {
	Q_OBJECT

	public:
		OSD();

	protected:
		virtual void paintEvent(QPaintEvent *e);
		virtual void resizeEvent(QResizeEvent *e);
		virtual void hideEvent(QHideEvent *e);

	public slots:
		void setText(QString);

	private:
		QTimer *timer;

		QStringList *text;
		QSvgRenderer *renderer;
		QPixmap       cache;
		bool          dirty;

};

#endif   /* ----- #ifndef OSD_INC  ----- */

