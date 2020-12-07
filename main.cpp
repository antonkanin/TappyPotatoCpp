#include <iostream>

#include "Engine.hpp"
#include "Exceptions.hpp"

int main()
{
    try
    {
        tp::Engine engine{};
        engine.run();
        std::cout << "Initialized" << std::endl;
    }
    catch (const tp::Exception& e)
    {
        std::cerr << e.message() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
