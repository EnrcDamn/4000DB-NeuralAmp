#include "ProcessorDiagnostics.h"

#if DIAGNOSTICS_ENABLED

#include <string>

namespace
{
    diagnostics::Config diagnosticsConfig()
    {
        diagnostics::Config config;
        config.application = JucePlugin_Name;
        config.scopes = { "processBlock", "controls", "RTNN::processBlock", "outputMeter" };
        config.values = { "sample_rate", "block_size", "channels" };
        return config;
    }

    diagnostics::BlockInfo blockInfo(double sampleRate, int samples, int channels) noexcept
    {
        diagnostics::BlockInfo info;
        info.budgetMs = sampleRate > 0.0 ? 1000.0 * samples / sampleRate : 0.0;
        info.values[0] = sampleRate;
        info.values[1] = samples;
        info.values[2] = channels;
        return info;
    }
}

ProcessorDiagnostics::ProcessorDiagnostics()
    : m_session(diagnosticsConfig())
{
}

void ProcessorDiagnostics::modelsLoaded(const std::array<RTNN, 2>& models)
{
    bool ready = true;
    for (std::size_t channel = 0; channel < models.size(); ++channel)
    {
        if (!models[channel].isLoaded())
        {
            ready = false;
            m_session.log("Model load failed (channel " + std::to_string(channel + 1)
                          + "): " + models[channel].getLastError(), "error");
        }
    }

    if (ready)
    {
        m_session.log("Neural models loaded");
    }
}

void ProcessorDiagnostics::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    m_sampleRate = sampleRate;
    m_session.log("prepareToPlay: " + std::to_string(sampleRate) + " Hz, "
                  + std::to_string(samplesPerBlock) + " samples");
}

void ProcessorDiagnostics::releaseResources()
{
    m_session.log("releaseResources");
}

ProcessorDiagnostics::Block ProcessorDiagnostics::beginBlock(int samples, int channels) noexcept
{
    return { *this, samples, channels };
}

ProcessorDiagnostics::Block::Block(ProcessorDiagnostics& owner, int samples, int channels) noexcept
    : m_profile(owner.m_session, blockInfo(owner.m_sampleRate, samples, channels))
{
}

diagnostics::Scope ProcessorDiagnostics::Block::measure(ProcessorScope scope) noexcept
{
    return { m_profile, static_cast<std::size_t>(scope) };
}

#endif
