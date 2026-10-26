# 271Block Secret Sauce

A JUCE-based VST3/Standalone stereo vocal micro-delay and tone-shaping effect for FL Studio.

## Features
- Independent left/right delay: **0–1000 ms**
- Independent left/right output level: **-24 to +6 dB**
- Blend control between original signal and delayed stereo layer (default 35% for a more natural vocal layer)
- Three-band tone shaping: low shelf (180 Hz), mid peak (1 kHz), high shelf (6.5 kHz), each ±12 dB
- Modern dark UI with lime accent
- Saves and restores plugin parameters with the host

## Build (Windows)
1. Install **Visual Studio 2022** with the *Desktop development with C++* workload.
2. Install **CMake** (3.22+) and **Git**.
3. Extract this folder.
4. Open PowerShell in this folder and run:

   ```powershell
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64
   cmake --build build --config Release
   ```

   CMake downloads JUCE 8.0.8 on the first configure, so an internet connection is needed.

5. The VST3 will be built under `build/271BlockSecretSauce_artefacts/Release/VST3/`.
6. Copy `271Block Secret Sauce.vst3` into your VST3 folder, commonly:
   `C:\Program Files\Common Files\VST3`
7. In FL Studio, open **Options → Manage plugins → Find installed plugins** and scan.

## Important
This package contains the **plugin source project**, not a precompiled `.vst3` binary. Build it using the steps above. The plugin is designed for stereo input/output. Use the delay values subtly (for example 10–35 ms) for vocal widening; 0–1000 ms is available for creative effects.

## Notes
The tone controls affect the combined output. The delay is a true stereo-channel delay, not a pitch-shifting doubler. Large delay times are more like an echo; short and different L/R times can create width, but mono compatibility should be checked in the mix.


## Optional: build in the cloud without installing Visual Studio
This project includes `.github/workflows/build-windows-vst3.yml`. Upload the project to a GitHub repository, then open **Actions → Build Windows VST3 → Run workflow**. When it finishes, download the artifact named `271Block-Secret-Sauce-Windows-VST3`. This requires a GitHub account and the build may need adjustments if the Windows runner or dependency download changes.
