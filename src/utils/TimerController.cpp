#include <limits>

#include "Log.hpp"
#include "TimerController.hpp"

using namespace utils;

typedef std::lock_guard<std::mutex> LockQuard;


TimerSubscriber::TimerSubscriber(size_t timeout, bool is_restart)
    : id(std::numeric_limits<size_t>::max()), timeout(timeout), is_restart(is_restart) {}


void TimerSubscriber::operator()() { complete(); }


std::string TimerSubscriber::toString() {
    std::stringstream ss;
    ss << "{ \"is_restart\": " << is_restart << ", \"timeout\": " << timeout << " }";
    return ss.str();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void TimerController::run() {
    _is_run = true;
    while (_is_run) {
        // Отпустить процессор.
        std::this_thread::sleep_for(chr::milliseconds(10));
        TimePoints points;
        {  // LOCK
            LockQuard lg(_mtx);
            // Выполнить проверку подписчиков.
            for (auto iter = _subs.begin(); iter not_eq _subs.end();) {
                TimePointSubscriber& tps = iter->second;
                if (tps.sub) {
                    auto cur_time = chr::steady_clock::now();
                    auto diff = std::chrono::duration_cast<chr::milliseconds>(cur_time - tps.start);
                    auto dur_cnt =
                        static_cast<size_t>(chr::duration_cast<chr::milliseconds>(diff).count());
                    if (tps.sub->timeout <= dur_cnt) {
                        // Добавить в очередь обработки.
                        points.push_back(iter->second);
                        if (not tps.sub->is_restart) {
                            // Удалить подписчика.
                            iter = _subs.erase(iter);
                        } else {
                            // Обновить стартовую точку времени подписчика.
                            tps.start = cur_time;
                            ++iter;
                        }
                    } else {
                        ++iter;
                    }
                } else {
                    // Нулевых подписчиков необходимо удалять.
                    iter = _subs.erase(iter);
                }
            }
        }
        // Выполнить обслуживание подписчиков.
        for (auto tps : points) {
            // Вызвать оператор функции подписчика.
            PTimerSubscriber sub = tps.sub;
            (*sub)();
        }
        points.clear();
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


TimerController::TimerController() : _counter(0), _is_run(false) { LOG(TRACE); }


TimerController::~TimerController() {
    LOG(TRACE);
    stop();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void TimerController::start() {
    LOG(TRACE);
    if (not _is_run) {
        if (not _thread) {
            auto del_fn = [](std::thread* pt) {
                pt->join();
                delete pt;
            };
            _thread = PThread(new std::thread(std::bind(&TimerController::run, this)), del_fn);
        }
    } else {
        LOG(WARNING) << "Timer controller is runing. Need stop.";
    }
}


void TimerController::stop() {
    LOG(TRACE);
    if (_is_run) {
        if (_thread) {
            _is_run = false;
            _thread.reset();
        }
    } else {
        LOG(WARNING) << "Timer controller is not runing. Need start.";
    }
}


void TimerController::clear() {
    LOG(TRACE);
    LockQuard lg(_mtx);  ///< LOCK
    _subs.clear();
}


size_t TimerController::add(const PTimerSubscriber& sub) {
    ++_counter;
    auto now = chr::steady_clock::now();
    sub->id = _counter;
    TimePointSubscriber point({now, sub});
    {  ///< LOCK
        LockQuard lg(_mtx);
        _subs.insert(std::make_pair(_counter, point));
    }
    LOG(TRACE) << "subscriber " << _counter << "; " << sub->toString();
    return _counter;
}


bool TimerController::reset(size_t sub_id) {
    bool res = false;
    if (sub_id not_eq std::numeric_limits<size_t>::max()) {
        LOG(TRACE) << "subscriber id " << sub_id;
        {  ///< LOCK
            LockQuard lg(_mtx);
            auto iter = _subs.find(sub_id);
            if (iter not_eq _subs.end()) {
                iter->second.start = chr::steady_clock::now();
                res = true;
            }
        }
    }
    return res;
}


bool TimerController::remove(size_t sub_id) {
    LOG(TRACE) << "subscriber id " << sub_id;
    bool res = false;
    LockQuard lg(_mtx);
    {  ///< LOCK
        auto iter = _subs.find(sub_id);
        if (iter not_eq _subs.end()) {
            _subs.erase(iter);
            res = true;
        }
    }
    return res;
}
