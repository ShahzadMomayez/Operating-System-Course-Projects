#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <vector>
#include <bits/stdc++.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>


using namespace std;

vector <string> tokenize(string chosen_pos,char delimiter)
{
    stringstream ss(chosen_pos); 
    string s; 
    vector <string> str;
    while (getline(ss, s, delimiter)) {    
        str.push_back(s);
    }
    return str; 
}

void decode_csv(const char name[],string& nums){
    vector <string> results;
    string _name = name, line;
    ifstream file(_name);
    vector <int> day;
    nums = "";
    while(getline(file, line)){
        day.clear();
        if(line[0]!='Y')
            nums += line + '\n';
    }
    return;
}

string num_to_string(vector<vector<int>> nums){
	string result = "";
	for(int i = 0; i < nums.size(); i++){
		result += to_string(nums[i][0]) + "," + to_string(nums[i][1]) + "," + to_string(nums[i][2]) + "," + to_string(nums[i][3]) + "," + to_string(nums[i][4]) + "," + to_string(nums[i][5]) + "\n";
        cout << nums[i][0] << "," << nums[i][1] << "," << nums[i][2] << "," << nums[i][3] << "," << nums[i][4] << "," << nums[i][5] << "\n";
        fflush(stdout);
    }
    
	return result;
}

int main(int argc , char* argv[])
{
    int read_fd = atoi(argv[0]);
    int write_fd = atoi(argv[1]);
    char buf [100] ; 
    read(read_fd , buf , 100);
    close(read_fd);
    vector <string> commands = tokenize(buf,'.');
    string address = commands[0] + ".csv";
	string nums;
    decode_csv(address.c_str(),nums);
	write(write_fd , nums.c_str() , nums.size());
    close(write_fd);
    return 0;
    exit(0);
}