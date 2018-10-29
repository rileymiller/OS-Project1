#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <iostream>
using namespace std;
class Event {
	private:
	  string type;
	  int scheduled_time;
	  string message;
	  int tid;
	  int ptype;
	  int pid;
	  int available_threads;
	  const string getProcessType(int n);
	  const string getMessage(string &type);
	public:
	  Event(string type, int t, int tid, int pid, int ptype, int av_threads);
	  const string getType();
	  const int getTime();
	  const int getTID();
	  const int getPType();
	  const int getPID();
	  void setThreadID(int n);
	  void setProcessType(int n);
	  void setType(string type);
	  void setTime(int t);
	  bool operator<(const Event& a) const;
	  friend ostream & operator <<(ostream &out, Event e);
};
#endif
