#include "OSD.h"
#include <unistd.h>
#include <QDesktopWidget>
#include <QDebug>

OSD::OSD() : QDialog() {
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(hide()));
	setupUi(this);
	setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);
}

void OSD::setValue(char* s, int v, bool m) {
	if (m)
		mixer->setText(QString(s)+" (muted)");
	else
		mixer->setText(QString(s));

	value->setValue(v);
	timer->start(2000);
	QDesktopWidget w;
	QRect r = w.screenGeometry(0);
	qDebug() << w.screenGeometry(0);
	move(r.x()+((r.width()-width())/2), r.y()+((r.height()-height())/2)+400);
	//setWindowOpacity(0.75);
	show();
	qDebug() << r.x() << r.width() << width();
	//move(1280, 0);
}
