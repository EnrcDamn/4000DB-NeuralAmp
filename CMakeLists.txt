cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME NeuralAmp)

project(${PROJECT_NAME} VERSION 0.0.1)

set(DIAGNOSTICS "AUTO" CACHE STRING "Block diagnostics: AUTO (Debug only), ON or OFF")
set_property(CACHE DIAGNOSTICS PROPERTY STRINGS AUTO ON OFF)
option(NEURALAMP_COPY_PLUGIN_AFTER_BUILD "Copy the VST3 to the system plugin directory" ON)

if(DIAGNOSTICS STREQUAL "AUTO")
    set(diagnostics_enabled "$<CONFIG:Debug>")
elseif(DIAGNOSTICS STREQUAL "ON")
    set(diagnostics_enabled 1)
elseif(DIAGNOSTICS STREQUAL "OFF")
    set(diagnostics_enabled 0)
else()
    message(FATAL_ERROR "DIAGNOSTICS must be AUTO, ON or OFF")
endif()

include(FetchContent)

if(NOT DIAGNOSTICS STREQUAL "OFF")
    FetchContent_Declare(diagnostics
        GIT_REPOSITORY https://github.com/EnrcDamn/DiagnosticsViewer.git
        GIT_TAG origin/main
        GIT_REMOTE_UPDATE_STRATEGY CHECKOUT)
    FetchContent_MakeAvailable(diagnostics)
endif()

FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG origin/master
    GIT_SHALLOW TRUE
    GIT_REMOTE_UPDATE_STRATEGY CHECKOUT)
FetchContent_MakeAvailable(JUCE)

# If you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.

# Set the plugin formats you'll be building here.
# Valid formats: AAX Unity VST AU AUv3 Standalone
set(FORMATS VST3 Standalone)

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "enricodamiani"
    COPY_PLUGIN_AFTER_BUILD ${NEURALAMP_COPY_PLUGIN_AFTER_BUILD}
    # VST3_COPY_DIR C:/Program Files/Common Files/VST3
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Mute
    FORMATS ${FORMATS}
    PRODUCT_NAME "Akai4000DB-PreAmp")

# Set language version
target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_17)

# create a JuceHeader.h
juce_generate_juce_header(${PROJECT_NAME})

# add sources
add_subdirectory(Source)
include_directories(Source)
add_subdirectory(Models)
add_subdirectory(RTNeural)

# Add embedded resources
juce_add_binary_data(NeuralAmpAssets SOURCES Assets/akai4000db_background.png)

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

target_compile_definitions(${PROJECT_NAME} PRIVATE
    DIAGNOSTICS_ENABLED=${diagnostics_enabled})

target_link_libraries (${PROJECT_NAME} PRIVATE 
    # JUCE Modules
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_audio_utils
    $<${diagnostics_enabled}:Diagnostics::Diagnostics>
    # juce::juce_dsp

    # Recommended flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
    juce::juce_recommended_config_flags)


target_link_libraries(${PROJECT_NAME} LINK_PUBLIC RTNeural)

target_link_libraries(${PROJECT_NAME} PRIVATE NeuralAmpAssets)
target_link_libraries(${PROJECT_NAME} PRIVATE BinaryData)
