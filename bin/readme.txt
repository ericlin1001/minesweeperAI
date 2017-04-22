# How to run?
In window, double click ./playMineMain.exe

# How to create a new AI.
write a program, which read in map.txt, and output all operation in operation.txt.

# How does the ./playMineMain.exe invokes your program? 
	It uses this code
	`./TrivalMineAI.exe map.txt operation.txt` to run your program, and get results from operation.txt
	
	

# Example
## A trivial AI c++ program.
~~~
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
~~~
