#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct ProcessParams
{
public:
	int creation_time;
	int duration; //seconds
	int priority;

	ProcessParams(int c, int d, int p) { 
		creation_time = c;
		duration = d;
		priority = p;
	}

	friend ostream &operator<<(ostream& os, const ProcessParams& p) {
		os << "Creation time = " << p.creation_time << " duration = " << p.duration << " priority = " << p.priority << endl;
		return os;
	}
};


class File {
	ifstream myfile; 
	vector<ProcessParams> processes;

public:
	File() {
		myfile.open("../entrada.txt");
		if (!myfile.is_open()) {
			cout << "Erro ao abrir o arquivo!\n";
		}
	}
	
	void read_file() {
	
		int a, b, c;
		
		if (!myfile.is_open()) {
			cout << "Arquivo não está aberto!" << endl;
		}
		
		while (myfile >> a >> b >> c) {
			ProcessParams p(a, b, c);
			processes.push_back(p);
		}

		cout << "Quantidade de processos lidos do arquivo: " << processes.size() << endl;
	}

	void print_processes_params() {
		vector<ProcessParams>::iterator iter = processes.begin();

		for(iter; iter < processes.end(); iter++) {
			ProcessParams p = *iter;
			cout << p;
		}
	}

	vector<ProcessParams> get_processes() {
		return processes;
	}
};