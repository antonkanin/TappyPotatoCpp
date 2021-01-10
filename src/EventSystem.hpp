#pragma once

namespace tp
{

class EventSystem
{
public:
    EventSystem();
    static bool pollEvents(struct GameGlobalState& gameGlobalState);
};
} // namespace tp