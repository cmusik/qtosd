#include "osd.h"
#include <QDesktopWidget>
#include <QDebug>

OSD::OSD() : QDialog() {
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(hide()));
	setupUi(this);
	setWindowFlags(Qt::ToolTip|Qt::WindowStaysOnTopHint);
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
	//setWindowOpacity(0.75);
}
