#include <QDebug>
#include "mixer.h"

MixerThread::MixerThread() {
	init_alsa();
}

void MixerThread::run() {
	qDebug() << "run()";
	do {
		Mixer *m = mixer;
		while(m) {
			if (alsa_mixer_get_volume(m) == 1) {
				display_osd(m);
				//app.processEvents();
			}

			m = m->next;
		}
	} while (!mixer_iteration(-1));
}

/*
void MixerThread::alsa_mixer_exit()
{
	Mixer *m = mixer;
	while (m) {
		Mixer *tmp = m;
		m = m->next;
		free(tmp);
	}
	//xosd_destroy(osd);
	free(alsa_mixer_device);
	snd_mixer_close(alsa_mixer_handle);
	exit(0);
}
*/

int MixerThread::alsa_mixer_open() {
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

int MixerThread::alsa_mixer_get_volume(Mixer *m) {
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

	snd_mixer_selem_get_playback_volume(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT, &lv);
	snd_mixer_selem_get_playback_volume(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &rv);
	snd_mixer_selem_get_playback_switch(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT, &m->muted_left);
	snd_mixer_selem_get_playback_switch(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &m->muted_right);

	m->vol_left = ((float) (lv - m->mixer_vol_min) / m->mixer_vol_max)*100.0+0.5;
	m->vol_right = ((float) (rv - m->mixer_vol_min) / m->mixer_vol_max)*100.0+0.5;

	m->muted_left = !m->muted_left;
	m->muted_right = !m->muted_right;

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

Mixer* MixerThread::get_new_mixer() {
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

/*
xosd* init_xosd(int lines) {
	xosd *osd = xosd_create(lines);

	xosd_set_shadow_offset (osd, OSD_SHADOW);
	xosd_set_pos (osd, XOSD_bottom);
	xosd_set_vertical_offset (osd, OSD_VOFFSET);
	xosd_set_align (osd, XOSD_center);
	xosd_set_font (osd, OSD_FONT);
	xosd_set_timeout(osd, 1);
	return osd;
}
*/

void MixerThread::init_alsa() {
	alsa_mixer_device = strdup("default");
	mixer = get_new_mixer();

	int rc = alsa_mixer_open();

	if (rc != SUCCESS) {
		fprintf(stderr, "Error: could not open mixer\n");
		exit(ERROR);
	}
}

void MixerThread::display_osd(Mixer *m) {
	char *muted = "";
	printf("show %s\n", m->name);

	if (strncmp(m->name, "Master", 6) != 0)
		return;

	if (m->muted_left && m->muted_right)
		muted = strdup("(muted)");
	/*
	else if (m->muted_left)
		muted = strdup("(left muted)");
	else if (m->muted_right)
		muted = strdup("(right muted)");
	*/

	emit valueChanged(m->name, m->vol_left, m->muted_left && m->muted_right);

}

int
MixerThread::mixer_iteration (int timeout) {
	int count, err;
	struct pollfd *fds;
	unsigned short revents;

	if ((count = snd_mixer_poll_descriptors_count(alsa_mixer_handle)) < 0) {
		return 1;
	}
	fds = (struct pollfd*) calloc(count, sizeof(struct pollfd));
	if (fds == NULL) {
		return 1;
	}
	if ((err = snd_mixer_poll_descriptors(alsa_mixer_handle, fds, count)) < 0) {
		return 1;
	}
	if (err != count) {
		return 1;
	}

	if (poll(fds, count, timeout) > 0) {
		if (snd_mixer_poll_descriptors_revents(alsa_mixer_handle, fds, count, &revents) >= 0) {
			if (revents & POLLNVAL)
				return 1;
			if (revents & POLLERR)
				return 1;
			if (revents & POLLIN)
				snd_mixer_handle_events(alsa_mixer_handle);
		}
	}

	free(fds);

	return 0;
}

