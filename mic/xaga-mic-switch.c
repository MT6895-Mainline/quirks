/* xaga-mic-switch: auto-select headset mic (AIN1) vs built-in mic (AIN0)
 * based on the accdet SW_MICROPHONE_INSERT switch on the headset jack input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <alsa/asoundlib.h>

#define EVENT_DEV "/dev/input/event3"
#define SW_MIC_BIT 4   /* SW_MICROPHONE_INSERT */

static int read_mic_switch(int fd)
{
unsigned char buf[18] = {0};

if (ioctl(fd, EVIOCGSW(sizeof(buf)), buf) < 0) {
perror("EVIOCGSW");
return -1;
}
unsigned long val = buf[0] | (buf[1] << 8) |
    (buf[2] << 16) | ((unsigned long)buf[3] << 24);
return !!(val & (1UL << SW_MIC_BIT));
}

static int set_pga_mux(const char *ain)
{
snd_mixer_t *h = NULL;
snd_mixer_selem_id_t *sid;
snd_mixer_elem_t *e;
int err = -1;

if (snd_mixer_open(&h, 0) < 0)
return -1;
if (snd_mixer_attach(h, "default") < 0)
goto out;
if (snd_mixer_selem_register(h, NULL, NULL) < 0)
goto out;
if (snd_mixer_load(h) < 0)
goto out;

snd_mixer_selem_id_alloca(&sid);
snd_mixer_selem_id_set_index(sid, 0);
snd_mixer_selem_id_set_name(sid, "PGA_L_Mux");
e = snd_mixer_find_selem(h, sid);
if (!e) {
fprintf(stderr, "PGA_L_Mux control not found\n");
goto out;
}

int items = snd_mixer_selem_get_enum_items(e);
int target = -1;
for (int i = 0; i < items; i++) {
char name[128];
snd_mixer_selem_get_enum_item_name(e, i, sizeof(name) - 1, name);
if (strcmp(name, ain) == 0) {
target = i;
break;
}
}
if (target < 0) {
fprintf(stderr, "enum item %s not found\n", ain);
goto out;
}
err = snd_mixer_selem_set_enum_item(e, SND_MIXER_SCHN_FRONT_LEFT, target);
if (err < 0)
fprintf(stderr, "set enum: %s\n", snd_strerror(err));

out:
snd_mixer_close(h);
return err;
}

int main(void)
{
int fd;

while ((fd = open(EVENT_DEV, O_RDONLY)) < 0) {
fprintf(stderr, "waiting for %s: %s\n", EVENT_DEV, strerror(errno));
sleep(1);
}

int last = -1;
int iter = 0;
for (;;) {
int mic = read_mic_switch(fd);
if (mic < 0) {
close(fd);
sleep(1);
while ((fd = open(EVENT_DEV, O_RDONLY)) < 0)
sleep(1);
last = -1;
continue;
}
if (mic != last) {
const char *ain = mic ? "AIN1" : "AIN0";
int rc = set_pga_mux(ain);
fprintf(stderr, "xaga-mic-switch: PGA_L_Mux -> %s (rc=%d)\n",
ain, rc);
last = mic;
} else if (iter % 10 == 0) {
/* self-heal: re-assert current selection periodically */
const char *ain = mic ? "AIN1" : "AIN0";
set_pga_mux(ain);
}
iter++;
usleep(500000);
}
}
