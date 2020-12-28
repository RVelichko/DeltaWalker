/*!
 * \brief  Приведение числа к ближайшему целому числу из ряда Фибоначчи.
 * \author R.N.Velichko rostislav.vel@gmail.com
 * \date   17.08.2016
 */

#include <algorithm>

#include "FibonacciReduction.hpp"

using namespace utils;


FibonacciReduction::FibonacciReduction() {
    _fnums.push_back(0);
    _fnums.push_back(1);
    // сумма (0 + 1) - является избыточной для задачи приведения
    _fnums.push_back(2);
}


size_t FibonacciReduction::reduction(size_t in) {
    auto it = (--_fnums.end());
    // Добавить числа если входное значение превосходит последнее вычесленное
    if (*it < in) {
        do {
            _fnums.push_back(*it + *(--it));
            it = (--_fnums.end());
        } while (*it < in);
    } else {
        // Найти число Фи, ближайшее большее входного числа
        it = find_if(_fnums.begin(), _fnums.end(), [in](size_t f) { return in < f; });
    }
    // Запомнить верхнее число Фи относительно переданного
    size_t res = *it;
    // Определить границу приведения к ближайшему Фи
    double k = (static_cast<double>(res) + static_cast<double>(*(--it))) / 2.0;
    // Если ближе к меньшему Фи - выбрать меньшее
    if (static_cast<double>(in) < k) {
        res = *it;
    }
    return res;
}
