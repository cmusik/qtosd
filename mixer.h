#ifndef  MIXER_INC
#define  MIXER_INC

#include <QThread>
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
	Mixer *next;
};

enum {
	SUCCESS = 1,
	ERROR = 255
};

class MixerThread : public QThread {
	Q_OBJECT

	public:
		MixerThread();
		~MixerThread();
		void run();

		int alsa_mixer_get_volume(Mixer*);
		int alsa_mixer_open();
		Mixer* get_new_mixer();
		void init_alsa();
		void display_osd(Mixer*);

		Mixer *mixer;

		snd_mixer_t *alsa_mixer_handle;
		char *alsa_mixer_device;

	signals:
		void valueChanged(char *, int, bool);
};

#endif   /* ----- #ifndef MIXER_INC  ----- */

