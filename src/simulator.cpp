/*
		cout << "    Avg response time:     " << response_time << endl;
		cout << "    Avg turnaround time:   " << turnaround_time << endl;
* Riley Miller
* 9/23/18
* OS - Project 1
* simulator.cpp
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <queue>
#include <math.h>
#include "Burst.h"
#include "Thread.h"
#include "Process.h"
#include "Event.h"

using namespace std;

string processToString(int ptype) {
	switch(ptype) { 
	   case 0: 
		return "[SYSTEM]";
	   case 1:
		return "[INTERACTIVE]";
	   case 2:
		return "[NORMAL]";
	   case 3:
		return "[BATCH]";
	}
	
	return "";
}

void printEventQueue(priority_queue<Event, vector<Event>, less<Event>> &e) {
	priority_queue<Event, vector<Event>, less<Event>> print = e;	
	while(!print.empty()) {
	   cout << print.top() << endl;
	   print.pop();
	}
}

bool isSimulationFinished(vector<Process> &p) {
		
	for(int i = 0; i < int(p.size()); i++) {
	  vector<Thread> threads = p[i].getThreads();
	  for(int j = 0; j < int(threads.size()); j++) {
		if(threads[j].getState() != "EXIT")
		return false;
	  }
	}
	return true;

}


void printPerThread(vector<Process> &p) {
	stringstream ss;
	string msg;
       for(int n = 0; n < int(p.size()); n++) { 
	for(int i = 0; i < int(p.size()); i++) {
	  if(p[i].getThreads().size() > 0 && p[i].getId() == n) {
	    ss << "Process " << p[i].getId()  << " " << processToString(i) << ":" << endl;
	    vector<Thread> threads = p[i].getThreads();
	    for(int j = 0; j < int(threads.size()); j++) {
	  	ss << "	Thread " << j << ":  ARR: " << threads[j].getTimeArrived() << "    CPU: " << threads[j].getCPUTime() << "    IO: " << threads[j].getIOTime() << "    TRT: " << threads[j].getTurnaroundTime() << "    END: " << threads[j].getEndTime() << endl;
	    }
	    ss << endl;
	  } 
        }
       }

	msg = ss.str();
	cout <<  msg << endl;

}

int getNumBlockedThreads(vector<Process> &p) {
	int count = 0;
	
	for(int i = 0; i < int(p.size()); i++) {
	  vector<Thread> threads = p[i].getThreads();
	  for(int j = 0; j < int(threads.size()); j++) {
		if(threads[j].getState() == "BLOCKED")
		  count++;
	  }
	}

	return count;
}

int getNumAvailableThreads(vector<Process> &p) {
	int count = 0;
	
	for(int i = 0; i < int(p.size()); i++) {
	  vector<Thread> threads = p[i].getThreads();
	  for(int j = 0; j < int(threads.size()); j++) {
		if(threads[j].getState() == "READY")
		  count++;
	  }
	}
	return count;
}


void printSimulationResults(vector<Process> &processes, int service_time, int io_time, int dispatch_time, int idle_time, int elapsed_time) {
	
	for(int i = 0; i < int(processes.size()); i++){ 
		switch(i) {
		  case 0:
			  cout << "SYSTEM THREADS:" << endl;
			  break;
		  case 1:
			  cout << "INTERACTIVE THREADS:" << endl;
			  break;
		  case 2:
			  cout << "NORMAL THREADS:" << endl;
			  break;
		  case 3:
			  cout << "BATCH THREADS:" << endl;
			  break;
		}
		
		cout << "    Total count:           " << processes[i].getThreads().size() << endl;
		double response_time, turnaround_time;
		vector<Thread> thds = processes[i].getThreads();
		for(int j = 0; j < int(thds.size()); j++) {
		  response_time += thds[j].getResponseTime();
		  turnaround_time += thds[j].getTurnaroundTime();
		}
	
		response_time = (response_time / double(thds.size()));
		turnaround_time = (turnaround_time / double(thds.size()));

		if(isnan(response_time) || isnan(turnaround_time)){
		
		cout << "    Avg response time:     " << setprecision(2) << fixed <<  0 << endl;
		cout << "    Avg turnaround time:   " << setprecision(2) << fixed <<  0 << endl;
		} else {
		cout << "    Avg response time:     " << setprecision(2) << fixed << response_time << endl;
		cout << "    Avg turnaround time:   " << setprecision(2) << fixed << turnaround_time << endl;
		}
		response_time = 0;
		turnaround_time = 0;
	}

	double cpu_utilization, cpu_efficiency;
	
	
	cpu_utilization = (double(elapsed_time - idle_time) / double(elapsed_time));
	cpu_efficiency = (double(service_time) / double(elapsed_time));
	
	cout << "Total elapsed time:      " << elapsed_time << endl;
	cout << "Service time:            " << service_time << endl;
	cout << "Total I/O time:          " << io_time << endl;
	cout << "Total dispatch time:     " << dispatch_time << endl;
	cout << "Total idle time:         " << idle_time << endl << endl;
	
	cout << "CPU utilization:       " << setprecision(2) << fixed << (cpu_utilization * 100) << "%" << endl;
	cout << "CPU efficiency:        " << setprecision(2) << fixed << (cpu_efficiency * 100) << "%" << endl;
	
}

void printFlagInstructions() {
	cout << "./simulator [flags] simulation_file.txt" << endl << endl;

	cout << "-t, --per_thread" << endl;
	cout << "  Output additional per-thread statistics for arrival time, service time, etc." << endl << endl;
	
	cout << "-v, --verbose" << endl;
	cout << "  Output information about every state-changing event and scheduling decision." << endl << endl;
	cout << "-a, --algorithm" << endl;
	cout << "  The scheduling algorithm to use. One of FCFS, RR, PRIORITY, or CUSTOM." << endl << endl;
	
	cout << "-h, --help" << endl;
	cout << "  Display a help message about these flags and exit" << endl << endl;
}

//implement Round Robin algorithm
void RR(queue<Thread> &rr, vector<Process> &processes, priority_queue<Event, vector<Event>, less<Event>> &events, int TIME, int p_switch_overhead, int t_switch_overhead, bool VERBOSE, bool PER_THREAD) {
	bool thread_running = false;
	Thread c_thd(-1,-1,-1, -1, "");
	int last_ptype = -1;
	int count = 0;
	int service_time = 0;
	int io_time = 0;
	int dispatch_time = 0;
	int idle_time = 0;
	int QUANTUM = 3;
	bool event_dispatched_on_complete = false;

	while(!events.empty()) {
		count++;
		if(!thread_running && getNumAvailableThreads(processes)){
		  //get next thread from the queue
		  c_thd = rr.front();
		  rr.pop();
		  int NUM_THD_AV = 0;
		  NUM_THD_AV = getNumAvailableThreads(processes); 
		  thread_running = true;
		  processes[c_thd.getPType()].updateThreadState(c_thd.getTID(), "RUNNING");
		  if(!event_dispatched_on_complete) {
		  	//dispatch event
		  	Event dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true); 
		  	events.push(dispatch);
		  } else {
				event_dispatched_on_complete = false;
		  }

 	   //determine if it's a thread or a process switch
		  if(c_thd.getPType() != last_ptype) {
		  //dispatch process switch
			TIME += p_switch_overhead;
			dispatch_time += p_switch_overhead;
		  	Event p_switch("PROCESS_DISPATCH_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
			events.push(p_switch);
			last_ptype = c_thd.getPType();
		  } else {			
			TIME += t_switch_overhead;
			dispatch_time += t_switch_overhead;
		  	Event t_switch("THREAD_DISPATCH_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
			events.push(t_switch);
		  }

		  /**
			 * Need to handle RR burst, will pass RR quantum, update vector of burts and return whether cpuBurstCompleted
			 * 
			 **/
		  Burst c_burst = c_thd.getTopBurst();
		  int cpu = c_burst.getCPU();
		  int io = c_burst.getIO();
			if(cpu > QUANTUM) {
				//going to dispatch a THREAD_PREEMPTED event, need to update Thread state
				// c_thd.updateBurst(QUANTUM);
				cpu = QUANTUM;
				TIME += cpu;
				service_time += cpu;
				Event cpu_preempted("THREAD_PREEMPTED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
				events.push(cpu_preempted);		   
				processes[c_thd.getPType()].updateThreadCPUTime(c_thd.getTID(), cpu);
				processes[c_thd.getPType()].updateThreadState(c_thd.getTID(), "READY");
				c_thd.updateBurst(QUANTUM);
				rr.push(c_thd);
				//need to dispatch next thread upon preemption
			} else {
				//CPU_BURST_COMPLETED && IO_BURST_COMPLETED
				c_burst = c_thd.getNextBurst();
				cpu = c_burst.getCPU();
				io = c_burst.getIO();

					if(c_thd.getNumBursts() == 0) {
						processes[c_thd.getPType()].updateThreadState(c_thd.getTID(), "THREAD_COMPLETED");
									
						processes[c_thd.getPType()].updateThreadCPUTime(c_thd.getTID(), cpu);
									
						TIME += cpu;
						service_time += cpu;
						Event t_complete("THREAD_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
						events.push(t_complete);
					} else {
					//create cpu_burst_completed and io_burst_completed events
						TIME += cpu;
						service_time += cpu;
						Event cpu_complete("CPU_BURST_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
						events.push(cpu_complete);		   
						processes[c_thd.getPType()].updateThreadCPUTime(c_thd.getTID(), cpu);
						TIME += io;
						io_time += io;
						Event io_complete("IO_BURST_COMPLETED", TIME, c_thd.getTID(),c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
						events.push(io_complete);
						processes[c_thd.getPType()].updateThreadIOTime(c_thd.getTID(), io);
						rr.push(c_thd);
				} 
			}
			


		}
		
		Event curr = events.top();
		events.pop();
		
		int NUM_THD_AV = 0;
		NUM_THD_AV = getNumAvailableThreads(processes); 
		//based on type of event needs to update time and state of thread
		if(curr.getType() == "CPU_BURST_COMPLETED") {
		  //whatever happens when a CPU burst completes
		  thread_running = false;
		  TIME = curr.getTime();
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "BLOCKED");	
		
			if(getNumAvailableThreads(processes) == 0) {
				Event idle_next_event = events.top();
				int beg_time = curr.getTime();
				int end_time = idle_next_event.getTime();
				idle_time += (end_time - beg_time);
			}
		} else if(curr.getType() == "THREAD_COMPLETED") {
		  //thread completed, update time and thread state


		  thread_running = false;
		  TIME = curr.getTime();
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "EXIT");	
		  
		  processes[curr.getPType()].updateThreadEndTime(curr.getTID(), TIME);	
		
		if(getNumAvailableThreads(processes) > 0 && events.empty()) {
		  	event_dispatched_on_complete = true;
		  	c_thd = rr.front();
				Event next_dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(),c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
				events.push(next_dispatch);
		  }
		} else if(curr.getType() == "THREAD_PREEMPTED") {
			//handle THREAD_PREEMPTED event
			thread_running = false;
			TIME = curr.getTime();
			processes[curr.getPType()].updateThreadState(curr.getTID(), "READY");
			
			if(getNumAvailableThreads(processes) > 0 && events.empty()) {
		  	event_dispatched_on_complete = true;
		  	c_thd = rr.front();
				Event next_dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(),c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
				events.push(next_dispatch);
		  }

		} else if(curr.getType() == "IO_BURST_COMPLETED") {
		  if(getNumAvailableThreads(processes) < 1) {
		    //going to handle idle time here potentially
		    TIME = curr.getTime();
		    
		    event_dispatched_on_complete = true;
		    c_thd = rr.front();
		    Event next_dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV, true);
		    events.push(next_dispatch);
		  }  
		
		//io burst completed, change state of thread to READY
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "READY");
		} else if(curr.getType() == "PROCESS_DISPATCH_COMPLETED" || curr.getType() == "THREAD_DISPATCH_COMPLETED") {
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "RUNNING");
		
		if(processes[curr.getPType()].getThread(curr.getTID()).getResponseTime() == 0) {
		  //update thread response time
		  processes[curr.getPType()].updateThreadResponseTime(curr.getTID(), curr.getTime());
		  }
		} else if(curr.getType() == "THREAD_ARRIVED") {
			processes[curr.getPType()].updateThreadState(curr.getTID(), "READY");
		}
		if(VERBOSE) {
		  cout << curr << endl;
		}

	
		if(isSimulationFinished(processes)) {
		  if(PER_THREAD) {
	      	    printPerThread(processes);
		  }
		  cout << "SIMULATION COMPLETED!" << endl << endl;
		  printSimulationResults(processes, service_time, io_time, dispatch_time, idle_time, TIME);
		}	

			
	}
} 

//gets next priority thread from queues
Thread getNextPriorityThread(queue<Thread> &sys, queue<Thread> &intr, queue<Thread> &norm, queue<Thread> &batc) {
	Thread ret(-1,-1,-1,-1, "");
	if(!sys.empty()) {
		ret = sys.front();
		sys.pop();
		return ret;
	} else if(!intr.empty()) {
		ret = intr.front();
		intr.pop();
		return ret;
	} else if(!norm.empty()) {
		ret = norm.front();
		norm.pop();
		return ret;
	} else if(!batc.empty()) {
		ret = batc.front();
		batc.pop();
		return ret;
	}

	return ret;
}

//peeks next priority thread from queues
Thread peekNextPriorityThread(queue<Thread> &sys, queue<Thread> &intr, queue<Thread> &norm, queue<Thread> &batc) {
	Thread ret(-1,-1,-1,-1, "");
	if(!sys.empty()) {
		return sys.front();
	} else if(!intr.empty()) {
		return intr.front();
	} else if(!norm.empty()) {
		return norm.front();
	} else if(!batc.empty()) {
		return batc.front();
	}
	return ret;
}

//reloads Thread into appropriate queue
void ReloadThread(Thread &rel, queue<Thread> &sys, queue<Thread> &intr, queue<Thread> &norm, queue<Thread> &batc) {
	int ptype = rel.getPType();
	cout << "Thread " << rel.getTID() << " of process " << rel.getPType() << " with process id: " << rel.getPID() << " in RELOADTHREAD!!" << endl;
	switch(ptype) {
		case 0:
			sys.push(rel);
			break;
		case 1: 
			intr.push(rel);
			break;
		case 2:
			norm.push(rel);
			break;
		case 3:
			batc.push(rel);
			break;
	}
}


//implement Prioritization algorithm
void Prioritization(vector<Process> &processes, priority_queue<Event, vector<Event>, less<Event>> &events, int TIME, int p_switch_overhead, int t_switch_overhead, bool VERBOSE, bool PER_THREAD) {
	bool thread_running = false;
	Thread c_thd(-1,-1,-1, -1, "");
	int last_ptype = -1;
	int count = 0;
	int service_time = 0;
	int io_time = 0;
	int dispatch_time = 0;
	int idle_time = 0;
	bool event_dispatched_on_complete = false;

	queue<Thread> sys;
	queue<Thread> intr;
	queue<Thread> norm;
	queue<Thread> batc;

	//seed process queues
	for(int i = 0; i < int(processes.size()); i++) {
		if(i == 0) {
			vector<Thread> p_thds = processes[i].getThreads();
			for(int j = 0; j < int(p_thds.size()); j++) {
				cout << "Thread " << p_thds[j].getTID() << " of ptype: " << p_thds[j].getPType() << endl; 
				sys.push(p_thds[j]);
			}
		} else if(i == 1) {
			vector<Thread> p_thds = processes[i].getThreads();
			for(int j = 0; j < int(p_thds.size()); j++) {
				intr.push(p_thds[j]);
			}
		} else if(i == 2) {
			vector<Thread> p_thds = processes[i].getThreads();
			for(int j = 0; j < int(p_thds.size()); j++) {
				norm.push(p_thds[j]);
			}
		} else if(i == 3) {
			vector<Thread> p_thds = processes[i].getThreads();
			for(int j = 0; j < int(p_thds.size()); j++) {
				batc.push(p_thds[j]);
			}
		}
	}

	while(!events.empty()) {
		count++;
		if(!thread_running && getNumAvailableThreads(processes)){
		  
			//get next thread from the queue
		  c_thd = getNextPriorityThread(sys,intr,norm, batc);
		  int NUM_THD_AV = 0;
		  NUM_THD_AV = getNumAvailableThreads(processes); 
		  thread_running = true;
		  
		  if(!event_dispatched_on_complete) {
		  	//dispatch event
		  	Event dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV); 
		  	events.push(dispatch);
		  } else {
				event_dispatched_on_complete = false;
		  }

 	   //determine if it's a thread or a process switch
		  if(c_thd.getPType() != last_ptype) {
		  //dispatch process switch
				TIME += p_switch_overhead;
				dispatch_time += p_switch_overhead;
				Event p_switch("PROCESS_DISPATCH_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
				events.push(p_switch);
				last_ptype = c_thd.getPType();
		  } else {			
				TIME += t_switch_overhead;
				dispatch_time += t_switch_overhead;
		  	Event t_switch("THREAD_DISPATCH_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
				events.push(t_switch);
		  }

		  //create CPU/IO Burst completed events or THREAD_COMPLETED
		  Burst c_burst = c_thd.getNextBurst();
		  int cpu = c_burst.getCPU();
		  int io = c_burst.getIO();
		  
		  if(c_thd.getNumBursts() == 0) {
		    processes[c_thd.getPType()].updateThreadState(c_thd.getTID(), "THREAD_COMPLETED");
		    		  
		    processes[c_thd.getPType()].updateThreadCPUTime(c_thd.getTID(), cpu);
      		    
		    TIME += cpu;
		    service_time += cpu;
		    Event t_complete("THREAD_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
		    events.push(t_complete);
		  } else {
		  //create cpu_burst_completed and io_burst_completed events
				TIME += cpu;
				service_time += cpu;
				Event cpu_complete("CPU_BURST_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
				events.push(cpu_complete);		   
				processes[c_thd.getPType()].updateThreadCPUTime(c_thd.getTID(), cpu);
				TIME += io;
				io_time += io;
				Event io_complete("IO_BURST_COMPLETED", TIME, c_thd.getTID(),c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
				events.push(io_complete);
				processes[c_thd.getPType()].updateThreadIOTime(c_thd.getTID(), io);
				//reloads thread if it still has bursts
				ReloadThread(c_thd,sys,intr,norm,batc);
		} 
			


		}
		
		Event curr = events.top();
		events.pop();
		//based on type of event needs to update time and state of thread
		if(curr.getType() == "CPU_BURST_COMPLETED") {
		  //whatever happens when a CPU burst completes
		  thread_running = false;
		  TIME = curr.getTime();
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "BLOCKED");	
//		 if(processes[curr.getPType()].getThread(curr.getTID()).getResponseTime() == 0) {
		  //update thread response time
//		   processes[curr.getPType()].updateThreadResponseTime(curr.getTID(), TIME);
//		}
		
		if(getNumAvailableThreads(processes) == 0) {
			Event idle_next_event = events.top();
			int beg_time = curr.getTime();
			int end_time = idle_next_event.getTime();
			idle_time += (end_time - beg_time);
		  }
		} else if(curr.getType() == "THREAD_COMPLETED") {
		  //thread completed, update time and thread state


		  thread_running = false;
		  TIME = curr.getTime();
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "EXIT");	
		  
		  processes[curr.getPType()].updateThreadEndTime(curr.getTID(), TIME);	
		  
		if(getNumAvailableThreads(processes) > 0 && events.empty()) {
		  	event_dispatched_on_complete = true;
				//may need logic to check if it returns a thread
		  	c_thd = peekNextPriorityThread(sys,intr,norm,batc);
				Event next_dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(),c_thd.getPID(), c_thd.getPType(), getNumAvailableThreads(processes));
				events.push(next_dispatch);
		  }
		} else if(curr.getType() == "IO_BURST_COMPLETED") {
		  if(getNumAvailableThreads(processes) < 1) {
		    //going to handle idle time here potentially
		    TIME = curr.getTime();
		    
		    event_dispatched_on_complete = true;
		    c_thd = peekNextPriorityThread(sys,intr,norm,batc);
		    Event next_dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), getNumAvailableThreads(processes));
		    events.push(next_dispatch);
		  }  
		
		//io burst completed, change state of thread to READY
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "READY");
		} else if(curr.getType() == "PROCESS_DISPATCH_COMPLETED" || curr.getType() == "THREAD_DISPATCH_COMPLETED") {
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "RUNNING");
		
		if(processes[curr.getPType()].getThread(curr.getTID()).getResponseTime() == 0) {
		  //update thread response time
		  processes[curr.getPType()].updateThreadResponseTime(curr.getTID(), curr.getTime());
		  }
		} else if(curr.getType() == "THREAD_ARRIVED") {
			processes[curr.getPType()].updateThreadState(curr.getTID(), "READY");
		}
		if(VERBOSE) {
		  cout << curr << endl;
		}

	
		if(isSimulationFinished(processes)) {
		  if(PER_THREAD) {
	      	    printPerThread(processes);
		  }
		  cout << "SIMULATION COMPLETED!" << endl << endl;
		  printSimulationResults(processes, service_time, io_time, dispatch_time, idle_time, TIME);
		}	

			
	}
}

//implement first come first serve algorithm
void FCFS(queue<Thread> &fcfs, vector<Process> &processes, priority_queue<Event, vector<Event>, less<Event>> &events, int TIME, int p_switch_overhead, int t_switch_overhead, bool VERBOSE, bool PER_THREAD) {
	bool thread_running = false;
	Thread c_thd(-1,-1,-1, -1, "");
	int last_ptype = -1;
	int count = 0;
	int service_time = 0;
	int io_time = 0;
	int dispatch_time = 0;
	int idle_time = 0;
	bool event_dispatched_on_complete = false;

	while(!events.empty()) {
		count++;
		if(!thread_running && getNumAvailableThreads(processes)){
		  //get next thread from the queue
		  c_thd = fcfs.front();
		  fcfs.pop();
		  int NUM_THD_AV = 0;
		  NUM_THD_AV = getNumAvailableThreads(processes); 
		  thread_running = true;
		  
		  if(!event_dispatched_on_complete) {
		  	//dispatch event
		  	Event dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV); 
		  	events.push(dispatch);
		  } else {
				event_dispatched_on_complete = false;
		  }

 	   //determine if it's a thread or a process switch
		  if(c_thd.getPType() != last_ptype) {
		  //dispatch process switch
			TIME += p_switch_overhead;
			dispatch_time += p_switch_overhead;
		  	Event p_switch("PROCESS_DISPATCH_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
			events.push(p_switch);
			last_ptype = c_thd.getPType();
		  } else {			
			TIME += t_switch_overhead;
			dispatch_time += t_switch_overhead;
		  	Event t_switch("THREAD_DISPATCH_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
			events.push(t_switch);
		  }

		  //create CPU/IO Burst completed events or THREAD_COMPLETED
		  Burst c_burst = c_thd.getNextBurst();
		  int cpu = c_burst.getCPU();
		  int io = c_burst.getIO();
		  
		  if(c_thd.getNumBursts() == 0) {
		    processes[c_thd.getPType()].updateThreadState(c_thd.getTID(), "THREAD_COMPLETED");
		    		  
		    processes[c_thd.getPType()].updateThreadCPUTime(c_thd.getTID(), cpu);
      		    
		    TIME += cpu;
		    service_time += cpu;
		    Event t_complete("THREAD_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
		    events.push(t_complete);
		  } else {
		  //create cpu_burst_completed and io_burst_completed events
			TIME += cpu;
			service_time += cpu;
			Event cpu_complete("CPU_BURST_COMPLETED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
			events.push(cpu_complete);		   
			processes[c_thd.getPType()].updateThreadCPUTime(c_thd.getTID(), cpu);
			TIME += io;
			io_time += io;
			Event io_complete("IO_BURST_COMPLETED", TIME, c_thd.getTID(),c_thd.getPID(), c_thd.getPType(), NUM_THD_AV);
			events.push(io_complete);
			processes[c_thd.getPType()].updateThreadIOTime(c_thd.getTID(), io);
			fcfs.push(c_thd);
		} 
			


		}
		
		Event curr = events.top();
		events.pop();
		//based on type of event needs to update time and state of thread
		if(curr.getType() == "CPU_BURST_COMPLETED") {
		  //whatever happens when a CPU burst completes
		  thread_running = false;
		  TIME = curr.getTime();
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "BLOCKED");	

		
		if(getNumAvailableThreads(processes) == 0) {
			Event idle_next_event = events.top();
			int beg_time = curr.getTime();
			int end_time = idle_next_event.getTime();
			idle_time += (end_time - beg_time);
		  }
		} else if(curr.getType() == "THREAD_COMPLETED") {
		  //thread completed, update time and thread state


		  thread_running = false;
		  TIME = curr.getTime();
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "EXIT");	
		  
		  processes[curr.getPType()].updateThreadEndTime(curr.getTID(), TIME);	
		  
		if(getNumAvailableThreads(processes) > 0 && events.empty()) {
		  	event_dispatched_on_complete = true;
		  	c_thd = fcfs.front();
			Event next_dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(),c_thd.getPID(), c_thd.getPType(), getNumAvailableThreads(processes));
			events.push(next_dispatch);
		  }
		} else if(curr.getType() == "IO_BURST_COMPLETED") {
		  if(getNumAvailableThreads(processes) < 1) {
		    //going to handle idle time here potentially
		    TIME = curr.getTime();
		    
		    event_dispatched_on_complete = true;
		    c_thd = fcfs.front();
		    Event next_dispatch("DISPATCHER_INVOKED", TIME, c_thd.getTID(), c_thd.getPID(), c_thd.getPType(), getNumAvailableThreads(processes));
		    events.push(next_dispatch);
		  }  
		
		//io burst completed, change state of thread to READY
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "READY");
		} else if(curr.getType() == "PROCESS_DISPATCH_COMPLETED" || curr.getType() == "THREAD_DISPATCH_COMPLETED") {
		  processes[curr.getPType()].updateThreadState(curr.getTID(), "RUNNING");
		
		if(processes[curr.getPType()].getThread(curr.getTID()).getResponseTime() == 0) {
		  //update thread response time
		  processes[curr.getPType()].updateThreadResponseTime(curr.getTID(), curr.getTime());
		  }
		} else if(curr.getType() == "THREAD_ARRIVED") {
			processes[curr.getPType()].updateThreadState(curr.getTID(), "READY");
		}
		if(VERBOSE) {
		  cout << curr << endl;
		}

	
		if(isSimulationFinished(processes)) {
		  if(PER_THREAD) {
	      	    printPerThread(processes);
		  }
		  cout << "SIMULATION COMPLETED!" << endl << endl;
		  printSimulationResults(processes, service_time, io_time, dispatch_time, idle_time, TIME);
		}	

			
	}
}

int main(int argc, char** argv) {

	bool PER_THREAD, VERBOSE = false;
	string fname, algorithm;

	//flag handling
	if(argc > 6) {
	  cerr << "Too many arguments" << endl;
	} else if (argc < 2) {
	  cerr << "Must specify an input file" << endl;
	} else if(argc == 3) {
	  if(string(argv[1]) == "-t" || string(argv[1]) == "--per_thread") {
		PER_THREAD = true;
	  } else if(string(argv[1]) == "-v" || string(argv[1]) == "--verbose") {
		VERBOSE = true;
	  } else if(string(argv[1]) == "-h" || string(argv[1]) == "--help") {
	    printFlagInstructions();
	    exit(42);
	  }
	  fname = argv[2];
	} else if(argc == 4) {
	  if(string(argv[1]) == "-t" || string(argv[1]) == "--per_thread" || string(argv[2]) == "-t" || string(argv[2]) == "--per_thread") {
		PER_THREAD = true;
	  }  if(string(argv[1]) == "-v" || string(argv[1]) == "--verbose" ||string(argv[2]) == "-v" || string(argv[2]) == "--verbose") {
		VERBOSE = true;
	  }  if(string(argv[1]) == "-h" || string(argv[1]) == "--help" || string(argv[2]) == "-h" || string(argv[2]) == "--help") {
	    printFlagInstructions();
	    exit(42);
	  } if (string(argv[1]) == "--algorithm" || string(argv[1]) == "-a") {
			algorithm = argv[2];
		}
	  fname = argv[3];
	} else if(argc == 5) {
		if(string(argv[1]) == "-t" || string(argv[1]) == "--per_thread" || string(argv[2]) == "-t" || string(argv[2]) == "--per_thread" || string(argv[3]) == "-t" || string(argv[3]) == "--per_thread") {
			PER_THREAD = true;
	  }  if(string(argv[1]) == "-v" || string(argv[1]) == "--verbose" || string(argv[2]) == "-v" || string(argv[2]) == "--verbose" || string(argv[3]) == "-v" || string(argv[3]) == "--verbose") {
			VERBOSE = true;
	  }  if(string(argv[1]) == "-h" || string(argv[1]) == "--help" || string(argv[2]) == "-h" || string(argv[2]) == "--help" || string(argv[3]) == "-h" || string(argv[3]) == "--help") {
	    printFlagInstructions();
	    exit(42);
	  } if (string(argv[1]) == "--algorithm" || string(argv[1]) == "-a") {
			algorithm = argv[2];
		} if (string(argv[2]) == "--algorithm" || string(argv[2]) == "-a") {
			algorithm = argv[3];
		}
	  fname = argv[4];
		}
		else if(argc == 6) {
		if(string(argv[1]) == "-t" || string(argv[1]) == "--per_thread" || string(argv[2]) == "-t" || string(argv[2]) == "--per_thread" || string(argv[3]) == "-t" || string(argv[3]) == "--per_thread" || string(argv[4]) == "-t" || string(argv[4]) == "--per_thread") {
			PER_THREAD = true;
	  }  if(string(argv[1]) == "-v" || string(argv[1]) == "--verbose" || string(argv[2]) == "-v" || string(argv[2]) == "--verbose" || string(argv[3]) == "-v" || string(argv[3]) == "--verbose" || string(argv[4]) == "-v" || string(argv[4]) == "--verbose") {
			VERBOSE = true;
	  }  if(string(argv[1]) == "-h" || string(argv[1]) == "--help" || string(argv[2]) == "-h" || string(argv[2]) == "--help" || string(argv[3]) == "-h" || string(argv[3]) == "--help" || string(argv[4]) == "-h" || string(argv[4]) == "--help") {
	    printFlagInstructions();
	    exit(42);
	  } if (string(argv[1]) == "--algorithm" || string(argv[1]) == "-a") {
			algorithm = argv[2];
		} if (string(argv[2]) == "--algorithm" || string(argv[2]) == "-a") {
			algorithm = argv[3];
		} if (string(argv[3]) == "--algorithm" || string(argv[3]) == "-a") {
			algorithm = argv[4];
		}
	  fname = argv[5];
		} else {
	  fname = argv[1];
	}

	//file opening
	ifstream fin(fname);
	if (!fin) {
 	   cerr << "Unable to open file datafile.txt";
   	   exit(1);   // call system to stop
	}
	
	int n_process, t_switch_overhead, p_switch_overhead, TIME;
	
	vector<Process> processes(4);
	priority_queue<Event, vector<Event>, less<Event>> events;	
	queue<Thread> fcfs;
	queue<Thread> rr;	

	fin >> n_process >> t_switch_overhead >> p_switch_overhead;
	for(int i = 0; i < n_process; i++) {
	  int pid, ptype, nthreads;
	  fin >> pid >> ptype >> nthreads;
	  Process p(ptype,pid);
	  
	  for(int j = 0; j < nthreads; j++) {
	    
	    int t_arrival_time, nbursts, cpu, io;
	    fin >> t_arrival_time >> nbursts;
	    if(j == 0 && i == 0){ //initialize the time variable with first thd arrival
				TIME = t_arrival_time;
	    }

	    Thread t(t_arrival_time, j, i, ptype, "NEW");
	    Event new_t("THREAD_ARRIVED",t_arrival_time, j, pid, ptype, 0);
	    events.push(new_t);    
        	for(int k = 0; k < nbursts - 1; k++) {
		//read in bursts before last one
		  fin >> cpu >> io;
		  Burst b(cpu, io);
		  t.addBurst(b);
		}
		fin >> cpu;
		Burst last(cpu, 0);
		t.addBurst(last);
		t.setState("READY"); //thread READY when added to dispatcher
		//read in last burst
		p.addThread(t);//append thread to process
		fcfs.push(t);
		rr.push(t);
	  }

	  processes[ptype] = (p);
	}
	fin.close(); 
	
	if(algorithm == "FCFS") {
		FCFS(fcfs, processes,events, TIME, p_switch_overhead, t_switch_overhead, VERBOSE, PER_THREAD);
	} else if (algorithm == "RR") {
		//call RR function
		RR(rr, processes,events, TIME, p_switch_overhead, t_switch_overhead, VERBOSE, PER_THREAD);
	} else if(algorithm == "PRIORITY") {
		//call PRIORITY function 
		Prioritization(processes,events, TIME, p_switch_overhead, t_switch_overhead, VERBOSE, PER_THREAD);
	} else if(algorithm == "CUSTOM") {
		//call custom function
	} else {
		FCFS(fcfs, processes,events, TIME, p_switch_overhead, t_switch_overhead, VERBOSE, PER_THREAD);
	}
	

		
return 0;
}
