// SmartMineAI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<vector>
#include<fstream>
#include<algorithm>
using namespace std;

//#define DEBUG //to generate a AI, comment this line.

#ifdef	DEBUG
#define ASSERT(cond) if(!(cond)){cout<<"Error: condition::"<<__LINE__<<#cond" fails!!"<<endl; system("pause"); abort();}
#define Trace(m) cout<<#m<<"="<<endl;
#else
#define ASSERT(cond) 
#define Trace()
#endif

//typedef pair<int, int> Pos;

struct Pos {
	int r, c;
	Pos(int a=0, int b=0) :r(a), c(b) {}
	bool operator<(const Pos&p)const {
		return r < p.r || (r == p.r && c < p.c);
	}
	bool operator==(const Pos&p)const {
		return r == p.r && c == p.c;
	}
	bool operator!=(const Pos&p)const {
		return !operator==(p);
		
	}
};
ostream &operator<<(ostream&out, const Pos &p) {
	cout << "("<<p.r+1 << "," << p.c+1 << ")";
	return out;
}

template<class T>
void printArr(const vector<T>&arr) {
	cout << "[";
	for (auto i : arr) {
		cout << i<< ",";
	}
	cout << "]";
}
class PosSet {
	int num;
	vector<Pos>poss;
public:
	
	void insert(const Pos &p) {
		poss.push_back(p);
		sort(poss.begin(), poss.end());
	}
	void setNum(int n) {
		num = n;
	}
	int getSize()const { return poss.size(); }
	void print()const {
		cout << "PosSet("<<num<<")";
		printArr(poss);
		cout <<""<< endl;
	}
	bool isDecided() {
		return num == 0 || num == poss.size();
	}
	int getNum()const { return num; }
	const vector<Pos> &getAllPoss()const {
		return poss;
	}
	void delNotMine(const Pos &p) {
		//TODO::
		for (int i = 0; i < poss.size(); i++) {
			if (poss[i] == p) {
				poss.erase(poss.begin() + i);
			}
		}
	}
	
	void delMine(const Pos &p) {
		//TODO::
		for (int i = 0; i < poss.size(); i++) {
			if (poss[i] == p) {
				poss.erase(poss.begin() + i);
				num--;
				ASSERT(num >= 0);
			}
		}
	}
	bool trySubtract(const PosSet&ps) {
		if (getSize() < ps.getSize())return false;
		//poss is in sorted order.
		for (int i = 0; i < ps.poss.size(); i++) {
			if (poss[i] != ps.poss[i]) {
				return false;
			}
		}

		for (int i = ps.poss.size(); i < poss.size(); i++) {
			poss[i - ps.poss.size()] = poss[i];
		}
		poss.resize(poss.size() - ps.poss.size());
		num -= ps.num;
		ASSERT(num >= 0);
		return true;

	}
};
ostream &operator<<(ostream&out, const PosSet &p) {
	p.print();
	return out;
}

class MineMap {
private:
	int numRows, numCols;
	vector<vector<int> > data;
	vector<Pos> operations;
private:
	//basic utility function.
	inline bool isValid(int r, int c) {
		return r >= 0 && c >= 0 && r < numRows&&c < numCols;
	}
	inline bool isValid(const Pos&p) {
		return isValid(p.r, p.c);
	}
	vector<Pos> getNeigbor(const Pos& p) {
		vector<Pos> res;
		Pos t = p;
		--t.r; if (isValid(t))res.push_back(t);
		--t.c; if (isValid(t))res.push_back(t);
		++t.r; if (isValid(t))res.push_back(t);
		++t.r; if (isValid(t))res.push_back(t);
		++t.c; if (isValid(t))res.push_back(t);
		++t.c; if (isValid(t))res.push_back(t);
		--t.r; if (isValid(t))res.push_back(t);
		--t.r; if (isValid(t))res.push_back(t);
		return res;
	}
	int getValue(const Pos&p) const{
		return data[p.r][p.c];
	}
	bool isUncovered(const Pos&p) {
		return getValue(p) == 9;
	}
	//
	void deducePosSets(vector<PosSet>&pss) {
		//assume pss.size()>=1.
		operations.resize(0);
		bool isChange = true;
		//printArr(pss);
#ifdef	DEBUG
		cout << "inside deduce:" << endl;
#endif
		while (isChange) {
			
			if (pss.empty())break;
			isChange = false;
			//fist deal with if exists ps[i].isDecided()		
		
			for (int i = 0; i < pss.size(); i++) {
				if (pss[i].isDecided()) {
					isChange = true;
					PosSet ps = pss[i];
					pss.erase(pss.begin() + i);
					for (auto pos : ps.getAllPoss()) {
						if (ps.getNum() == 0) {//none of is mine.
							operations.push_back(pos);
						}
						for (int j = 0; j < pss.size(); j++) {
							if (ps.getNum() == 0) {//none of is mine.
								pss[j].delNotMine(pos);
							}
							else {//means every one is mine.
								pss[j].delMine(pos);
							}
						}
					}
				}
			}
		//

			for (int i = 0; i < pss.size(); i++) {
				for (int j = i + 1; j < pss.size(); j++) {
					if (!pss[i].trySubtract(pss[j])) {
						if (pss[j].trySubtract(pss[i])) {
							isChange = true;
						}
					}
					else {
						isChange = true;
					}
				}
			}
#ifdef	DEBUG
			cout << "pss:";  printArr(pss);
			cout << "operations:";  printArr(operations);
#endif
		}
#ifdef	DEBUG
		cout << "exit deduce" << endl;
#endif
	}
	void solve() {

		vector<PosSet>pss;
		//setup pss
		for (int r = 0; r < numRows; r++) {
			for (int c = 0; c < numCols; c++) {
				int &d = data[r][c];
				if (1 <= d && d <= 8) {
					vector<Pos> ns = getNeigbor(Pos(r, c));
					PosSet ps;
					ps.setNum(d);
#ifdef	DEBUG
					cout << "checking: "<<Pos(r, c)  << endl;

					printArr(ns);

#endif			
					for (auto neighbor : ns) {
						if (isUncovered(neighbor)) {
							ps.insert(neighbor);
						}
					}
#ifdef	DEBUG
					cout << ps<<endl<<"end check" << endl;
#endif
					if (ps.getSize() > 0) {
						//ps.print();
						pss.push_back(ps);
					}
				}
			}
		}
#ifdef	DEBUG
		cout << "after init:" << endl;
		printArr(pss);
#endif
		if (pss.size() >= 1) {
			deducePosSets(pss);
			if (operations.empty()) {
				if (pss.empty()) {
					
					operations.push_back(Pos(0, 0));
				}
				else {
					operations.push_back(pss.front().getAllPoss().front());
				}
				
			}

		}
		else {
			operations.resize(0);
			operations.push_back(Pos(0, 0));
		}
		
	}
	
public:
	MineMap(int r = 0, int c = 0) :numRows(r), numCols(c) {
		data.resize(numRows);
		for (int i = 0; i < numRows; i++) {
			data[i].resize(numCols);
		}
	}
	void set(int r, int c, int value) {
		data[r][c] = value;
	}	
	vector<Pos> getOperations(){
		solve();
	
		return operations;
	}
	void print() {
		for (auto r : data) {
			for (auto i : r) {
				cout << i << ",";
			}
			cout << endl;
		}
	}
};
int main(int argc, char *argv[])
{
	const char *infileName;
	const char *outfileName;

	if (argc >= 3) {
		infileName = argv[1];
		outfileName = argv[2];
	}
	else if (argc == 2) {
		infileName = argv[1];
		outfileName = "operation.txt";
	}
	else {
		infileName = "map.txt";
		outfileName = "operation.txt";
	}
	ifstream in(infileName);
	ofstream out(outfileName);
	if (in.fail() ) {
		cout << "Can't open file:" << infileName << endl;
		return 0;
	}
	if ( out.fail()) {
		cout << "Can't create/write file:" << outfileName << endl;
		return 0;
	}

	int numR, numC;
	in >> numR >> numC;
	
#ifdef DEBUG
#define cout cout
#else
#define cout out
#endif
	MineMap map(numR, numC);
	for (int r = 0; r < numR; r++) {
		for (int c = 0; c < numC; c++) {
			int t;
			in >> t;
			map.set(r, c, t);
		}
	}
#ifdef DEBUG
	map.print();
#endif

	
	auto res = map.getOperations();
	if (res.empty()) {
		cout << 0 << " " << 0 << endl;
		return 0;
	}
	//ASSERT(!res.empty());
	int count = 0;
	for (auto i : res) {
		count++;
		cout << i.r << endl << i.c << endl;
		if (count >= 5)return 0;
	}
#ifdef DEBUG
	system("pause");
#endif
	return 0;
}
