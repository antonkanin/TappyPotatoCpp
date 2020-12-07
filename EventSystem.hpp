#pragma once

namespace tp
{
class EventSystem
{
public:
    EventSystem();
    void processEvents(bool& isActive);
};
} // namespace tp