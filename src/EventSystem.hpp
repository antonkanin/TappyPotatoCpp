#pragma once

namespace tp
{

class EventSystem
{
public:
    EventSystem();
    static bool pollEvents(struct GameGlobalData& gameGlobalData);
};
} // namespace tp