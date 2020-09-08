#ifndef MSCFINALPROJECT_GAME_HPP
#define MSCFINALPROJECT_GAME_HPP

namespace game {
    void init();

    void render();
    void handle_event();
    void update();

    bool should_quit();

    void terminate();
}

#endif//MSCFINALPROJECT_GAME_HPP
