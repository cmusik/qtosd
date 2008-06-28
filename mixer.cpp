#include <QDebug>
#include "mixer.h"

MixerThread::MixerThread() {
	init_alsa();
}

MixerThread::~MixerThread() {
	Mixer *m = mixer;
	while (m) {
		Mixer *tmp = m;
		m = m->next;
		free(tmp);
	}
	free(alsa_mixer_device);
	snd_mixer_close(alsa_mixer_handle);
}

void
MixerThread::run() {
	do {
		snd_mixer_handle_events(alsa_mixer_handle);
		Mixer *m = mixer;
		while (m) {
			if (alsa_mixer_get_volume(m) == 1)
				display_osd(m);

			m = m->next;
		}
	} while (!snd_mixer_wait(alsa_mixer_handle, -1));

	qDebug() << "end";
}

int
MixerThread::alsa_mixer_open() {
	snd_mixer_selem_id_t *sid;
	snd_mixer_elem_t *elem;
	int count;
	int rc;

	if (snd_mixer_selem_id_malloc(&sid) != 0) {
		fprintf(stderr, "Error: Couldn't alloc mixer\n");
		exit(ERROR);
	}

	rc = snd_mixer_open(&alsa_mixer_handle, 0);
	if (rc < 0)
		return ERROR;
	rc = snd_mixer_attach(alsa_mixer_handle, alsa_mixer_device);
	if (rc < 0)
		return ERROR;
	rc = snd_mixer_selem_register(alsa_mixer_handle, NULL, NULL);
	if (rc < 0)
		return ERROR;
	rc = snd_mixer_load(alsa_mixer_handle);
	if (rc < 0)
		return ERROR;
	count = snd_mixer_get_count(alsa_mixer_handle);
	if (count == 0) {
		return ERROR;
	}
	elem = snd_mixer_first_elem(alsa_mixer_handle);
	Mixer *m = mixer;

	while (elem) {
		snd_mixer_selem_get_id(elem, sid);
		if (snd_mixer_selem_has_playback_volume(elem)) {
			snd_mixer_selem_get_playback_volume_range(elem, &m->mixer_vol_min, &m->mixer_vol_max);

			m->name = strdup(snd_mixer_selem_id_get_name(sid));
			m->mixer_elem = elem;

			alsa_mixer_get_volume(m);

			m->next = get_new_mixer();
			m = m->next;
		}
		elem = snd_mixer_elem_next(elem);
	}
	return SUCCESS;
}

int
MixerThread::alsa_mixer_get_volume(Mixer *m) {
	long lv = 0, rv = 0;

	int old_vol_left = 0;
	int old_vol_right = 0;
	int old_muted_left = 0;
	int old_muted_right = 0;

	if (m->mixer_elem == NULL)
		return ERROR;

	old_vol_left = m->vol_left;
	old_vol_right = m->vol_right;
	old_muted_left = m->muted_left;
	old_muted_right = m->muted_right;

	if (snd_mixer_selem_has_playback_channel(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT))
		snd_mixer_selem_get_playback_volume(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT, &lv);
	if (snd_mixer_selem_has_playback_channel(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT))
		snd_mixer_selem_get_playback_volume(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &rv);

	m->vol_left = ((float) (lv - m->mixer_vol_min) / m->mixer_vol_max)*100.0+0.5;
	m->vol_right = ((float) (rv - m->mixer_vol_min) / m->mixer_vol_max)*100.0+0.5;

	if (snd_mixer_selem_has_playback_switch(m->mixer_elem)) {
		snd_mixer_selem_get_playback_switch(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT, &m->muted_left);
		snd_mixer_selem_get_playback_switch(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &m->muted_right);

		m->muted_left = !m->muted_left;
		m->muted_right = !m->muted_right;
	}

	// volume has changed
	if (	m->vol_left != old_vol_left ||
			m->vol_right != old_vol_right ||
			m->muted_left != old_muted_left ||
			m->muted_right != old_muted_right
			) {
		return SUCCESS;
	}
	return 0;
}

Mixer*
MixerThread::get_new_mixer() {
	Mixer *m = NULL;
	if ((m = (Mixer*) malloc(sizeof(Mixer))) == NULL) {
		fprintf(stderr, "Error: Couldn't alloc mixer\n");
		exit(ERROR);
	}
	m->mixer_elem = NULL;

	m->name = NULL;
	m->mixer_vol_max = 0;
	m->mixer_vol_min = 0;
	m->vol_left = -1;
	m->vol_right = -1;
	m->muted_left = 0;
	m->muted_right = 0;
	m->next = NULL;
	return m;
}

void
MixerThread::init_alsa() {
	alsa_mixer_device = strdup("default");
	mixer = get_new_mixer();

	int rc = alsa_mixer_open();

	if (rc != SUCCESS) {
		fprintf(stderr, "Error: could not open mixer\n");
		exit(ERROR);
	}
}

void
MixerThread::display_osd(Mixer *m) {
	char *muted = strdup("");

	if (m->muted_left == 0 && m->muted_right == 0)
		muted = strdup("(muted)");

	emit valueChanged(m->name, m->vol_left, m->muted_left && m->muted_right);
}
