#include <iostream>

#include "Engine.hpp"
#include "Exceptions.hpp"

#ifdef __ANDROID__
#include <SDL_main.h>
#endif

int main(int argc, char* argv[])
{
    try
    {
        tp::Engine engine{};
        std::cout << "Initialized" << std::endl;
        engine.run();
    }
    catch (const tp::Exception& e)
    {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
