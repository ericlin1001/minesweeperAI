// ImagePHash.cpp : Defines the entry point for the console application.
//
//#define DLL
#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include<algorithm>
#include<vector>
#include<stdio.h>
#include "CImg.h"

#ifndef DLL
#define PHASHDLL_API 
#endif

typedef unsigned long long  ulong64;
typedef signed long long long64;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

using namespace cimg_library;
using namespace std;

typedef  int PixelType ;
typedef  CImg<PixelType> Img;
struct Color{
	static Color BLACK,WHITE;
	PixelType r,g,b;
	Color(unsigned long c){
		b=c%256;
		c/=256;
		g=c%256;
		c/=256;
		r=c%256;
	}
	bool operator==(const Color &c){
		return r==c.r&&g==c.g&&b==c.b;
	}
	Color(){}
	Color(PixelType tr,PixelType tg,PixelType tb):r(tr),g(tg),b(tb){}
	void print(){
	printf("(%d,%d,%d)\n",r,g,b);
	}
	int getGray(){
		return (r*30+g*59+b*11)/100;
	}
	int subabs(PixelType a,PixelType b){
		if(a>b)return a-b;
		else return b-a;
	}
	bool isEachSim(Color c,double threSim){
		int t=threSim*256.0f;
		return subabs(r,c.r)<t&&subabs(g,c.g)<t&&subabs(b,c.b)<t;
	}
	bool isSim(Color c,double threSim){
		return abs(this->getGray()-c.getGray())<255.0f*threSim;
	}
	bool isBlack(){
		return r==0&&g==0&&b==0;
	}
};
Color Color::BLACK(0),Color::WHITE(0xffffff);

template<class T>
void showImage(const CImg<T> c) {
	CImgDisplay display(c, "ImageViewer", 0);//important,0
	while (!display.is_closed()) {
		cimg::wait(20);
	}
}

CImg<float>* ph_dct_matrix(const int N){
    CImg<float> *ptr_matrix = new CImg<float>(N,N,1,1,1/sqrt((float)N));
    const float c1 = sqrt(2.0f/N); 
    for (int x=0;x<N;x++){
        for (int y=1;y<N;y++){
            *ptr_matrix->data(x,y) = c1*(float)cos((cimg::PI/2/N)*y*(2*x+1));
        }
    }
    return ptr_matrix;
}

inline Color getColor(const Img&img, int i, int j, Color&c) {
	c.r = img(i, j, 0, 0);
	c.g = img(i, j, 0, 1);
	c.b = img(i, j, 0, 2);
	return c;
}

#ifdef DEBUG
#define ASSERT(cond) if(!(cond)){cout<<"Error: condition::"#cond" fails!."<<endl;}
#define Trace(m) cout<<#m"="<<(m)<<endl;
#else
#define ASSERT(cond)
#define Trace(m) 
#endif

void printImg(Img &img) {
	int w=img.width();
	int h = img.height();
	Trace(w);
	Trace(h);
	cout << "Img:[" << endl;
	for (int y = 0; y < h; y++) {
	for (int x = 0; x < w; x++) {
		int r = img(x, y, 0, 0);
		int g= img(x, y, 0, 1);
		int b= img(x, y, 0, 2);
		int res = sqrt((r*r + g*g + b*b) / 3);
		cout << res << ",";			
		}
	cout << endl;
	}
	cout << "]" << endl;
}

int tileSize = 18;
Img imgs[10];
Img imgMasks[10];
vector<vector<Img> >oimgs;
//#define DEBUG

Img genMaskImg(Img img) {
	Color c1;
	Img res = img;
	vector<int> tmpC;
	for (int r = 0; r < res.height(); r++) {
		for (int c = 0; c < res.width(); c++) {
			getColor(res, c, r, c1);
			int gray = c1.getGray();
			tmpC.push_back(gray);
		}
	}
	int numK = 20;
	nth_element(tmpC.begin(), tmpC.begin() + numK, tmpC.end());
	int cutColor = tmpC[numK];
	for (int r = 0; r < res.height(); r++) {
		for (int c = 0; c < res.width(); c++) {
			getColor(res, c, r, c1);
			int gray = c1.getGray();
			if (gray > cutColor) {
				gray = 255;
			}
			else {
				gray = 0;
			}
			res(c, r, 0) = gray;
			res(c, r, 1) = gray;
			res(c, r, 2) = gray;
		}
	}
	return res;
}
bool processImgs() {
#define FilePrefix ".\\imgs\\"
	char files[10][100] = { FilePrefix"0.bmp", FilePrefix"1.bmp", FilePrefix"2.bmp", FilePrefix"3.bmp", FilePrefix"4.bmp", FilePrefix"5.bmp", FilePrefix"6.bmp",
		FilePrefix"7.bmp", FilePrefix"8.bmp", FilePrefix"9.bmp" };
	vector<vector<string> > ofiles = { {},{},{},{},{},
						{},{},{},{},{ FilePrefix"9-0-0.bmp",FilePrefix"9-4-4.bmp", FilePrefix"9-7-7.bmp", FilePrefix"9.bmp" } };
	oimgs.resize(10);
	oimgs[9].resize(ofiles[9].size());
	for (int i = 0; i < oimgs[9].size(); i++) {
		oimgs[9][i].load(ofiles[9][i].c_str());
		oimgs[9][i] = oimgs[9][i].crop(4, 3, oimgs[9][i].width() - 1 - 3, oimgs[9][i].height() - 1 - 2);
	}
	for (int i = 0; i < 10; i++) {
		imgs[i].load(files[i]);
		Img &img = imgs[i];		
		img = img.crop(4, 3, img.width()-1 -3, img.height()-1-2);
		imgMasks[i] = genMaskImg(img);
	}
	return true;
}

int debugR, debugC;
int old_detectType(Img & src, int &maxSim);
int detectType(Img & src) {
	ASSERT(src.width() == 18 && src.height() == 18);
	int offset[5][2] = { {0, 0}, {1,0}, {-1, 0}, {0, -1}, {0, 1} };
	int maxSim = INT_MIN;
	int sim;
	int type = -1;
	for (int i = 0; i < 5; i++) {
		//if (i == 1)break;
		int dx = offset[i][0];
		int dy = offset[i][1];
		Img a = src.get_crop(4+dx, 3+dy, src.width() - 1 - 3+dx, src.height() - 1 - 2+dy);
		int t=old_detectType(a, sim);
		if (sim > maxSim) {
			maxSim = sim;
			type = t;
		}
	}
	if (maxSim < 220 * 100000) {// bigger than 200, less than 300.
		type = 9;
	}
	return type;
}
int old_detectType(Img & src , int &maxSim) {
	
	int r = debugR;
	int c = debugC;

	//src.normalize(0, 255);
	Img srcMask = genMaskImg(src);
#ifdef DEBUG
	if (r >= 7 && r <= 9 && c >= 13 && c <= 16) {
		cout << "*************" << endl;
		cout << "R, c=" << r << "," << c << endl;
		//showImage(src);
		//showImage(srcMask);
	}
#endif

	int type = -1;
	maxSim = INT_MIN;
	int sim = maxSim;
	Color c1, c2;
	int nineSim = INT_MIN;
	int trackNine = 0;
	for (int i = 0; i < oimgs[9].size(); i++) {
		Img &img = oimgs[9][i];
		sim = 0;
		for (int r = 0; r < img.height(); r++) {
			for (int c = 0; c < img.width(); c++) {
				getColor(img, c, r, c1);
				getColor(src, c, r, c2);
				int colorDiff = abs(c1.r - c2.r)*abs(c1.r - c2.r) + abs(c1.b - c2.b)*abs(c1.b - c2.b) + abs(c1.g - c2.g)*abs(c1.g - c2.g);
				sim -= colorDiff;
			}
		}
		sim /= img.height()*img.width() * 3;
		if (-sim < 20 * 20) {
			type = 9;
			maxSim = INT_MAX;
			return type;
		}
	}

	for (int i = 0; i < 10; i++) {
		Img & img = imgs[i];
		sim = 0;
		if(i==0){
			bool is0 = true;
			int min0 = INT_MAX;
			int max0 = INT_MIN;
		for (int r = 0; r < img.height(); r++) {
			for (int c = 0; c < img.width(); c++) {
				getColor(img, c, r, c1);
				getColor(src, c, r, c2);
				if (c2.getGray() > max0)max0 = c2.getGray();
				if (c2.getGray() < min0)min0 = c2.getGray();

				int colorDiff = abs(c1.r - c2.r)*abs(c1.r - c2.r) + abs(c1.b - c2.b)*abs(c1.b - c2.b) + abs(c1.g - c2.g)*abs(c1.g - c2.g);
				sim -= colorDiff;
			}
		}
		sim /= img.height()*img.width() * 3;
		if (i == 0) {
			if(((c>=8 && -sim < 30 * 30 )||(4<=c&&c<8 && -sim<31*31)||(c<4 && -sim<33*33))&&(max0-min0)<50){
				type = 0;
				maxSim = INT_MAX;
#ifdef DEBUG
				if (r >= 7 && r <= 9 && c >= 13 && c <= 16) {
					Trace(maxSim);
					Trace(sim);
					cout << "type:" << type << endl;
				}
#endif
				break;
			}
			else {

				continue;
			}
		}

	}
	else {
			// i>=1.
			int count1 = 0;
			int count2 = 0, count3 = 0;
			Color c3, c4;
			sim = 0;
			for (int r = 0; r < img.height(); r++) {
				for (int c = 0; c < img.width(); c++) {
					getColor(imgMasks[i], c, r, c1);
					getColor(srcMask, c, r, c2);
		
					getColor(img, c, r, c3);
					getColor(src, c, r, c4);

										
					if(c1.r==0||c2.r==0) {
					
						count2++;
						
					}
					if (c1.r == 0 && c2.r != 0) {
						count3++;
					}
					if (c1.r==0&&c1.r == c2.r) {
						count1++;
						//
						int colorDiff = abs(c3.r - c4.r) + abs(c3.b - c4.b) + abs(c3.g - c4.g);
						
						sim +=255*3- colorDiff;
					
						ASSERT(sim >= 0);
					}
					
				}
			}
			if (count1 <= 5) {
				//if(count1)
				continue;
			}
			//sim = sim / count1;
			sim = (1000 * count1 / count2 )* (count1 > 0 ? 1000 * sim / count1 / 3 / 255 : -1)*100 - 1000 * count3 / count2;
			if (sim > maxSim) {
				maxSim = sim;
				type = i;
			}
#ifdef DEBUG
			if (r >= 7 && r <= 9 && c >= 13 && c <= 16) {
				if (r == 8) {
					Trace(count2);
					Trace(count1);
					Trace(count3);
					//Trace(colorDiff)
					Trace(maxSim);
					Trace(sim);
					cout << "type:" << type << endl;
				}
			}
#endif
		}
	}
	//if(maxSim)
	if (r >= 7 && r <= 9 && c >= 13 && c <= 16) {
		//showImage(srcMask);
	}
	if (type == 8) {
		int i = 3;
		Img img = imgs[i];
		int newM = INT_MIN;
		
		for (int i = 0; i < 10; i++) {
			sim = 0;
			if (i != 3 && i != 8)continue;
			for (int r = 0; r < img.height(); r++) {
				for (int c = 0; c < img.width(); c++) {
					getColor(img, c, r, c1);
					getColor(src, c, r, c2);

					int colorDiff = abs(c1.r - c2.r) + abs(c1.b - c2.b) + abs(c1.g - c2.g);
					sim = 255 * 3 - colorDiff;
				}
			}
			if (sim > newM) {
				newM = sim;
				type = i;
				
			}
#ifdef DEBUG
			if (r >= 7 && r <= 9 && c >= 13 && c <= 16) {
				if (r == 8) {
					cout << "special:::***********" << endl;
					Trace(sim);
//					Trace(sim);

				}
			}
#endif
		}
		if (type != 8) {
			if (maxSim < 0)maxSim = 1;
			maxSim += 10*100000;
		}
	}
	return type;
}
//sprintf(retStr, "file:%s, this is my test\n new line", file);
vector<vector<int > > parseMineType(const char *file){
	vector<vector<int > > res;
	
	if (!processImgs()) {
		return res;
	}

	if (!file) {
		return res;
	}
	Img img;
	try {
		img.load(file);
	}
	catch (CImgIOException ex) {
		return res;
	}
	//showImage(img);
	//ofstream outfile("log.txt");
#define outfile cout
	int w, h;
	w = img.width();
	h = img.height();
	int numRow = h / tileSize;
	int numCol = w / tileSize;
	Color c1, c2;
	res.resize(numRow);
	for (int r = 0; r < numRow; r++) {
		res[r].resize(numCol);
		for (int c = 0; c < numCol; c++) {
			//
			int type = -1;
			Img img1 = img.get_crop(c*tileSize, r*tileSize, c*tileSize + tileSize - 1, r*tileSize + tileSize - 1);			
			debugC = c;
			debugR = r;
			res[r][c] = detectType(img1);
		}
	}
	return res;

}


int ph_dct_imagehash(const char* file, unsigned long long  & hash){

    if (!file){
        return -1;
    }
    CImg<uint8_t> src;
    try {
        src.load(file);
    } catch (CImgIOException ex){
        return -1;
    }
    CImg<float> meanfilter(7,7,1,1,1);
    CImg<float> img;
    if (src.spectrum() == 3){
        img = src.RGBtoYCbCr().channel(0).get_convolve(meanfilter);
    } else if (src.spectrum() == 4){
        int width = img.width();
        int height = img.height();
        int depth = img.depth();
        img = src.crop(0,0,0,0,width-1,height-1,depth-1,2).RGBtoYCbCr().channel(0).get_convolve(meanfilter);
    } else {
        img = src.channel(0).get_convolve(meanfilter);
    }

    img.resize(32,32);
    CImg<float> *C  = ph_dct_matrix(32);
    CImg<float> Ctransp = C->get_transpose();

    CImg<float> dctImage = (*C)*img*Ctransp;

    CImg<float> subsec = dctImage.crop(1,1,8,8).unroll('x');;

    float median = subsec.median();
    unsigned long long one = 0x0000000000000001;
    hash = 0x0000000000000000;
    for (int i=0;i< 64;i++){
        float current = subsec(i);
        if (current > median)
            hash |= one;
        one = one << 1;
    }

    delete C;

    return 0;
}




PHASHDLL_API unsigned long long WINAPI getHash(const char *file){
	unsigned long long hash;
	ph_dct_imagehash(file,hash);
	return hash;
}
PHASHDLL_API  int WINAPI getBinDiff(unsigned long long a,unsigned long long b){
	int i;
	int count=0;
	for(i=0;i<32;i++){
		if((a&1)!=(b&1))count++;
		a>>=1;
		b>>=1;
	}
	return count;
}

int getColorCount(const Img&img,Color color,double sim=0.1){
	Color c;
	int w,h;
	w=img.width();
	h=img.height();
	int count=0;
	vector<double>vs;
	for(int i=0;i<w;i++){
		for(int j=0;j<h;j++){
			getColor(img,i,j,c);
			if(c.isEachSim(color,sim)){
				count++;
			}
		}
	}
	return count;
}

char retStr[100];
PHASHDLL_API char* WINAPI getCardInfo(const char *file){
	/************decide whether card is ready*****/
	/****the threshold of count of green(0x5bff3b) is 200*/
	bool isCardReady=false;
	int cost=-1;
	int hp=-2;
	int hit=-3;
	if(getColorCount(Img(file),Color(0x5bff3b),0.02)>200){
		isCardReady=true;
	}else {
		isCardReady=false;
	}
	/********/
	sprintf(retStr,"(ready,cost,hp,hit)=|%d|%d|%d|%d|%d|",isCardReady?1:0,cost,hp,hit,getColorCount(Img(file),Color(0x5bff3b),0.02));
	return retStr;
}


int numf=10;
char files[20][100];
unsigned long long hashs[100];
int diff[20][20];

int main(){
	//Img img=Img("a.bmp").normalize(0,255);
	//showImage(img);
	cout << "version 2" << endl;
	vector<vector<int> > res;
	res= parseMineType(".\\imgs\\a.bmp");
	cout << "res:" << endl;
	for (int i = 0; i < res.size(); i++) {
		for (int j = 0; j < res[i].size(); j++) {
			cout << res[i][j] << ",";

		}
		cout << endl;
	}

	//cout<<"hello"<<endl;

	system("pause");
	return 0;
}
