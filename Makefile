DEVICE ?= qqcandy
DESTDIR ?=
UCM_DIR := $(DESTDIR)/usr/share/alsa/ucm2

.PHONY: install check
check:
	test "$(DEVICE)" = qqcandy
	test -s ucm/qqcandy/HiFi.conf
	test -s ucm/qqcandy/qqcandy.conf

install: check
	@if [ -z "$(DESTDIR)" ]; then \
		tr '\000' '\n' < /proc/device-tree/compatible | grep -qx 'oplus,qqcandy' || \
			{ echo "Refusing qqcandy configuration on another board" >&2; exit 1; }; \
	fi
	install -d "$(UCM_DIR)/MediaTek/qqcandy" "$(UCM_DIR)/conf.d/mt6895-mt6368"
	install -m 0644 ucm/qqcandy/*.conf "$(UCM_DIR)/MediaTek/qqcandy/"
	ln -sfn ../../MediaTek/qqcandy/qqcandy.conf \
		"$(UCM_DIR)/conf.d/mt6895-mt6368/mt6895-mt6368.conf"
