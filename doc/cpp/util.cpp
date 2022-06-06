//#include <vector>
//#include <map>
//#include <fstream>
//#include <algorithm>
//#include <set>
#include <boost/date_time.hpp>

using namespace std;
typedef pair<int,int> pii;

int mirand(int hasta){
        return (int) (hasta * (rand() / (RAND_MAX + 1.0)));
}

void init_time(timeval& start)
{
        gettimeofday(&start,NULL);
}

double get_time(timeval& start)
{
        timeval end;
        gettimeofday(&end,NULL);
        return (1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec))/1000000.0;
}


