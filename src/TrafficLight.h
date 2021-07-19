#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    void send(T &&);
    T receive();
private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mtx;
};

enum class TrafficLightPhase {
  red,
  green
};

class TrafficLight  : public TrafficObject
{
public:


    
    TrafficLight();
    

    // getters / setters

    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
	
    void cycleThroughPhases();

    std::shared_ptr<MessageQueue<TrafficLightPhase> > _queue;

    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif