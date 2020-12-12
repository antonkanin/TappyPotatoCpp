#include <iostream>

#include "Engine.hpp"
#include "Log.hpp"
#include "Exceptions.hpp"

#ifdef __ANDROID__
#include <SDL_main.h>
#endif

int main(int argc, char* argv[])
{
    tp::logError("Starting the game");
    try
    {
        tp::Engine engine{};
        std::cout << "Initialized" << std::endl;
        engine.run();
    }
    catch (const tp::Exception& e)
    {
        tp::logError(e.message());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
