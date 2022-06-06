
#include <cstdlib>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <sys/errno.h>
#include <dirent.h>
#include <algorithm>
#include "lib/instancia.h"
#include "lib/ilp.h"

using namespace std;
//typedef pair<int,int> pii;

Ilp::Ilp(Instancia& inst):_inst(&inst){
}

void Ilp::cantGW(int c){
	try {
                _env = IloEnv();
                _model = IloModel(_env);
                _solver = IloCplex(_env);
                //_solver.setParam(IloCplex::MIPDisplay, 0);
                //_solver.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 0.01);
                //_solver.setParam(IloCplex::Param::Preprocessing::Presolve, 0);
                //_solver.setParam(IloCplex::Param::Preprocessing::Linear, 0);
                //_solver.setParam(IloCplex::Param::Emphasis::MIP, CPX_MIPEMPHASIS_FEASIBILITY);
                _vars = IloNumVarArray(_env);
                _rests = IloRangeArray(_env);
                _objfun = IloObjective(IloMinimize(_env));
                resolver(c);
                _objfun.end();
                _rests.clear();
                _rests.end();
                _vars.endElements();
                _vars.end();
                _solver.end();
                _model.end();
                _env.end();

        }
        catch (IloException& e) {
                cerr << "ERRORRRR: " << e.getMessage() << endl;
        }
        catch (...) {
        cerr << "Error" << endl;
        }
}

Ilp::~Ilp(){
}


void Ilp::mostrarSol(IloNumArray& sol){
/*cc	int n = _inst->n();
	int m = _inst->m();
	int k = _inst->k();
	int K = _inst->K();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED();

	for(int i=0;i<n;i++){
		cout << "\tED " << i+1 << ":" << endl;
		int cantInst = ED[i].cantInst();
		int T = ED[i].T();
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();
                for(int j=0;j<cantInst;j++){
			cout << "\t\tInst " << j+1 << ": ";
			for(int r=0;r<m;r++) if(ED[i].gw(r)!=-1) for(int s=mink;s<=maxk;s++) if(_Y[i][j][s-k][r]!=-1 && sol[_Y[i][j][s-k][r]]>0.9999){
				cout << "GW" << r+1 << " - SF " << s << " (";
				for(int h=j*T;h<=(j+1)*T-pow(2,s-k);h++) if(_X[i][j][h][s-k]!=-1 && sol[_X[i][j][h][s-k]]>0.00001){
					for(int hp=h;hp<h+pow(2,s-k);hp++){
						cout << hp+1 << " ,";
					}
					cout << ")" << endl;
				}
			}
			cout << endl;
		}
	}


	//cout << "Gateways: " << endl;
	for(int r=0;r<m;r++)if(sol[_W[r]]>0.00001){
		cout << "\tGW " << r+1 << ": " << endl;
		for(int s=k;s<=K;s++){
			cout << "\t\tSF " << s << ": " << endl;

			for(int i=0;i<n;i++){
				int cantInst = ED[i].cantInst();
				for(int j=0;j<cantInst;j++) if(_Y[i][j][s-k][r]!=-1 && sol[_Y[i][j][s-k][r]]>0.00001){
					cout << "\t\t\t" << i+1 << " " << j+1 << endl;
				}
			}
		}
		cout << endl;
	}
*/
}


void Ilp::resolver(int c){
   	//timeval start;
        //init_time(start);

	//set<int> gws;
	//vector<vector<pii> > asignacion;

        int cantVar = armarLP(c);

        _solver.extract(_model);
        _solver.exportModel("modelo.lp");
	//_solver.resetTime();
        _solver.solve();
	cout << "Status: " <<_solver.getStatus() << endl;
    	//double tiempo = _solver.getTime();
	//cout << "Tiempo: " <<  get_time(start) << endl << endl;
    	try{
		double objval = _solver.getObjValue();
		cout << "FO: " << objval << endl << endl;
        	IloNumArray sol(_env);
        	_solver.getValues(sol,_vars);
		mostrarSol(sol);
      	} catch(IloException& e){
                _env.out() << " ERROR: " << e.getMessage() << endl;
        }

        _solver.clearModel();
}

int Ilp::armarLP(int c){
cout << "Vars" << endl;
        int cantVar = agregarVars(c);
cout << "Vars" << endl;
        calcularFO(cantVar,c);
cout << "FO" << endl;
        //generacion del ieq para ver facetas
        int cantRest = agregarRests(c);
cout << "Rest" << endl;
        _model.add(_objfun);
        _model.add(_rests);
	return cantVar;

}

int Ilp::agregarVars(int c){
        int cont = 0;
	int n = _inst->n(c);
	int m = _inst->m(c);
	int k = _inst->k();
	int K = _inst->K();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED(c);

        //variables w_r
        _W = vector<int>(m,-1);
        for(int r=0;r<m;r++){
		char* nombre = new char[40];
                sprintf(nombre,"w_%d",r+1);
                _vars.add(IloNumVar(_env,0,1,ILOBOOL,nombre));
                _W[r] = cont++;
		//cout << "W_" << r+1 << " : " << cont << endl;
                delete[] nombre;
        }

        //variables x_ijhk
        _X = vector<vector<vector<vector<int> > > >(n);

        for(int i=0;i<n;i++) {
		int cantInst = ED[i].cantInst();
		int T = ED[i].T();
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();
		_X[i] = vector<vector<vector<int> > >(cantInst,vector<vector<int> >(H,vector<int>(K-k+1,-1)));
		for(int j=0;j<cantInst;j++){
			for(int s=mink;s<=maxk;s++)for(int h=j*T;h<=(j+1)*T-pow(2,s-k);h++){
				char* nombre = new char[40];
	        	        sprintf(nombre,"x_%d_%d_%d_%d",i+1,j+1,h+1,s);
       		       		_vars.add(IloNumVar(_env,0,1,ILOBOOL,nombre));
                		_X[i][j][h][s-k] = cont++;
				//cout << "Y_" << i+1 << "_" << j+1 << "_" << s << "_" << h+1 << " : " << cont << endl;
	               		delete[] nombre;
			}
		}
        }

        //variables y_ijkr
        _Y = vector<vector<vector<vector<int> > > >(n);

        for(int i=0;i<n;i++) {
		int cantInst = ED[i].cantInst();
		int T = ED[i].T();
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();
		_Y[i] = vector<vector<vector<int> > >(cantInst,vector<vector<int> >(K-k+1,vector<int>(m,-1)));
		for(int j=0;j<cantInst;j++){
			for(int r=0;r<m;r++) if(ED[i].gw(r)!=-1) for(int s=ED[i].gw(r); s<=ED[i].maxk();s++){
				char* nombre = new char[40];
	        	        sprintf(nombre,"y_%d_%d_%d_%d",i+1,j+1,s,r+1);
       		       		_vars.add(IloNumVar(_env,0,1,ILOBOOL,nombre));
                		_Y[i][j][s-k][r] = cont++;
				//cout << "Y_" << i+1 << "_" << j+1 << "_" << s << "_" << h+1 << " : " << cont << endl;
	               		delete[] nombre;
			}
		}
        }


	return cont;
}

void Ilp::calcularFO(int cantVar,int c){
	int m = _inst->m(c);
        IloNumArray fo(_env,cantVar);

//        for(int i=0;i<cantVar;i++) fo[i] = 1;

        for(int i=0;i<cantVar;i++) fo[i] = 0;

        for(int r=0;r<m;r++){
                fo[_W[r]] = 1;
        }

        _objfun.setSense(IloObjective::Minimize);
        _objfun.setLinearCoefs(_vars,fo);
        fo.end();
}


int Ilp::agregarRests(int c){
        int cont = 0;
	cont += restAsignacion(c);
        cont += restExclusividad1(c);
        cont += restExclusividad2(c);
	cont += restPrendeGW(c);
//	cont += restF1();
	//cont += restF2();
	//cont += restF3();

        //cout << "cantRest: " << cont << endl;

	return cont;
}

int Ilp::restAsignacion(int c){
	int n = _inst->n(c);
	int m = _inst->m(c);
	int k = _inst->k();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED(c);
	int cont = 0;

	for(int i=0;i<n;i++){
		int cantInst = ED[i].cantInst();
		int T = ED[i].T();
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();
		for(int j=0;j<cantInst;j++){
	               	char* nombre = new char[40];
		        sprintf(nombre,"Asignacion_%d_%d",i+1,j+1);
       	        	IloExpr expre(_env);
			for(int s=mink;s<=maxk;s++) for(int h=j*T;h<=(j+1)*T-pow(2,s-k);h++)if(_X[i][j][h][s-k]!=-1){
		             	expre += _vars[_X[i][j][h][s-k]];
			}
	        	_rests.add(IloRange(_env,1,expre,1,nombre));
	        	expre.end();
        	        cont++;
                	delete[] nombre;
		}
	}
	return cont;
}

int Ilp::restExclusividad1(int c){
	int n = _inst->n(c);
	int m = _inst->m(c);
	int k = _inst->k();
	int K = _inst->K();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED(c);
	int cont = 0;


	for(int i=0;i<n;i++){
		int cantInst = ED[i].cantInst();
		int T = ED[i].T();
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();

		for(int j=0;j<cantInst;j++)for(int s=mink;s<=maxk;s++){
	               	char* nombre = new char[40];
		        sprintf(nombre,"Yxclu1_%d_%d_%d",i+1,j+1,s);
       	        	IloExpr expre(_env);
			for(int h=j*T;h<=(j+1)*T-pow(2,s-k);h++)if(_X[i][j][h][s-k]!=-1){
		             	expre += _vars[_X[i][j][h][s-k]];
			}

			for(int r=0;r<m;r++) if(_Y[i][j][s-k][r]!=-1){
		             	expre += -_vars[_Y[i][j][s-k][r]];
			}

			//_rests.add(IloRange(_env,-IloInfinity,expre,0,nombre));
			_rests.add(IloRange(_env,0,expre,0,nombre));
	        	expre.end();
        	        cont++;
                	delete[] nombre;
		}
        }
	return cont;
}

int Ilp::restExclusividad2(int c){
	int n = _inst->n(c);
	int m = _inst->m(c);
	int k = _inst->k();
	int K = _inst->K();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED(c);
	int cont = 0;

       	for(int i=0;i<n;i++){
		int cantInst = ED[i].cantInst();
		int T = ED[i].T();
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();

		for(int j=0;j<cantInst;j++) for(int r=0;r<m;r++) if(ED[i].gw(r)!=-1) for(int s=ED[i].gw(r); s<=ED[i].maxk();s++) if(_Y[i][j][s-k][r]!=-1) 
			for(int h=j*T;h<=(j+1)*T-pow(2,s-k);h++)if(_X[i][j][h][s-k]!=-1){
			char* nombre = new char[40];
                	sprintf(nombre,"Yxclu2_%d_%d_%d_%d_%d",i+1,j+1,h+1,s,r+1);
       	        	IloExpr expre(_env);
			int contED = 0;

        		for(int ip=0;ip<n;ip++)if(i!=ip && ED[ip].gw(r)!=-1 && ED[ip].gw(r)<=s && s<=ED[ip].maxk()){
				for(int hp=h;hp<h+pow(2,s-k);hp++){
					int jp = hp / ED[ip].T();
					if(_X[ip][jp][hp][s-k]!=-1) {
						contED++;
						expre += _vars[_X[ip][jp][hp][s-k]];
					}
				}
			}

			if(contED > 0){
			        expre += contED*_vars[_X[i][j][h][s-k]];
		               	expre += contED*_vars[_Y[i][j][s-k][r]];

				_rests.add(IloRange(_env,-IloInfinity,expre,2*contED,nombre));
-	                	cont++;
			}
	        	expre.end();
        	        delete[] nombre;
		}
        }
	return cont;
}

int Ilp::restF1(int c){
	int n = _inst->n(c);
	int m = _inst->m(c);
	int k = _inst->k();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED(c);
	int cont = 0;

	for(int i=0;i<n;i++){
		int cantInst = ED[i].cantInst();
		int T = ED[i].T();
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();
		for(int j=0;j<cantInst;j++){
			for(int s=mink;s<=maxk;s++){
		               	char* nombre = new char[40];
			        sprintf(nombre,"F1_%d_%d_%d",i+1,j+1,s);
       	        		IloExpr expre(_env);
				for(int r=0;r<m;r++) if(ED[i].gw(r)!=-1 && ED[i].gw(r)<=s){
			             	expre += _vars[_W[r]];
				}
				for(int sp=s+1;sp<=maxk;sp++) for(int h=j*T;h<=(j+1)*T-pow(2,sp-k);h++)if(_Y[i][j][sp-k][h]!=-1){
			             	expre += _vars[_Y[i][j][sp-k][h]];
				}
		        	_rests.add(IloRange(_env,1,expre,IloInfinity,nombre));
		        	expre.end();
        		        cont++;
                		delete[] nombre;
			}
		}
	}
	return cont;
}

int Ilp::restF2(int c){
	int n = _inst->n(c);
	int m = _inst->m(c);
	int k = _inst->k();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED(c);
	int cont = 0;

	for(int i=0;i<n;i++){
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();
		int T = ED[i].T();
		for(int ip=i+1;ip<n;ip++){
			int maxkp = ED[ip].maxk();
			int minkp = ED[ip].mink();
			int Tp = ED[ip].T();
			for(int s=max(mink,minkp);s<=min(maxk,maxkp);s++){
				bool hay = false;
				for(int r=0;r<m && !hay;r++) if(ED[i].gw(r)!=-1 && ED[i].gw(r)<=s && ED[ip].gw(r)!=-1 && ED[ip].gw(r)<=s) hay = true;
				if(hay){
					for(int h=0;h<H;h++){
		        	       		char* nombre = new char[40];
						sprintf(nombre,"F2_%d_%d_%d_%d",i+1,ip+1,s,h);
						int j = h / ED[i].T();
						int jp = h / ED[ip].T();
       		        			IloExpr expre(_env);
						int contr = 0;
						for(int r=0;r<m;r++) if(ED[i].gw(r)!=-1 && ED[i].gw(r)<=s && ED[ip].gw(r)!=-1 && ED[ip].gw(r)<=s){
							//cout << "r: " << r << endl;
					             	expre += _vars[_W[r]];
							contr++;
						}
						if(contr>0){
	   						for(int hp=h; hp>h-pow(2,s-k) && hp>=0 ;hp--){
								if(_Y[i][j][s-k][hp]!=-1) expre += contr * _vars[_Y[i][j][s-k][hp]];
								if(_Y[ip][jp][s-k][hp]!=-1) expre += contr * _vars[_Y[ip][jp][s-k][hp]];
							}
					        	_rests.add(IloRange(_env,-IloInfinity,expre,2*contr,nombre));
					        	expre.end();
        					        cont++;
						}
						delete[] nombre;
					}
				}
			}
		}
	}
	return cont;
}

int Ilp::restF3(int c){
	int n = _inst->n(c);
	int m = _inst->m(c);
	int k = _inst->k();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED(c);
	int cont = 0;

	for(int i=0;i<n;i++){
		int maxk = ED[i].maxk();
		int mink = ED[i].mink();
		int T = ED[i].T();
		for(int ip=i+1;ip<n;ip++){
			int maxkp = ED[ip].maxk();
			int minkp = ED[ip].mink();
			int Tp = ED[ip].T();
			for(int s=max(mink,minkp);s<=min(maxk,maxkp);s++){
				bool hay12 = true;
				bool hay21 = true;
				for(int r=0;r<m;r++){
					if(ED[i].gw(r)!=-1 && ED[i].gw(r)<=s && (ED[ip].gw(r)==-1 || ED[ip].gw(r)>s)) hay12 = false;
					if((ED[i].gw(r)==-1 || ED[i].gw(r)>s) && ED[ip].gw(r)!=-1 && ED[ip].gw(r)<=s) hay21 = false;
				}
				if(hay12 || hay21){
					for(int h=0;h<H;h++){
		        	       		char* nombre = new char[40];
						sprintf(nombre,"F2_%d_%d_%d_%d",i+1,ip+1,s,h);
						int j = h / ED[i].T();
						int jp = h / ED[ip].T();
       		        			IloExpr expre(_env);
   						for(int hp=h; hp>h-pow(2,s-k) && hp>=0 ;hp--){
							if(_Y[i][j][s-k][hp]!=-1) expre += _vars[_Y[i][j][s-k][hp]];
							if(_Y[ip][jp][s-k][hp]!=-1) expre += _vars[_Y[ip][jp][s-k][hp]];
						}
				        	_rests.add(IloRange(_env,-IloInfinity,expre,1,nombre));
				        	expre.end();
       					        cont++;
						delete[] nombre;
					}
				}
			}
		}
	}
	return cont;
}


int Ilp::restPrendeGW(int c){
	int n = _inst->n(c);
	int m = _inst->m(c);
	int k = _inst->k();
	int K = _inst->K();
	int H = _inst->H();
	vector<EndDiv> ED = _inst->ED(c);
	int cont = 0;

	for(int r=0;r<m;r++){
            	char* nombre = new char[40];
       	        sprintf(nombre,"GW_%d",r+1);
   	        IloExpr expre(_env);
		int cantInstTot = 0;
		for(int i=0;i<n;i++) if(ED[i].gw(r)!=-1){
			int mink = ED[i].mink();
			int maxk = ED[i].maxk();
			int T = ED[i].T();
			int cantInst = ED[i].cantInst();
			cantInstTot += cantInst;

			for(int s=mink;s<=maxk;s++) if(ED[i].gw(r)<=s) for(int j=0;j<cantInst;j++) if(_Y[i][j][s-k][r]!=-1){
       	       			expre += _vars[_Y[i][j][s-k][r]];
			}
		}
		if(cantInstTot > 0){
	        	expre -= cantInstTot * _vars[_W[r]];
		        _rests.add(IloRange(_env,-IloInfinity,expre,0,nombre));
	                cont++;
		}
        	expre.end();
	        delete[] nombre;
	}
	return cont;
}

