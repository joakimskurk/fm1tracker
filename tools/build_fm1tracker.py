#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""FM1Tracker identity wrapper around the pinned Dinghy build driver.

WP1.4 deliberately keeps tools/build.py unchanged. This wrapper supplies the
FM1Tracker-visible app name, the fixed FM-1 package identity, release version
text, and FM1Tracker artifact names while reusing Dinghy's verified loader,
package, safety-check and firmware-build implementation.
"""
import re
import sys
from pathlib import Path

import build as dinghy_build

PRODUCT = "FM-1_987"
APP_NAME = "FM1TRACKER"


def parse_release(argv):
    """Remove --release from argv for Dinghy's driver and return its value."""
    out = [argv[0]]
    release = None
    i = 1
    while i < len(argv):
        arg = argv[i]
        if arg == "--release":
            if i + 1 >= len(argv):
                raise SystemExit("--release requires a version")
            if release is not None:
                raise SystemExit("--release specified more than once")
            release = argv[i + 1]
            i += 2
            continue
        if arg.startswith("--release="):
            if release is not None:
                raise SystemExit("--release specified more than once")
            release = arg.split("=", 1)[1]
            i += 1
            continue
        out.append(arg)
        i += 1

    if release is not None:
        m = re.fullmatch(r"(\d)\.(\d)(\.\d)?(-[A-Za-z0-9]+)?", release)
        if not m:
            raise SystemExit(f"--release {release}: use X.Y, X.Y.Z or X.Y-suffix, one digit each")
    return release, out


def rename_artifacts(release):
    out = dinghy_build.OUT
    for suffix in ("o", "elf", "dis", "bin"):
        src = out / f"dinghy.{suffix}"
        if src.exists():
            src.replace(out / f"fm1tracker.{suffix}")

    pkg_src = out / "dinghy.fwsc"
    pkg_name = f"fm1tracker-{release}.fwsc" if release else "fm1tracker.fwsc"
    if not pkg_src.exists():
        raise SystemExit(f"FM1Tracker build: expected package missing: {pkg_src}")
    pkg_dst = out / pkg_name
    pkg_src.replace(pkg_dst)
    return pkg_dst


def main():
    release, forwarded = parse_release(sys.argv)

    # Keep the package identity fixed across releases. The human-readable
    # version remains APP_VER and the release package filename.
    dinghy_build.PRODUCT = PRODUCT
    dinghy_build.VERSION = release
    dinghy_build.CFLAGS = [*dinghy_build.CFLAGS, f'-DAPP_NAME="{APP_NAME}"']

    # Dinghy's main() only derives a version-dependent identity when it sees
    # --release, so remove that option after applying VERSION above.
    sys.argv = forwarded
    rc = dinghy_build.main()
    if rc:
        return rc

    pkg = rename_artifacts(release)
    app = dinghy_build.OUT / "fm1tracker.bin"
    print(f"FM1Tracker app      {app}")
    print(f"FM1Tracker package  {pkg}, identity {PRODUCT}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
