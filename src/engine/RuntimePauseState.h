#pragma once

#include <entt/entt.hpp>

#include <algorithm>
#include <cstdint>
#include <typeindex>
#include <type_traits>
#include <vector>

namespace Engine {

// The enum type is part of the key, so independent callers can use the same numeric value safely.
struct RuntimePauseSource {
    std::type_index type;
    std::uint64_t value;

    bool operator==(const RuntimePauseSource&) const = default;
};

struct RuntimePauseState {
    std::vector<RuntimePauseSource> sources;
};

template <typename Reason> requires std::is_enum_v<Reason>
inline RuntimePauseSource PauseSourceKey(Reason reason)
{
    return {typeid(Reason), static_cast<std::uint64_t>(reason)};
}

inline RuntimePauseState& GetRuntimePauseState(entt::registry& registry)
{
    if (auto* state = registry.ctx().find<RuntimePauseState>()) {
        return *state;
    }

    return registry.ctx().emplace<RuntimePauseState>();
}

inline bool IsGameplayPaused(const entt::registry& registry)
{
    const auto* state = registry.ctx().find<RuntimePauseState>();
    return state && !state->sources.empty();
}

template <typename Reason> requires std::is_enum_v<Reason>
inline bool HasPauseReason(const entt::registry& registry, Reason reason)
{
    const auto* state = registry.ctx().find<RuntimePauseState>();
    return state && std::find(state->sources.begin(), state->sources.end(), PauseSourceKey(reason)) != state->sources.end();
}

// Idempotent per key, not a reference count. Independently resumable owners need distinct keys.
template <typename Reason> requires std::is_enum_v<Reason>
inline void SetPauseReason(entt::registry& registry, Reason reason, bool enabled)
{
    const auto key = PauseSourceKey(reason);
    if (enabled) {
        auto& sources = GetRuntimePauseState(registry).sources;
        if (std::find(sources.begin(), sources.end(), key) == sources.end()) {
            sources.push_back(key);
        }
    }
    else if (auto* state = registry.ctx().find<RuntimePauseState>()) {
        std::erase(state->sources, key);
    }
}

// Session reset only; individual callers should release only their own key.
inline void ClearRuntimePause(entt::registry& registry)
{
    if (auto* state = registry.ctx().find<RuntimePauseState>()) {
        state->sources.clear();
    }
}

}
