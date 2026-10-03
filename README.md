# MT6895 Device Configurations and Workarounds

The `qqcandy-mt6895` branch adds board-specific configuration for OPlus qqcandy
(PGZ110, mixed 21143/22801). Existing xaga material is retained below for Redmi
Note 11T Pro(+) / POCO X4 GT / Redmi K50i; it must not be applied to qqcandy.

## qqcandy Sound

Install into an offline image or onto a verified qqcandy device:

```sh
make DEVICE=qqcandy check
make DEVICE=qqcandy DESTDIR=/absolute/path/to/rootfs install
# On qqcandy only, as root: make DEVICE=qqcandy install
```

Live installation checks `oplus,qqcandy` before changing UCM. Offline installation
requires the caller to select the correct image. Both speaker and headphones use
PCM2/DL2; recording uses PCM1 with AIN0 for the internal microphone and AIN1 for
the headset. Xaga's PCM0/DL1 headphone route and microphone daemon do not apply.

The UCM controls and PCM routes were checked against the local device's mixer
and authoritative board sources. Offline installation and all 15 named controls
passed validation. Live playback, recording and call audio were not exercised
in this integration stage; control existence is not acoustic validation.

The [rootfs builder](https://github.com/MT6895-Mainline/rootfs) installs a pinned
revision of this profile. This repo contains device configuration, not firmware,
the initramfs boot chain or modem owner services. Punch-hole/top-bar adaptation
has not been added; it needs separate Phosh/gmobile device data and validation.

## Xaga Sound (Existing Support)

### 1. UCM (Main ALSA config)
```
# On device, as root
cp -r ucm/mt6895-mt6368 /usr/share/alsa/ucm2/conf.d/
```
### 2. Microphone auto switch
On xaga, built in mic and 3.5mm headset are connected to the same pipe, they can't record stereo stream at same time.
Instead of patching UCM, leave one "Built-in Microphone" device and switch routing in a daemon.

```
# On device, as root
gcc -O2 -Wall -o xaga-mic-switch mic/xaga-mic-switch.c -lasound
mv xaga-mic-switch /usr/local/sbin/xaga-mic-switch
cp -r mic/xaga-mic-switch.service /etc/systemd/system/xaga-mic-switch.service
systemctl daemon-reload
systemctl enable --now xaga-mic-switch.service
```
