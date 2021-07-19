#include <iostream>
#include <random>
#include <thread>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> mtx(_mtx);
    _condition.wait(mtx, [this]() {return !_queue.empty();} );
    T value =  std::move(_queue.back());
    _queue.pop_back();

    return value;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> mtx(_mtx);
    _queue.push_back(std::move(msg)); 
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _queue = std::make_shared<MessageQueue<TrafficLightPhase> >();
}

void TrafficLight::waitForGreen()
{
    while (true) {
        _currentPhase =  _queue->receive(); 
        if (_currentPhase == TrafficLightPhase::green) {
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    double minDuration = 4;
    double maxDuration = 6;
    auto previousTimestamp = std::chrono::system_clock::now(); 
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        auto currentTimestamp = std::chrono::system_clock::now();
        std::chrono::duration<double> delta = currentTimestamp-previousTimestamp;
        auto interval = delta.count();

        if (interval >= minDuration && interval <= maxDuration) {
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;

            auto updatedPhase = _currentPhase;
            auto ftr = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, _queue, std::move(updatedPhase));
            ftr.wait();

            previousTimestamp = std::chrono::system_clock::now(); 
        }


    }
}
