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

#define DAYS 30
#define MONTH 12
#define FIFO "fifo_"
#define WRITE 1
#define READ 0
#define MAX 10000
#define BUILDINGS "./buildings.out"
#define BILLER "./biller.out"
#define BILLS_PATH "buildings/bills.csv"
#define BUILDING_NUMBER 3
#define RESOURCES_NUMBER 3

vector <string> find_buildings_dir(string path)
{
    vector <string> files;
    vector <string> org_files;
    DIR *dr;
    struct dirent *en;
    char* temp[MAX];
    dr = opendir(path.c_str());
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            files.push_back(en->d_name);
        }
        closedir(dr);
    }
    for (int i = 0; i< files.size(); i++)
    {
        if ( files[i] != "." && files[i] != ".." && files[i] != "bills.csv" && files[i] != ".DS_Store")
        {
            org_files.push_back(files[i]);
        }
    }
    return org_files;
}
vector <string> tokenize(string inp_str,char delimiter)
{
    stringstream ss(inp_str); 
    string s; 
    vector <string> str;
    while (getline(ss, s, delimiter)) {    
        str.push_back(s);
    }
    return str; 
}
vector <string> read_bills_csv()
{
    fstream fin;
    vector<string> row;
    string line, word, temp;

    fin.open(BILLS_PATH, ios::in);

    getline(fin, line);
    return tokenize(line,',');
}
void show_postions(vector <string> pos)
{
    int count = 0;
    cout<<"All positions:"<<endl;
    while (true)
    {
        cout<<pos[count];
        count ++;
        if (pos.size() == count)
        {
            cout<<endl;
            break;
        }
        cout<<" - ";
    }
    cout << "Enter positions to get stats:"<<endl;
    return;
}
void create_fifo(string fifo_name)
{
    unlink(fifo_name.c_str());
    auto name = fifo_name.c_str();
    if(mkfifo(name, 0666 | S_IRWXU) != 0){
        cerr << "Couldn't create fifo!" << endl;
        return;
    }
}
void create_all_fifo(int building_size,int resource_size,vector <string>& fifo)
{
    for (int j = 0; j< building_size; j++)
    {
        string fifo_name = FIFO + to_string(j);
        create_fifo(fifo_name);
        fifo.push_back(fifo_name);
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
    cout << "\033[33m"; // Set color to yellow
    cout << "Unnamed pipe created!" << endl;
    //cout << "\033[0m"; // Reset color to default
    fflush(stdout);
    int pid = fork();
    if (pid == -1) {
        perror("fork");
    }
    cout << "\033[32m"; // Set color to green
    cout << "Child process created!" << endl;
    fflush(stdout);
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

string building_data(string folder, string resources,int id)
{
    folder = folder + "-" + resources + "-" + to_string(id);
    return folder;
}

void create_buildings_process(const vector<string>& folders, string resources,vector <int>& child_pids)
{
    for (int i = 0; i < folders.size(); i++)
    {
        int write_pipe;
        int read_pipe;
        int pid = create_process(write_pipe,read_pipe,BUILDINGS);
        string data = building_data(folders[i],resources,i);
        write(write_pipe, data.c_str(), data.length());
        cout << "Building data sent!" << endl;
        fflush(stdout);
        child_pids.push_back(pid);
        close(write_pipe);
        char buf[10240];
        read(read_pipe, buf, 10240);
        cout << "Result sent to main!" << endl;
        cout << "\033[0m"; // Set color to default
        fflush(stdout);
        close(read_pipe);
        printf("%s\n",buf);
        fflush(stdout);
        
    }
}

string coded_resources(vector <string> resources);

void create_biller_process(vector <int>& child_pids ,vector <string> buildings, vector<string> resources){
    int write_pipe;
    int read_pipe;
    int pid = create_process(write_pipe,read_pipe,BILLER);
    string data = "Biller-" + coded_resources(resources) + "-" + coded_resources(buildings);
    cout << "Biller data sent!" << endl;
    fflush(stdout);
    write(write_pipe, data.c_str(), sizeof(data));
    child_pids.push_back(pid);
    close(write_pipe);
    close(read_pipe);
}

string resource_data(string resource,string resource_size,int country_size,int club_size)
{
    string data = resource_size ;//+ ',' ;//+ to_string(country_size) + ',' + to_string(club_size);
    data = data + "-" + resource;
    return data;
}


string coded_resources(vector <string> resources)
{
	string result = resources[0];
	for (int i=1; i< resources.size(); i++)
    {
		result = result + ',' + resources[i];
    }
    return result;
}

void make_path(vector <string>& buildings,string path)
{
    for(int i=0; i< buildings.size(); i++)
    {
        buildings[i] = path + "/" + buildings[i];
    }
    return; 
}

vector <string> input_chosen_buildings(){
    vector <string> buildings;
    string temp;
    getline(cin, temp);
    vector <string> chosen_buildings = tokenize(temp,' ');
    for (int i = 0; i< chosen_buildings.size(); i++)
    {
        buildings.push_back(chosen_buildings[i]);
    }
    return buildings;
}

vector <string> input_chosen_resources(){
    vector <string> resources;
    string temp;
    cout << "Enter resources from {Water,Electricity,Gas}:" << endl;
    getline(cin, temp);
    vector <string> chosen_resources = tokenize(temp,' ');
    for (int i = 0; i< chosen_resources.size(); i++)
    {
        resources.push_back(chosen_resources[i]);
    }
    return resources;
}

void show_buildings(vector <string> buildings){
    int count = 0;
    cout << "Num of buildings: " << buildings.size() << endl;
    cout<<"All buildings:"<<endl;
    while (true)
    {
        cout<<buildings[count];
        count ++;
        if (buildings.size() == count)
        {
            cout<<endl;
            break;
        }
        cout<<" - ";
    }
    cout << "Enter buildings to get stats:"<<endl;
    return;
}

void unlink_all_fifo(vector <string> fifo)
{
    for( int i = 0; i< fifo.size(); i++)
    {
        unlink(fifo[i].c_str());
    }
}

void make_unnamed_pipes(int ** pipes , int building_size){
    for (int i=0 ; i<building_size ; i++){
        if (pipe(pipes[i]) < 0){
            perror("unnamed pipe");
        }
    }
}

int main(int argc, char const *argv[])
{
    vector <int> child_pids;
    string path = argv[1]; 
    vector <string> buildings = find_buildings_dir(path);
    show_buildings(buildings);
    vector <string> chosen_buildings = input_chosen_buildings();
    make_path(buildings,path);
    //vector <string> bills = read_bills_csv();
    //show_positions(bills);
    vector <string> chosen_resources = input_chosen_resources();
	string coded = coded_resources(chosen_resources);
    //vector <string> fifo;
    //create_all_fifo(chosen_buildings.size(),chosen_resources.size(),fifo);
    //int pipes[2*buildings.size()][2];
    create_biller_process(child_pids,chosen_buildings,chosen_resources);
    create_buildings_process(buildings,coded,child_pids);
    printf("done\n");
    exit(0);
    //create_resources_process(chosen_resources,child_pids);
    //unlink_all_fifo(fifo);
    return 0;
}
