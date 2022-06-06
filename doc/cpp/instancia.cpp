#include <cstdlib>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <sys/errno.h>
#include <dirent.h>
#include <algorithm>
#include "lib/instancia.h"

#define D7 62.5
#define D8 125
#define D9 250
#define D10 500
#define D11 1000
#define D12 2000

/*#define D7 200
#define D8 300
#define D9 400
#define D10 500
#define D11 600
#define D12 710*/

int calcularCanal(int lado,int miniLado, int coordx, int coordy){
	int cantPart = ceil(lado/miniLado);
	//int canal = cantPart*(coordx/miniLado)+coordy/miniLado;
	int canal = mirand(miniLado); //PPP
	return canal;
}

void generar(vector<vector<pii> >& EDs,int n, vector<vector<pii> >& GWs, int m,int lado, int miniLado){
	//cout << "----------------------------------------------" << endl;
        for(int i=0;i<n;i++){
                int coordx = mirand(lado);
                int coordy = mirand(lado);
		//cout << "ED " << i+1 << ": " << coordx << "," << coordy << endl;
		int canal = calcularCanal(lado,miniLado,coordx,coordy);
                EDs[canal].push_back(make_pair(coordx,coordy));
        }

/*        for(int i=0;i<lado/1000;i++){
                for(int j=0;j<lado/1000;j++){
                        int coordx = i*1000+500;
                        int coordy = j*1000+500;
                        GWs.push_back(make_pair(coordx,coordy));
                }
        }
*/
	//cout << "----------------------------------------------" << endl;
	//int j=m;
	for(int i=0;i<m;i++){
	  int coordx = mirand(lado);
	  int coordy = mirand(lado);
	  //int coordx,coordy;
	  /*	  if ((j%4)==0){
	    	coordx=lado/2;
	    coordy=lado/2;
	  }
	  else if ((j%4)==1) {
	    coordx=lado/4;
	    coordy=lado/4;
	  }
	  else if ((j%4)==2) {
	    coordx=3*lado/4;
	    coordy=3*lado/4;
	  }
	  else if ((j%4)==3) {
	    coordx=lado/4;
	    coordy=3*lado/4;
	  }
	  else {
	    coordx=3*lado/4;
	    coordy=lado/4;
	    }*/
	  //j--;
	  int canal = calcularCanal(lado,miniLado,coordx,coordy);
	  //cout << "GW " << i+1 << ": " << coordx << "," << coordy << endl;
          GWs[canal].push_back(make_pair(coordx,coordy));
        }
	//cout << "----------------------------------------------" << endl;
}


void Instancia::crearInstancia(int lado,int miniLado, int inst){

	//_cantCanales = ceil(lado/miniLado) * ceil(lado/miniLado);
	_cantCanales = miniLado; //PPP
        vector<vector<pii> > EDs = vector<vector<pii> >(_cantCanales);
        _GWs = vector<vector<pii> >(_cantCanales);
        _ED = vector<vector<EndDiv> >(_cantCanales);

        //saque el m para que quede una instancia incluida dentro de la otra
        srand (_nt*_k*_K*_H*(inst+1));
        generar(EDs,_nt,_GWs,_mt,lado,miniLado);
        //_m = _GWs.size();
        _GWk = vector<vector<vector<vector<int> > > >(_cantCanales);
        for(int c=0; c<_cantCanales; c++) _GWk[c] = vector<vector<vector<int> > >(_GWs[c].size(),vector<vector<int> >(_K-_k+1));

        cout << _nt << " " << _mt << " " << _k << " " << _K << " " << _H << " " << _cantCanales << endl;

        int T;
        _cont1 = vector<int>(_cantCanales,0);
        _cont2 = vector<int>(_cantCanales,0);
        _cont3 = vector<int>(_cantCanales,0);
        _cont4 = vector<int>(_cantCanales,0);
        //reinicio la semilla para que quede una instancia incluida dentro de la otra, no dependa de m lo random
        srand (_nt*_k*_K*_H*(inst+1)*5);

        for(int c=0; c<_cantCanales; c++){
		int n = EDs[c].size();
		//cout << "Canal: " << c+1 << " - n: " << n << endl;
		for(int i=0; i<n; i++){
                	//cout << "n: " << i+1 << " " ;
	                //defino periodo T segun tipo de instancia
        	        int t;
                	switch(_tipo){
                        	case 0:
                                	if(i<((float)6300/6450)*n){
                                        	T = 3600;
	                                        _cont1[c]++;
        	                        } else if(i<((float)6400/6450)*n){
                	                        T = 900;
                        	                _cont3[c]++;
                                	} else {
                                        	T = 1800;
	                                        _cont2[c]++;
        	                        }
        	                        //cout << T << endl;
                	                break;
                        	case 1:
                                	t = (mirand(4)+1);
	                                if(t==1) {_cont1[c]++; T = 16000;}
        	                        if(t==2) {_cont2[c]++; T = 8000;}
                	                if(t==3) {_cont3[c]++; T = 4000;}
                        	        if(t==4) {_cont4[c]++; T = 3200;}
                                	break;
	                        case 2:
        	                        t = (mirand(4)+1);
                	                if(t==1) {_cont1[c]++; T = 1600;}
                        	        if(t==2) {_cont2[c]++; T = 800;}
                                	if(t==3) {_cont3[c]++; T = 400;}
	                                if(t==4) {_cont4[c]++; T = 320;}
        	                        //cout << "T: " << T << endl;
                	                break;
                        	default:
                                	t = (mirand(2)+1);
	                                if(t==1) {_cont1[c]++; T = 200;}
        	                        if(t==2) {_cont2[c]++; T = 100;}
                	                //cout << "T: " << T << endl;
	                }
        	        //maximo k para respetar 1%
                	int maxk = 7 + log2(0.01*T);
	                if(maxk>_K) maxk = _K;

	                //para cada gw calculo desde que k lo ve
        	        int cantGW = 0;
                	int mink = _k+6;
	                int coordx = EDs[c][i].first;
        	        int coordy = EDs[c][i].second;
                	vector<int> gw;
	                vector<vector<int> > gk(_K-_k+1);
        	        for(int j=0; j<_GWs[c].size(); j++){
                	        int coordxgw = _GWs[c][j].first;
                        	int coordygw = _GWs[c][j].second;
	                        float dist = sqrt(pow(coordx-coordxgw,2) + pow(coordy-coordygw,2));
        	                int kgw = -1;
                	        if(dist < D7){
                        	        kgw = _k;
	                        } else if(dist < D8){
        	                        kgw = _k+1;
                	        } else if(dist < D9){
                        	        kgw = _k+2;
	                        } else if(dist < D10){
				        kgw = _k+3;
                	        } else if(dist < D11){
                        	        kgw = _k+4;
	                        } else if(dist < D12){
        	                        kgw = _k+5;
                	        }
                        	if(maxk < kgw) kgw = -1;
	                        gw.push_back(kgw);
        	                //cout << j+1 << ": " << kgw << endl;
                	        if(kgw!=-1){
                        	        mink = min(mink,kgw);
	                                cantGW++;
        	                        for(int s=kgw;s<=maxk;s++){
                	                        gk[s-_k].push_back(j);
                        	                _GWk[c][j][s-_k].push_back(i);
                                	}
	                        }
        	        }
                	if(mink>_k+5){
                        	cout << "ED "<< i+1 << " no alcanzable " << T << " " << coordx << " " << coordy << endl;
	                        exit(0);
        	        }

                	EndDiv ed(T,_H/T,gw,gk,mink,maxk,cantGW); _ED[c].push_back(ed);
	        }
        	switch(_tipo){
               		case 0:
                       		cout << "Eds con 3600: " << _cont1[c] << endl;
		                cout << "Eds con 1800: " << _cont2[c] << endl;
        	                cout << "Eds con 900: " << _cont3[c] << endl;
               		        break;
		        case 1:
        	                cout << "Eds con 16000: " << _cont1[c] << endl;
               		        cout << "Eds con 8000: " << _cont2[c] << endl;
                       		cout << "Eds con 4000: " << _cont3[c] << endl;
		                cout << "Eds con 3200: " << _cont4[c] << endl;
        	                break;
               		case 2:
		                cout << "Eds con 1600: " << _cont1[c] << endl;
        	                cout << "Eds con 800: " << _cont2[c] << endl;
               		        cout << "Eds con 400: " << _cont3[c] << endl;
	                	cout << "Eds con 320: " << _cont4[c] << endl;
	                        break;
        	       	default:
	        	        cout << "Eds con 200: " << _cont1[c] << endl;
                        	cout << "Eds con 100: " << _cont2[c] << endl;
        	}
	}
/*
        	for(int i=0;i<_n;i++) {
	                vector<int> gws = _ED[i].GW();
        	        cout << i+1 << " - " << _ED[i].T() << ": ";
                	for(int j=0;j<_m;j++) cout << j+1 <<" " << gws[j] << " ";
	                cout << endl;

	                for(int k=_k;k<=_K;k++){
        	                cout << "k: " << k << ": " << endl;
                	        vector<int> kk = _ED[i].Gk(k-_k);
                        	for(int j=0;j<kk.size();j++) cout << kk[j]+1 << " ";
	                        cout << endl;
        	        }
                	cout << endl;
        	}

	        cout << "------------------" << endl;
        	for(int j=0;j<_m;j++){
                	cout << j+1 << ": " << endl;
	                for(int k=_k;k<=_K;k++){
        	                cout << "k: " << k << ": " << endl;
                	        for(int i=0;i<_GWk[j][k-_k].size();i++) cout << _GWk[j][k-_k][i]+1 << " ";
                        	cout << endl;
	                }
        	}*/
}




Instancia::Instancia(int n, int m, int mink, int maxk, int H,int lado,int miniLado,int tipo, int inst):_nt(n),_mt(m),_k(mink),_K(maxk),_H(H),_tipo(tipo){
        crearInstancia(lado,miniLado,inst);
}

vector<EndDiv>& Instancia::ED(int c){
	return _ED[c];
}

vector<int>& Instancia::GWk(int r, int s,int c){
	return _GWk[c][r][s];
}

Instancia::~Instancia(){
}

int Instancia::m(int c){
	return _GWs[c].size();
}

int Instancia::n(int c){
	return _ED[c].size();
}

int Instancia::k(){
	return _k;
}

int Instancia::K(){
	return _K;
}

int Instancia::H(){
	return _H;
}

int Instancia::cantCanales(){
	return _cantCanales;
}

int Instancia::cont2(int c){
	return _cont2[c];
}

int Instancia::cont3(int c){
	return _cont3[c];
}

int Instancia::cont4(int c){
	return _cont4[c];
}

int Instancia::coordxGW(int c,int gw){
	return _GWs[c][gw].first;
}

int Instancia::coordyGW(int c,int gw){
	return _GWs[c][gw].second;
}
