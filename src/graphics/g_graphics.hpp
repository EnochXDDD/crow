#pragma once

#ifdef ANDROID
#include <android/log.h>
#else
#include <glog/logging.h>
#endif

#include <execution>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

#include <vulkan/vulkan.h>

namespace crow::graphics
{

class Environment
{
    public:
        Environment();
        ~Environment();

    private:
        struct impl;
        std::unique_ptr<impl> pImpl;
};

class Surface
{
    public:
        Surface();
        ~Surface();

    private:
        struct impl;
        std::unique_ptr<impl> pImpl;
};

} 
