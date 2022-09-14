#include <iostream>
#include <fstream>
#include <sstream>

#include "utils-inl.h"

using namespace std;

int main(int argc, char** argv)
{
    char path[SIZE];
    char command_cstr[SIZE];
    char store_name[SIZE];
    int pipe = atoi(argv[1]);
    read(pipe, store_name, SIZE);
    read(pipe, path, SIZE);
    read(pipe, command_cstr, SIZE);
    Utils::add_slash(path);
	strcat(path, store_name);

    string command(command_cstr);
    // st id start_date finish_date
    vector<string> cdata(4, "");
    stringstream ss(command);

    int i = 0;
    for (char c; ss >> c;)
    {
        cdata[i] = cdata[i] + c;
        if (ss.peek() == ' ')
        {
            ss.ignore();
            i++;
        }
    }

    string final_price = "-1";
    ifstream file;
	string line;
    file.open(path);
    getline(file, line);
    while (getline(file, line))
    {
        // date id price
        vector<string> data(3, "");
        stringstream ss(line);

        i = 0;
        for (char c; ss >> c;)
        {
            data[i] = data[i] + c;
            if (ss.peek() == ',')
            {
                ss.ignore();
                i++;
            }
        }

        if (data[1] == cdata[1] && Utils::between(cdata[2], data[0], cdata[3]))
        {
            if (final_price == "-1")
                final_price = data[2];
            else
            {
                if (cdata[0] == "MAX")
                {
                    if (atoi(data[2].c_str()) > atoi(final_price.c_str()))
                        final_price = data[2];
                }
                else
                {
                    if (atoi(data[2].c_str()) < atoi(final_price.c_str()))
                        final_price = data[2];
                }
            }
        }
    }
    Utils::write_to_pipe(final_price);
    return 0;
}