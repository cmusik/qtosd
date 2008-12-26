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
#include <QDesktopWidget>
#include <QDebug>
#include <QPainter>
#include <QRegExp>

#include "osd.h"

#define WIDTH 750
#define HEIGHT 65
#define MINFONTSIZE 12

OSD::OSD() : QDialog() {
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(hide()));
	setupUi(this);
	setWindowFlags(Qt::ToolTip|Qt::WindowStaysOnTopHint);

	renderer = new QSvgRenderer(QLatin1String("/home/christof/src/osd/background.svg"), this);
	dirty = true;

	label_1->setFixedWidth(WIDTH);
	label_1->setFixedHeight(HEIGHT);
	label_2->setFixedWidth(WIDTH);
	label_2->setFixedHeight(label_2->fontMetrics().height()*4);
	label_2->setAlignment(Qt::AlignCenter);

	QDesktopWidget w;
	QRect r = w.screenGeometry(0);
	move(r.x()+((r.width()-width())/2), r.y()+((r.height()-height())/2)+400);
	text = new QStringList();
}

void OSD::setText(QString s) {
	QRegExp rx("^(\\d+)/(\\d+) (.*)");
	if (rx.exactMatch(s)) {
		stackedWidget->setCurrentWidget(page_1);
		QStringList l = rx.capturedTexts();

		fitText(label_1, &l[3]);

		label_1->setText(l[3]);
		value_1->setMaximum(l[2].toInt());
		value_1->setValue(l[1].toInt());
	}
	else {
		stackedWidget->setCurrentWidget(page_2);

		bool clear = false;
		if (s.startsWith("c/")) {
			clear = true;
			text->clear();
			s = s.remove(0, 2);
		}

		s = label_2->fontMetrics().elidedText(s, Qt::ElideMiddle, WIDTH);

		if (text->count() >= 4) {
			text->removeFirst();
		}
		(*text) << s;

		label_2->setText(text->join(QString('\n')));
		if (clear)
			text->clear();
	}

	timer->start(4000);
	show();
}

void OSD::fitText(QLabel *l, QString *str) {
	int w = 0;
	int h = 0;
	int s = MINFONTSIZE;
	QFont optimal = l->font();
	optimal.setPointSize(s);
	QFont f = optimal;

	while (w < WIDTH && h < HEIGHT) {
		optimal = f;
		f.setPointSize(++s);
		l->setFont(f);
		w = l->fontMetrics().width(*str);
		h = l->fontMetrics().height();
	}
	l->setFont(optimal);

	if (optimal.pointSize() == MINFONTSIZE) {
		*str = l->fontMetrics().elidedText(*str, Qt::ElideMiddle, WIDTH);
	}
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
	text->clear();
}
