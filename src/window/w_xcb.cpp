#include "w_window.hpp"

namespace crow::window
{
    struct Window::impl
    {
        std::shared_ptr<xcb_connection_t> connection = nullptr;
        
        xcb_screen_t screen;
        xcb_window_t window;
        xcb_gcontext_t gcontext;
    };

    Window::Window() : pImpl(new impl), state(State::EXIT)
    {
        pImpl->connection = std::shared_ptr<xcb_connection_t>(xcb_connect(nullptr, nullptr), [](xcb_connection_t* c){ xcb_disconnect(c); });
        if (xcb_connection_has_error(pImpl->connection.get())) {
            LOG(FATAL) << "Open display failed" << std::endl;
            exit(1);
        }
        DLOG(INFO) << "Open display success" << std::endl;

        pImpl->screen = *xcb_setup_roots_iterator(xcb_get_setup(pImpl->connection.get())).data;
        pImpl->window = xcb_generate_id (pImpl->connection.get());

        {
            uint32_t mask;
            uint32_t values[2];

            mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
            values[0] = pImpl->screen.white_pixel;
            values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

            xcb_create_window(pImpl->connection.get(), XCB_COPY_FROM_PARENT, pImpl->window, pImpl->screen.root, 0, 0, 1600, 450, 1, XCB_WINDOW_CLASS_INPUT_OUTPUT, pImpl->screen.root_visual, mask, values);
        }

        {
            uint32_t mask;
            uint32_t values[2];
            
            mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
            values[0] = pImpl->screen.black_pixel;
            values[1] = 0;

            pImpl->gcontext = xcb_generate_id(pImpl->connection.get());
            xcb_create_gc(pImpl->connection.get(), pImpl->gcontext, pImpl->window, mask, values);
        }

        xcb_map_window(pImpl->connection.get(), pImpl->window);
        xcb_flush(pImpl->connection.get());

        state = State::LOOP;
        std::shared_ptr<xcb_generic_event_t> event;
        xcb_rectangle_t rect = { 0, 0, 1600, 450 };
        while (state == State::LOOP && (event = std::shared_ptr<xcb_generic_event_t>(xcb_wait_for_event(pImpl->connection.get())))) {
            switch (event->response_type) {
                case XCB_EXPOSE:
                    DLOG(INFO) << "EXPOSE" << std::endl;
                    xcb_poly_fill_rectangle(pImpl->connection.get(), pImpl->window, pImpl->gcontext, 1, &rect);
                    xcb_flush(pImpl->connection.get());
                    break;

                case XCB_KEY_PRESS:
                    State::EXIT;
                    break;
            }
            event.reset();
        }
    }

    Window::~Window()
    {
        pImpl->connection.reset();
    }
}
