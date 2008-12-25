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
#include "osd.h"
#include <QDesktopWidget>
#include <QDebug>


#include <QSvgRenderer>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <QRegExp>



OSD::OSD() : QDialog() {
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(hide()));
	setupUi(this);
	setWindowFlags(Qt::ToolTip|Qt::WindowStaysOnTopHint);

	renderer = new QSvgRenderer(QLatin1String("/home/christof/src/osd/background.svg"), this);
	dirty = true;

	QDesktopWidget w;
	QRect r = w.screenGeometry(0);
	move(r.x()+((r.width()-width())/2), r.y()+((r.height()-height())/2)+400);
	text = new QStringList();
}

void OSD::setText(QString s) {
	QRegExp rx("(\\d+)/(\\d+) (.*)");
	if (rx.exactMatch(s)) {
		stackedWidget->setCurrentWidget(page_1);
		QStringList l = rx.capturedTexts();

		label_1->setText(l[3]);
		value_1->setMaximum(l[2].toInt());
		value_1->setValue(l[1].toInt());
	}
	else {
		stackedWidget->setCurrentWidget(page_2);

		s = label_2->fontMetrics().elidedText(s, Qt::ElideMiddle, 600);
		label_2->setFixedWidth(600);
		label_2->setFixedHeight(80);
		label_2->setFont(QFont("Helvetica", 12));
		label_2->setAlignment(Qt::AlignCenter);

		if (text->count() >= 4) {
			text->removeFirst();
		}
		(*text) << s;

		label_2->setText(text->join(QString('\n')));
	}

	timer->start(4000);
	show();
}

void OSD::paintEvent(QPaintEvent *e) {
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	p.setClipRect(e->rect());

	p.save();
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.fillRect(rect(), Qt::transparent);
	p.restore();

	if (dirty) {
		cache.fill(Qt::transparent);
		QPainter p1(&cache);
		p1.setRenderHint(QPainter::Antialiasing);
		renderer->render(&p1);
		p1.end();
		dirty = false;
	}
	p.drawPixmap(0, 0, cache);
}

void OSD::resizeEvent(QResizeEvent *e) {
	if (e->size() != cache.size()) {
		cache = QPixmap(e->size());
		dirty = true;
	}
}

void OSD::hideEvent(QHideEvent *) {
	label_2->setText("");
	text->clear();
}
