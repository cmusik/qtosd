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

void OSD::setValue(QString s, int v, bool m) {
	if (m)
		label->setText(s+" (muted)");
	else
		label->setText(s);

	value->setMaximum(100);
	value->setValue(v);
	stackedWidget->setCurrentIndex(0);
	timer->start(2000);
	show();
}

void OSD::setText(QString s) {
	QRegExp rx("(\\d+)/(\\d+) (.*)");
	if (rx.exactMatch(s)) {
		stackedWidget->setCurrentIndex(0);
		QStringList l = rx.capturedTexts();

		label->setText(l[3]);
		value->setMaximum(l[2].toInt());
		value->setValue(l[1].toInt());
	}
	else {
		stackedWidget->setCurrentIndex(2);

		label_3->setWordWrap(true);
		label_3->setFixedWidth(600);
		label_3->setFixedHeight(80);
		label_3->setFont(QFont("Helvetica", 12));
		label_3->setAlignment(Qt::AlignCenter);

		if (text->count() >= 4) {
			text->removeFirst();
		}
		(*text) << s; //->push_back(s);


		label_3->setText(text->join(QString('\n')));
	}

	timer->start(2000);
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
