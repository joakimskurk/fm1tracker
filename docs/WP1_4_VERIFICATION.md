# WP1.4 FM1Tracker Identity Verification

Date: 2026-10-23

## Scope

Verify that the pristine Dinghy-derived baseline now presents FM1Tracker identity without changing musical behavior, tracker behavior or protected platform code.

WP1.4 branch start:

`ff0a095b2384532f19724204b9059e88aac5a52c`

Verified implementation commit:

`78a8f70879a2daec1ad5f99f95fbe5cfbdd5bba3`

GitHub Actions run:

`39969032366`

Job:

`120466471126` (`identity-host-gate`)

Result: **SUCCESS**

## Package identity selection

The live Firmware Hub identity list was checked on 2026-10-23 before code changes.

Selected identity:

`FM-1_987`

Current evidence showed no `FM-1_987` entry. The fixed `FM-1_9NN` form was deliberately retained because the inherited Felucca/Dinghy installer resume path recognizes that family. Release version is therefore carried in `APP_VER` and artifact filenames rather than by changing the package identity.

The choice is recorded as D016 in `DECISIONS.md`. The live Hub list must be checked again before a public release.

## Implementation boundary

WP1.4 intentionally did **not** edit the verified application DSP/platform implementation.

CI compared the following paths against the WP1.4 start commit and reported:

`PROTECTED_BASELINE_UNCHANGED=1`

Protected unchanged paths:

- `firmware/src/dinghy.c`
- `firmware/src/platform.c`
- `firmware/hal/`
- `firmware/loader/`
- `tools/build.py`
- `tools/fm1pkg_make.py`
- `tools/fm1_install.py`

FM1Tracker identity is applied by:

- `build.sh` routing device builds through `tools/build_fm1tracker.py`;
- `tools/build_fm1tracker.py` setting `APP_NAME=FM1TRACKER`, fixed product identity `FM-1_987`, release text and final `fm1tracker*` artifact names while delegating to the verified Dinghy build driver;
- `tests/run_host.sh` compiling the same pristine app source with `APP_NAME=FM1TRACKER` and writing `fm1tracker*` host artifacts.

No tracker, STEP REC, p-lock, song or new synth-engine behavior was introduced.

## Host verification

`./tests/run_host.sh` completed successfully on Ubuntu 24.04 in GitHub Actions.

Observed baseline behavior remained the same:

- keyboard/MIDI note events;
- octave change;
- settings save;
- 4-voice sine/test-tone render;
- two screen captures.

The host executable contains `FM1TRACKER`, and the captured screen was also visually inspected after the run and shows `FM1TRACKER` as the visible app identity.

Produced artifacts:

| Artifact | Size | SHA-256 |
|---|---:|---|
| `build/host/fm1tracker.wav` | 1,145,900 bytes | `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b` |
| `build/host/fm1tracker-1000.png` | 2,122 bytes | `7f2ef1c30c3981118a4cf79a842b912a462c51a9db16ca5b13545d25a53a935c` |
| `build/host/fm1tracker-3000.png` | 2,098 bytes | `c3ff407b56fac8771e48c49acf01ac711f81c76a5f9470832a0eb82324e19300` |

The WAV contains 286,464 frames.

### Musical-behavior regression check

The WP1.4 WAV SHA-256 is **exactly identical** to the pristine WP1.3 WAV SHA-256:

`dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`

This is strong evidence that the identity work did not alter the baseline musical/audio behavior.

## Build-wrapper verification

CI syntax-checked/imported `tools/build_fm1tracker.py` and asserted:

- `PRODUCT == "FM-1_987"`;
- `APP_NAME == "FM1TRACKER"`;
- release parsing preserves `0.1` as version text while removing the inherited version-dependent product-ID behavior before delegating to Dinghy's build driver.

The original Dinghy build driver remains unchanged.

## Device firmware build status

A complete renamed device package was **not** built in WP1.4.

The project still lacks the separately required JieLi AC79 SDK checkout (`AC79NN_SDK_V1.2.1_2023-12-13`). WP1.3 already proved that the JieLi compiler/toolchain installs and executes, and isolated the remaining full-device-build dependency to that SDK checkout.

WP1.4 CI also confirmed that `build.sh` still fails explicitly when required build prerequisites are absent. Actual renamed firmware/package generation, safety checks and resource capture belong to WP1.5.

Verification level:

- current package identity collision check: verified against live Firmware Hub evidence on 2026-10-23;
- protected baseline code unchanged: verified by Git diff gate;
- FM1Tracker host compile/run: verified;
- visible host screen identity: verified;
- host audio output: verified and bit-identical to WP1.3;
- identity wrapper syntax/configuration: verified;
- complete device firmware/package build: not completed; external AC79 SDK checkout still absent;
- physical FM-1: not tested; hardware is not currently available.

## Evidence artifact

GitHub Actions uploaded artifact:

`wp1-4-identity-evidence`

Artifact ID:

`12507024467`

The uploaded ZIP SHA-256 reported by Actions was:

`f1b2a766f3154e3d6c7b3810beda363e0210f427bf5e43be09746f4a1758a13d`

The Actions artifact is temporary. The durable project record is this document plus the hashes above.

## WP1.4 exit decision

The identity-only implementation and host verification are complete. Device package execution is deliberately deferred to WP1.5, where renamed-baseline build/resource verification belongs, and remains externally blocked until an AC79 SDK checkout is available.

**WP1.4 is COMPLETE.**

The next work package is **WP1.5 - renamed baseline verification and budget capture**. Do not start Phase 2 tracker implementation before WP1.5 is closed.
