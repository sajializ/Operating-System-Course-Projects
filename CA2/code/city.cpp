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
    char city_name[SIZE];
    int _pipe = atoi(argv[1]);
    read(_pipe, city_name, SIZE);
    read(_pipe, path, SIZE);
    read(_pipe, command, SIZE);
    Utils::add_slash(path);
	strcat(path, city_name);

    vector<int> prices;
	vector<string> not_finished_addrs;
							
	vector<const char*> files = Utils::get_filenames(path);
    for (auto f : files)
	{
		int city_to_store_pipe[2];
		pipe(city_to_store_pipe);
		int pid;
		if ((pid = fork()) == 0)
		{
			char filename[SIZE] = "./bin/store.o";
			char* args[] = {filename, (char*)to_string(city_to_store_pipe[0]).c_str(), NULL}; 
        	execv(args[0], args); 
		}
	    else
	    {
			write(city_to_store_pipe[1], f, SIZE);
			write(city_to_store_pipe[1], path, SIZE);
			write(city_to_store_pipe[1], command, SIZE);
			Utils::create_pipe_and_read(prices, not_finished_addrs, pid);
		}
	}
	if (files.size() != 0)
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