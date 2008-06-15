/*
 * Copyright 2004-2007 Christof Musik, Timo Hirvonen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

//#include <alsa/asoundlib.h>
//#include <unistd.h>
//#include <xosd.h>
#include <string.h>
#include <signal.h>

#include "mixer.h"

#include <OSD.h>
#include <QApplication>

// Configuration
#define OSD_FONT "-*-verdana-medium-r-*-*-30-*-*-*-*-*-*-15"
#define OSD_VOFFSET 30
#define OSD_SHADOW 3

OSD *osd;



int main (int argc, char *argv[]) {
	//osd = init_xosd(2);
	//init_alsa();

	QApplication app(argc, argv);
	osd = new OSD();
	MixerThread *t = new MixerThread();
	QObject::connect(t, SIGNAL(valueChanged(char*, int, bool)), osd, SLOT(setValue(char*, int, bool)));
	t->start();

	int ret = app.exec();



	// Signal handler
	//struct sigaction sig;
	//sig.sa_handler = alsa_mixer_exit;
	//sigemptyset (&sig.sa_mask);
	//sigaction(SIGINT, &sig, NULL);


	delete osd;
	return ret;
}
