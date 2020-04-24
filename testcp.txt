#ifndef EXTRN_H
#define EXTRN_H
#include <deque>
#include "Job.H"
using namespace std;
extern deque<Job> jobs;
extern int fg_pid;
extern deque<string> history_queue;
#endif
