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

class OSD : public QDialog, private Ui::OSD {
	Q_OBJECT

	public:
		OSD();

	public slots:
		void setValue(char *, int, bool);

	private:
		QTimer *timer;
};

#endif   /* ----- #ifndef OSD_INC  ----- */

