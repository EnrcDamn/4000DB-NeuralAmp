#pragma once

#include "../RTNN.h"
#include <array>
#include <cstddef>

enum class ProcessorScope : std::size_t
{
    controls = 1,
    neural,
    meter
};

#if DIAGNOSTICS_ENABLED

#include <diagnostics/Diagnostics.h>

class ProcessorDiagnostics
{
public:
    ProcessorDiagnostics();
    void modelsLoaded(const std::array<RTNN, 2>& models);
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();

    class Block
    {
    public:
        Block(ProcessorDiagnostics& owner, int samples, int channels) noexcept;
        diagnostics::Scope measure(ProcessorScope scope) noexcept;

    private:
        diagnostics::Block m_profile;
    };

    Block beginBlock(int samples, int channels) noexcept;

private:
    diagnostics::Session m_session;
    double m_sampleRate = 0.0;
};

#else

// The same calls compile away when diagnostics are disabled.
class ProcessorDiagnostics
{
public:
    void modelsLoaded(const std::array<RTNN, 2>&) const noexcept {}
    void prepareToPlay(double, int) const noexcept {}
    void releaseResources() const noexcept {}

    class Block
    {
    public:
        struct Scope {};
        Scope measure(ProcessorScope) const noexcept { return {}; }
    };

    Block beginBlock(int, int) const noexcept { return {}; }
};

#endif
