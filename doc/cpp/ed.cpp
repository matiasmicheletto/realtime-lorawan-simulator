#include <cstdlib>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <sys/errno.h>
#include <dirent.h>
#include <algorithm>
#include "lib/ed.h"

using namespace std;

EndDiv::EndDiv(){
}

EndDiv::EndDiv(int T, int ci,vector<int>& gw,vector<vector<int> >& gk,int mink, int maxk,int cantGW):_T(T),_cantInst(ci),_GW(gw),_Gk(gk),_mink(mink),_maxk(maxk),_cantGW(cantGW){
	//_maxk = 7 + log2(0.01*T);
	//if(_maxk>K) _maxk = K;
	//_mink = _maxk;
	//int m = gw.size();
	//_cantGW = 0;
	//for(int r=0;r<m;r++){
		//if(gw[r]!=-1 && gw[r]<=_maxk){
		//if(gw[r]!=-1){
			//for(int s=gw[r];s<=_maxk;s++) _Gk[s-k].push_back(r);
			//_cantGW++;
			//if(_mink > gw[r]) _mink = gw[r];
		//} else {
		//	_GW[r]=-1;
		//}
	//}
/*	if(_cantGW==0) {
		cout << "Instancia infactible" << endl;
		exit(0);
	}
*/
}

EndDiv::~EndDiv(){
}


int EndDiv::T(){
	return _T;
}

int EndDiv::cantInst(){
	return _cantInst;
}

vector<int>& EndDiv::Gk(int k){
	return _Gk[k];
}

vector<int>& EndDiv::GW(){
	return _GW;
}

int EndDiv::gw(int r){
	return _GW[r];
}

int EndDiv::maxk(){
	return _maxk;
}

int EndDiv::mink(){
	return _mink;
}

int EndDiv::cantGW(){
	return _cantGW;
}

