#include "Job.H"
#include <string>
#include <iostream>
using namespace std;

Job::Job(int PID, string Command, int startTime) : _PID(PID),
													_Command(Command),
													_startTime(startTime),
													_runTime(0),
													_running(true)
{
}

void Job::print()
{
	if (_running)
		cout << _Command << " : " << _PID << " " << _runTime << " secs\n";
	else
		cout << _Command << " : " << _PID << " " << _runTime << " secs " << "(Stopped)\n";
}

void Job::resume(){
	  	if(!_running){
		  kill(_PID, SIGCONT);
		  cout << "smash > signal SIGCONT was sent to pid " << _PID << endl;
		  _running = true;
		} 
}