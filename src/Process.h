#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include <vector>
#include "Thread.h"

class Process {
	private:
	  int id;
	  int type;
	  vector<Thread> threads;
	public:
	  Process();
	  Process(int type, int id);
	  const int getType();
	  const int getId();
	  void addThread(Thread t);
	  vector<Thread> getThreads();
	  Thread getThread(int i);
	  void updateThreadState(int tid, string state);
	  void updateThreadCPUTime(int tid, int t);
	  void updateThreadIOTime(int tid, int t);	
	  void updateThreadEndTime(int tid, int t);
	  void updateThreadResponseTime(int tid, int curr_time);
};
#endif
