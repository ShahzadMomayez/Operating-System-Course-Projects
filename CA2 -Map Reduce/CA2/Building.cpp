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
#include <algorithm>

using namespace std;

#define FIFO "fifo_"
#define WRITE 1
#define READ 0
#define MAX 10000
#define BUILDING "./buildings.out"
#define WATER "./resources.out"
#define GAS "./resources.out"
#define ELECTRICITY "./resources.out"

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
    char stats[12000];
    read(read_pipe, stats, 12000);
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
    char stats[12000];
    read(read_pipe, stats, 12000);
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
    //printf("stats : %s\n",stats);
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

vector<vector<int>> make_message(vector <vector<int>>& data){
    vector<vector<int>> result;
    for (int month = 0; month < 12; month++) {
        int sum_usage_per_month = 0;
        int max_usage_pick_hour_per_month = 0;
        int sum_usage_pick_hour_per_month = 0;
        vector<int> hour_usage (6, 0);

        for (int day = 0; day < 30; day++) {
            int sum_usage_per_day = 0;
            for (int hour = 3; hour < data[day+month*30].size(); hour++) {
                int current_hour_usage = data[day+month*30][hour];
                sum_usage_per_day += current_hour_usage;
                hour_usage[hour-3] += (current_hour_usage);
            }
            sum_usage_per_month += sum_usage_per_day;
        }
        auto max_IT = max_element(hour_usage.begin(), hour_usage.end());
        max_usage_pick_hour_per_month = distance(hour_usage.begin(), max_IT);
        sum_usage_pick_hour_per_month = hour_usage[max_usage_pick_hour_per_month];
        int mean_usage_per_month = sum_usage_per_month / 30;
        int diff_usage_per_month = sum_usage_pick_hour_per_month / 30 - mean_usage_per_month;
        vector<int> message = {month+1, mean_usage_per_month , sum_usage_per_month , max_usage_pick_hour_per_month , sum_usage_pick_hour_per_month , diff_usage_per_month};
        //printf("message : %d %d %d %d %d %d\n",hour_usage[0],hour_usage[1],hour_usage[2],hour_usage[3],hour_usage[4],hour_usage[5]);
        result.push_back(message);
    }
    return result;
}

string coded_result(vector <vector<int>> data){
    string result = "";
    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[i].size(); j++) {
            result += to_string(data[i][j]) + ",";
        }
        result += "/";
    }
    return result;
}

string create_msg(vector <string> chosen_resource ,vector <vector<int>> water,vector <vector<int>> gas,vector <vector<int>> electricity){
    string masg = "";
    for (int i = 0; i< chosen_resource.size(); i++){
        string msg = "";
        if (chosen_resource[i] == "Water"){
            vector <vector<int>> message = make_message(water);
            msg = "Water/" + coded_result(message);
        }
        else if (chosen_resource[i] == "Gas"){
            vector <vector<int>> message = make_message(gas);
            msg = "Gas/" + coded_result(message);
        }
        else if (chosen_resource[i] == "Electricity"){
            vector <vector<int>> message = make_message(electricity);
            msg = "Electricity/" + coded_result(message);
        }
        masg = masg + msg + "$" ;
        
    }
    return masg;
}

string name;

string send_msg_on_named_pipe(string msg){
    //printf("%s\n",msg.c_str());
    
    int read_fd = open(name.c_str() , O_RDONLY);
    if(read_fd == -1){
        close(read_fd);
        return "NO_FILE";
        
    }
    char buf[100];
    read(read_fd , buf , 100);
    buf[99] = '\0';
    //printf  ("%s\n",buf);
    string buf_str;
    buf_str = buf;
    close(read_fd);
    return buf_str;
}

string make_screen(vector <vector<string>> results){
	string ans = name + "\n";
    ans += results[0][0] + ":\n";
    ans += "Month|Mean|Sum|hour|SumH|Diff|Bill\n";
	for(int i = 1 ; i < results.size() ; i++){
		for(int j = 0 ; j < results[i].size() ; j++){
			ans += results[i][j]+ "|";
		}
		ans += '\n';
	}
	return ans;
}

string compute_water_bill(vector <string> results , int coef){
	vector <vector<string>> results2;
	//results2[0].push_back(results[0]);
    //printf("avvali%s\n",results[0].c_str());
    //fflush(stdout);
	for(int i = 0; i < results.size() ; i++){
		vector <string> temp = tokenize(results[i],',');
        if(i > 0){
            int bill = (atoi(temp[2].c_str()) - atoi(temp[4].c_str())) * coef + 1.25*atoi(temp[4].c_str())*coef;
            temp.push_back(to_string(bill));
        }
		results2.push_back(temp);

	}

    //cout << results2[0][0] << endl;

	return make_screen(results2);
}

string compute_electricity_bill(vector <string> results , int coef){
	vector <vector<string>> results2;
	//results2[0].push_back(results[0]);
    //printf("avvali%s\n",results[0].c_str());
    //fflush(stdout);
	for(int i = 0; i < results.size() ; i++){
		vector <string> temp = tokenize(results[i],',');
        if(i > 0){
            int bill = (atoi(temp[2].c_str()) - atoi(temp[4].c_str())) * coef + 1.25*atoi(temp[4].c_str())*coef;
            temp.push_back(to_string(bill));
        }
		results2.push_back(temp);

	}

    //cout << results2[0][0] << endl;

	return make_screen(results2);
}

string compute_gas_bill(vector <string> results , int coef){
	vector <vector<string>> results2;
	//results2[0].push_back(results[0]);
    //printf("avvali%s\n",results[0].c_str());
    //fflush(stdout);
	for(int i = 0; i < results.size() ; i++){
		vector <string> temp = tokenize(results[i],',');
        if(i > 0){
            int bill = (atoi(temp[2].c_str())) * coef ;
            temp.push_back(to_string(bill));
        }
		results2.push_back(temp);

	}

    //cout << results2[0][0] << endl;

	return make_screen(results2);
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

string compute_bill(string buf, int water_coef , int gas_coef ,int elec_coef){
    //printf("%s\n",buf.c_str());
    string ans = "";
    vector<string> resources = tokenize(buf,'$');
    for(int r =0 ; r < resources.size() ; r++){
        vector <string> results = tokenize(resources[r],'/');
        //printf("results[0] = %s %s" , results[0].c_str() , results[1].c_str());
        //fflush(stdout);
        if(results[0] == "Water"){
            ans += compute_water_bill(results , water_coef);
        }else if(results[0] == "Electricity"){
            ans += compute_electricity_bill(results , elec_coef);
        }else if(results[0] == "Gas"){
            ans += compute_gas_bill(results , gas_coef);
        }
        else
            return "nothing";
    }
    return ans;

}

string make_results(vector <string> chosen_resource ,vector <vector<int>> water,vector <vector<int>> gas,vector <vector<int>> electricity){
    string msg = create_msg(chosen_resource,water,gas,electricity);
    //printf("%s\n",msg.c_str());
    string coefs_str = send_msg_on_named_pipe(msg);
    if(coefs_str == "NO_FILE"){
        return "NO_FILE";
    }
    //printf("%s\n",coefs_str.c_str());
    vector <string> coefs = tokenize(coefs_str,',');
    int water_coef = atoi(coefs[0].c_str());
    int gas_coef = atoi(coefs[1].c_str());
    int elec_coef = atoi(coefs[2].c_str());
    string res = compute_bill(msg,water_coef,gas_coef,elec_coef);
    //printf("%s\n",res.c_str());
    //fflush(stdout);
    return res;
}

int main(int argc , char* argv[])
{
    int read_fd = atoi(argv[0]);
    int write_fd = atoi(argv[1]);
    char buf [100] ; 
    read(read_fd , buf , 100);
    close(read_fd);
    vector <string> commands = tokenize(buf,'-');
    vector <string> pathes = tokenize(commands[0],'/');
    name = pathes[pathes.size()-1];
    vector <string> chosen_resource = tokenize(commands[1],',');
    vector <vector<int>> water;
    vector <vector<int>> gas;
    vector <vector<int>> electricity;
    vector<int> child_pids;
    create_water_process(child_pids,commands[0] , water);
    create_gas_process(child_pids,commands[0] , gas);
    create_electricity_process(child_pids,commands[0] , electricity);
    cout << "\033[33m"; // Set color to yellow
    cout << "Resources and process unnamed pipe created" << endl;
    fflush(stdout);
    string result = make_results(chosen_resource,water,gas,electricity);
    if(result == "NO_FILE"){
        cout << "\033[0m"; // Set color to red
        exit(0);
    }
    cout << "\033[32m"; // Reset color to green
    cout << "Result sent" << endl;
    cout << "\033[0m"; // Reset color to default
    fflush(stdout);
    write(write_fd , result.c_str() , result.length());
    close(write_fd);


    
    return 0;
    exit(0);
}