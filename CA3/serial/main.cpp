#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

vector<vector<double>> weights;
vector<vector<double>> train;

void read_from_file(string path)
{
    string weights_path = path + "/weights.csv";
    
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

    
    string train_path = path + "/train.csv";
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
        train.push_back(res);
    }
    file.close();
}

void normalize()
{
    int len = train[0].size() - 1;
    int i = 0;
    vector<double> maxs;
    vector<double> mins;
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

    for (i = 0; i < train.size(); i++)
    {
        for (int j = 0; j < len; j++)
        {
            if (maxs[j] != mins[j])
                train[i][j] = (train[i][j] - mins[j]) / (maxs[j] - mins[j]);
        }
    }
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

int calculate()
{
    int corrects = 0;
    for (auto row : train)
    {
        int _class = get_class(row);
        if (_class == static_cast<int>(row[row.size() - 1]))
            corrects += 1;
    }
    return corrects;
}

int main(int argc, char** argv)
{
    read_from_file(string(argv[1]));
    normalize();
    double res = calculate();
    printf("Accuracy: %.2f%%\n", res / train.size() * 100);
    return 0;
}