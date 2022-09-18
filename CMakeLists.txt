cmake_minimum_required(VERSION 3.15)

project(NEURAL_REEL_SATURATOR VERSION 0.0.1)


# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
add_subdirectory(JUCE)                      # If you've put JUCE in a subdirectory called JUCE

# If you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.

# juce_set_vst2_sdk_path(...)
# juce_set_aax_sdk_path(...)

set(FormatsToBuild VST3)

juce_add_plugin(NeuralReelSaturator
    COPY_PLUGIN_AFTER_BUILD TRUE
    # VST3_COPY_DIR C:/Program Files/Common Files/VST3
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Mute
    FORMATS ${FormatsToBuild}
    PRODUCT_NAME "Akai4000DB-PreAmp")

# Set language version
target_compile_features (NeuralReelSaturator PRIVATE cxx_std_14)

# create a JuceHeader.h
juce_generate_juce_header(NeuralReelSaturator)

# add sources
add_subdirectory(Source)
include_directories(Source)
add_subdirectory(Resources)

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(NeuralReelSaturator PUBLIC
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries (NeuralReelSaturator PRIVATE
    # JUCE Modules
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_audio_utils
    # juce::juce_dsp

    # Recommended flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
    juce::juce_recommended_config_flags)