/*!
 * \brief  Класс, реализующий выборку всех путей из переданной директории.
 * \author Величко Ростислав rnvelich@mts.ru
 * \date   28.10.2020
 */


#pragma once

#include <dirent.h>

#include <functional>
#include <iterator>
#include <memory>
#include <string>


namespace utils {

/*!
 * \brief Объект, описывающий итерирование по файлам и директориям внутри переданной директории.
 */
class PathIterator : public std::iterator<std::input_iterator_tag, const char*> {
    typedef std::shared_ptr<DIR> PDir;

    std::string _path_name;  ///< Имя диретории для итерации.
    PDir _dir;  ///< Указатель на открытыю директорию для итерирования.
    dirent* _entry = nullptr;  ///< Указатель на запись с краткой информацией о файле или диретории.

public:
    /*!
     * \brief Конструктор, описывающий END итератор.
     */
    PathIterator();

    /*!
     * \brief Инициализирующий итератор  по имени пути
     * \param path_name  Имя директории внутри которой происходит итерирование.
     */
    PathIterator(const char* path_name);

    /*!
     * \brief Коипирующий конструктор
     * \param pit  Итератор для копирования.
     */
    PathIterator(const PathIterator& pit);

    /*!
     * \brief Прединкрементный оператор.
     */
    PathIterator& operator++();

    /*!
     * \brief Постинкрементный оператор.
     */
    PathIterator operator++(int);

    /*!
     * \brief Оператор равенства текущего итератора переданному.
     */
    bool operator==(const PathIterator& rhs) const;

    /*!
     * \brief Оператор не равенства текущего итератора переданному.
     */
    bool operator!=(const PathIterator& rhs) const;

    /*!
     * \brief Оператор, возвращающий текущее имя файла или директории внутри итерируемой директории.
     */
    const char* operator*();
};
}  // namespace utils
