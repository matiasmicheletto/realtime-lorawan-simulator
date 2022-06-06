#ifndef INSTANCIA_H
#define INSTANCIA_H
//#include <ilcplex/ilocplex.h>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <set>
#include <boost/date_time.hpp>
#include "ed.h"
#include "util.h"

class Instancia{
	public:
		Instancia(int,int,int,int,int,int,int,int,int);
  		~Instancia();

		int m(int);
		int n(int);
		int k();
		int K();
		int H();
		int cantCanales();
		int cont2(int);
		int cont3(int);
		int cont4(int);
		vector<EndDiv>& ED(int);
		vector<int>& GWk(int,int,int);
		int coordxGW(int,int);
		int coordyGW(int,int);

	private:
		//lectura de la instancia
		void crearInstancia(int,int,int);

		//variables internas
		int _nt; //cant de ED
		int _mt; //cant de GW
		int _k; //min de SFk
		int _K; //max de SFk
		int _H; //hiperperiodo
		int _cantCanales; //cantidad de canales
		vector<vector<EndDiv> > _ED;
		vector<vector<pii> > _GWs;
   		vector<vector<vector<vector<int> > > > _GWk; //para cada gw, para cada k, guarda lo eds que ve


	        int _tipo;
                vector<int> _cont1;
                vector<int> _cont2;
                vector<int> _cont3;
                vector<int> _cont4;

};

#endif




