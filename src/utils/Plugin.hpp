/*!
 * \brief  Базовый класс плагина
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   12.10.2012
 */

////////////////////////////////////////////////////////////////////////////////////////////////////
// class MyPlugin
//     : public Plugin {
// public:
//     MyPlugin() {
//     }
// } my_plugin;
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <map>
#include <string>


#define OPEN_FUNC_NAME "Get"

class Config;
class Plugin;


extern "C" {
/*!
 * \brief Типы получения объекта загруженного плагина.
 */
typedef Plugin *(*GetPluginFunc)(Plugin *parent, const char *name, int argc, char **argv,
                                 char **env);

/*!
 * \brief Метод получения объекта из загруженной библиотеки и его инициализации
 * \param parent Указатель на базовый плагин
 * \param name   Имя загруженной библиотеки (полный путь)
 * \param argc   Количество аргументов
 * \param argv   Массив аргументов
 * \param env    Массив переменных окружения
 */
Plugin *Get(Plugin *parent, const char *name, int argc, char **argv, char **env);
}  // extern "C"


class Plugin {
    //! \brief Хендлер загруженного плагина.
    typedef void *Handler;

    //! \brief Типы контейнера загруженных плагинов.
    typedef std::pair<Handler, Plugin *> Lib;
    typedef std::map<std::string, Lib> Plugins;
    typedef Plugins::iterator PluginIter;
    typedef Plugins::value_type PluginValue;

    Plugins _plugins;

    /*!
     * \brief Метод инициализации загруженного плагина
     * \param parent Указатель на базовый плагин
     * \param name   Имя загруженной библиотеки (полный путь)
     * \param argc   Количество аргументов
     * \param argv   Массив аргументов
     * \param env    Массив переменных окружения
     */
    void init(Plugin *parent, const std::string &name, int argc, char **argv, char **env);

    friend class Config;

    /*!
     * \brief Метод загрузки и запуска единичного плагина
     * \param lib_path Имя плагина
     */
    void loadLibrary(const std::string &lib_path);

protected:
    Plugin *_parent;
    std::string _name;
    int _argc;
    char **_argv;
    char **_env;

public:
    static Plugin *s_plugin;

    virtual bool execute() = 0;

    Plugin();
    virtual ~Plugin();

    /*!
     * \brief Метод загрузки плагинов из указанной директории
     * \param lib_path Имя директории содержащей плагины
     */
    void load(const std::string &lib_path = ".");

    /*!
     * \brief Метод загрузки, инициализации и запуска плагина
     * \param name Имя загружзаемой библиотеки (полный путь)
     */
    Plugin *get(const std::string &name);

    /*!
     * \brief Метод выгрузки плагина
     * \param name Имя загруженной библиотеки (сохраняется в параметрах объекта плагина)
     */
    bool close(const std::string &name);

    /*!
     * \brief Метод выгрузки плагинов.
     */
    void close();

    /*!
     * \brief Метод получения текущего количества загруженных плагинов.
     */
    std::size_t size();
};
