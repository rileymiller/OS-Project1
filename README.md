### Riley's CS442 Project1

	To compile program cd into Project1 and run make
	
	To run program use `./simulator src/simulation_file.txt`

Process.h: Header file for process class
Process.cpp: Implementation for processes
Burst.h: Header file for Burst class
Burst.cpp: Implementation file for burst
Thread.h: Header file for thread class
Thread.cpp: Implementation file for threads
Event.h: Header file for event class
Event.cpp: Implementation file for events
simulator.cpp: Driver for simulation
simulation_file.txt: input for simulation

Nothing really unusual for my program, I utilized a vector of the 4 processes to keep track of thread state and I tried to keep the majority of the data encapsulated within classes.

Approximate hrs spent: 45hrs




Custom Algorithm:
	For my custom algorithm I decided to make use of a mixture of Round Robin and Prioritization scheduling. To fulfill the 4 queue requirement I created 4 prioritization queues that function on an FCFS basis from SYSTEM to BATCH, this decision was made in part to satisfy the custom algorithm queue/prioritization requirements but also to minimize the overhead incurred by process switching. Since a higher priority process will be favored, it will minimize the high number of process switches that typically occurs during RR scheduling. 
	The decision to implement a pseudo RR approach was to minimize response time for higher priority processes. This ensures that the higher priority processes do not starve out and are the primary focus of the CPU. This approach can lead to starvation among lower priority processes but does an excellent job of handling the processes that have the highest priority to the Operating System. This approach also was used to make use of preemption to focus on chipping away at higher order processes.
	This algorithm is not fair due largely in part to the emphasis of prioritization but it is optimized to work on systems that have a large number of SYSTEM processes that can't afford to wait for an extended period of time. If I had more time I would have tried to implement an algorithm based off of HRRN to increase fairness and prevent starvation. An HRRN is the antithesis of my approach since it is predicated off of fairness. Fairness means that a given process is not served based on some type of priority but rather how long its been waiting or when it arrived.
	-Riley Miller
