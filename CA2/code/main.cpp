#include <iostream>
#include <fstream>

#include <wait.h>
#include <bits/stdc++.h> 

#include "utils-inl.h"

using namespace std;

vector<pair<int, int>>::iterator ab(vector<pair<int, int>>::iterator it1, vector<pair<int, int>>::iterator it2)
{
	if ((*it1).first < (*it2).first)
		return it1;
	return it2;
}

int main(int argc, char** argv)
{
	string line;
	char path[SIZE] = {0};
	ifstream commands;
	vector<pair<int, int>> results;
	vector<int> not_finished;

	strcat(path, argv[2]);
	commands.open(argv[1]);
	
	while (getline(commands, line))
	{
		int pid;
		int send_to_child[2];
		pipe(send_to_child);
		if ((pid = fork()) == 0)
		{
			char filename[SIZE] = "./bin/command.o";
			char* args[] = {filename, (char*)to_string(send_to_child[0]).c_str(), NULL}; 
        	execv(args[0], args); 
		}
		else
		{
			write(send_to_child[1], line.c_str(), SIZE);
			write(send_to_child[1], path, SIZE);
			char result[SIZE];
			string addr = "bin/pipes/" + std::to_string(pid);
			mkfifo(addr.c_str(), 0666);
			int fd = open(addr.c_str(), O_RDONLY);
			read(fd, result, SIZE);
			if (strcmp(result, "") == 1)
				not_finished.push_back(pid);
			else
			{
				results.push_back({pid, atoi(result)});
				close(fd);
			}
		}
	}
	if (results.size() != 0)
		wait(NULL);

	for (auto pid : not_finished)
	{
		string addr = "bin/pipes/" + to_string(pid);
		char result[SIZE];
		int fd = open(addr.c_str(), O_RDONLY);
		read(fd, result, SIZE);
		results.push_back({pid, atoi(result)});
		close(fd);
	}
	sort(results.begin(), results.end(), [](const pair<int, int> lhs, const pair<int, int> rhs) {
            return lhs.first < rhs.first;
        });
	for (auto i : results)
		cout << i.second << endl;

	return 0;
}