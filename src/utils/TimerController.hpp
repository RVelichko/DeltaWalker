/**
 * \brief  Однопоточный контроллер таймаутов.
 * \author R.N.Velichko rostislav.vel@gmail.com
 * \date   17.09.2018
 */

#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>


namespace chr = std::chrono;
namespace utils {

/**
 * \brief Структура, хранящая вызов метода подписчика.
 */
struct TimerSubscriber {
    size_t id;       ///< Идентификатор таймаута.
    size_t timeout;  ///< Таймаут в милисекундах.
    bool is_restart;  ///< Флаг указывающий на повторный запуск таймаута.

    TimerSubscriber(size_t timeout, bool is_restart = false);

    /**
     * \brief Вызывается при завершении интервала таймаута.
     */
    virtual void complete() = 0;

    /**
     * \brief Оператор вызывается внутри системы контроля таймаутов.
     */
    void operator()();

    /**
     * \brief Метод возвращает строку с основными значениями подписчика таймера.
     */
    virtual std::string toString();
};


typedef std::shared_ptr<TimerSubscriber> PTimerSubscriber;


class TimerController {
private:
    /**
     * \brief Структура содержит информацию о подписчике таймаута.
     */
    struct TimePointSubscriber {
        chr::steady_clock::time_point start;  ///< Время запуска таймаута.
        PTimerSubscriber sub;                 ///< Подписчик таймаута.
    };

    typedef std::vector<TimePointSubscriber> TimePoints;
    typedef std::map<size_t, TimePointSubscriber> Subscribers;
    typedef std::unique_ptr<std::thread, std::function<void(std::thread*)>> PThread;

    size_t _counter;  ///< Переменная для выделения идентификаторов подписчиков.
    Subscribers _subs;  ///< Подписчики на контроль по таймеру.
    PThread _thread;  ///< Поток обслуживания подписчиков таймаутов.
    std::atomic_bool _is_run;  ///< Потокобезопасный флаг состояния работы потока.
    std::mutex _mtx;  ///< Объект синхронизации.

    /**
     * \brief Выполняет цикл обслуживания подписчиков.
     */
    void run();

public:
    TimerController();
    virtual ~TimerController();

    /**
     * \brief Запуск потока контролера таймаутов.
     */
    void start();

    /**
     * \brief Остановка потока контроллера таймаутов.
     */
    void stop();

    /**
     * \brief Удаление всех подписчиков.
     */
    void clear();

    /**
     * \brief Добавляет нового подписчика.
     * \param sub Подписцик на определённый таймаут.
     * \return Идентификатор добавленного подписчика при успешном добавлении, 0 - в противном
     * случае.
     */
    size_t add(const PTimerSubscriber& sub);

    /**
     * \brief Сбрасывает начальную точку отсчёта для подписчика.
     * \param sub_id Идентификатор подписчика, выданный при добавлении.
     * \return TREU - при успешном перезапуске, FALSE - в противном случае.
     */
    bool reset(size_t sub_id);

    /**
     * \brief Принудительно удаляет подписчика из потока обслуживания.
     * \param sub_id Идентификатор подписчика, выданный при добавлении.
     * \return TREU - при успешном удалении, FALSE - в противном случае.
     */
    bool remove(size_t sub_id);
};
}  // namespace utils
