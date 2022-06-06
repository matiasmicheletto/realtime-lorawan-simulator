#ifndef HEU_H
#define HEU_H
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <set>
#include <boost/date_time.hpp>
#include "util.h"
#include "instancia.h"

class Heu{
	public:
		Heu(Instancia&);
  		~Heu();

		void cantGWTotal(int);
		void cantGW(int,int);

	private:
		int asignar(int c,vector<pii>& eds, vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh,
		        vector<vector<vector<int> > >& mapa, vector<vector<vector<bool> > >& mapaED, vector<bool>&);

		void asignarTodo(int c,int i, int r,vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh,
		        vector<vector<vector<int> > >& mapa, vector<vector<vector<bool> > >& mapaED, int& sinAsignar,vector<int>& ranuras);


		void busquedaLocal(vector<pii>& eds, vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh,
        		vector<vector<vector<int> > >& mapa, vector<int>& sinAsignar, vector<int>& gwabiertos);
		//bool asignar2(int i, vector<int>& gwabiertos, vector<int>&,vector<vector<int> >&,vector<vector<int> >&,vector<vector<int> >&,vector<vector<vector<int> > >&,vector<int>& cantInstGW);
		//void busquedaLocal(vector<int>& gwabiertos, vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh, vector<vector<vector<int> > >& mapa, vector<int>& cantInstGW);
		//void busquedaLocal2(vector<int>& gwabiertos, vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh, vector<vector<vector<int> > >& mapa, vector<int>& cantInstGW);
		//bool sePuedeVaciar(int rant, int ri,vector<pii>& gws,vector<vector<vector<int> > >& mapa,vector<int>& cantInstGW,vector<Cambio>& cambios);
		//bool busca(int i, int j,int kant,int hant,int r, int mink,int maxk,int T,vector<vector<vector<int> > >& mapa,vector<Cambio>& cambios);
		//void vaciar(int rant,vector<vector<int> >& solGW, vector<vector<int> >& solk, vector<vector<int> >& solh,
        	//	vector<vector<vector<int> > >& mapa, vector<int>& cantInstGW, vector<Cambio>& cambios);

		void mostrarSol(int c);
		void ordenar(vector<vector<pii> >& eds,int);

		//estructuras que guardan la instancia
    		Instancia* _inst;
		//int _n; //cant de ED
		//int _m; //cant de GW
		//int _k; //min de SFk
		//int _K; //max de SFk
		//int _H; //hiperperiodo
		//vector<EndDiv> _ED;
		//vector<pii> _GWs; //ubicacion de los gws
		//vector<vector<vector<int> > > _GWk; //para cada gw, para cada k, guarda lo eds que ve

		//estructuras que guardan la solucion
		vector<int> _sol;
		int _sol2;
		vector<vector<int> >_solGW;
		vector<vector<int> >_solk;
		vector<vector<int> >_solh;
		vector<vector<vector<int> > > _mapa;
		vector<bool> _gwabiertos;
};

#endif




