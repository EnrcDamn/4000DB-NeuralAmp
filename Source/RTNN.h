/*
  ==============================================================================

    RTNN.h
    Created: 6 Jul 2022 12:30:54pm
    Author:  Enrico Damiani

  ==============================================================================
*/

#pragma once

#include "../RTNeural/RTNeural/RTNeural.h"

#include <cstddef>
#include <string>

/**
 * Real-time wrapper for the conditioned Akai model.
 *
 * Model loading may allocate and must be done away from the audio thread.
 * processSample() and reset() do not allocate.
 */
class RTNN
{
public:
    static constexpr int m_inputSize = 2;
    static constexpr int m_hiddenSize = 20;

    bool loadModel(const void* jsonData, std::size_t jsonSize);
    void reset() noexcept;

    float processSample(float audioSample, float gainParameter) noexcept;

    // Testing utilities
    bool isLoaded() const noexcept { return m_loaded; }
    const std::string& getLastError() const noexcept { return m_lastError; }

private:
    using Model = RTNeural::ModelT<float, m_inputSize, 1,
        RTNeural::LSTMLayerT<float, m_inputSize, m_hiddenSize>,
        RTNeural::DenseT<float, m_hiddenSize, 1>>;

    Model m_model;
    bool m_loaded = false;
    std::string m_lastError;
};

