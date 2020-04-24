//		commands.c
//********************************************
#include "commands.H"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include "signals.H"
#include <vector>
using namespace std;
//********************************************
string sig_str[64] = {"SIGHUP" ,"SIGINT" ,"SIGQUIT" ,"SIGILL" ,"SIGTRAP", "SIGABRT" ,"SIGBUS" ,"SIGFPE" ,"SIGKILL" ,"SIGUSR", "SIGSEGV" ,"SIGUSR" ,
"SIGPIPE" ,"SIGALRM" ,"SIGTERM", "SIGSTKFLT" ,"SIGCHLD" ,"SIGCONT" ,"SIGSTOP" ,"SIGTSTP", "SIGTTIN" ,"SIGTTOU" ,"SIGURG" ,"SIGXCPU" ,"SIGXFSZ",
 "SIGVTALRM" ,"SIGPROF" ,"SIGWINCH" ,"SIGIO" ,"SIGPWR", "SIGSYS" ,"SIGRTMIN" ,"SIGRTMIN+1" ,"SIGRTMIN+2" ,"SIGRTMIN+3", "SIGRTMIN+4" ,"SIGRTMIN+5" ,
 "SIGRTMIN+6" ,"SIGRTMIN+7" ,"SIGRTMIN+8", "SIGRTMIN+9" ,"SIGRTMIN+10","SIGRTMIN+11" ,"SIGRTMIN+12" ,"SIGRTMIN+13", "SIGRTMIN+14" ,"SIGRTMIN+15" ,
 "SIGRTMAX-14" ,"SIGRTMAX-13" ,"SIGRTMAX-12", "SIGRTMAX-11" ,"SIGRTMAX-10" ,"SIGRTMAX-9" ,"SIGRTMAX-8" ,"SIGRTMAX-7", "SIGRTMAX-6" ,"SIGRTMAX-5" ,
 "SIGRTMAX-4" ,"SIGRTMAX-3" ,"SIGRTMAX-2", "SIGRTMAX-1" ,"SIGRTMAX"};

bool pre_cwd_valid = true;
bool first_cwd = true;
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(char* lineSize, const char* cmdString)
{
	static char* pre_cwd;
	char* cmd;
	string errorMsg = "";
	vector <char*> args;
	// char pwd[MAX_LINE_SIZE];
	const char* delimiters = " \t\n";
	int num_arg = 0;
	bool illegal_cmd = false; // illegal command
    cmd = strtok(lineSize, delimiters);
	// cout << cmd << endl;
	if (cmd == NULL)
		return 0; 
   	args.push_back(cmd);
	while(args[num_arg] != NULL){
		args.push_back(strtok(NULL, delimiters));
		num_arg++;
	}
	num_arg --;
	// cout << "num args: " <<num_arg << endl << cmd;
	// for (i=1; i<MAX_ARG; i++)
	// {
	// 	args[i] = strtok(NULL, delimiters); 
	// 	if (args[i].c_str() != NULL) 
	// 		num_arg++; 
 
	// }
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		string cd;
		if (num_arg == 1)
		{
			if (!strcmp(args[1], "-"))
			{
				if (first_cwd) {
					illegal_cmd = true;
					errorMsg = "No previous working directory";
				}
				else {
					cd = pre_cwd;
				}
			}
			else
			{
				cd = args[1];
			}
		}
		else
		{
			illegal_cmd = true;
		}
		if (!illegal_cmd)
		{
			// getcwd(pre_cwd, sizeof(pre_cwd));
			if (!pre_cwd_valid)
				free(pre_cwd);
			pre_cwd = get_current_dir_name();
			pre_cwd_valid = false;
			if (chdir(cd.c_str()) == -1)
			{
				errorMsg = "\"" + cd + "\"" + " - No such file or directory";
				illegal_cmd = true;
			}
			else {
				first_cwd = false;
			}
		}
	}
	
	/*************************************************/
	 else if (!strcmp(cmd, "pwd")) 
	{
		char* cwd;
		if (num_arg > 0)
		{
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		else
		{
			cwd = get_current_dir_name();
			// getcwd(cwd, sizeof(cwd));
			cout << cwd << endl;
			free(cwd);
		}
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		if (num_arg > 0)
		{
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		else
		{
			for(int i = 0; i < history_queue.size(); i++)
			{
				cout << history_queue[i] << endl;
			}
		}
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
		int curr_time = time(NULL);
		if (num_arg > 0)
		{
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		else {
			for (int i = 0; i < jobs.size(); i++)
			{
				jobs[i].updateRunTime(curr_time);
				cout << "[" << i + 1 << "] ";
				jobs[i].print();
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		int pid;
		if (!is_ligal_kill(args, num_arg))
		{
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		else {
			int sig_num = -atoi(args[1]);
			int job_num = atoi(args[2]);
			pid = jobs[job_num - 1].getPID();
			if (job_num > jobs.size() || job_num < 1) {
				illegal_cmd = true;
				errorMsg = "kill " + (string)args[2] + " - job does not exist";
			}
			else {

				if ((kill(pid, sig_num)) == -1){
					errorMsg = "kill " + (string)args[2] + " - job cannot send signal";
					illegal_cmd = true;
				}
				else{
				// TODO: find signal define as string!
					job_handler(sig_num, pid);
					cout << "smash > signal " << sig_str[sig_num-1] << " was sent to pid " << pid << endl;
				}			
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if (num_arg > 0)
		{
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		else {
			cout << "smash pid is " << getpid() << endl;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		int pid;
		string process;
		int job_num;
		if(num_arg != 0 && num_arg != 1){
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		else{
			if(num_arg == 0){
				if(jobs.size() == 0 )
					return 0;
				pid = jobs.back().getPID();
				process = jobs.back().getCommand();
				job_num = jobs.size();
			}
			else{
				try{
					job_num = atoi(args[1]);
					if(job_num < 1 || job_num > jobs.size()){
						illegal_cmd = true;
						errorMsg = "\"" + (string)cmdString + "\"";
					}
					else{
					pid = jobs[job_num-1].getPID();
					process = jobs[job_num-1].getCommand();		
					}
				}
				catch (invalid_argument){
					illegal_cmd = true;
					errorMsg = "\"" + (string)cmdString + "\"";
				}
			}
		}
		if(!illegal_cmd){
			Job& fg_job = jobs[job_num-1];
			int return_status;
			cout << process << endl;
			jobs[job_num-1].resume();
			fg_pid = pid;
			waitpid(pid, &return_status, WUNTRACED);
			if (fg_job.is_running() && (get_job_idx(pid) != -1))
				jobs.erase(jobs.begin() + get_job_idx(pid));
		} 	
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		string process;
		int job_num;
		if(num_arg != 0 && num_arg != 1){
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		else{
			if(num_arg == 0){
				if(jobs.size() == 0 )
					return 0;
				job_num = find_last_stopped();
				if (job_num == 0)
					return 0;
				process = jobs[job_num-1].getCommand();
			}
			else{
				try{
					job_num = atoi(args[1]);
					if(job_num < 1 || job_num > jobs.size()){
						illegal_cmd = true;
						errorMsg = "\"" + (string)cmdString + "\"";
					}
					else{
						if (jobs[job_num-1].is_running()){
							cout << "job is already runnning!" << endl;
							return 0;
						}
						process = jobs[job_num-1].getCommand();		
					}
				}
				catch (invalid_argument){
					illegal_cmd = true;
					errorMsg = "\"" + (string)cmdString + "\"";
				}
			}
		}
		if(!illegal_cmd){
			cout << process << endl;
			jobs[job_num-1].resume();
		} 	
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if (num_arg > 1)
		{
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		else if (num_arg == 0) {
			free(pre_cwd);
			return -1;
		}
		else {
			if (strcmp(args[1], "kill")) {
				illegal_cmd = true;
				errorMsg = "\"" + (string)cmdString + "\"";
			}
			else {
				kill_all();
				free(pre_cwd);
				return -1;
			}
		}
	}
	/*************************************************/
	else if(!strcmp(cmd, "cp"))
	{
		int old_file, new_file, read_err;
		char c;
		if (num_arg != 2)
		{
			illegal_cmd = true;
			errorMsg = "2 arguments are required!";
		}
		if (!strcmp(args[1], args[2])){
			illegal_cmd = true;
			errorMsg = "same file!";
		}
		if(!illegal_cmd){
			if (((old_file = open(args[1],O_RDONLY)) == -1) || ((new_file = open(args[2], O_RDWR| O_TRUNC |O_CREAT)) == -1)) {
				illegal_cmd = true;
			}
			else {
				while (read_err = read(old_file, &c, 1) != 0) {
					if(read_err == -1){
						perror("smash error: > ");
						break;
					}
					if(write(new_file, &c, 1) == -1){
						perror("smash error: >");
						break;
					}
				}
				cout << args[1] << " has been copied to " << args[2] << endl;
				close(old_file);
				close(new_file);
			}
		}
	}

	else if(!strcmp(cmd, "diff"))
	{
		int file_1, file_2, read_err_1, read_err_2;
		char c_1, c_2;
		if (num_arg != 2)
		{
			illegal_cmd = true;
			errorMsg = "\"" + (string)cmdString + "\"";
		}
		if(!illegal_cmd){
			if (((file_1 = open(args[1], O_RDONLY)) == -1) || ((file_2 = open(args[2], O_RDONLY)) == -1)) {
				illegal_cmd = true;
			}
			else{
				while (true){
					read_err_2 = read(file_2, &c_2, 1);
					read_err_1 = read(file_1, &c_1, 1);
					if(read_err_1 == -1 || read_err_2 == -1){
						perror("smash error: > ");
						break;
					}
					else if((c_1 != c_2) || (read_err_2 != read_err_1)){
						cout << 1 << endl;
						break;
					}
					else if(read_err_1 == 0){
						cout << 0 << endl;
						break;
					}
				}
			}

		}
	}
	
	/*************************************************/
	else // external command
	{
		if (ExeExternal(&args[0], cmdString) == -1)
			return -1;
		return 0;
	}
	if (illegal_cmd == true)
	{
		if (errorMsg != "")
		{
			cerr << "smash error: > " << errorMsg << endl;
		}
		else if (errno != 0){
			perror("smash error: > ");
			errno = 0;
		}
		else {
			printf("smash error: > \"%s\"\n", cmdString);
		}
		return 1;
	}

    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
int ExeExternal(char** args, const char* cmdString)
{
	int pID;
	switch (pID = fork())
	{
	case -1:
		// Add your code here (error)
		perror("smash error: > ");
		return 0;
	case 0:
		// Child Process
		setpgrp();
		// Add your code here (execute an external command)
		execv(args[0], args + 1);
		perror("smash error: > ");
		return -1;

	default:
		int return_status;
		Job child(pID, args[0], time(NULL));
		jobs.push_back(child);
		Job& fg_job = jobs.back();
		fg_pid = pID;
		waitpid(pID, &return_status, WUNTRACED);
		if ((fg_job.is_running()) && (get_job_idx(pID) != -1)) {
			jobs.erase(jobs.begin() + get_job_idx(pID));
		}
		return 0;
	}
	return 0;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize)
{
	const char* delimiters = " \t\n";
	vector <char*> args;
	if (lineSize[strlen(lineSize) - 1] == '&')
	{
		lineSize[strlen(lineSize) - 1] = '\0';
		// Add your code here (execute a in the background)

		int pID, num_arg = 0, parentpID;
		char* cmd;

		cmd = strtok(lineSize, delimiters);
		if (cmd == NULL)
			return 0;
		args.push_back(cmd);
		while (args[num_arg] != NULL) {
			args.push_back(strtok(NULL, delimiters));
			num_arg++;
		}
		num_arg--;
		parentpID = getpid();

		switch (pID = fork())
		{
			case -1:
			{
				// Add your code here (error)
				perror("smash error: > ");
				return 0;
			}
			case 0:
			{
				// Child Process
				setpgrp();
				// execute an external command
				execv(cmd, &(args[1]));
				perror("smash error: > ");
				cout << "smash > ";
				return -1;
			}
			default:
			{
				Job child(pID, args[0], time(NULL));
				jobs.push_back(child);
				return 0;
			}
		}
	}
	return 1;
}



bool is_ligal_kill(vector <char*> args, int num_arg) {

	if (num_arg != 2)
		return false;
	if (args[1][0] != '-')
		return false;


	int arg1 = atoi(args[1]);
	int arg2 = atoi(args[2]);
	if (arg1 == 0 || arg2 == 0) {
		return false;
	}
	return true;
}


int find_last_stopped(){
	for (int i = jobs.size(); i>0; i--){
		if (!jobs[i-1].is_running())
			return i;
	}
	return 0;
}

int get_job_idx(int pid){

	for(int i = 0; i < jobs.size(); i++){
		if(jobs[i].getPID() == pid)
			return i;
	}
	return -1;
}

void kill_all() {
	int return_status, pid, start_time;
	int i = 1;
	bool Done = false;
	while (jobs.size()){
		Done = false;
		pid = jobs.front().getPID();
		cout << "[" << i << "] " <<  jobs.front().getCommand() << " - Sending SIGTERM...";
		kill(pid, SIGTERM);
		start_time = time(NULL);
		while (time(NULL) < start_time + 5) {
			if (waitpid(pid, &return_status, WNOHANG) == -1) {
				cout << " Done." << endl;
				Done = true;
				break;
			}
		}
		if (!Done) {
			cout << " (5 sec passed) Sending SIGKILL...";
			kill(pid, SIGKILL);

			cout << " Done." << endl;
		}
		if(get_job_idx(pid) != -1)
			jobs.erase(jobs.begin());
		i++;
	}
}

void job_handler(int sig_num, int pid){
	if (sig_str[sig_num - 1] == "SIGTSTP" || sig_str[sig_num - 1] == "SIGSTOP") {
		jobs[get_job_idx(pid)].stop();
	}
	else if (sig_str[sig_num - 1] == "SIGCONT") {
		jobs[get_job_idx(pid)].run();
	}
}