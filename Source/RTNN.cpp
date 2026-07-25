/*
  ==============================================================================

    RTNN.cpp
    Created: 6 Jul 2022 12:30:54pm
    Author:  Enrico Damiani

  ==============================================================================
*/

#include "RTNN.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace
{
    using Json = nlohmann::json;
    using Vector = std::vector<float>;
    using Matrix = std::vector<Vector>;

    void require(bool condition, const char* message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }            
    }

    Vector readVector(const Json& value, std::size_t expectedSize, const char* name)
    {
        require(value.is_array() && value.size() == expectedSize, name);
        return value.get<Vector>();
    }

    Matrix readMatrix(const Json& value, std::size_t expectedRows, std::size_t expectedColumns, const char* name)
    {
        require(value.is_array() && value.size() == expectedRows, name);

        auto matrix = value.get<Matrix>();
        require(std::all_of(matrix.begin(), matrix.end(), [expectedColumns](const auto& row)
        {
            return row.size() == expectedColumns;
        }), name);

        return matrix;
    }

    Matrix transpose(const Matrix& matrix)
    {
        Matrix result(matrix.front().size(), Vector(matrix.size()));

        for (std::size_t row = 0; row < matrix.size(); ++row)
        {
            for (std::size_t column = 0; column < matrix[row].size(); ++column)
            {
                result[column][row] = matrix[row][column];
            }
        }

        return result;
    }
}

bool RTNN::loadModel(const void* jsonData, std::size_t jsonSize)
{
    m_loaded = false;
    m_lastError.clear();

    try
    {
        require(jsonData != nullptr && jsonSize > 0, "Model data is empty");

        const auto* begin = static_cast<const char*> (jsonData);
        const auto json = Json::parse(begin, begin + jsonSize);

        // Two important sections in the json:
        // - model_data: contains the model architecture, such as input/output sizes and layer types
        // - state_dict: contains the model weights and biases, organized by layer and parameter name
        const auto& metadata = json.at("model_data");
        const auto& state = json.at("state_dict");

        // Check architecture metadata to ensure the JSON matches the class member m_model:
        // 2 inputs -> LSTM with 20 units -> 1 output. The 2 inputs are:
        //     - input[0] = audio sample
        //     - input[1] = gain parameter
        require(metadata.at("model").get<std::string>() == "SimpleRNN", "Expected a SimpleRNN model");
        require(metadata.at("unit_type").get<std::string>() == "LSTM", "Expected an LSTM recurrent layer");
        require(metadata.at("input_size").get<int>() == m_inputSize, "Model input size does not match RTNN");
        require(metadata.at("hidden_size").get<int>() == m_hiddenSize, "Model hidden size does not match RTNN");
        require(metadata.at("output_size").get<int>() == 1, "Expected a single model output");
        require(metadata.at("num_layers").get<int>() == 1, "Only one recurrent layer is supported");
        require(metadata.value("bias_fl", true), "The exported dense layer must contain a bias");

        // A LSTM has 4 internal gates: input, forget, cell, and output.
        constexpr auto gateSize = static_cast<std::size_t>(4 * m_hiddenSize);

        // PyTorch stores LSTM matrices as [4 * hidden size][input size] in i, f, c, o gate order.
        // RTNeural expects their transpose, [input size][4 * hidden size], in the same gate order.
        const auto inputWeights = transpose(readMatrix(
            state.at("rec.weight_ih_l0"), gateSize, m_inputSize,
            "Invalid LSTM input weights"));
        const auto recurrentWeights = transpose(readMatrix(
            state.at("rec.weight_hh_l0"), gateSize, m_hiddenSize,
            "Invalid LSTM recurrent weights"));

        auto bias = readVector(state.at("rec.bias_ih_l0"), gateSize, "Invalid LSTM input bias");
        const auto recurrentBias = readVector(state.at("rec.bias_hh_l0"), gateSize, "Invalid LSTM recurrent bias");

        // PyTorch applies both bias vectors. RTNeural has one LSTM bias vector.
        for (std::size_t index = 0; index < bias.size(); ++index)
        {
            bias[index] += recurrentBias[index];
        }

        // 20 LSTM values -> 1 output sample
        auto denseWeights = readMatrix(state.at("lin.weight"), 1, m_hiddenSize, "Invalid dense weights");
        auto denseBias = readVector(state.at("lin.bias"), 1, "Invalid dense bias");

        // Copy everything into m_model
        m_model.template get<0>().setWVals (inputWeights);
        m_model.template get<0>().setUVals (recurrentWeights);
        m_model.template get<0>().setBVals (bias);
        m_model.template get<1>().setWeights (denseWeights);
        m_model.template get<1>().setBias (denseBias.data());

        // Weights are now loaded, but an LSTM also contains changing internal state.
        // Resetting makes its hidden and cell states start at zero.
        m_model.reset();

        m_loaded = true;
        return true;
    }
    catch (const std::exception& exception)
    {
        m_lastError = exception.what();
        return false;
    }
}

void RTNN::reset() noexcept
{
    m_model.reset();
}

float RTNN::processSample (float audioSample, float gainParameter) noexcept
{
    if (!m_loaded)
    {
        return audioSample;
    }

    const float input[m_inputSize]{ audioSample, gainParameter };

    // The training model uses skip=1, adding the first input to the output of the LSTM/dense stack.
    return m_model.forward(input) + audioSample;
}
