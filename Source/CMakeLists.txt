#add_subdirectory(headless)

target_sources(NeuralReelSaturator PUBLIC
	PluginEditor.cpp
	PluginEditor.h
	PluginProcessor.cpp
	PluginProcessor.h
	RTNN.cpp
	RTNN.h
)