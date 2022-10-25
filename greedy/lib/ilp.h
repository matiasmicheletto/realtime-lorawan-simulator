#ifndef ILP_H
#define ILP_H
#include <ilcplex/ilocplex.h>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <set>
#include <boost/date_time.hpp>
#include "util.h"
#include "ed.h"
#include "instancia.h"

using namespace std;

class Ilp{
	public:
		Ilp(Instancia&);
  		~Ilp();
		void cantGW(int );

	private:
		//lp
		void resolver(int);
		int armarLP(int);
		int agregarVars(int);
		void calcularFO(int,int);
		int agregarRests(int);
		int restExclusividad1(int);
		int restExclusividad2(int);
		//int restDuracion();
		//int restDuracion2();
		//int restContinuidad();
		//int restInicio();
		int restAsignacion(int);
		int restPrendeGW(int);
		int restF1(int);
		int restF2(int);
		int restF3(int);
		void mostrarSol(IloNumArray&);

		//variables internas
		Instancia* _inst;
		vector<vector<vector<vector<int> > > >_X;
		vector<vector<vector<vector<int> > > >_Y;
		vector<int> _W;


         	//de cplex
                IloEnv _env;
                IloCplex _solver;
                IloModel _model;
                IloNumVarArray _vars;
                IloRangeArray _rests;
                IloObjective _objfun;

};

#endif




