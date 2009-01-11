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
#include <QDesktopWidget>
#include <QDebug>
#include <QPainter>
#include <QRegExp>
#include <QFileInfo>
#include <QBitmap>
#include <QX11Info>

#include "osd.h"

#define MINFONTSIZE 12

OSD::OSD(QString bg, float t, int w, int h, int s) : QDialog(), timeout(t) {
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

	if (QFileInfo(bg).exists())
		renderer = new QSvgRenderer(bg, this);
	else
		renderer = new QSvgRenderer(QString::fromUtf8(":/background/background.svg"), this);

	setFixedSize(w, h);

	cache = QPixmap(size());
	cache.fill(Qt::transparent);
	QPainter p1(&cache);
	p1.setRenderHint(QPainter::Antialiasing);
	renderer->render(&p1);
	p1.end();

	if (!QX11Info::isCompositingManagerRunning()) {
		setMask(cache.createHeuristicMask());
	}

	QDesktopWidget desktop;
	QRect r = desktop.screenGeometry(s);
	move(r.x()+((r.width()-width())/2), r.y()+((r.height()-height())/2)+400);
	text = new QStringList();
	setWindowOpacity(0.1);

	progressRegexp = new QRegExp("^(\\d+)/(\\d+) (.*)");
	fileRegexp = new QRegExp("^f:(/.*\\.png)/(.*)");
}

OSD::~OSD() {
	if (progressRegexp)
		delete progressRegexp;
}

void OSD::setText(QString s) {
	timer->start(int (timeout*1000));
	if (isHidden() || fadeOutTimer->isActive()) {
		fadeIn();
	}

	if (progressRegexp->exactMatch(s)) {
		stackedWidget->setCurrentWidget(page_1);
		QStringList l = progressRegexp->capturedTexts();

		fitText(label_1, &l[3], 1);

		label_1->setText(l[3]);
		value_1->setMaximum(l[2].toInt());
		value_1->setValue(l[1].toInt());
	}
	else if (fileRegexp->exactMatch(s)) {
		QString f = fileRegexp->cap(1);
		QString t = fileRegexp->cap(2);
		stackedWidget->setCurrentWidget(page_3);

		QImage img(f);
		img = img.scaledToHeight(image->height());

		image->setPixmap(QPixmap::fromImage(img));
		fitText(label_3, &t, 1);
		label_3->setText(t);
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

}

void OSD::fitText(QLabel *l, QString *str, int lines=1) {
	int w = 0;
	int h = 0;
	int s = MINFONTSIZE;

	QFont optimal = l->font();
	optimal.setPointSize(s);
	QFont f = optimal;

	while (w < l->width() && h * lines < l->height()) {
		optimal = f;
		f.setPointSize(++s);
		l->setFont(f);
		w = l->fontMetrics().width(*str);
		h = l->fontMetrics().height();
	}
	l->setFont(optimal);

	if (optimal.pointSize() == MINFONTSIZE && lines == 1) {
		*str = l->fontMetrics().elidedText(*str, Qt::ElideMiddle, l->width());
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
			(*list) << l->fontMetrics().elidedText(s, Qt::ElideMiddle, l->width());
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

	p.drawPixmap(0, 0, cache);
}

void OSD::mouseReleaseEvent(QMouseEvent *) {
	fadeOut();
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
