#include <iostream>
#include <fstream>

#include <wait.h>
#include <bits/stdc++.h> 

#include "utils-inl.h"

using namespace std;

int main(int argc, char** argv)
{
	char path[SIZE];
    char command[SIZE];
    int _pipe = atoi(argv[1]);
    read(_pipe, command, SIZE);
    read(_pipe, path, SIZE);

    vector<int> prices;
	vector<string> not_finished_addrs;
	vector<const char*> dirs = Utils::get_folders(path);

	for (auto dir : dirs)
	{
        int pid;
		int prov[2];
		pipe(prov);
		if ((pid = fork()) == 0)
		{
			char filename[SIZE] = "./bin/providence.o";
			char* args[] = {filename, (char*)to_string(prov[0]).c_str(), NULL}; 
        	execv(args[0], args); 
		}
		else
		{
			write(prov[1], dir, SIZE);
			write(prov[1], path, SIZE);
			write(prov[1], command, SIZE);
			Utils::create_pipe_and_read(prices, not_finished_addrs, pid);
		}
	}
	if (dirs.size() != 0)
		wait(NULL);

	for (auto addr : not_finished_addrs)
		Utils::read_from_pipe(prices, addr);

	string st = string(command).substr(0,3);
	string result = "-1";
	if (prices.size() != 0)
	{
		result = to_string(*max_element(prices.begin(), prices.end()));
		if (st == "MIN")
		{
			vector<int> temp;
			for (int i = 0; i < prices.size(); i++)
				if (prices[i] != -1)
					temp.push_back(prices[i]);
			if (temp.size() == 0)
				result = "-1";
			else
				result = to_string(*min_element(temp.begin(), temp.end()));
		}
	}
    Utils::write_to_pipe(result);

    return 0;
}