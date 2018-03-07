// Simulation of the L-line in NYC between DeKalb and Bedford stations.
// Written by Wes Suttle, Fall 2017 at Stony Brook University for AMS 553 group
// project, project team Sayaka Ishizawa, Garnett Meise, Wesley Suttle.
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <iomanip>
#include "lcgrand.h"
#include "train_header.h"


// declare the functions defined and explained at the bottom
float nonstat_poisson_interarrival(float K, float seed, float simclock);
void passenger_arrival(float *event_list, int i, station &s, float &simclock);
float multi_min(float arg[], int n);
void create_train(float *event_list, int i, float &simclock, float intertrain_time,
    station &s, std::vector<train> &t);
void train_arrival(float *event_list, int i, float &simclock, float intertrain_time,
    station &s, std::vector<train> &t1, std::vector<train> &t2);
void destroy_train(float *event_list, int i, float &simclock, float intertrain_time,
        station &s, std::vector<train> &t);
float stdnormal_rv(int seed);
float lambda(float t, float K);

int main()
{
// C is the cost per minute customer wait, primary_I and secondary_I are
// interbus arrival times for the schedules, S is the simulation start time, T is the
// simulation end time, primary_B and secondary_B are the number of buses per
// bus schedule arrival, n is the number of runs to be performed

float C, primary_I=5.0, bus_cost=30.0, S=180, T=300;
int n;

std::cout << "Cost per minute waited: "; std::cin >> C;
std::cout << "Runs per configuration: "; std::cin >> n;

int traffic_intensity_seed = 18;


// print output to a file called 'output.txt'
std::ofstream output;
output.open("output.txt");
output << " Below is simulation output to be used to determine best bus schedule.\n"
    " The cost per minute a customer has to wait is $" << C << ", and the cost"
    " per bus is $" << bus_cost << ".\n"
    " The simulation runs from 7:00AM to 9:00AM.\n\n"
    " Column format is: \n"
    " run_number, primary_buses_per_departure,\n"
    " primary_bus_interarrival, secondary_buses_per_departure,\n"
    " secondary_bus_interarrival, total_buses_used, total_minutes_waited,\n"
    " customer_total_arrivals, average_minutes_waited, total_cost \n\n";


for (int exponent=0; exponent<2; exponent++)
{
    float secondary_I = pow(2.5, exponent);

    for (int primary_B=1; primary_B<=7; primary_B++)
    {
        for (int secondary_B=0; secondary_B<=2; secondary_B++)
        {
            // generate standard normal variable to determine traffic on current day
            for (int i=1; i<=n; i++) {
                float N = stdnormal_rv(traffic_intensity_seed);

                int total_passengers = 0;

                // initialize stations.
                station DeKalb {0, 0, N, 14313, 966, 2};
                DeKalb.__init__();
                station Jefferson {1, 0, N, 3889, 229, 4};
                Jefferson.__init__();
                station Morgan {2, 0, N, 3250, 229, 6};
                Morgan.__init__();
                station Montrose {3, 0, N, 3386, 235, 8};
                Montrose.__init__();
                station Grand {4, 0, N, 8778, 485, 10};
                Grand.__init__();
                station Graham {5, 0, N, 5749, 367, 12};
                Graham.__init__();
                station Lorimer {6, 0, N, 6414, 419, 14};
                Lorimer.__init__();
                station Bedford {7, 0, N, 12055, 847, 16};
                Bedford.__init__();

                // initialize primary bus schedule running primary_B buses every
                // primary_I minutes
                bus_schedule primary_schedule {primary_B, primary_I, 93, bus_cost, 0};

                // initialize secondary bus schedule running secondary_B buses every
                // secondary_I minutes
                bus_schedule secondary_schedule {secondary_B, secondary_I, 93, bus_cost, 0};


                // set time of first train arrival at each station. Notice that the
                // difference first_train(i+1) - first_train(i) is the time (in minutes)
                // it takes for a train to travel from station i to station i+1.
                float intertrain_time = 5;
                float first_train1 = 7;
                float first_train2 = 9;
                float first_train3 = 11;
                float first_train4 = 12;
                float first_train5 = 15;
                float first_train6 = 16;
                float first_train7 = 17;


                // initialize simulation clock, counters
                float simclock = S;
                float time_limit = T;

                // initialize event_list: for 0 <= i <= 7, index i corresponds to next
                // passenger arrival at station i. for 8 <= j <= 15, index j corresponds
                // to the next train arrival at station (j-8). index 16 corresponds
                // to shuttle bus arrivals at Bedford
                float event_list[18] = {simclock, simclock, simclock, simclock, simclock,
                    simclock, simclock, simclock, simclock+intertrain_time,
                    simclock+first_train1, simclock+first_train2, simclock+first_train3,
                    simclock+first_train4, simclock+first_train5, simclock+first_train6,
                    simclock+first_train7, simclock+first_train7, simclock+first_train7};
                event_list[0] += nonstat_poisson_interarrival(DeKalb.K, DeKalb.seed, simclock);
                event_list[1] += nonstat_poisson_interarrival(Jefferson.K, Jefferson.seed, simclock) + first_train1 - intertrain_time;
                event_list[2] += nonstat_poisson_interarrival(Morgan.K, Morgan.seed, simclock) + first_train2 - intertrain_time;
                event_list[3] += nonstat_poisson_interarrival(Montrose.K, Montrose.seed, simclock) + first_train3 - intertrain_time;
                event_list[4] += nonstat_poisson_interarrival(Grand.K, Grand.seed, simclock) + first_train4 - intertrain_time;
                event_list[5] += nonstat_poisson_interarrival(Graham.K, Graham.seed, simclock) + first_train5 - intertrain_time;
                event_list[6] += nonstat_poisson_interarrival(Lorimer.K, Lorimer.seed, simclock) + first_train6 - intertrain_time;
                event_list[7] += nonstat_poisson_interarrival(Bedford.K, Bedford.seed, simclock) + first_train7 - intertrain_time;


                // create seven vectors, one to hold the trains between each consecutive
                // pair of stations.
                std::vector<train> trains01;
                std::vector<train> trains12;
                std::vector<train> trains23;
                std::vector<train> trains34;
                std::vector<train> trains45;
                std::vector<train> trains56;
                std::vector<train> trains67;


                // Run the simulation for time_limit number of minutes.
                while (simclock < time_limit)
                {
                    float min = multi_min(event_list, 18);

                    if (event_list[0] == min)
                    {
                        passenger_arrival(event_list, 0, DeKalb, simclock);
                        total_passengers += 1;
                        // std::cout << "event type " << 0 << "\n";
                    }

                    else if (event_list[1] == min)
                    {
                        passenger_arrival(event_list, 1, Jefferson, simclock);
                        total_passengers += 1;
                        // std::cout << "event type " << 1 << "\n";
                    }

                    else if (event_list[2] == min)
                    {
                        passenger_arrival(event_list, 2, Morgan, simclock);
                        total_passengers += 1;
                        // std::cout << "event type " << 2 << "\n";
                    }

                    else if (event_list[3] == min)
                    {
                        passenger_arrival(event_list, 3, Montrose, simclock);
                        total_passengers += 1;
                        // std::cout << "event type " << 3 << "\n";
                    }

                    else if (event_list[4] == min)
                    {
                        passenger_arrival(event_list, 4, Grand, simclock);
                        total_passengers += 1;
                        // std::cout << "event type " << 4 << "\n";
                    }

                    else if (event_list[5] == min)
                    {
                        passenger_arrival(event_list, 5, Graham, simclock);
                        total_passengers += 1;
                        // std::cout << "event type " << 5 << "\n";
                    }

                    else if (event_list[6] == min)
                    {
                        passenger_arrival(event_list, 6, Lorimer, simclock);
                        total_passengers += 1;
                        // std::cout << "event type " << 6 << "\n";
                    }

                    else if (event_list[7] == min)
                    {
                        passenger_arrival(event_list, 7, Bedford, simclock);
                        total_passengers += 1;
                        // std::cout << "event type " << 7 << "\n";
                    }

                    else if (event_list[8] == min)
                    {
                        create_train(event_list, 8, simclock, intertrain_time,
                            DeKalb, trains01);
                        // std::cout << "event type " << 8 << "\n";
                    }

                    else if (event_list[9] == min)
                    {
                        train_arrival(event_list, 9, simclock, intertrain_time,
                            Jefferson, trains01, trains12);
                        // std::cout << "event type " << 9 << "\n";
                    }

                    else if (event_list[10] == min)
                    {
                        train_arrival(event_list, 10, simclock, intertrain_time,
                            Morgan, trains12, trains23);
                        // std::cout << "event type " << 10 << "\n";
                    }

                    else if (event_list[11] == min)
                    {
                        train_arrival(event_list, 11, simclock, intertrain_time,
                            Montrose, trains23, trains34);
                        // std::cout << "event type " << 11 << "\n";
                    }

                    else if (event_list[12] == min)
                    {
                        train_arrival(event_list, 12, simclock, intertrain_time,
                            Grand, trains34, trains45);
                        // std::cout << "event type " << 12 << "\n";
                    }

                    else if (event_list[13] == min)
                    {
                        train_arrival(event_list, 13, simclock, intertrain_time,
                            Graham, trains45, trains56);
                        // std::cout << "event type " << 13 << "\n";
                    }

                    else if (event_list[14] == min)
                    {
                        train_arrival(event_list, 14, simclock, intertrain_time,
                            Lorimer, trains56, trains67);
                        // std::cout << "event type " << 14 << "\n";
                    }

                    else if (event_list[15] == min)
                    {
                        destroy_train(event_list, 15, simclock, intertrain_time,
                            Bedford, trains67);
                        // std::cout << "event type " << 15 << " time" << simclock << "\n";
                    }

                    // primary bus arrival
                    else if (event_list[16] == min)
                    {
                        simclock = event_list[16];
                        Bedford.total_time_waited += Bedford.passengers*(simclock
                             - std::max(Bedford.last_arrival_time, (simclock - primary_schedule.bus_interarrival)));
                        // need to update last_arrival_time here to make sure no passengers
                        // get counted twice in total_time_waited
                        Bedford.last_arrival_time = simclock;
                        Bedford.bus_departure(primary_schedule.bus_capacity * primary_schedule.buses_per_arrival);
                        event_list[16] += primary_schedule.bus_interarrival;
                        primary_schedule.buses_used += primary_schedule.buses_per_arrival;

                        // std::cout << "event type " << 16 << " time" << simclock << "\n";
                    }

                    // secondary bus arrival
                    else if (event_list[17] == min)
                    {
                        // If the primary buses just left, the secondary buses aren't needed
                        if ((event_list[16] - primary_schedule.bus_interarrival) == event_list[17])
                        {
                            simclock = event_list[17];
                            event_list[17] += secondary_schedule.bus_interarrival;
                        }

                        else
                        {
                            simclock = event_list[17];
                            Bedford.total_time_waited += Bedford.passengers*(simclock
                                 - std::max(Bedford.last_arrival_time, (simclock - secondary_schedule.bus_interarrival)));
                            // need to update last_arrival_time here to make sure no passengers
                            // get counted twice in total_time_waited
                            Bedford.last_arrival_time = simclock;
                            Bedford.bus_departure(secondary_schedule.bus_capacity * secondary_schedule.buses_per_arrival);
                            event_list[17] += secondary_schedule.bus_interarrival;
                            // std::cout << "event type " << 17 << " time" << simclock << "\n";
                            secondary_schedule.buses_used += secondary_schedule.buses_per_arrival;
                        }
                    }
                }

                // Write simulation run and statistical data to "output.txt"
                output << std::setw(3) << i << ", " << std::setw(3) << primary_schedule.buses_per_arrival << ", "
                    << std::setw(3) << primary_schedule.bus_interarrival << ", "
                    << std::setw(3) << secondary_schedule.buses_per_arrival << ", "
                    << std::setw(3) << secondary_schedule.bus_interarrival << ", "
                    << std::setw(5) << primary_schedule.buses_used + secondary_schedule.buses_used << ", "
                    << std::setw(10) << Bedford.total_time_waited << ", "
                    << std::setw(10) << Bedford.total_passengers_arrived << ", "
                    << std::setw(10) << Bedford.total_time_waited / (float)Bedford.total_passengers_arrived << ", "
                    << std::setw(10) << primary_schedule.cost_per_bus*primary_schedule.buses_used
                        + secondary_schedule.cost_per_bus*secondary_schedule.buses_used
                        + C*Bedford.total_time_waited << ",\n";
            }
        }
    }
}

    output.close();

    return 0;
}


// arrival rate function to be used in generating customer interarrival times
float lambda(float t, float K)
{
    return (t <= 270 ? K*(1/(float)24 + ((float)23/(float)24)*pow(t/270, 3)) : K*(1-(1/(float)315)*(t-270)));
}

// Generates realizations from a nonstationary poisson process with continuous
// intensity function lambda(t)
float nonstat_poisson_interarrival(float K, float seed, float simclock)
{
    float t=simclock, u1, u2, interarrival;

    do {
        u1 = lcgrand(seed);
        u2 = lcgrand(seed+1);
        t = t - (1/K)*log(u1);
    } while(u2 > lambda(t, K)/K);

    return t - simclock;
}

// for passenger arrivals to a station, this function takes event_list,
// event index, station, and simclock, then updates them all
void passenger_arrival(float *event_list, int i, station &s, float &simclock)
{
    s.total_time_waited += s.passengers*(event_list[i] - s.last_arrival_time);
    s.passengers += 1;
    s.total_passengers_arrived += 1;
    simclock = event_list[i];
    s.last_arrival_time = simclock;
    event_list[i] += nonstat_poisson_interarrival(s.K, s.seed, simclock);
}

// takes an n-vector of floats and outputs the min
float multi_min(float arg[], int n)
{
    float smallest = arg[0];
    for (int i=1; i<n; i++)
        if (smallest > arg[i])
            smallest = arg[i];
    return smallest;
}

// creates a train between DeKalb and Jefferson, loads DeKalb's waiting
// passengers to it, updates event_list and simclock
void create_train(float *event_list, int i, float &simclock, float intertrain_time,
    station &s, std::vector<train> &t)
{
    t.insert(t.begin(), train {0, 1});
    t.front().passengers = s.pop_passengers();
    simclock = event_list[i];
    event_list[i] += intertrain_time;
}

// copies the train arriving at station (i-8) from stations(i-9)(i-8) to the
// stations(i-8)(i-7), deletes the former, processes disembarkations,
// loads station (i-8) passengers onto the train, advances simclock,
// and updates event_list.
void train_arrival(float *event_list, int i, float &simclock, float intertrain_time,
    station &s, std::vector<train> &t1, std::vector<train> &t2)
{
    t2.insert(t2.begin(), t1.back());
    t2.front().next_station += 1;
    t1.pop_back();
    t2.front().passengers *= (1-s.disembarkations());
    t2.front().passengers += s.pop_passengers();
    simclock = event_list[i];
    event_list[i] += intertrain_time;
}

// deposits all passengers on the train upon arrival at station s, then
// destroys the train, advances simclock, updates event_list
void destroy_train(float *event_list, int i, float &simclock, float intertrain_time,
    station &s, std::vector<train> &t)
{
    s.passengers += t.back().passengers;
    s.total_passengers_arrived += t.back().passengers;
    t.pop_back();
    simclock = event_list[i];
    event_list[i] += intertrain_time;
}

// generates a variate from the standard normal distribution to be used in
// determining the overall traffic intensity of the system on the day being
// simulated
float stdnormal_rv(int seed)
{
    float w, u1, u2, v1, v2;
    do {
        u1 = lcgrand(seed);
        u2 = lcgrand(seed+1);
        v1 = 2*u1-1;
        v2 = 2*u2-1;
        w = pow(v1, 2) + pow(v2, 2);
    } while(w > 1);

    return v1*sqrt((-2*log(w))/w);
}

// end
