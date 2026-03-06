// Copyright 2026 Beisent
// TimeStep class for stable frame timing

#ifndef FLUX_CORE_SRC_TIMESTEP_HPP_
#define FLUX_CORE_SRC_TIMESTEP_HPP_

namespace flux
{

    class TimeStep
    {
    public:
        constexpr TimeStep(float time = 0.0f) : time_(time) {}

        constexpr operator float() const { return time_; }

        [[nodiscard]] constexpr float GetSeconds() const { return time_; }
        [[nodiscard]] constexpr float GetMilliseconds() const { return time_ * 1000.0f; }

    private:
        float time_;
    };

} // namespace flux

#endif // FLUX_CORE_SRC_TIMESTEP_HPP_
