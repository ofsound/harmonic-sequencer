# AGENTS.md

This file provides guidance for AI coding agents working in this repository.

## About This Project

**Harmonic Sequencer** by **ofsound** — a MIDI effect focused on sequencing and automating **MIDI CC** messages to the DAW with realtime-safe, high-performance scheduling. Unlike **MIDI Phrases** (sibling project), this plugin does not target note phrase composition first.

Built on the [Pamplejuce](https://github.com/sudara/pamplejuce) template with JUCE 8, CMake, C++23, Catch2 tests, Svelte WebView UI, and CLAP/AU/VST3/AUv3/Standalone formats.

**Hosts:** Same MIDI-effect placement guidance as MIDI Phrases — Logic AU in the **MIDI FX** slot; Ableton **VST3** on the instrument track with MIDI routing to downstream instruments.

CI is macOS-only for now; code signing is disabled until release.

## Build Commands (VS Code / Cursor)

Use the **CMake Tools** extension (`.vscode/settings.json` points at `Builds/` and Ninja).

**macOS prerequisites** (once): Xcode or Xcode Command Line Tools, Homebrew CMake ≥ 3.25, and Ninja:

```bash
xcode-select --install
brew install cmake ninja
```

```bash
git submodule update --init --recursive

cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build Builds

ctest --test-dir Builds --verbose --output-on-failure
# or: ./Builds/Tests

cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build Builds
```

On macOS for universal binary: add `-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"` to configure.

Built plugins copy to `~/Library/Audio/Plug-Ins/` when `COPY_PLUGIN_AFTER_BUILD` is on.

## WebView UI (Svelte + Vite + Tailwind)

The plugin editor is a JUCE 8 `WebBrowserComponent`. Frontend lives in `ui/`.

```bash
# Hot reload (Debug loads http://localhost:5175)
cd ui && npm install && npm run dev

# Production bundle → assets/webview/ui.zip
cd ui && npm run build
```

`cmake --build Builds` rebuilds the UI zip when `ui/` changes (requires `npm`).

JUCE JS helpers: `JUCE/modules/juce_gui_extra/native/javascript/` (Vite alias `@juce`).

## Agent workflow (rebuild every time)

After changes to `ui/`, `source/`, or `CMakeLists.txt`:

1. Reconfigure only if CMake changed: `cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Debug`
2. **Always** build all formats: `cmake --build Builds` (not only Standalone).
3. Fix compile errors before finishing.

Run `./Builds/Tests` when processor or test behavior changes.

## Project Structure

- `source/` — PluginProcessor, PluginEditor, WebViewResourceProvider
- `ui/` — Svelte/Vite/Tailwind WebView frontend
- `tests/` — Catch2 tests
- `benchmarks/` — Catch2 benchmarks
- `cmake/` — Pamplejuce CMake modules (submodule)
- `cmake_project/` — Project-specific CMake (WebViewUI.cmake)
- `modules/` — clap-juce-extensions submodule
- `JUCE/` — JUCE framework submodule
- `assets/` — Binary resources (includes `assets/webview/ui.zip` when built)

## Architecture notes

- **Audio thread:** `processBlock` must stay realtime-safe — no allocations, locks, or unbounded work. CC scheduling belongs here.
- **UI thread:** WebView native functions and parameter edits must publish to the audio thread via bounded queues or atomic snapshots (see MIDI Phrases patterns in the sibling repo when implementing).
- **SharedCode** INTERFACE library links plugin sources to plugin targets and Tests.

## Key Configuration

Edit `CMakeLists.txt` for `PROJECT_NAME`, `PRODUCT_NAME`, `COMPANY_NAME`, `BUNDLE_ID`, `FORMATS`, and plugin four-char codes.

Version is read from `VERSION` at the project root.

## Code Quality

Resolve compile warnings before considering a task complete. `HARMONIC_SEQUENCER_WARNINGS_AS_ERRORS` is ON for owned targets.

LSP/clangd may show false positives for JUCE headers — trust the actual build.

Uses `.clang-format` (Allman braces, 4-space indent).
