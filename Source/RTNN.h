/*
  ==============================================================================

    RTNN.h
    Created: 6 Jul 2022 12:30:54pm
    Author:  Enrico Damiani

  ==============================================================================
*/

#pragma once

#include <RTNeural.h>

#define RTNEURAL_DEFAULT_ALIGNMENT=16
#define RTNEURAL_USE_EIGEN=1


std::ifstream jsonStream("model_weights.json", std::ifstream::binary);
auto model = RTNeural::json_parser::parseJson<double>(jsonStream);