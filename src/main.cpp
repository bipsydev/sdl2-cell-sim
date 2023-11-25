#include "Game.hpp"

#include <iostream>

int main()
{
    std::cout << "Hello!\n";
    LCode::Game game;   // initialize window
    return game.run();  // run loop
}