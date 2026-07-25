juce_add_binary_data(BinaryData
    NAMESPACE ModelData
    HEADER_NAME ModelData.h
    SOURCES
    model_best.json
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)