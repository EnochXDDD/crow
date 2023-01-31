#include "window/w_window.hpp"
#include "graphics/g_graphics.hpp"

int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    auto vulkanInstance = std::make_unique<crow::graphics::Environment>();
    auto window = std::make_unique<crow::window::Window>();
    return 0;
}
