# Dinghy Import Manifest

This manifest defines the **exact** Dinghy baseline files to import for FM1Tracker Phase 1.

Upstream repository: `https://github.com/zvenson/dinghy`  
Pinned commit: `d44df7db09b98ccad2f1233b8f83f5f09c04829e`  
Import policy: copy verbatim in WP1.2; do not rename/modify behavior until pristine verification in WP1.3.

The blob SHA values below are the upstream Git blob IDs at the pinned commit and provide a review target for the import.

---

## Root/build files — import

| Path | Upstream blob | WP1.2 action |
|---|---|---|
| `.gitignore` | `048042bb0b28e625f6a50df3a498f467e29d35ee` | copy verbatim unless project ignore rules already strictly supersede it |
| `LICENSE` | `f288702d2fa16d3cdf0035b15a9fcbc552cd88e7` | copy verbatim; GPL-3.0-only full text |
| `build.sh` | `753a1f32457d4e9e1e915aa8f5791bfbe27c21df` | copy verbatim |

Upstream `LICENSING.md` (`47e02bbf4278edbc6435ca7ab9261edffb895514`) is not used as FM1Tracker's project licensing file because it describes Dinghy itself. Its provenance/credits are retained in `THIRD_PARTY.md` and `docs/UPSTREAM_DINGHY.md`; its content remains available at the pinned upstream commit.

---

## Font license/material — import

| Path | Upstream blob | Action |
|---|---|---|
| `assets/fonts/Terminus-LICENSE.txt` | `d65472b9ec0421b5be370847a54a6fcfb2b25150` | copy verbatim |
| `firmware/gen/felucca_font.h` | `e11f708bd5ee9bfac53736b626e0e95fc478bd31` | copy verbatim |

The generated font header is required by the display code. The OFL license travels with it.

---

## Firmware root — import verbatim

| Path | Upstream blob |
|---|---|
| `firmware/app.ld` | `3e142e0708461ab9860ee1d73a62bf6fa23ab53d` |
| `firmware/crt0.S` | `5176548c94e58be9dd6f09327a5309686281d6b2` |

---

## Firmware HAL — import verbatim

| Path | Upstream blob |
|---|---|
| `firmware/hal/fm1_adc.h` | `19b253640c2aebe24dfb6acc37fe53d5596e54b4` |
| `firmware/hal/fm1_audio.h` | `02e2cc32dd1cd9f00aad1e92fc87cb45771bc40a` |
| `firmware/hal/fm1_cc.h` | `948e81dfbb32b15cd4c1bdfed9d44d1e290a5f0c` |
| `firmware/hal/fm1_flash.h` | `18ec13e1db7e6b747a5f8d57859eccc20e48218b` |
| `firmware/hal/fm1_gpio.h` | `cb7db8cc0cacf9878d997cfb00d6fe38780f5e63` |
| `firmware/hal/fm1_guard.h` | `8231e314aedfb122936d89d4aab481cca28004a1` |
| `firmware/hal/fm1_input.h` | `b278122651c9e7bcae1c757839700693c551eee9` |
| `firmware/hal/fm1_irq.h` | `f76f3f6a94ca34804dd809cab8cf07fff96ee12e` |
| `firmware/hal/fm1_isr.S` | `7bf35833e90f8f2429e614eedfc2348b914f5def` |
| `firmware/hal/fm1_lcd_hw.h` | `72e0f2db0ac1767f88fd1488144710d27bb4fe6b` |
| `firmware/hal/fm1_sys.h` | `f1bccda83f1572b5e107f9109e28a6066c4943b9` |
| `firmware/hal/fm1_time.h` | `d62920d5a72ed30e4876ea39af87db0a86ce98aa` |
| `firmware/hal/fm1_timer.h` | `c20e10fee980481b6292782e755f95ddcee40e10` |
| `firmware/hal/fm1_uart.h` | `5d5ef83dad665f239df83f5f09a9eb89705d3d9` |
| `firmware/hal/fm1_usb.h` | `392c768223578ef698e44a6a2e5d9b043968df35` |
| `firmware/hal/fm1_vec.S` | `a0a76430bfd8d8c047a92fe7a64af85e2ea3cf46` |
| `firmware/hal/fm1_xip.h` | `45830c04bb8463ee27d40ccda2d5317757723872` |

> Note: before WP1.2 closes, verify every copied HAL file blob against the upstream path because a manifest typo is possible and the upstream commit is the ultimate source of truth.

---

## Firmware loader — import verbatim

| Path | Upstream blob |
|---|---|
| `firmware/loader/crt0_ldr.S` | `3d8876630b66d30451525cf8f09a9eb89705d3d9` |
| `firmware/loader/ldr_core.c` | `e69d430504683757f7688b2c4d954562b25d89df` |
| `firmware/loader/loader.c` | `ad1ffedaa07ee6d8833a5984a79623871bf964a2` |
| `firmware/loader/loader.ld` | `acadd26f556107d8e43125bc929278db2e31cca3` |

---

## Firmware source — import verbatim for pristine baseline

| Path | Upstream blob | Later disposition |
|---|---|---|
| `firmware/src/bootguard.h` | `193d985e72b530d553f77af613d066bc03529991` | keep platform-like |
| `firmware/src/dinghy.c` | `346e268910167ff360d0acb363c2b6ce19bd7abb` | import unchanged; rename/replace only after WP1.3 |
| `firmware/src/gfx.c` | `05bbfd63b2876ca4d2dfddac34ceeb3a3c4d7314` | keep platform-like |
| `firmware/src/lcd.c` | `a337a3cbc4c81a7faa1cd1b6705b62500e1f3bdb` | keep platform-like |
| `firmware/src/libc.c` | `3a5637f4ca09eeced805d309be17f92ead99b0f7` | keep platform-like |
| `firmware/src/ota.c` | `9dbfb303686527fb298def54e6e76bdc8c634aad` | keep platform-like |
| `firmware/src/platform.c` | `70c8af33d5adade24c5b6455aa413f52e17950e3` | keep platform-like |
| `firmware/src/recovery.c` | `977ae21cfb8039e76c4ec3c458b513f2c25670d4` | keep platform-like |
| `firmware/src/uac_fir.h` | `7e86c6bbc5650fee2f03e7479116e87c3ef14580` | keep platform-like |
| `firmware/src/usb.c` | `0061db827a38ecf0dc19fc180022c8b1ee413b30` | keep platform-like |

WP1.2 must preserve source headers and behavior. The first intentional app-level changes belong to WP1.4 after pristine verification.

---

## Host tests — import verbatim

| Path | Upstream blob |
|---|---|
| `tests/host.c` | `df2280e975ff3bea7a80bbf7e4db4ce56dc0665c` |
| `tests/run_host.sh` | `52cd7df588a0a48714b4fc3cc72ce58d96e63c74` |

At the pinned baseline, `tests/run_host.sh` compiles `firmware/src/dinghy.c` directly in host mode and produces `build/host/dinghy.wav` plus screen PNG output.

---

## Build/install tooling — import verbatim

| Path | Upstream blob |
|---|---|
| `tools/build.py` | `2a8e4b871cb049ee410464790b99457db503c869` |
| `tools/fm1_install.py` | `abd675aca75806f66e057166ddae9f205205132f` |
| `tools/fm1pkg_make.py` | `efdbdffd1a6da1fc5363b82920fc47204ebcdac2` |
| `tools/get_toolchain.sh` | `ff2eed500ed4604526a57a0e394b5063096437da` |
| `tools/lz4blk.py` | `43fbdaaa0d5c4ac1d321464d77fec702ca983818` |

These files remain Dinghy-named during the pristine baseline. Product/artifact renaming belongs to WP1.4.

---

## Required external SDK inputs — do **not** vendor

Expected SDK: `AC79NN_SDK_V1.2.1_2023-12-13`

The build resolves SDK files under:

`$AC79_SDK/cpu/wl82/tools/`

Reference files/hashes checked by pinned `tools/build.py`:

| Relative SDK path | SHA-256 |
|---|---|
| `uboot.boot` | `4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4` |
| `cfg_tool.bin` | `276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b` |
| `cfg/eq_cfg_hw.bin` | `41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d` |

These are read from the user's separately obtained JieLi SDK checkout at build time. They are not copied into FM1Tracker source control.

The build also requires the JieLi `pi32v2` toolchain, expected through `JIELI_TOOLCHAIN` (or the helper script) and Python/Pillow. Non-Linux x86-64 systems may use Dinghy's Docker path.

---

## Explicitly do not import

| Upstream path | Reason |
|---|---|
| `assets/logo.svg` | Dinghy/designburg branding; upstream licensing says not under GPL |
| `assets/logo.png` | same |
| `docs/screen.png` | Dinghy product screenshot/branding, not needed for build |
| upstream `README.md` | Dinghy product documentation, not FM1Tracker documentation |
| upstream `docs/MAKE-IT-YOURS.md` | reference via pinned upstream; FM1Tracker has its own workflow |
| upstream `docs/API.md` | reference via pinned upstream; do not duplicate stale docs unnecessarily |
| upstream `docs/RULES.md` | reference via pinned upstream; key constraints are already frozen in FM1Tracker docs |

No Furnace source/assets are part of this import.

---

## Verification procedure for WP1.2

After the files are copied:

1. Compare imported paths against this manifest.
2. Confirm imported verbatim file contents match the pinned upstream versions before modifications.
3. Confirm Dinghy logo files are absent.
4. Confirm `LICENSE` and Terminus OFL text are present.
5. Confirm source copyright/SPDX headers remain intact.
6. Do not modify `dinghy.c`, `build.py`, product IDs or output names yet.
7. Update `THIRD_PARTY.md` status from `baseline import in progress` to `baseline imported, pristine verification pending`.
8. Move to WP1.3 only after the import itself is reviewable.

## Why this is intentionally strict

Phase 1 must separate three questions:

1. Did we copy the known-good platform correctly?
2. Does that pristine copy still build?
3. Do our FM1Tracker renames still build?

Combining those into one commit would make later failures ambiguous.
