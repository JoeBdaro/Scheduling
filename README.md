# Scheduling
C++ Implementation of Job Scheduling Algorithm

to implement the Partial Ordering, dependency graph, and scheduling problem. There are four options (listed below) for constructing a scheduling table 

Option 1:  using limited processors and all nodes/jobs have equal (constant) processing time.

Option 2:  using un-limited processors and all nodes/jobs have equal (constant) processing time

Option 3:  using limited processors and nodes/jobs may have different processing time

Option 4:  using un-limited processors and nodes/job may have different processing time

There are 2 sets of test data given, where each set includes two files -- a file contains the dependency graph (a list of partial relations) and a file contains processing time for each node.  The number of processors to be used is given as an argument via command-line.

In this implementation of the scheduling algorithm. two input files per set are given. The first file is the dependency graph while the second is the processing times for all the nodes. I use an adjacent matrix to represent the graph and a 2d array for the schedule table. Please refer to the spec sheet for further explanation

<b>How to run</b>

This code runs from the command line and requires the following arguments from command line upon running

argv[1]: inFile1.txt is provided and is the dependency graph

argv[2]: inFile2.txt is provided and has the processing times for each job

argv[3]: number of processors we can use

argv[4]: outFile1.txt is created to display the schedule table when the program is finished

argv[5]: debug
