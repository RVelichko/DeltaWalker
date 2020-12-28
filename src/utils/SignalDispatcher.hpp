/*!
 * \brief  Диспетчер сигналов для управления подписавшимися объектами.
 * \author R.N.Velichko rostislav.vel@gmail.com
 * \date   18.10.2013
 */

#pragma once

#include <condition_variable>
#include <csignal>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>


namespace common {

/*!
 * \brief  Класс выполняющий вызов функторов, подписанных к определённому сигналу.
 */
class SignalDispatcher {
    typedef std::function<void(int)> Handle;  ///< Функтор, принимающий на идентификатор сигнала.
    typedef std::vector<Handle> Handlers;  ///< Массив Функторов.
    typedef std::condition_variable Condition;  ///< Семафор, для сигнализации о новом сигнале.
    typedef std::mutex Mutex;  ///< Объект синхронизации для применения с семафором.

    /*!
     * \brief Возвращает ститичный список вызываемых функторов.
     */
    static Handlers& getResources() {
        static Handlers resources;
        return resources;
    }

    /*!
     * \brief Возвращает ститичный объект симафора.
     */
    static Condition& getCondition() {
        static Condition wait_condition;
        return wait_condition;
    }

    /*!
     * \brief Возвращает ститичный синхронизации потоков.
     */
    static Mutex& getMutex() {
        static Mutex wait_mutex;
        return wait_mutex;
    }

    /*!
     * \brief Статичный метод, связываемый с определённый сигналом.
     *        Системные сигналы обслуживаются строго последовательно.
     *        По идентификатору сигнала в каждом функторе можно выполнить отдельные действия,
     *        а так же переинициализировать диспетчер.
     * \param sig  Идентификатор принятого сигнала.
     */
    static void onSignal(int sig) {
        getCondition().notify_all();
        for (auto res : getResources()) {
            res(sig);
        }
    }

public:
    typedef std::vector<int> Signals;  ///< Вектор обрабатываемых сигналов.

    /*!
     * \brief Конструктор принимает на вход массив с идентификаторами сигналов и связываемый
     *        с каждым сигналом массив функторов обработчиков.
     * \param sigs Массив обслуживаемых сигналов.
     * \param args Переменный набор функторов.
     */
    template <class... Args>
    SignalDispatcher(const Signals& sigs, const Args&... args) {
        getResources() = Handlers({args...});
        for (auto& s : sigs) {
            std::signal(s, &SignalDispatcher::onSignal);
        }
        std::unique_lock<Mutex> lock(getMutex());
        getCondition().wait(lock);
    }
};
}  // namespace common
