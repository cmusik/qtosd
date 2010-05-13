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
#ifndef  MIXER_INC
#define  MIXER_INC

#include <QThread>
#include <QString>
#include <alsa/asoundlib.h>

typedef struct mixer Mixer;

struct mixer {
	snd_mixer_elem_t *mixer_elem;
	long mixer_vol_min, mixer_vol_max;

	char *name;
	int vol_left;
	int vol_right;
	int old_vol_left;
	int old_vol_right;
	int muted_left;
	int muted_right;
	int type;
	int has_switch;
	Mixer *next;
};

enum {
	SUCCESS = 1,
	ERROR = 255
};

class MixerThread : public QThread {
	Q_OBJECT

	public:
		MixerThread(const QString&);
		~MixerThread();
		void run();

		int alsa_mixer_get_volume(Mixer*);
		int alsa_mixer_open();
		Mixer* get_new_mixer();
		bool init_alsa();
		void display_osd(Mixer*);

		Mixer *mixer;

		snd_mixer_t *alsa_mixer_handle;
		char *alsa_mixer_device;

	signals:
		//void valueChanged(QString, int, bool);
		void showText(QString);
};

#endif   /* ----- #ifndef MIXER_INC  ----- */

