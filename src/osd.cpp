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

#define MINFONTSIZE 12

OSD::OSD() : QDialog() {
	timer = new QTimer(this);
	fadeOutTimer = new QTimer(this);
	fadeInTimer = new QTimer(this);
	timer->setSingleShot(true);
	fadeOutTimer->setSingleShot(true);
	fadeInTimer->setSingleShot(true);

	connect(timer, SIGNAL(timeout()), this, SLOT(fadeOut()));
	connect(fadeOutTimer, SIGNAL(timeout()), this, SLOT(fadeOut()));
	connect(fadeInTimer, SIGNAL(timeout()), this, SLOT(fadeIn()));

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
	QRegExp rx("^(\\d+)/(\\d+) (.*)");
	if (rx.exactMatch(s)) {
		stackedWidget->setCurrentWidget(page_1);
		QStringList l = rx.capturedTexts();

		fitText(label_1, &l[3], 1);

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

		if (text->count() >= 4) {
			text->removeFirst();
		}
		(*text) << s;

		fitText(label_2, text);

		label_2->setText(text->join(QString('\n')));
		if (clear)
			text->clear();
	}

	timer->start(4000);
	if (isHidden() || fadeOutTimer->isActive()) {
		fadeIn();
	}
}

void OSD::fitText(QLabel *l, QString *str, int lines=1) {
	int w = 0;
	int h = 0;
	int s = MINFONTSIZE;

	QFont optimal = l->font();
	optimal.setPointSize(s);
	QFont f = optimal;

	while (w < l->minimumWidth() && h * lines < l->minimumHeight()) {
		optimal = f;
		f.setPointSize(++s);
		l->setFont(f);
		w = l->fontMetrics().width(*str);
		h = l->fontMetrics().height();
	}
	l->setFont(optimal);

	if (optimal.pointSize() == MINFONTSIZE && lines == 1) {
		*str = l->fontMetrics().elidedText(*str, Qt::ElideMiddle, l->minimumWidth());
	}
}

void OSD::fitText(QLabel *l, QStringList *list) {
	int max = 0;
	int maxline = 0;
	int cur = 0;
	int line = 0;
	foreach (QString s, *list) {
		if ((cur = l->fontMetrics().width(s)) > max) {
			max = cur;
			maxline = line;
		}
		line++;
	}

	fitText(l, &(*list)[maxline], list->count());

	if (l->font().pointSize() == MINFONTSIZE) {
		QStringList list_new = QStringList(*list);
		list->clear();

		foreach (QString s, list_new) {
			(*list) << l->fontMetrics().elidedText(s, Qt::ElideMiddle, l->minimumWidth());
		}
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

void OSD::fadeOut() {
	qreal w;
	fadeInTimer->stop();
	if ((w = windowOpacity()) > 0.1) {
		fadeOutTimer->start(25);
		setWindowOpacity(w - 0.1);
	}
	else {
		hide();
	}
}

void OSD::fadeIn() {
	qreal w;
	show();
	fadeOutTimer->stop();
	if ((w = windowOpacity()) <= 0.9) {
		fadeInTimer->start(25);
		setWindowOpacity(w + 0.1);
	}
	else {
		setWindowOpacity(1);
	}
}
