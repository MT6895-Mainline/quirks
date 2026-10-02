# qqcandy device configuration

OPPO K10 / OnePlus Ace Racing Edition, projects 21143 and 22801.
This branch does not install Xaga's microphone-switch daemon.

```sh
make check
sudo make install                         # requires oplus,qqcandy compatible
make DESTDIR=/path/to/qqcandy-rootfs install
```

The UCM is based on the configuration deployed on qqcandy and the board's
MT6895/MT6368/TFA9874 ALSA controls. Speaker/headphones use PCM2 with mutually
exclusive I2S3/ADDA_DL routes; internal/headset microphones use PCM1 and
AIN0/AIN1. This differs from the Xaga configuration in this repository.

Validation does not write mixer controls. Build checks and control-name
checks are not a substitute for a listening/recording test on a fresh image.
There is no modem voice verb: IMS and modem audio are separate unfinished
bring-up work. No fingerprint, power-register or charge-pump quirks are added.

The new qqcandy files carry MIT SPDX identifiers; this does not relicense
pre-existing Xaga files with unspecified licensing.
