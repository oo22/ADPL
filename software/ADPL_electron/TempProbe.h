#ifndef TempProbe_h
#define TempProbe_h

#include "application.h"

class TempProbe {
    public:
        TempProbe(int pin);
        void read();
        float temp;
        unsigned long timeRead;
        void publish();
    private:
        int _pin;
        static constexpr int _NUMSAMPLES = 100;    // number of samples to read and average
        static constexpr int _SAMPLE_DELAY = 10;   // ms
        int _samples[_NUMSAMPLES];
        static constexpr float _THERMISTORNOMINAL = 10000.0;   // resistance (Ohm) @ 25 deg C
        static constexpr float _TEMPERATURENOMINAL = 25.0;     // temp. (C) for nominal R
        static constexpr float _SERIESRESISTOR  = 10000.0;     // value of 'other' R (Ohm)
        static constexpr float _BCOEFFICIENT = 3950.0;         // beta coefficient
};

#endif