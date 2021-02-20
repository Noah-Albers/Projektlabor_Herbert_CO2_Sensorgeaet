#pragma once

class Timer{
    private:
        // Since when the timer has been running
        long since;
    public:
        Timer();

        // Resets the timer to restart
        void reset();

        // Return if the timer has reached the given millis
        bool hasReached(long time);

        // Return how long the timer has been running
        long getRunTime();
};