// Header for file L_line.cpp Written Fall 2017 at Stony Brook University
// for AMS 553 group project, project team Sayaka Ishizawa, Garnett Meise,
// Wesley Suttle. Below are declarations of train, station, and bus_schedule
// objects to be used in the simulation of the L line.

#ifndef TRAIN_HEADER
#define TRAIN_HEADER

#include <cmath>
#include "lcgrand.h"

struct train
{
    // number of passengers on the train and next station to be stopped at
    int passengers;
    int next_station;
    
    // pop all passengers from the train
    int pop_passengers()
    {
        int passengers_disembarking = passengers;
        passengers = 0;
        return passengers_disembarking;
    }
};

struct station
{
    int station_number;
    int passengers;
    float stdnormal_variate;
    int mean; // for normally distributed daily arrivals
    float std_dev;
    int seed; // for lcgrand generating interarrival times; INPUT EVEN NUMBERS ONLY
    float disembarkation_parameter; // for use in generating disembarkations
    int c; // to be used in the arrival intensity function when generating
        // arrivals in the main program's nonstationary poisson process
    float K; // also to be used in generating nonstation poisson interarrivals
    float last_arrival_time;
    float total_time_waited;
    float total_passengers_arrived;

    void __init__()
    {
        c = (int)floor(std_dev*stdnormal_variate + mean);
        K = (1/215.94)*c;
        total_time_waited = 0;
        total_passengers_arrived = 0;
    }

    int pop_passengers()
    {
        int passengers_boarding = passengers;
        passengers = 0;
        return passengers_boarding;
    }

    float disembarkations()
    {
        return disembarkation_parameter;
    }

    void bus_departure(int i)
    {
        if (passengers >= i)
        {
            passengers -= i;
        }

        else
        {
            passengers = 0;
        }
    }
};

struct bus_schedule
{
    int buses_per_arrival;
    float bus_interarrival;
    int bus_capacity;
    float cost_per_bus;
    int buses_used;
};

#endif
