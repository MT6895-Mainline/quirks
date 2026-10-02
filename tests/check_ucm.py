#!/usr/bin/env python3
"""Read-only validation of qqcandy UCM against an amixer contents export."""
import pathlib
import re
import sys

ucm = pathlib.Path(__file__).resolve().parents[1] / "ucm/qqcandy/HiFi.conf"
text = ucm.read_text()
controls = pathlib.Path(sys.argv[1]).read_text()
available = set(re.findall(r"^numid=.*?,name='([^']+)'$", controls, re.M))
required = set(re.findall(r'cset "name=\x27([^\x27]+)\x27', text))
required.update(re.findall(r'(?:JackControl|PlaybackMixerElem) "([^"]+)"', text))
missing = required - available
if missing:
    raise SystemExit("Missing ALSA controls: " + ", ".join(sorted(missing)))
assert text.count('PlaybackPCM "hw:${CardId},2"') == 2
assert 'PlaybackPCM "hw:${CardId},0"' not in text
assert text.count('CapturePCM "hw:${CardId},1"') == 2
print(f"Validated {len(required)} qqcandy control names and PCM routes")
