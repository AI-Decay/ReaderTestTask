#ifndef SIMPLETIMER_HPP
#define SIMPLETIMER_HPP
#include <chrono>
class SimpleTimer
{
private:
    std::chrono::time_point<std::chrono::system_clock> start;

public:
    SimpleTimer() {};

    void Start()
    {
        start = std::chrono::system_clock::now();
    }

    void ResetTime()
    {
        start = std::chrono::system_clock::now();
    }

    size_t Duration()
    {
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

        return duration.count();
    }
};
#endif // SIMPLETIMER_HPP
