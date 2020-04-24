// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.H"
#include <iostream>
using namespace std;

void childEnded_Handler(int sig)
{
    int status, res;
    for (int i = 0; i < jobs.size(); i++)
    {
        res = (waitpid(jobs[i].getPID(), &status, WNOHANG));
        if (res > 0) {
            jobs.erase(jobs.begin() + i);
            i--;
        }
    }
}

void ctrl_z_handler(int sig){
    for(int i = 0; i < jobs.size(); i++){
        if (jobs[i].getPID() == fg_pid)
        {
            jobs[i].stop();
            kill(fg_pid, SIGTSTP);
            cout << endl << "smash > signal SIGTSTP was sent to pid " << fg_pid;
            fg_pid = 0;
            cout << endl;
            break;
        }
    }
}

void ctrl_c_handler(int sig){
    // if(!fg_pid){
    //     cout << "\nsmash > ";
    //     return;
    // }
    for(int i = 0; i < jobs.size(); i++){
        if (jobs[i].getPID() == fg_pid)
        {
            kill(fg_pid, SIGINT);
            cout << endl << "smash > signal SIGINT was sent to pid " << fg_pid;
            fg_pid = 0;
            cout << endl;
            break;
        }
    }
}