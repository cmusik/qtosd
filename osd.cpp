#include "osd.h"
#include <QDesktopWidget>
#include <QDebug>


#include <QSvgRenderer>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <QCheckBox>



OSD::OSD() : QDialog() {
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(hide()));
	setupUi(this);
	setWindowFlags(Qt::ToolTip|Qt::WindowStaysOnTopHint);

	renderer = new QSvgRenderer(QLatin1String("/home/christof/src/osd/background.svg"), this);
	dirty = true;
}

void OSD::setValue(char* s, int v, bool m) {
	if (m)
		label->setText(QString(s)+" (muted)");
	else
		label->setText(QString(s));

	value->setValue(v);
	timer->start(2000);
	QDesktopWidget w;
	QRect r = w.screenGeometry(0);
	move(r.x()+((r.width()-width())/2), r.y()+((r.height()-height())/2)+400);
	show();
}

void OSD::setText(QString s) {
	label->setText(QString(s));

	timer->start(2000);
	QDesktopWidget w;
	QRect r = w.screenGeometry(0);
	move(r.x()+((r.width()-width())/2), r.y()+((r.height()-height())/2)+400);
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


