#pragma once
#include <cstdint>

namespace sample_player{
    extern uint32_t pos;
    extern int sample_type;
    void play(int type);
    int32_t update();
}