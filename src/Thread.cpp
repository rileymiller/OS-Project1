#include "Thread.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

Thread::Thread(int time_arrived, int tid, int pid, int ptype, string state){
	this->time_arrived = time_arrived;
	this->tid = tid;
	this->pid = pid;
	this->ptype = ptype;
	this->state = state;
	this->cpu_time = 0;
	this->io_time = 0;
	this->response_time = 0;
}

const int Thread::getTimeArrived() {
	return time_arrived;
}

const int Thread::getTID() {
	return tid;
}

const int Thread::getPID() {
	return pid;
}

const int Thread::getPType() {
	return ptype;
}

const int Thread::getNumBursts() {
	return bursts.size();
}

const int Thread::getCPUTime() {
	return cpu_time;
}

const int Thread::getIOTime() {
	return io_time;
}

const int Thread::getResponseTime() {
	return response_time;
}

const string Thread::getState() {
	return state;
}

void Thread::addBurst(Burst b) {
	bursts.push_back(b);
}

vector<Burst> Thread::getBursts() {
	return bursts;
}

void Thread::setState(string s) {
	state = s;
}

const int Thread::getTurnaroundTime() {
	return turnaround_time;
}

const int Thread::getEndTime() {
	return end_time;
}

void Thread::setTurnaroundTime(int t) {
	turnaround_time = t;
}

void Thread::setEndTime(int t) {
	end_time = t;
	turnaround_time = end_time - time_arrived;
}

void Thread::setTimeArrived(int t) {
	time_arrived = t;
}

void Thread::addCPUTime(int t) {
	cpu_time += t;
}

void Thread::setResponseTime(int curr_time) {
	response_time = curr_time - time_arrived;
}

void Thread::addIOTime(int t) {
	io_time += t;
}

Burst Thread::getNextBurst() {
	Burst b = bursts[0];
	if(bursts.size() > 0)
	bursts.erase(bursts.begin());
	return b;
}
