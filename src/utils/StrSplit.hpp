/*!
 * \brief  Класс для разбиения строки на подстроки.
 * \author Величко Ростислав rnvelich@mts.ru
 * \date   05.03.2020
 */


#pragma once

#include <string>
#include <vector>

namespace common {

/*!
 * \brief Класс для разбиения строки на подстроки.
 */
class StrSplit {
    typedef std::vector<std::string> VecStr;
    typedef VecStr::iterator Iter;

    VecStr _tokens;  ///< Массив токенов после разбиения.

public:
    /*!
     * \brief Класс для разбиения строки на подстроки.
     * \param str  Входная строка для разбиения.
     * \param delim  Строка - разделитель, по умолчанию пробел.
     */
    StrSplit(const std::string& str, const std::string& delim = " ") {
        size_t prev = 0, pos = 0;
        do {
            pos = str.find(delim, prev);
            if (pos == std::string::npos) {
                pos = str.length();
            }
            std::string token = str.substr(prev, pos - prev);
            if (!token.empty()) {
                _tokens.emplace_back(token);
            }
            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());
    }

    /*!
     * \brief Возвращает массив с подстроками.
     */
    const VecStr& tokens() { return _tokens; }

    /*!
     * \brief Опертор доступа к построке по индексу.
     */
    std::string operator[](size_t i) {
        if (i < _tokens.size()) {
            return _tokens[i];
        }
        return "";
    }
};
}  // namespace common
