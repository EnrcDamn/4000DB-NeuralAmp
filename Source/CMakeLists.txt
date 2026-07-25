#add_subdirectory(headless)

target_sources(${PROJECT_NAME} PUBLIC
	PluginEditor.cpp
	PluginEditor.h
	PluginProcessor.cpp
	PluginProcessor.h
	RTNN.cpp
	RTNN.h
	TransparentLookAndFeel.cpp
	TransparentLookAndFeel.h
)
