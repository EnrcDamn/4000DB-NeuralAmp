#add_subdirectory(headless)

target_sources(${PROJECT_NAME} PRIVATE
	PluginEditor.cpp
	PluginEditor.h
	PluginProcessor.cpp
	PluginProcessor.h
	RTNN.cpp
	RTNN.h
	Diagnostics/ProcessorDiagnostics.cpp
	Diagnostics/ProcessorDiagnostics.h
	TransparentLookAndFeel.cpp
	TransparentLookAndFeel.h
)
