cmake_minimum_required(VERSION 3.25)

# This tells cmake we have goodies in the /cmake folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake_project")
include (PamplejuceVersion)

# Modern concise way to add dependencies to your project
include (CPM)

# Configures universal binaries and decides which version of macOS to support
include(PamplejuceMacOS)

# Couple tweaks that IMO should be JUCE defaults
include(JUCEDefaults)

set(PROJECT_NAME "HarmonicSequencer")
set(PRODUCT_NAME "Harmonic Sequencer")
set(COMPANY_NAME "ofsound")
set(BUNDLE_ID "com.ofsound.harmonicsequencer")

# Valid choices: AAX Unity VST VST3 AU AUv3 Standalone
set(FORMATS Standalone AU VST3 AUv3)

# Force JUCE to work with sccache
# instruct MSVC to embed debug info (/Z7) instead of emitting a .pdb
set(CMAKE_POLICY_DEFAULT_CMP0141        NEW      CACHE STRING "" FORCE)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded CACHE STRING "" FORCE)

# For simplicity, the name of the CMake project is also the name of the target
project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

# Log build environment info (after project() so compiler info is available)
include(PamplejuceLog)

# JUCE is setup as a submodule in the /JUCE folder
# Locally, you must run `git submodule update --init --recursive` once
# and later `git submodule update --remote --merge` to keep it up to date
# On Github Actions, this is done as a part of actions/checkout
add_subdirectory(JUCE)

# Add CLAP format
add_subdirectory(modules/clap-juce-extensions EXCLUDE_FROM_ALL)

# See `docs/CMake API.md` in the JUCE repo for all config options
juce_add_plugin("${PROJECT_NAME}"
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/packaging/icon.png"

    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "${BUNDLE_ID}"

    # On MacOS, plugin is copied to /Users/yourname/Library/Audio/Plug-Ins/
    COPY_PLUGIN_AFTER_BUILD TRUE

    NEEDS_WEB_BROWSER TRUE
    NEEDS_WEBVIEW2 TRUE

    PLUGIN_MANUFACTURER_CODE Osnd
    PLUGIN_CODE HSeq
    FORMATS "${FORMATS}"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    VST3_CATEGORIES Fx Instrument

    PRODUCT_NAME "${PRODUCT_NAME}")

# This lets us use our code in both the JUCE targets and our Test target
# Without running into ODR violations
add_library(SharedCode INTERFACE)

clap_juce_extensions_plugin(TARGET "${PROJECT_NAME}"
    CLAP_ID "${BUNDLE_ID}"
    CLAP_FEATURES midi-effect)

# Enable fast math, C++20 and a few other target defaults
include(SharedCodeDefaults)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h")
target_sources(SharedCode INTERFACE ${SourceFiles})

include(WebViewUI)

# Adds a BinaryData target for embedding assets into the binary
include(Assets)

if (TARGET WebViewUIBuild)
    add_dependencies(Assets WebViewUIBuild)
endif()

# MacOS only: Cleans up folder and target organization on Xcode.
include(XcodePrettify)

target_compile_definitions(SharedCode
    INTERFACE

    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=1
    JUCE_VST3_CAN_REPLACE_VST2=0

    # Uncomment if you are paying for a an Indie/Pro license or releasing under GPLv3
    # JUCE_DISPLAY_SPLASH_SCREEN=0

    CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    VERSION="${CURRENT_VERSION}"

    PRODUCT_NAME_WITHOUT_VERSION="Harmonic Sequencer"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(SharedCode INTERFACE HARMONIC_SEQUENCER_UI_DEV_SERVER=1)
endif()

target_link_libraries(SharedCode
    INTERFACE
    Assets
    juce_audio_utils
    juce_audio_processors
    juce_dsp
    juce_gui_basics
    juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

target_link_libraries("${PROJECT_NAME}" PRIVATE SharedCode)

# IPP support (SIMD DSP) — not needed for MIDI CC sequencing
# include(PamplejuceIPP)

include(Tests)

option(HARMONIC_SEQUENCER_WARNINGS_AS_ERRORS "Treat warnings in owned targets as errors" ON)

if (HARMONIC_SEQUENCER_WARNINGS_AS_ERRORS)
    if (MSVC)
        target_compile_options(SharedCode INTERFACE /WX)
        target_compile_options(Tests PRIVATE /WX)
    else()
        target_compile_options(SharedCode INTERFACE -Werror)
        target_compile_options(Tests PRIVATE -Werror)
    endif()
endif()

include(Benchmarks)

include(GitHubENV)
