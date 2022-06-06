#ifndef ED_H
#define ED_H
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <set>
#include <boost/date_time.hpp>

using namespace std;
//typedef pair<int,int> pii;

class EndDiv{
	public:
		EndDiv();
		EndDiv(int T, int,vector<int>& gw,vector<vector<int> >& gk,int mink,int maxk,int cantGW);
  		~EndDiv();

		//comunicacion con isntancia
		int T();
		int cantInst();
		vector<int>& GW();
		int gw(int);
		int maxk();
		int mink();
		int cantGW();
		vector<int>& Gk(int);

	private:

		//variables internas
		int _T;
		int _cantInst;
		int _maxk; //para que entre 
		int _mink; //min k al que ve algun gw
		int _cantGW; //cant de gw que ve (a algun k)
		vector<int> _GW; //para cada gw tiene desde que k lo ve, -1 si no ve al gw
		vector<vector<int> > _Gk; //para cada k tiene los gws que ve
		//vector<vector<vector<bool> > > _fact;
		//vector<int> _cant;
};

#endif




