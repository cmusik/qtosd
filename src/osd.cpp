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

OSD::OSD(QString bg, float t, int w, int h, int sb, int sl, int s) : QWidget(), timeout(t) {
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
    setWindowFlags(Qt::ToolTip|Qt::X11BypassWindowManagerHint|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);

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
    int x = 0;
    int y = r.y()+((r.height()-height()))-sb;

    if (sl >= 0)
        x = r.x()+sl;
    else
        x = r.x()+((r.width()-width())/2);

    move(x, y);
    textList = new QStringList();
    setWindowOpacity(0.1);

    progressRegexp = new QRegExp("^(\\d+)/(\\d+)$");
    fileRegexp = new QRegExp("^f:(/.*\\.(jpg|png))", Qt::CaseInsensitive);
    // FIXME: don't why this helps, but with qt 4.6 it doesn't crash anymore
    show();
    hide();
}

OSD::~OSD() {
    if (progressRegexp)
        delete progressRegexp;

    textList->clear();
    delete textList;
}

void OSD::setText(QString s) {
    QString text;
    QString file;
    QRegExp rx("^(\\((.+)\\))*(.*)");

    // show everything, to get proper size
    timer->start(int (timeout*1000));
    if (isHidden() || fadeOutTimer->isActive()) {
        fadeIn();
    }

    setUpdatesEnabled(false);

    progressBar->setHidden(true);
    image->setHidden(true);
    label->setHidden(true);

    rx.setMinimal(true);
    rx.exactMatch(s);

    foreach (QString p, s.split("%", QString::SkipEmptyParts)) {
        if (progressRegexp->exactMatch(p)) {
            progressBar->setHidden(false);

            QStringList l = progressRegexp->capturedTexts();
            progressBar->setMaximum(l[2].toInt());
            progressBar->setValue(l[1].toInt());
        }
        else if (fileRegexp->exactMatch(p)) {
            file = fileRegexp->cap(1);
        }
        else {
            label->setHidden(false);
            text += p + " ";
        }
    }

    // open image
    if (!file.isEmpty()) {
        QImage img(file);
        if (!img.isNull()) {
            img = img.scaledToHeight(image->height());
            image->setPixmap(QPixmap::fromImage(img));
            image->setHidden(false);
        }
    }

    // show text
    int lines = 1;
    QRegExp linesRegex("^(\\d+)/(.*)");
    if (linesRegex.exactMatch(text)) {
        lines = linesRegex.cap(1).toInt();
        text = linesRegex.cap(2);
    }
    else
        textList->clear();

    if (lines <= 0)
        lines = 1;

    while (textList->count() >= lines) {
        textList->removeFirst();
    }
    textList->push_back(text.trimmed());

    fitText(label, textList);

    label->setText(textList->join(QString('\n')));
    if (lines == 1)
        textList->clear();

    setUpdatesEnabled(true);
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
    textList->clear();
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
