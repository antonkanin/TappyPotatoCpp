#pragma once

namespace tp
{

enum class EventType
{
    Quit,
    Click
};

class EventSystem
{
public:
    EventSystem();
    bool pollEvents(EventType& eventType);
};
} // namespace tp