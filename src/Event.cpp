#include "Event.h"
#include <sstream>

using namespace std;

const string Event::getProcessType(int ptype) {	
	switch(ptype) { 
	   case 0: 
		return "SYSTEM";
	   case 1:
		return "INTERACTIVE";
	   case 2:
		return "NORMAL";
	   case 3:
		return "BATCH";
	}
	
	return "";
}

const string Event::getMessage(string &type) {
	stringstream ss;
	string msg;
	if(type == "THREAD_ARRIVED") {
	  ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Transitioned from NEW to READY";
	  msg = ss.str();
	  return msg;
	} else if(type == "CPU_BURST_COMPLETED") { 
	  ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Transitioned from RUNNING to BLOCKED";
	  msg = ss.str();
	  return msg;
	} else if(type == "THREAD_DISPATCH_COMPLETED") {
	  ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Transitioned from READY to RUNNING";
	  msg = ss.str();
	  return msg;
	} else if(type == "PROCESS_DISPATCH_COMPLETED") {
	  ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Transitioned from READY to RUNNING";
	  msg = ss.str();
	  return msg;
	} else if(type == "IO_BURST_COMPLETED") {
	  ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Transitioned from BLOCKED to READY";
	  msg = ss.str();
	  return msg;
	} else if(type == "THREAD_COMPLETED") {
	  ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Transitioned from RUNNING to EXIT";
	  msg = ss.str();
	  return msg;
	} else if(type == "DISPATCHER_INVOKED") {
		if(preemp) {
			ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Selected from " << this->available_threads << " threads; will run for at most 3 ticks";
	  	msg = ss.str();
		} else {
			ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Selected from " << this->available_threads << " threads; will run to completion of burst";
			msg = ss.str();
		}
	  return msg;
	}	else if(type == "THREAD_PREEMPTED") {
	  ss << "THREAD " << this->tid << " in process " << this->pid << " [" << getProcessType(this->ptype) << "]" << endl << "	Transitioned from RUNNING to READY";
	  msg = ss.str();
	  return msg;
	}	
	return "";
}

Event::Event(string type, int t, int tid, int pid, int ptype, int av_threads, bool preemp){
	this->type = type;
	this->scheduled_time = t;
	this->tid = tid;
	this->pid = pid;
	this->ptype = ptype;
	if(av_threads > 0) {
	this->available_threads = av_threads;
	} else {
		this->available_threads = 1;

	} 
	this->message = getMessage(type);
	this->preemp = preemp;
}

const string Event::getType() {
	return type;
}

const int Event::getTime() {
	return scheduled_time;
}

const int Event::getTID() {
	return tid;
}

const int Event::getPID() {
	return pid;
}

const int Event::getPType() {
	return ptype;
}

void Event::setThreadID(int n) {
	this->tid = n;
}

void Event::setProcessType(int n) {
	this->ptype = n;
}

void Event::setType(string type) {
	this->type = type;
}


void Event::setTime(int t) {
	this->scheduled_time = t;
}


bool Event::operator<(const Event& a) const {
	return scheduled_time > a.scheduled_time;
}


ostream & operator <<(ostream &out, Event e) {
	return out << "At time " << e.scheduled_time << ":" << endl <<  "	" << e.type << endl << "	" << e.message << endl << endl;
}
