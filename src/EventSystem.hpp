#pragma once

#include "Engine.hpp"
namespace tp
{

class EventSystem
{
public:
    EventSystem();
    static void pollEvents(tp::InputData* gameInputData);
};
} // namespace tp