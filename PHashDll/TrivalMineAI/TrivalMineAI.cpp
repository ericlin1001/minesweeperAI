// TrivalMineAI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include<iostream>
#include<fstream>
using namespace std;

int main(int argc, char *argv[])
{
	if (argc <= 2) {
		cout << "fails, please input filename." << endl;
		return 0;
	}
	ifstream in(argv[1]);
	ofstream out(argv[2]);
	int numR, numC;
	in >> numR >> numC;
	int count = 0;
	for (int r = 0; r < numR; r++) {
		for (int c = 0; c < numC; c++) {
			int t;
			in >> t;
			if (t == 9) {
				out << r << endl << c << endl;
				count++;
				if (count >= 2) {
					return 0;
				}
			}
		}
	}
	out << 0 << endl << 0 << endl;
    return 0;
}

