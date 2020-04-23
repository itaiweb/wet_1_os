#ifndef JOB_H
#define JOB_H
#include <string>
#include <deque>
#include <signal.h>
using namespace std;
class Job
{
protected:
	int _PID;
    string _Command;
	int _startTime;
	int _runTime;
	bool _running;
public:
	Job(int PID, string Command, int startTime);

	void print();
	inline int getPID() { return _PID; }
	inline string getCommand() { return _Command; }
	inline void updateRunTime(int Time) { _runTime = Time - _startTime; }
	inline void stop() { _running = false; }
	void resume();
	inline bool is_running(){return _running;}
	inline void run() { _running = true; }
};
#endif

