#include "Process.h"
#include <string>
#include <vector>

using namespace std;

Process::Process() {
	id = -1;
	type = -1;	
}

Process::Process(int type, int id){
	this->type = type;
	this->id = id;
}

const int Process::getId() {
	return id;
}

const int Process::getType() {
	return type;
}

void Process::addThread(Thread t) {
	threads.push_back(t);
}

vector<Thread> Process::getThreads() {
	return threads;
}

Thread Process::getThread(int i) {
	return threads[i];
}

void Process::updateThreadState(int tid, string state) {
	threads[tid].setState(state);
}

void Process::updateThreadCPUTime(int tid, int t) {
	threads[tid].addCPUTime(t);
}

void Process::updateThreadIOTime(int tid, int t) {
	threads[tid].addIOTime(t);
}

void Process::updateThreadEndTime(int tid, int t) {
	threads[tid].setEndTime(t);
}

void Process::updateThreadResponseTime(int tid, int curr_time) {
	threads[tid].setResponseTime(curr_time);
}
