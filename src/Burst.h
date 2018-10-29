#ifndef BURST_H
#define BURST_H

class Burst {
	private:
	  int cpu;
	  int io;
	public:
	  Burst(int cpu, int io);
	  const int getIO();
	  const int getCPU();
	  void setIO(int io);
	  void setCPU(int cpu);
};
#endif
