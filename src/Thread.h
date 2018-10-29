#ifndef THREAD_H
#define THREAD_H
#include <string>
#include <vector>
#include "Burst.h"

using namespace std;

class Thread {
	private:
		int time_arrived;
		vector<Burst> bursts;
		int tid;
		int pid;
		int ptype;
		string state;
		int turnaround_time;
		int end_time;
		int cpu_time;
		int io_time;
		int response_time;
	public:
		Thread(int time_arrived, int tid, int pid, int ptype, string state);
		const int getTimeArrived();
		void addBurst(Burst b);
		const int getPID();
		const int getTID();
		const int getPType();
		const int getNumBursts();
		const string getState();
		vector<Burst> getBursts();
		void setState(string s);
		const int getResponseTime();
		const int getTurnaroundTime();
		const int getEndTime();
		const int getCPUTime();
		const int getIOTime();
		void setTurnaroundTime(int t);
		void setEndTime(int t);
		void setTimeArrived(int t);
		void addCPUTime(int t);
		void addIOTime(int t);
		void setResponseTime(int curr_time);
		Burst getNextBurst();
};
#endif
