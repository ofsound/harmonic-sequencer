# Harmonic Sequencer

A JUCE MIDI effect plugin by [ofsound](https://github.com/sudara/pamplejuce) stack — CMake, C++23, Catch2, Svelte WebView UI, CLAP/AU/VST3/AUv3/Standalone.

**Focus:** high-performance **MIDI CC** sequencing and automation to the host. (Sibling project **MIDI Phrases** handles note phrase sequencing.)

## Quick start

```bash
git submodule update --init --recursive
cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build Builds
./Builds/Tests
```

See `CLAUDE.md` / `AGENTS.md` for WebView dev server, CI, and agent workflows.

## Template lineage

Derived from [Pamplejuce](https://github.com/sudara/pamplejuce). [Pamplejuce docs](https://melatonin.dev/manuals/pamplejuce/).
