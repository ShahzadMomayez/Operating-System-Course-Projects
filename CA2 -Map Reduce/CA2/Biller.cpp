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

#define FIFO "fifo_"
#define WRITE 1
#define READ 0
#define MAX 10000
#define BUILDING "./buildings.out"
#define WATER "./resources.out"
#define GAS "./resources.out"
#define ELECTRICITY "./resources.out"
#define WATER_COEFFICIENT 4
#define GAS_COEFFICIENT 3
#define ELECTRICITY_COEFFICIENT 2

string results_to_string(vector <int> age)
{
    string result = to_string(age[0]);
    for (int i=1; i< age.size()-1; i++)
    {
        result = result + "," + to_string(age[i]); 
    }
    result = result + "," + to_string(age[3]);
    return result;
}

vector<string> splitStringByDelimiter(const string& input, char delimiter) {
    vector<string> result;
    string temp;
    
    for (char c : input) {
        if (c == delimiter) {
            result.push_back(temp);
            temp.clear();
        } else {
            temp.push_back(c);
        }
    }
    
    // Add the remaining characters as the last element
    if (!temp.empty()) {
        result.push_back(temp);
    }
    
    return result;
}

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

vector <int> calc_results(vector <string> position,vector <int> age,string chosen_pos)
{
    vector <int> results{100,0,0,0};
    for (int i = 0; i< position.size(); i++)
    {
        if(position[i] == chosen_pos)
        {
            if (results[0] > age[i])
            {
                results[0] = age[i]; //min
            }
            if (results[1] < age[i])
            {
                results[1] = age[i]; //max
            }
            results[2] += age[i]; //sum
            results[3] += 1; //count
        }
    }
    return results;
}
void write_results(string token, string fifo_name)
{   
    int pipe_fd;
    pipe_fd = open(fifo_name.c_str(), O_WRONLY);
    write(pipe_fd, token.c_str(), token.length());
    close(pipe_fd);
}

void write_all_results(vector <string>& position, vector <int>& age, vector <string> chosen_pos,string process_id)
{
    for (int i=0; i < chosen_pos.size(); i++)
    {
        string answer;
        vector <int> results = calc_results(position,age,chosen_pos[i]);
        if (results[3] != 0)
        {   
            answer = results_to_string(results);
        }
        if (results[3] == 0)
        {
            answer = "nothing";
        }
        string fifo_name = FIFO + to_string(i) + process_id; 
        write_results(answer,fifo_name);
    }
}

int create_process(int& write_pipe, int& read_pipe, string executable)
{
    int pipe_fd[2];
    int pipe_fd2[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
    }
    if (pipe(pipe_fd2) == -1) {
        perror("pipe");
    }
    int pid = fork();
    if (pid == 0) {
        // Child process
        //dup2(pipe_fd[READ], STDIN_FILENO);
        //cout << executable;
        //fflush(stdout);
        close(pipe_fd[WRITE]);
        close(pipe_fd2[READ]);
        //close(pipe_fd[READ]);
        char read_fd[20];
        char write_fd[20];
        sprintf(read_fd , "%d" , pipe_fd[READ]);
        sprintf(write_fd , "%d" , pipe_fd2[WRITE]);
        execl(executable.c_str(), read_fd , write_fd, NULL);
        perror("execl");
    } else if (pid > 0) {
        // Parent process
        close(pipe_fd[READ]);
        close(pipe_fd2[WRITE]);
        read_pipe = pipe_fd2[READ] ;
        write_pipe = pipe_fd[WRITE];
    }else{
        perror("fork");
    }
    return pid;
}

void create_gas_process(vector <int>& child_pids ,string address , vector<vector<int>>& gas){
    int write_pipe;
    int read_pipe;
    int pid = create_process(write_pipe,read_pipe,GAS);
    char data[100] ;
    sprintf(data , "%s/Gas.csv" , address.c_str());
    write(write_pipe, data, strlen(data));
    char stats[732];
    read(read_pipe, stats, 732);
    vector <string> days = tokenize(stats,'\n');
    for (int i = 0; i < days.size(); i++)  
    {
        vector <string> results2 = tokenize(days[i],',');
        vector <int> results3;
        for (int j = 0; j < results2.size(); j++)
        {
            results3.push_back(atoi(results2[j].c_str()));
        }
        gas.push_back(results3);
    }
    child_pids.push_back(pid);
    close(write_pipe);
}

void create_electricity_process(vector <int>& child_pids ,string address , vector<vector<int>>& elec){
    int write_pipe;
    int read_pipe;
    int pid = create_process(write_pipe,read_pipe,ELECTRICITY);
    char data[100] ;
    sprintf(data , "%s/Electricity.csv" , address.c_str());
    write(write_pipe, data, strlen(data));
    char stats[732];
    read(read_pipe, stats, 732);
    vector <string> days = tokenize(stats,',');
    for (int i = 0; i < days.size(); i++)  
    {
        vector <string> results2 = tokenize(days[i],',');
        vector <int> results3;
        for (int j = 0; j < results2.size(); j++)
        {
            results3.push_back(atoi(results2[j].c_str()));
        }
        elec.push_back(results3);
    }
    child_pids.push_back(pid);
    close(write_pipe);
}

void create_water_process(vector <int>& child_pids ,string address , vector<vector<int>>& water){
    int write_pipe;
    int read_pipe;
    int pid = create_process(write_pipe,read_pipe,WATER);
    char data[100] ;
    sprintf(data , "%s/Water.csv" , address.c_str());
    write(write_pipe, data, sizeof(data));
    char stats[12000];
    read(read_pipe, stats, 12000);
    printf("stats : %s\n",stats);
    vector <string> days = tokenize(stats,'\n');
    for (int i = 0; i < days.size(); i++) 
    {
        vector <string> results2 = tokenize(days[i],',');
        vector <int> results3;
        for (int j = 0; j < results2.size(); j++)
        {
            results3.push_back(atoi(results2[j].c_str()));
        }
        water.push_back(results3);
    }
    child_pids.push_back(pid);
    close(write_pipe);
}

string make_screen(vector <vector<string>> results){
	string ans = "";
	for(int i = 0 ; i < results.size() ; i++){
		for(int j = 0 ; j < results[i].size() ; j++){
			ans += results[i][j];
		}
		ans += '\n';
	}
	return ans;
}

string compute_water_bill(vector <string> results){
	vector <vector<string>> results2;
	results2[0].push_back(results[0]);
	for(int i = 1; i < results.size() ; i++){
		vector <string> temp = tokenize(results[i],',');
		int bill = (atoi(temp[2].c_str()) - atoi(temp[4].c_str())) * WATER_COEFFICIENT + 1.25*atoi(temp[4].c_str())*WATER_COEFFICIENT;
		temp.push_back(to_string(bill));
		results2.push_back(temp);
	}
	return make_screen(results2);
}

string compute_electricity_bill(vector <string> results){
	vector <vector<string>> results2;
	results2[0].push_back(results[0]);
	for(int i = 1; i < results.size() ; i++){
		vector <string> temp = tokenize(results[i],',');
		int bill = (atoi(temp[2].c_str()) - atoi(temp[4].c_str())) * ELECTRICITY_COEFFICIENT + 1.25*atoi(temp[4].c_str())*ELECTRICITY_COEFFICIENT;
		temp.push_back(to_string(bill));
		results2.push_back(temp);
	}
	return make_screen(results2);
}

string compute_gas_bill(vector <string> results){
	vector <vector<string>> results2;
	results2[0].push_back(results[0]);
	for(int i = 1; i < results.size() ; i++){
		vector <string> temp = tokenize(results[i],',');
		int bill = (atoi(temp[2].c_str()) - atoi(temp[4].c_str())) * GAS_COEFFICIENT + 1.25*atoi(temp[4].c_str())*GAS_COEFFICIENT;
		temp.push_back(to_string(bill));
		results2.push_back(temp);
	}
	return make_screen(results2);
}

string compute_bill(char buf[1000]){
	//printf("buf is: %s\n",buf);
	string str;
	str = buf;
    
	printf("buf_str is: %s\n",str.c_str());
	vector <string> results = tokenize(str,'-');
    printf("results[0] = %s %s" , results[0].c_str() , results[1].c_str());
	if(results[0] == "Water"){
		return compute_water_bill(results);
	}else if(results[0] == "Electricity"){
		return compute_electricity_bill(results);
	}else if(results[0] == "Gas"){
		return compute_gas_bill(results);
	}
	else
		return "nothing";
}

void input_named_pipe(vector <string> buildings ){
	for (int i = 0 ; i < buildings.size() ; i++){
		if (mkfifo(buildings[i].c_str() , 0666) == -1){
			perror("mkfifo");
		}
		int read_fd = open(buildings[i].c_str() , O_RDONLY);
		char buf [1000] ;
		read(read_fd , buf , 1000);
        buf[999] = '\0';
		close(read_fd);
		//printf("buf is: %s\n",buf);
		string bills = compute_bill(buf);
		//printf("bills : %s" , bills);
		int write_fd = open(buildings[i].c_str() , O_WRONLY);
		write(write_fd , bills.c_str() , bills.length());
		close(write_fd);
		//printf("%s\n",buf);
		
	}
}

void input_named_pipe2(vector <string> buildings ){
	for (int i = 0 ; i < buildings.size() ; i++){
		if (mkfifo(buildings[i].c_str() , 0666) == -1){
			perror("mkfifo");
		}
        cout << "\033[33m"; // Set color to yellow
        cout << "Named pipe created: " << buildings[i] << endl;
        //cout << "\033[0m"; // Reset color to default
        fflush(stdout);
		int write_fd = open(buildings[i].c_str() , O_WRONLY);
        string coefs = to_string(WATER_COEFFICIENT) +","+ to_string(GAS_COEFFICIENT) +","+ to_string(ELECTRICITY_COEFFICIENT);
        write(write_fd , coefs.c_str() , coefs.length());
		close(write_fd);
        cout << "\033[32m"; // Set color to green
        cout << "Coefs received: " << coefs << endl;
        //cout << "\033[0m"; // Reset color to default
        fflush(stdout);
		//printf("%s\n",buf);
		
	}
}

int main(int argc , char* argv[])
{
    int read_fd = atoi(argv[0]);
    int write_fd = atoi(argv[1]);
    char buf [100] ; 
    read(read_fd , buf , 100);
    close(read_fd);
    close(write_fd);
    vector <string> commands = tokenize(buf,'-');
	string name = commands[0];
	vector <string> resources = tokenize(commands[1],',');
	vector <string> buildings = tokenize(commands[2],',');
	//printf("%s\n",name.c_str());
	input_named_pipe2(buildings );
    //write_all_results(positions,age,chosen_pos,commands[2]);
    return 0;
    exit(0);
}