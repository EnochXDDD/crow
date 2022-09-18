#pragma once

#ifdef ANDROID
#include <android/log.h>
#include <android/native_window.h>
#else
#include <glog/logging.h>
#include <xcb/xcb.h>
#endif

#include <memory>
#include <vector>

namespace crow::window
{

enum class State : int
{
    EXIT,
    LOOP
};

class Window
{
    public:
        Window();
        ~Window();
    
    private:
        struct impl;
        std::unique_ptr<impl> pImpl;

        State state = State::EXIT;
};

}
