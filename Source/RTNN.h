/*
  ==============================================================================

    RTNN.h
    Created: 6 Jul 2022 12:30:54pm
    Author:  Enrico Damiani

  ==============================================================================
*/

#pragma once

#include <../RTNeural/RTNeural/RTNeural.h>

class RT_LSTM
{
public:
    void prepareToPlay();

    void reset();

    void load_model(std::ifstream& model_json);

    void process(const float* inData, float param1, float* outData, int numSamples);

    float previousParam1 = 0.0;
    float steppedValue1 = 0.0;
    bool changedParam1 = false;

private:
    RTNeural::ModelT<float, 3, 1
        RTNeural::LSTMLayerT<float, 3, 20>,
        RTNeural::DenseT<double, 20, 1>
    > model;

    std::ifstream jsonStream("../Resources/model.json", std::ifstream::binary);
    auto model = RTNeural::json_parser::parseJson<double>(jsonStream);
    model.parseJson(jsonStream);
    
    float inArray[3] = { 0.0, 0.0, 0.0 };
};