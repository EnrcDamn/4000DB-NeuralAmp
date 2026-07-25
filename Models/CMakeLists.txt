juce_add_binary_data(BinaryData
    NAMESPACE ModelData
    HEADER_NAME ModelData.h
    SOURCES
    model_best.json
)

set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)