#include <cstdlib>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <sys/errno.h>
#include <dirent.h>
#include <algorithm>
#include "lib/heu.h"


Heu::Heu(Instancia& inst):_inst(&inst){
}

Heu::~Heu(){
}

void Heu::ordenar(vector<vector<pii> >& eds){
	// ordeno los eds segun
	// 0: cantidad de gws (menor a mayor)
	// 1: periodo (menor a mayor)
	// 2: random

	int n = _inst->n();
	vector<EndDiv> ED = _inst->ED();

	for(int i=0; i<n; i++) eds[0].push_back(pii(ED[i].cantGW(),i));
        sort(eds[0].begin(), eds[0].end());

	for(int i=0; i<n; i++) eds[1].push_back(pii(ED[i].T(),i));
        sort(eds[1].begin(), eds[1].end());

	for(int i=0; i<n; i++) eds[2].push_back(pii(i,i));
	random_shuffle(eds[2].begin(), eds[2].end(), mirand);
}


void Heu::cantGW(int maxIter){
	int n = _inst->n();
        int m = _inst->m();
        int k = _inst->k();
        int K = _inst->K();
        int H = _inst->H();
	vector<EndDiv> ED = _inst->ED();
	//int cont1 = _inst->cont1();
	int cont2 = _inst->cont2();
	int cont3 = _inst->cont3();
	int cont4 = _inst->cont4();

	timeval start;
	init_time(start);

	_sol = m+2;
	vector<vector<pii> > eds(3);
	ordenar(eds);

	//sol de cada iter
	vector<vector<int> > solGW = vector<vector<int> >(n);
	vector<vector<int> > solk = vector<vector<int> >(n);
	vector<vector<int> > solh = vector<vector<int> >(n);
	for(int i=0; i<n; i++){
		int cantInst = ED[i].cantInst();
		solGW[i] = vector<int>(cantInst);
		solk[i] = vector<int>(cantInst);
		solh[i] = vector<int>(cantInst);
	}

	for(int iter=0; iter<maxIter;iter++){
		//mapa de asignaciones hechas en cada iter
		//mapa[r][k][h]== i significa que ahi transmitir el ED i
		//mapa[r][k][h]==-1 significa que ahi puede comenzar a transmitir una instancia (hay que verificar en cada caso que no se vaya de su periodo y _H)
		//mapa[r][k][h]==-2 significa que ahi no puede comenzar a transmitir una instancia porque hay otra instancia mas adelante y no entra
		//Estiy cambiando esto
		//mapa[r][k][h]==-3 significa que no puede transmitir porque ya hay un gw alcanzable por un ed transmitiendo
		vector<vector<vector<int> > > mapa(m,vector<vector<int> >(K-k+1,(vector<int>(H,-1))));
		vector<vector<vector<bool> > > mapaED(n,vector<vector<bool> >(K-k+1,(vector<bool>(H,true))));
		vector<int> gwabiertos;
		//asigno
		int opcion = iter%3;
		switch(opcion){
			case 0:
				random_shuffle(eds[0].begin(), eds[0].begin()+n/4, mirand);
				random_shuffle(eds[0].begin()+n/4, eds[0].begin()+n/2, mirand);
				random_shuffle(eds[0].begin()+n/2, eds[0].begin()+3*n/4, mirand);
				random_shuffle(eds[0].begin()+3*n/4, eds[0].end(), mirand);
				break;
			case 1:
				random_shuffle(eds[1].begin(), eds[1].begin()+cont4, mirand);
				random_shuffle(eds[1].begin()+cont4, eds[1].begin()+cont4+cont3, mirand);
				random_shuffle(eds[1].begin()+cont3+cont4, eds[1].begin()+cont2+cont3+cont4, mirand);
				random_shuffle(eds[1].begin()+cont2+cont3+cont4, eds[1].end(), mirand);
				break;
			case 2:
				random_shuffle(eds[2].begin(), eds[2].end(), mirand);
				break;
		}
		int sol = asignar(eds[opcion],solGW,solk,solh,mapa,mapaED,gwabiertos);
		if(sol<_sol){ //si encuentro un sol mejor
			_gwabiertos = gwabiertos;
			//_cantInstGW = cantInstGW;
			_sol = sol;
			_solGW = solGW;
			_solk = solk;
			_solh = solh;
			_mapa = mapa;
			cout << "Sol: " << iter << " " << _sol << "\t" << get_time(start) << endl;
		}
	}

	//if(_sol<=m) mostrarSol();
	/*if(_sol<=_m){
		cout << "Ubicacion gws: " << endl;
		for(int i=0; i<_gwabiertos.size();i++){
			int gw = _gwabiertos[i];
			pii coord = GWs[gw];
			cout << gw << ": " << coord.first << " - " << coord.second << endl;
		}
		cout << endl;
	}*/
	/*
	cout << "Mapa: " << endl;
	for(int r=0;r<_m;r++){
		cout << "r: " << r+1 << endl;
		for(int s=0;s<_K-_k-1;s++){
			cout << "\tk: " << s+_k << endl;
			for(int h=0;h<_H;h++){
				cout << "\t\t" << h+1 << " " << _mapa[r][s][h]+1 << endl;
			}
		}
		cout << endl;
	}
	*/
	cout << "Tiempo total: " << get_time(start) << endl;
	if(_sol<=m) cout << "Mejor Sol: " << _sol << endl;
	else cout << "NO se encontro sol" << endl;
}


int Heu::asignar(vector<pii>& eds, vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh,
	vector<vector<vector<int> > >& mapa, vector<vector<vector<bool> > >& mapaED,vector<int>& gwabiertos){

	int n = _inst->n();
        int m = _inst->m();
        int k = _inst->k();
        int K = _inst->K();
        int H = _inst->H();
	vector<EndDiv> ED = _inst->ED();


	vector<int> gwcerrados; //conjunto de gws todavia no usados
	for(int r=0; r<m; r++) gwcerrados.push_back(r);

	vector<int> sinAsignar = vector<int>(n);
	for(int i=0; i<n; i++){
		int cantInst = ED[i].cantInst();
		for(int j=0;j<cantInst;j++) solGW[i][j] = -1;
		sinAsignar[i] = cantInst;
	}

	vector<int> ranuras(H);
	for(int h=0;h<H;h++) ranuras[h] = h;
	random_shuffle(ranuras.begin(), ranuras.end(), mirand);


	//Va recorriendo los EDs en el orden que vienen, y al que tiene alguna instancia sin asignar le busca un gw entre los cerrados
	//Entre los abiertos seguro que no puede asignar pq ya tiene que haber probado cnd los abre
	//Despues asigna todo lo que puede a ese gw que abre
	//Si se traba, para y devuelve m+2

	//for(int ip=0; ip<_n; ip++) cout << eds[ip].second << " ";
	//cout << endl; getchar();

	for(int ip=0; ip<n; ip++){
		int i = eds[ip].second; //nro de ED
		vector<int> gws = ED[i].GW(); // lista de los k minimos para Ed i cada GW. Si es -1 es que no lo alcanza
		int cantInst = ED[i].cantInst();
		int T = ED[i].T();
		int maxk = ED[i].maxk(); //max SFk al que puede transmitir EDi (para respetar el 1%)

		for(int j=0;j<cantInst && sinAsignar[i]>0;j++)if(solGW[i][j]==-1){ //toma una instancia sin asignar
			//cout << "i: " << i+1 << " " << j+1 << endl;
			if(gwabiertos.size()==_sol-1) return m+2; //si ya no puede mejorar la sol actual retorna m+2
			bool encontro = false;
			//abro un gw, en ppio al azar
			//recorre los gws cerrados al azar para ver si le encuentra uno factible
			random_shuffle(gwcerrados.begin(), gwcerrados.end(), mirand);
			for (int ri=0; ri<gwcerrados.size() && !encontro; ri++) if(gws[gwcerrados[ri]]!=-1){
				//r es un gw cerrado y factible para i. Ahora le busca un k y un h
				int r = gwcerrados[ri];
				int mink = gws[r];
				//cout << "r: " << r+1 << endl;
				for(int s = mink;s<=maxk && !encontro; s++) for(int h=j*T;h<=(j+1)*T-pow(2,s-k) && !encontro;h++) {
					//cout << "k: " << s << " - h: " << h << endl;
					if(mapa[r][s-k][h]==-1 && mapaED[i][s-k][h]){
						//paso el gw a abiertos y lo borro de cerrados
						//cout << "Pasa k: " << s << " - h: " << h << endl;
						gwabiertos.push_back(r);
						gwcerrados.erase(gwcerrados.begin()+ri);
						//cuando se abre el gw r se trata de asignar todo lo que se pueda a el
						//para esto se recorren los ED que todavia tengan instancias sin asignar y son factibles para r
						for(int ipp=ip;ipp<n;ipp++) if(sinAsignar[eds[ipp].second]>0 && ED[eds[ipp].second].gw(r)!=-1)
							asignarTodo(eds[ipp].second,r,solGW, solk, solh,mapa,mapaED,sinAsignar[eds[ipp].second],ranuras);
						encontro = true;
					}
				}
			}
			if(!encontro) {
				//cout << "n: " << ip << " " << i+1 << endl; getchar();
				return m+2;
			}
		}
	}
	return gwabiertos.size();
}

//Dado un ED i y un GW r, trata de asignar todas las instancias que pueda de i a r
void Heu::asignarTodo(int i, int r,vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh,
	vector<vector<vector<int> > >& mapa, vector<vector<vector<bool> > >& mapaED, int& sinAsignar,vector<int>& ranuras){

        int k = _inst->k();
        int H = _inst->H();
	EndDiv ed = _inst->ED()[i];

	vector<int> gws = ed.GW();
	int cantInst = ed.cantInst();
	int T = ed.T();
	int maxk = ed.maxk();
	for(int j=0;j<cantInst && sinAsignar>0;j++)if(solGW[i][j]==-1){ //toma una instancia sin asignar
		bool encontro = false;
		int mink = gws[r];
		//for(int s = mink;s<=maxk && !encontro; s++) for(int h=j*T;h<=(j+1)*T-pow(2,s-_k) && !encontro;h++) {
		for(int s = mink;s<=maxk && !encontro; s++){
			//int h = mirand(T-pow(2,s-_k)+1)+j*T;
			for(int hp=0;hp<=H && !encontro;hp++) if(ranuras[hp]>=j*T && ranuras[hp]<=(j+1)*T-pow(2,s-k) &&
				mapa[r][s-k][ranuras[hp]]==-1 && mapaED[i][s-k][ranuras[hp]]){
			//if(mapa[r][s-k][h]==-1){
				int h = ranuras[hp];
				//cout << "Asigna: " << i+1 << " " << j+1 << " " << s << " " << h+1 << endl;
				ranuras.erase(ranuras.begin()+hp);
				ranuras.push_back(h);
				//cout << i+1 << " " << j+1 << " " << r+1 << " " << s << " " << h+1 << endl;
				encontro = true;
				sinAsignar--;
				vector<int> gk = ed.Gk(s-k); //GWs que ve el ED i a SFk
				vector<int> ek = _inst->GWk(r,s-k); //EDs que ve el GW r a SFk

				//marca los slots que usa
				for(int hp = h;hp < pow(2,s-k)+h;hp++) mapa[r][s-k][hp] = i;
				//for(int hp = h;hp < pow(2,s-_k)+h;hp++) cout << "1- " << r+1 << " " << s << " " << hp+1 << " " << i+1 << endl;
				//anula los slots anteriores si no se usaban porque no entra una instancia (les pone -2)
				//for(int hp=1;hp<pow(2,s-_k) && h-hp>=0;hp++) if(mapa[r][s-_k][h-hp]==-1) cout << "2- " << r+1 << " " << s << " " << h-hp+1 << " " << -2 << endl;
				for(int hp=1;hp<pow(2,s-k) && h-hp>=0;hp++) if(mapa[r][s-k][h-hp]==-1) mapa[r][s-k][h-hp] = -2;

				//anula los slots en los gws que tambien ve el EDi para ese k (les pone i)
				for(int rp=0;rp<gk.size();rp++){
      					int rpp = gk[rp];
                        	        if(rpp!=r){
						for(int hp = h;hp < pow(2,s-k)+h;hp++){
							mapa[rpp][s-k][hp] = i; //si ya veia a otro ed lo tapa, no importa. No deberia haber abiertos aca
							//cout << "3- " << rpp+1 << " " << s << " " << hp+1 << " " << i+1 << endl;
						}
						//anula los slots anteriores si eran -1 (les pone -2)
						//for(int hp=1;hp<pow(2,s-_k) && h-hp>=0;hp++) if(mapa[rpp][s-_k][h-hp]==-1) cout << "4- " << rpp+1 << " " << s << " " << h-hp+1 << " " << -2 << endl;
						for(int hp=1;hp<pow(2,s-k) && h-hp>=0;hp++) if(mapa[rpp][s-k][h-hp]==-1) mapa[rpp][s-k][h-hp] = -2;
					}
				}

				//anula los slots en los eds que tambien ve el GW r para ese k (les pone false)
				for(int ip=0;ip<ek.size();ip++){
      					int ipp = ek[ip];
                        	        if(ipp!=i){
						for(int hp = h;hp < pow(2,s-k)+h;hp++){
							mapaED[ipp][s-k][hp] = false; //si ya veia a otro ed lo tapa, no importa. No deberia haber abiertos aca
							//cout << "5- " << ipp+1 << " " << s << " " << hp+1 << " false " << endl;
						}
						//anula los slots anteriores si eran -1 (les pone -2)
						for(int hp=1;hp<pow(2,s-k) && h-hp>=0;hp++) mapaED[ipp][s-k][h-hp] = false;
						//for(int hp=1;hp<pow(2,s-_k) && h-hp>=0;hp++) cout << "6- " << ipp+1 << " " << s << " " << h-hp+1 << " false" << endl;
					}
				}

				solGW[i][j] = r;
				solk[i][j] = s;
				solh[i][j] = h;
			}
		}
	}
}


/*
void Instancia::busquedaLocal(vector<int>& gwabiertos, vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh, vector<vector<vector<int> > >& mapa, vector<int>& cantInstGW){
	//solGW[i][j] = r : instancia j del ED i usa GW r
	//solk[i][j] = k : instancia j de ED i usa SFk
	//solh[i][j] = h : instancia j de ED i comienza a transmitir en slot h
	//mapa[r][s-_k][h] = i : ED i usa GW r con SFs en slot h
	//cantInstGW[r] : cant de instancias asignadas al GW r

	timeval start;
        init_time(start);
        //while(get_time(start)<2){
	for(int iter=0;iter<40*_n;iter++){
		//Elijo una instancia al azar
		int i = mirand(_n);
		vector<int> gws = _ED[i].GW();
		int cantInst = _ED[i].cantInst();
		int T = _ED[i].T();
		int maxk = _ED[i].maxk();
		int j = mirand(cantInst);
		int rant = solGW[i][j];
		int kant = solk[i][j];
		int hant = solh[i][j];

		//le busco otro GW de los que ya estan abiertos
		bool encontro = false;
		random_shuffle(gwabiertos.begin(), gwabiertos.end(), mirand);
		for (int ri = 0; ri<gwabiertos.size() && !encontro; ri++) {
			int r = gwabiertos[ri];
			if(r!=rant && gws[r]!=-1 && (cantInstGW[rant]==1 || cantInstGW[r]>=cantInstGW[rant])){ //r es un gw abierto y factible para i, trato de asignar la instancia j
				int mink = gws[r];
				for(int s=mink;s<=maxk && !encontro;s++){
					for(int h=j*T;h<=(j+1)*T-pow(2,s-_k) && !encontro;h++){ //no se si recorrer en forma random, lo malo seria que va a dejar huecos
						if(mapa[r][s-_k][h]==-1){
							encontro = true;
							cantInstGW[rant]--;
							for(int hp=0;hp<pow(2,kant-_k);hp++){
								if(hant+hp+pow(2,kant-_k)-1<_H && (hp==0 || mapa[rant][kant-_k][hant+hp+pow(2,kant-_k)-1]==-1||
								   mapa[rant][kant-_k][hant+hp+pow(2,kant-_k)-1]==-2)){
									mapa[rant][kant-_k][hant+hp] = -1; //desocupa los slots correspondientes
								} else {
									mapa[rant][kant-_k][hant+hp] = -2; //desocupa los slots correspondientes
								}
							}
							for(int hp=1;hp<pow(2,kant-_k) && hant-hp>=0;hp++) if(mapa[rant][kant-_k][hant-hp]==-2) mapa[rant][kant-_k][hant-hp] = -1; //anula los slots correspondientes
							//hago la nuevas asignaciones
							for(int hp=0;hp<pow(2,s-_k);hp++) mapa[r][s-_k][hp+h] = i; //anula los slots correspondientes
							for(int hp=1;hp<pow(2,s-_k) && h-hp>=0;hp++) if(mapa[r][s-_k][h-hp]==-1) mapa[r][s-_k][h-hp] = -2; //anula los slots correspondientes
							cantInstGW[r]++;
							solGW[i][j] = r;
							solk[i][j] = s;
							solh[i][j] = h;
						}
					}
				}
			}
		}
		//si se vacio un GW lo saco del conjunto de abiertos
		if(cantInstGW[rant]==0){
			for(int i=0;i<gwabiertos.size();i++) if(gwabiertos[i]==rant){
				gwabiertos.erase(gwabiertos.begin()+i);
				break;
			}
			//cout << "Sol BL: " << gwabiertos.size() << "\t" << get_time(start) << "\t" << iter << endl;
		}
	}
}
*/


void Heu::mostrarSol(){
	int n = _inst->n();
        int m = _inst->m();
        int k = _inst->k();
        int K = _inst->K();
        int H = _inst->H();
	vector<EndDiv> ED = _inst->ED();

	cout << "Solucion: " << endl;
	for(int i=0;i<n;i++){
		cout << "\tED " << i+1 << ":" << endl;
                int cantInst = ED[i].cantInst();
                for(int j=0;j<cantInst;j++){
			int s = _solk[i][j];
                        cout << "\t\tInst " << j+1 << ": ";
                        cout << "GW " << _solGW[i][j]+1 << " - SF " << s << " (";
                        for(int hp=_solh[i][j];hp<_solh[i][j]+pow(2,s-k);hp++){
	                        cout << hp+1 << " ,";
                        }
                        cout << ")" << endl;
                        cout << endl;
                }
	}

	cout << "Gateways: " << endl;
	for(int j=0;j<_gwabiertos.size();j++) cout << "\t" << _gwabiertos[j]+1 << endl;

        for(int s=k;s<=K;s++){
                cout << "SF " << s << ": " << endl;
                for(int h=0;h<H;h++){
                        bool prim = false;
                        for(int i=0;i<n;i++){
                                int j = h / ED[i].T();
				if(_solk[i][j]==s && h-pow(2,s-k)<_solh[i][j] && _solh[i][j]<=h){
                                        if(!prim){
                                                cout << "\t"<< h+1 << ": " ;
                                                prim = true;
                                        }
                                        cout << i+1 << " ";
                                }
                        }
                        if(prim) cout << endl;
                }
                cout << endl;
        }

/*        for(int r=0;r<_m;r++){
                cout << "\tGW " << r+1 << ":" << endl;
                for(int s=_k;s<=_K;s++){
                        cout << "\t\tSF " << s << ": ";
                        for(int h=0;h<_H;h++){
                                if(_mapa[r][s-_k][h]!=-1 && _mapa[r][s-_k][h]!=-2) cout << _mapa[r][s-_k][h]+1 << " ";
                                else cout << "- ";
                        }
                        cout << endl;
                }
                cout << endl;
        }
*/
}
