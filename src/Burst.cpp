#include "Burst.h"

using namespace std;

Burst::Burst(int cpu, int io){
	this->cpu = cpu;
	this->io = io;
}

const int Burst::getIO() {
	return io;
}

const int Burst::getCPU() {
	return cpu;
}

void Burst::setIO(int io) {
	this->io = io;
}


void Burst::setCPU(int cpu) {
	this->cpu = cpu;
}
