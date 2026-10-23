# WP1.3 Pristine Baseline Verification

Date: 2026-10-23

## Scope

Verify the untouched Dinghy-derived baseline before any FM1Tracker identity rename or tracker/synth behavior changes.

Verified source commit:

`21cb59ee455205a8243fee381e24eda517da2c23`

GitHub Actions run:

`39958478536`

Job:

`120430163866` (`pristine-host-and-build-gate`)

Result: **SUCCESS**

## Host verification

`./tests/run_host.sh` completed successfully on Ubuntu 24.04 in GitHub Actions.

Observed host behavior included MIDI note-on/off events, settings save, audio render and two captured screen frames.

Produced artifacts:

| Artifact | Size | SHA-256 |
|---|---:|---|
| `build/host/dinghy.wav` | 1,145,900 bytes | `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b` |
| `build/host/dinghy-1000.png` | 2,039 bytes | `420dc518fb835809e4a19a283226be5b84b4f297e7754d72509b1443b7bd7cdd` |
| `build/host/dinghy-3000.png` | 2,013 bytes | `e61f498d528e2bf5a3c78fca3be0cad60532bd89ff083b84b6e24f954f4303df` |

The WAV render contained 286,464 frames.

The evidence bundle was uploaded by the run as artifact `wp1-3-pristine-evidence`, artifact ID `12503228521`. The GitHub artifact is temporary and is not the durable project record; the hashes and result above are the durable record.

## JieLi toolchain verification

Dinghy's own `tools/get_toolchain.sh` completed successfully in the CI environment.

Installed toolchain:

`jieli-linux-toolchains-20260730.1`

Compiler reported:

`clang version 4.0.1`

Therefore the JieLi toolchain itself was not the blocker in WP1.3.

## Device firmware build attempt

A pristine device build was attempted with the verified toolchain but without vendoring or fabricating the separately required JieLi AC79 SDK checkout.

Exact build failure:

`build.sh: no JieLi AC79 SDK in /home/runner/work/_temp/ac79-sdk-not-vendored`

Result: **environment-blocked by the external AC79 SDK checkout**.

This is an expected external dependency, already documented in `THIRD_PARTY.md` and `docs/DINGHY_IMPORT_MANIFEST.md`. The firmware image itself was therefore **not built or device-tested** in WP1.3.

## Verification level

- Pinned import integrity: verified byte-for-byte and Git-mode-for-Git-mode in WP1.2.
- Host build/run: verified.
- Host audio output: verified present and hashed.
- Host screen output: verified present and hashed.
- JieLi toolchain acquisition/execution: verified.
- Device firmware build: not completed because the external AC79 SDK checkout is absent.
- Physical FM-1: not tested; project owner does not currently have the hardware available.

## WP1.3 exit decision

WP1.3 exit criteria allow completion when the pristine baseline is verified or remaining criteria have a precise external dependency blocker. That condition is satisfied.

**WP1.3 is COMPLETE.**

The next work package is **WP1.4 - FM1Tracker identity rename**. It must change identity only, not musical behavior or tracker functionality.
