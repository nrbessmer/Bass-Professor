cmake_minimum_required(VERSION 3.22)
project(BassProfessor VERSION 1.9.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)

# JUCE_PATH can be set to a pre-cloned local copy. This avoids git
# safe.directory issues when building on external drives, and lets you point at
# whatever JUCE checkout you already use for your other plugins.
if(NOT DEFINED JUCE_PATH)
  set(JUCE_PATH "${CMAKE_SOURCE_DIR}/juce" CACHE PATH "Path to JUCE")
endif()

if(NOT EXISTS "${JUCE_PATH}/CMakeLists.txt")
  message(FATAL_ERROR
    "JUCE not found at ${JUCE_PATH}\n"
    "Clone JUCE (matching your other projects) e.g.:\n"
    "  git clone https://github.com/juce-framework/JUCE.git --branch 8.0.7 ${JUCE_PATH}\n"
    "or configure with -DJUCE_PATH=/path/to/JUCE")
endif()

add_subdirectory("${JUCE_PATH}" juce_build)

juce_add_plugin(BassProfessor
    COMPANY_NAME            "Tully EDM Vibe"
    COMPANY_COPYRIGHT       "(c)(p) 2026 Tully EDM Vibe"
    COMPANY_WEBSITE         "www.tullyedmvibe.com"
    PLUGIN_MANUFACTURER_CODE Tull
    PLUGIN_CODE             Bpro
    FORMATS                 AU VST3 Standalone
    PRODUCT_NAME            "Bass Professor"
    BUNDLE_ID               com.TullyEDMVibe.BassProfessor
    IS_SYNTH                TRUE
    NEEDS_MIDI_INPUT        TRUE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES         "Instrument|Synth"
    AU_MAIN_TYPE            kAudioUnitType_MusicDevice
)

target_sources(BassProfessor PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SynthVoice.cpp
)

# Expose Source/ so headers resolve cleanly.
target_include_directories(BassProfessor PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
)

target_compile_definitions(BassProfessor PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

# Noisy-but-harmless warnings, matching your Ancient Synth setup.
target_compile_options(BassProfessor PRIVATE
    -Wno-sign-conversion
    -Wno-conversion
    -Wno-shorten-64-to-32
    -Wno-switch-enum
    -Wno-float-equal
    -Wno-shadow
    -Wno-unused-parameter
    -Wno-pedantic
    -Wno-deprecated
)

target_link_libraries(BassProfessor PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)
