#include "RuntimePauseState.h"
#include <iostream>
#include <stdexcept>

namespace {
enum class OwnerA { First = 0, Second = 1 };
enum class OwnerB { First = 0 };

void Check(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}
}

int main()
{
    try {
        entt::registry registry;
        const auto& readOnly = registry;
        Check(!Engine::IsGameplayPaused(readOnly), "A fresh registry is not paused");
        Engine::SetPauseReason(registry, OwnerA::First, false);
        Check(!registry.ctx().contains<Engine::RuntimePauseState>(), "Releasing a missing key does not create state");
        Engine::SetPauseReason(registry, OwnerA::First, true);
        Engine::SetPauseReason(registry, OwnerA::First, true);
        Check(Engine::GetRuntimePauseState(registry).sources.size() == 1, "Setting a key twice is idempotent");
        Check(!Engine::HasPauseReason(readOnly, OwnerB::First), "Equal numbers in different enum types do not alias");
        Engine::SetPauseReason(registry, OwnerB::First, true);
        Engine::SetPauseReason(registry, OwnerA::Second, true);
        Check(Engine::GetRuntimePauseState(registry).sources.size() == 3, "Independent callers retain separate keys");
        Engine::SetPauseReason(registry, OwnerA::First, false);
        Engine::SetPauseReason(registry, OwnerA::First, false);
        Check(Engine::IsGameplayPaused(readOnly) && Engine::HasPauseReason(readOnly, OwnerB::First),
              "Repeated release does not resume another owner");
        Engine::SetPauseReason(registry, OwnerB::First, false);
        Check(Engine::IsGameplayPaused(readOnly) && Engine::HasPauseReason(readOnly, OwnerA::Second),
              "Different reasons in one enum remain independent");
        entt::registry other;
        Check(!Engine::IsGameplayPaused(other), "Pause ownership is registry-local");
        Engine::SetPauseReason(registry, OwnerA::Second, false);
        Check(!Engine::IsGameplayPaused(readOnly), "Releasing the final key resumes gameplay");
        Engine::SetPauseReason(registry, OwnerA::First, true);
        Engine::SetPauseReason(registry, OwnerB::First, true);
        Engine::ClearRuntimePause(registry);
        Check(!Engine::IsGameplayPaused(readOnly) && !Engine::HasPauseReason(readOnly, OwnerA::First) &&
                  !Engine::HasPauseReason(readOnly, OwnerB::First), "Session reset clears all keys");
        std::cout << "Runtime pause ownership tests passed.\n";
    }
    catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
