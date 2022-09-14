#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <pthread.h>

using namespace std;

#define NUMBER_OF_THREADS 4

vector<vector<double>> weights;
vector<vector<double>> train;
vector<double> mins;
vector<double> maxs;
double corrects = 0;

pthread_mutex_t _mutex;

void* read_weights(void* args)
{
    string weights_path = string((char*)args);
    weights_path = weights_path + "/weights.csv";
    ifstream file;
    file.open(weights_path);
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        stringstream ss(line);
        int i = 0;
        vector<string> data(21, "");
        for (char c; ss >> c;)
        {
            data[i] = data[i] + c;
            if (ss.peek() == ',')
            {
                ss.ignore();
                i++;
            }
        }
        vector<double> res;
        for (auto str : data)
            res.push_back(stof(str.c_str()));
        weights.push_back(res);
    }
    file.close();
	pthread_exit(NULL);
}

void* read_trains(void* args)
{
    string train_path = string((char*)args);
    vector<vector<double>> train_thread;
    ifstream file;
    string line;
    file.open(train_path);
    getline(file, line);

    while (getline(file, line))
    {
        stringstream ss(line);
        int i = 0;
        vector<string> data(21, "");
        for (char c; ss >> c;)
        {
            data[i] = data[i] + c;
            if (ss.peek() == ',')
            {
                ss.ignore();
                i++;
            }
        }
        vector<double> res;
        for (auto str : data)
            res.push_back(atof(str.c_str()));
        train_thread.push_back(res);
    }
    file.close();

    pthread_mutex_lock(&_mutex);
    train.insert(train.end(), train_thread.begin(), train_thread.end());
    pthread_mutex_unlock(&_mutex);

    pthread_exit(NULL);
}

void get_min_max()
{
    int len = train[0].size() - 1;
    int i = 0;
    while (i < len)
    {
        double min = train[0][i];
        double max = train[0][i];
        for (auto row : train)
        {
            if (row[i] < min)
                min = row[i];
            if (row[i] > max)
                max = row[i];
        }
        mins.push_back(min);
        maxs.push_back(max);
        i++;
    }
}

void* normalize(void* args)
{
    long id = (long)args;
    int start = id * train.size() / 4;
    int end = (id + 1) * train.size() / 4;
    if (id == NUMBER_OF_THREADS - 1)
        end = train.size();
    int len_attr = train[0].size() - 1;
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < len_attr; j++)
        {
            if (maxs[j] != mins[j])
                train[i][j] = (train[i][j] - mins[j]) / (maxs[j] - mins[j]);
        }
    }
    pthread_exit(NULL);
}

double get_score(vector<double> row, vector<double> _class)
{
    double res = 0;
    for (int i = 0; i < _class.size() - 1; i++)
        res += row[i] * _class[i];
    res += _class[_class.size() - 1];
    return res;
}

int get_class(vector<double> row)
{
    int c = 0;
    int i = 0;  
    double score = numeric_limits<double>::max() * (-1);
    double this_score;
    for (auto _class : weights)
    {
        this_score = get_score(row, _class);
        if (this_score > score)
        {
            score = this_score;
            c = i;
        }
        i = i + 1;
    }
    return c;
}

void* calculate(void* args)
{
    long id = (long)args;
    int start = id * train.size() / 4;
    int end = (id + 1) * train.size() / 4;
    
    if (id == NUMBER_OF_THREADS - 1)
        end = train.size();
    int this_corrects = 0;
    for (int k = start; k < end; k++)
    {
        int _class = get_class(train[k]);
        if (_class == static_cast<int>(train[k][train[k].size() - 1]))
            this_corrects += 1;
    }
    pthread_mutex_lock(&_mutex);
    corrects += this_corrects;
    pthread_mutex_unlock(&_mutex);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    void* status;
    pthread_t wtid = NUMBER_OF_THREADS + 1;
    pthread_t ttid[NUMBER_OF_THREADS];
    vector<string> paths;
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
        paths.push_back(string(argv[1]) + "/train_" + to_string(i) + ".csv");
    pthread_create(&wtid, NULL, read_weights, (void*)argv[1]);
    for(long tid = 0; tid < NUMBER_OF_THREADS; tid++)
		pthread_create(&ttid[tid], NULL, read_trains, (void*)(paths[tid].c_str()));
    
    for(long tid = 0; tid < NUMBER_OF_THREADS; tid++)
        pthread_join(ttid[tid], &status);
    pthread_join(wtid, &status);

    get_min_max();

    for(long tid = 0; tid < NUMBER_OF_THREADS; tid++)
		pthread_create(&ttid[tid], NULL, normalize, (void*)(tid));
    for(long tid = 0; tid < NUMBER_OF_THREADS; tid++)
        pthread_join(ttid[tid], &status);

    for(long tid = 0; tid < NUMBER_OF_THREADS; tid++)
		pthread_create(&ttid[tid], NULL, calculate, (void*)(tid));
    for(long tid = 0; tid < NUMBER_OF_THREADS; tid++)
        pthread_join(ttid[tid], &status);

    double res = corrects / train.size() * 100;
    printf("Accuracy: %.2f%%\n", res);
    pthread_exit(NULL);
}