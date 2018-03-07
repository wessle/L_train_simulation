README FOR L_LINE SIMULATION MODEL
written by Wes Suttle for AMS 553 final project
Stony Brook University, Fall 2017

__________________________________

This program takes the cost per minute waited and runs per
bus departure configuration, then runs the simulation the
specified number of runs for each of 42 different departure
configurations. It outputs a table containing statistical
information about each run, including the value of the 
cost function. This output is either written to a new file
called "output.txt", or, if this file already exists, the
output is appended to this file.

Command for compiling on UNIX-type systems:

g++ -std=c++11 L_line.cpp train_header.h lcgrand.cpp lcgrand.h

I don't know how to compile in Windows. If you do, be sure to
use C++11.

Simply run the program from the command line and answer the prompts. 

I suggest running no more than 100 runs per configuration,
since the program will test 42 configurations per run.

