#include "window/w_window.hpp"

int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    auto window = std::make_unique<crow::window::Window>();
    return 0;
}
