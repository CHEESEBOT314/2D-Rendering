#ifndef MSCFINALPROJECT_GAME_HPP
#define MSCFINALPROJECT_GAME_HPP

namespace Game {
    void init();

    void render();
    void handleEvent();
    void update();

    bool shouldQuit();

    void terminate();
}

#endif//MSCFINALPROJECT_GAME_HPP
