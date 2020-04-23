/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <signal.h>
#include <iostream>
#include "extrn.H"
#include "commands.H"
#include "signals.H"
#define MAX_LINE_SIZE 5000
#define MAXARGS 20
using namespace std;
char* L_Fg_Cmd;
deque<Job> jobs; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
int fg_pid = 0;
char* lineSize; 
deque<string> history_queue;
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    string cmdString; 	   

	
	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */

	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */
	/************************************/
	signal(SIGCHLD, childEnded_Handler);
	signal(SIGTSTP, ctrl_z_handler);
	signal(SIGINT, ctrl_c_handler);
	// if(SIG_ERR == signal(SIGFPE, test_handler))
	//  	cout << "no hafifa" << endl;
	/************************************/
	// Init globals 


		
    	while (1)
    	{
		cin.clear();
	 	printf("smash > ");
		// fgets(lineSize, MAX_LINE_SIZE, stdin);
		getline(cin, cmdString);

		
		// strcpy(cmdString, lineSize);
		lineSize = new char[cmdString.length()+1];
		strcpy(lineSize, cmdString.c_str()); 
		  	
		lineSize[strlen(cmdString.c_str())]='\0';
		// exit(2);
					// background command
		if (!BgCmd(lineSize))	continue;
					// built in commands
		ExeCmd(lineSize, cmdString.c_str());
		if (history_queue.size() == 50)
		{
			history_queue.pop_front();
		}
		history_queue.push_back(cmdString);

		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

