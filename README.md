# Configurations/Workarounds for Redmi Note 11T Pro(+) / POCO X4 GT / Redmi K50i (xaga) Linux Mainline
## Sound
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
mv xaga-mic-switch /usr/local/bin/xaga-mic-switch
cp -r mic/xaga-mic-switch.service /etc/systemd/system/xaga-mic-switch.service
systemctl daemon-reload
systemctl enable --now xaga-mic-switch.service
```
