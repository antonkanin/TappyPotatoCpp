#include "Engine.hpp"
#include "Exceptions.hpp"
#include "Log.hpp"

#ifdef __ANDROID__
#include <SDL_main.h>
#endif

int main(int argc, char* argv[])
{
    tp::logError("Starting the game");
    try
    {
        tp::Engine engine{};
        engine.run();
    }
    catch (const tp::Exception& e)
    {
        tp::logError(e.message());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
