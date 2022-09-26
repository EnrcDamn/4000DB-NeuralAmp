/*
  ==============================================================================

    RTNN.cpp
    Created: 6 Jul 2022 12:30:54pm
    Author:  Enrico Damiani

  ==============================================================================
*/

#include "RTNN.h"

void RT_LSTM::prepareToPlay()
{

}

void RT_LSTM::reset()
{
    modelT.reset();
}
    
void RT_LSTM::load_model()
{
    std::ifstream jsonStream("../Resources/model.json", std::ifstream::binary);

    auto model = RTNeural::json_parser::parseJson<float>(jsonStream);
    modelT.parseJson(jsonStream);
}

void RT_LSTM::process(const float* inData, float param1, float* outData, int numSamples)
{
    // Check for parameter changes for smoothing calculations
    if (param1 != previousParam1) 
    {
        steppedValue1 = (param1 - previousParam1) / numSamples;
        changedParam1 = true;
    } 
    else
        changedParam1 = false;

    for (int i = 0; i < numSamples; ++i) 
    {
        inArray[0] = inData[i];

        // Perform ramped value calculations to smooth out sound
        if (changedParam1 == true) 
            inArray[1] = previousParam1 + (i + 1) * steppedValue1;
        else 
            inArray[1] = param1;

        // Run forward pass through neural network
        outData[i] = modelT.forward(inArray) + inData[i];
    }
    previousParam1 = param1;
}