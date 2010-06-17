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
#include "mixer.h"

MixerThread::MixerThread(const QString& d) {
    alsa_mixer_device = strdup(d.toLatin1().data());
    if (init_alsa())
        start();
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

void MixerThread::run() {
    do {
        snd_mixer_handle_events(alsa_mixer_handle);
        Mixer *m = mixer;
        while (m) {
            if (alsa_mixer_get_volume(m) == 1)
                display_osd(m);

            m = m->next;
        }
    } while (!snd_mixer_wait(alsa_mixer_handle, -1));
}

int MixerThread::alsa_mixer_open() {
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t *elem;
    int count;
    int rc;

    if (snd_mixer_selem_id_malloc(&sid) != 0) {
        fprintf(stderr, "Error: Couldn't alloc mixer\n");
        return ERROR;
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
        if (snd_mixer_selem_has_playback_volume(elem) && snd_mixer_selem_has_playback_channel(elem, SND_MIXER_SCHN_FRONT_LEFT)) {
            m->type = 1;
            snd_mixer_selem_get_playback_volume_range(elem, &m->mixer_vol_min, &m->mixer_vol_max);
            if (snd_mixer_selem_has_playback_switch(elem))
                m->has_switch = 1;
        }
        else if (snd_mixer_selem_has_capture_volume(elem) && snd_mixer_selem_has_capture_channel(elem, SND_MIXER_SCHN_FRONT_LEFT)) {
            m->type = 2;
            snd_mixer_selem_get_capture_volume_range(elem, &m->mixer_vol_min, &m->mixer_vol_max);
            if (snd_mixer_selem_has_capture_switch(elem))
                m->has_switch = 1;
        }
        else {
            elem = snd_mixer_elem_next(elem);
            continue;
        }

        m->name = strdup(snd_mixer_selem_id_get_name(sid));
        m->mixer_elem = elem;

        alsa_mixer_get_volume(m);

        if ((m->next = get_new_mixer()) == NULL) {
            fprintf(stderr, "Error: Couldn't alloc mixer\n");
            return ERROR;
        }
        m = m->next;

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

    if (m->type == 1) {
        if (snd_mixer_selem_has_playback_channel(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT))
            snd_mixer_selem_get_playback_volume(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT, &lv);
        if (snd_mixer_selem_has_playback_channel(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT))
            snd_mixer_selem_get_playback_volume(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &rv);

        m->vol_left = ((float) (lv - m->mixer_vol_min) / m->mixer_vol_max)*100.0+0.5;
        m->vol_right = ((float) (rv - m->mixer_vol_min) / m->mixer_vol_max)*100.0+0.5;

        if (m->has_switch) {
            snd_mixer_selem_get_playback_switch(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT, &m->muted_left);
            snd_mixer_selem_get_playback_switch(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &m->muted_right);

            m->muted_left = !m->muted_left;
            m->muted_right = !m->muted_right;
        }
    }
    else {
        if (snd_mixer_selem_has_capture_channel(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT))
            snd_mixer_selem_get_capture_volume(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT, &lv);
        if (snd_mixer_selem_has_capture_channel(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT))
            snd_mixer_selem_get_capture_volume(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &rv);

        m->vol_left = ((float) (lv - m->mixer_vol_min) / m->mixer_vol_max)*100.0+0.5;
        m->vol_right = ((float) (rv - m->mixer_vol_min) / m->mixer_vol_max)*100.0+0.5;

        if (m->has_switch) {
            snd_mixer_selem_get_capture_switch(m->mixer_elem, SND_MIXER_SCHN_FRONT_LEFT, &m->muted_left);
            snd_mixer_selem_get_capture_switch(m->mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &m->muted_right);

            m->muted_left = !m->muted_left;
            m->muted_right = !m->muted_right;
        }
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

Mixer* MixerThread::get_new_mixer() {
    Mixer *m = NULL;
    if ((m = (Mixer*) malloc(sizeof(Mixer))) == NULL) {
        fprintf(stderr, "Error: Couldn't alloc mixer\n");
        return NULL;
    }
    m->mixer_elem = NULL;

    m->name = NULL;
    m->mixer_vol_max = 0;
    m->mixer_vol_min = 0;
    m->vol_left = -1;
    m->vol_right = -1;
    m->muted_left = 0;
    m->muted_right = 0;
    m->type = 1;
    m->has_switch = 0;
    m->next = NULL;
    return m;
}

bool MixerThread::init_alsa() {
    if ((mixer = get_new_mixer()) == NULL) {
        return false;
    }

    if (alsa_mixer_open() != SUCCESS) {
        fprintf(stderr, "Error: could not open mixer\n");
        return false;
    }
    return true;
}

void MixerThread::display_osd(Mixer *m) {
    QString name(m->name);
    if (m->has_switch && m->muted_left && m->muted_right) {
        name = name + " (muted)";
    }
    emit showText(QString("%1/%2\%%3").arg(m->vol_left).arg("100").arg(name));
}
