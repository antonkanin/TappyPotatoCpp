#pragma once

#include "Engine.hpp"
namespace tp
{

class EventSystem
{
public:
    EventSystem();
    static bool pollEvents(tp::InputData* gameInputData);
};
} // namespace tp