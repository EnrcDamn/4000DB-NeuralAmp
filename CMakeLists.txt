cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME NeuralReelSaturator)

project(${PROJECT_NAME} VERSION 0.0.1)


# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
add_subdirectory(JUCE)                      # If you've put JUCE in a subdirectory called JUCE

# If you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.

# Set the plugin formats you'll be building here.
# Valid formats: AAX Unity VST AU AUv3 Standalone
set(FORMATS AU VST3)

juce_add_plugin(${PROJECT_NAME}
    COPY_PLUGIN_AFTER_BUILD TRUE
    # VST3_COPY_DIR C:/Program Files/Common Files/VST3
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Mute
    FORMATS ${FORMATS}
    PRODUCT_NAME "Akai4000DB-PreAmp")

# Set language version
target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_14)

# create a JuceHeader.h
juce_generate_juce_header(${PROJECT_NAME})

# add sources
add_subdirectory(Source)
include_directories(Source)
add_subdirectory(Resources)
add_subdirectory(RTNeural)

# Build LV2 only on Linux
if(UNIX AND NOT APPLE)
    message(STATUS "Building LV2 plugin format")
    list(APPEND JUCE_FORMATS LV2)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries (${PROJECT_NAME} PRIVATE 
    # JUCE Modules
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_audio_utils
    # juce::juce_dsp

    # Recommended flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
    juce::juce_recommended_config_flags)


target_link_libraries(${PROJECT_NAME} LINK_PUBLIC RTNeural)