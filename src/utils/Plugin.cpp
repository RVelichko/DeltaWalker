/*!
 * \brief  Базовый класс плагина
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   12.10.2012
 */

#include <dirent.h>
#include <dlfcn.h>

#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>

#include "Plugin.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////

#define LIBRARY_EXTENSION ".so"
#define LIB_OPEN(pLibName) dlopen((pLibName), RTLD_LAZY)
#define LIB_GET_PROC(pLib, pLibName) dlsym((pLib), (pLibName))
#define LIB_ERROR dlerror()
#define LIB_CLOSE(pLib) dlclose((pLib))
#define LIB_CLOSE_RESULT(result) ((result) == 0)


namespace fs = std::filesystem;


class Config {
public:
    Config(Plugin *parent, const std::string &name, int argc, char **argv, char **env) {
        Plugin::s_plugin->init(parent, name, argc, argv, env);
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////////


Plugin *Plugin::s_plugin = nullptr;


void Plugin::init(Plugin *parent, const std::string &name, int argc, char **argv, char **env) {
    _parent = parent;
    _name = name;
    _argc = argc;
    _argv = argv;
    _env = env;
}


void Plugin::loadLibrary(const std::string &lib_path) {
    fs::path path(lib_path);
    std::string extension = path.extension().string();

    if (extension == LIBRARY_EXTENSION) {
        Plugin *plugin = get(path.string());

        if (plugin) {
            plugin->execute();
        }
    }
}


Plugin::Plugin() : _parent(NULL) { Plugin::s_plugin = this; }


Plugin::~Plugin() {}


void Plugin::load(const std::string &lib_path) {
    fs::path work_path = fs::absolute(lib_path);

    if (fs::is_directory(work_path)) {
        for (auto it = fs::directory_iterator(work_path); it != fs::directory_iterator(); ++it) {
            loadLibrary(it->path().string());
        }
    } else {
        loadLibrary(work_path.string());
    }
}


Plugin *Plugin::get(const std::string &name) {
    PluginIter it = _plugins.find(name);

    if (it not_eq _plugins.end()) {
        if (this not_eq it->second.second) {
            std::clog << "Get library: `" << name << "`\n" << std::flush;
            return it->second.second;
        }
    } else {
        auto lib = LIB_OPEN(name.c_str());

        if (lib) {
            std::clog << "Load library: `" << name << "`\n" << std::flush;

            auto get_ptr = LIB_GET_PROC(lib, "Get");
            GetPluginFunc get_func;
            std::memcpy(&get_func, &get_ptr, sizeof(get_ptr));

            if (get_func not_eq nullptr) {
                Plugin *plugin = get_func(this, name.c_str(), _argc, _argv, _env);

                if (plugin) {
                    plugin->_name = name;
                    _plugins[name] = std::make_pair(lib, plugin);
                    return plugin;
                } else {
                    std::cerr << "Error: Get Plugin in `" << name.c_str() << "`.\n" << std::flush;
                }
            } else {
                std::cerr << "Error: Get Func in `" << name.c_str() << "`." << std::endl;
            }
        } else {
            std::cerr << "Error: Load lib `" << name.c_str() << "`: " << LIB_ERROR << ".\n"
                      << std::flush;
        }
    }
    return nullptr;
}


bool Plugin::close(const std::string &name) {
    PluginIter it = _plugins.find(_name);

    if (it not_eq _plugins.end()) {
        if (LIB_CLOSE_RESULT(LIB_CLOSE(it->second.first))) {
            _plugins.erase(it);
            return true;
        }
    }
    return false;
}


void Plugin::close() {
    for (const PluginValue &value : _plugins) {
        if (not LIB_CLOSE_RESULT(LIB_CLOSE(value.second.first))) {
            std::cerr << "Can`t close library: `" << value.first << "`\n" << std::flush;
        } else {
            std::clog << "Close library: `" << value.first << "`\n" << std::flush;
        }
    }
}


std::size_t Plugin::size() { return _plugins.size(); }
////////////////////////////////////////////////////////////////////////////////////////////////////


extern "C" {

Plugin *Get(Plugin *parent, const char *name, int argc, char **argv, char **env) {
    if (Plugin::s_plugin not_eq nullptr) {
        Config(parent, name, argc, argv, env);
        return Plugin::s_plugin;
    }
    return nullptr;
}
}  // extern "C"
