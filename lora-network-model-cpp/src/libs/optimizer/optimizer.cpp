#include "optimizer.h"

using namespace std;
using namespace std::chrono;

const string exitCodes[6] = {
    "  Exit code [%d]: Optimization not started\n",
    "  Exit code [%d]: Timeout\n",
    "  Exit code [%d]: Iterations completed\n",
    "  Exit code [%d]: 100%% coverage reached\n",
    "  Exit code [%d]: Unknown code\n",
};

Optimizer::Optimizer(Builder builder) {

    // Import parameters from builder
    this->network = builder.network;
    this->maxIter = builder.maxIter;
    this->timeout = builder.timeout;
    this->stepMethod = builder.stepMethod;
    this->initialGW = builder.initialGW;
    this->updatePeriod = builder.updatePeriod;

    // Initialize local variables
    this->currentIter = 0;
    this->elapsed = 0;
    //this->coverage = this->network->getEDCoverage();
    this->nced = this->network->getNCEDCount();
    this->exitCode = NOT_RUN;
}

Optimizer::~Optimizer() {

}

Optimizer::Builder* Optimizer::Builder::setNetwork(Network *network) {
    this->network = network;
    return this;
}

Optimizer::Builder* Optimizer::Builder::setMaxIter(unsigned int maxIter) {
    this->maxIter = maxIter;
    return this;
}

Optimizer::Builder* Optimizer::Builder::setInitialGW(unsigned int initialGW) {
    this->initialGW = initialGW;
    return this;
}

Optimizer::Builder* Optimizer::Builder::setTimeout(unsigned int timeout) {
    this->timeout = timeout;
    return this;
}

Optimizer::Builder* Optimizer::Builder::setStepMethod(STEP_METHOD stepMethod) {
    this->stepMethod = stepMethod;
    return this;
}

Optimizer::Builder* Optimizer::Builder::setUpdatePeriod(int updatePeriod) {
    this->updatePeriod = updatePeriod;
    return this;
}

Optimizer Optimizer::Builder::build(){
    #ifdef DEBUG_MODE
        printf("--------------------------------\n");
        printf("Optimizer configuration:\n");
        printf("  Iterations: %d\n", this->maxIter);
        printf("  Timeout: %d\n", this->timeout);
        printf("  Method: %s\n", Optimizer::getStepMethodName(this->stepMethod).c_str());
        printf("--------------------------------\n\n");
    #endif

    return Optimizer(*this);
}


void Optimizer::run( void (*progressCallback)(Network *network) ) {

    steady_clock::time_point begin = steady_clock::now();

    const unsigned int mapsize = this->network->getMapSize();
    this->network->removeAllGateaways(); 
    
    // Initialize GW at random positions
    Uniform random( -(double)mapsize/2, (double)mapsize/2);
    double x, y;    
    for(unsigned int i = 0; i < this->initialGW; i++){
        random.setRandom(x, y);
        this->network->createGateway(x, y);
    }
    this->network->autoConnect();

    this->exitCode = MAX_ITER;
    const unsigned long int timeoutms = (unsigned long int) this->timeout * 1000;
    
    unsigned int noProgressStepCounter = 0; // Number of iterations without significative progress
    const long int progressThres = 1 + this->network->getEDCount()/1000; // New connected devices number to be considered as progress
    const unsigned int addGWAfter = 10; // Number of iterations without progress after which a new gateway is added

    #ifdef DEBUG_MODE
        printf("--------------------------------\n");
        printf("Optimizer started:\n");        
        printf("  Initial number of GW: %d\n", this->initialGW);
        printf("  Initial coverage: %.2f%%\n", this->network->getEDCoverage());
        printf("  Progress thresshold: %ld (min. new connected EDs per iteration)\n", progressThres);
        printf("  Add gateway after: %d steps with no progress\n", addGWAfter);
        printf("--------------------------------\n\n");
    #endif
    
    for(this->currentIter = 0; this->currentIter < this->maxIter; this->currentIter++){
        
        switch (this->stepMethod) {
            case SPRINGS:
                this->network->stepSprings();
                break;
            case RANDOM:
                this->network->stepRandom();
                break;
            default:
                break;
        }

        if((this->currentIter % this->updatePeriod == 0) && progressCallback != nullptr)
            progressCallback(this->network);

        // Exit condition: timeout
        steady_clock::time_point end = steady_clock::now();
        this->elapsed = (unsigned long int) duration_cast<milliseconds>(end - begin).count();
        if(this->elapsed > timeoutms){
            this->exitCode = TIMEOUT;
            break;
        }

        // Objective function: gw network coverage
        //double newCoverage = this->network->getEDCoverage();
        //double coverageDiff = newCoverage - this->coverage;
        //this->coverage = newCoverage;

        unsigned int newNCED = this->network->getNCEDCount();
        long int ncedDiff =  (long int) this->nced - (long int) newNCED;
        this->nced = newNCED;

        #if defined(DEBUG_MODE) && !defined(EMSCRIPTEN)
            printf("\r");
            printf(
                "Iteration %d (%.2f %%) -- not connected: %d (change: %ld)", 
                this->currentIter, 
                this->currentIter/(double)this->maxIter*100, 
                newNCED,
                ncedDiff
            );
        #endif

        if(newNCED == 0){ // If not connected EDs is 0, then max coverage reached
            this->exitCode = MAX_COVERAGE;
            break;
        }else{
            if(ncedDiff < progressThres)
                noProgressStepCounter++;
            if(noProgressStepCounter > addGWAfter){  
                // Determine number of gw to add
                
                random.setRandom(x, y); // x, and y are declared at initialization
                this->network->createGateway(x, y);
                noProgressStepCounter = 0;
                #ifdef DEBUG_MODE
                    printf("New gateway added at (%f, %f)\n", x, y);
                #endif
            }
        }
    }

    // Apply coloring algorithm to graph of gateways
    this->network->configureGWChannels();

    if(progressCallback != nullptr)
        progressCallback(this->network);
}

string Optimizer::getExitCodeName(EXIT_CODE exitCode) {
    switch(exitCode){
        case NOT_RUN:
            return "Not run";
        case TIMEOUT:
            return "Timeout";
        case MAX_ITER:
            return "Max. iterations";
        case MAX_COVERAGE:
            return "Max. coverage";
        default:
            return "Unknown";
    }
}

string Optimizer::getExitCodeName() {
    return this->getExitCodeName(this->exitCode);
}

string Optimizer::getStepMethodName(STEP_METHOD stepMethod) {
    switch(stepMethod){
        case SPRINGS:
            return "Springs";
        case RANDOM:
            return "Random";
        default:
            return "Unknown";
    }
}

string Optimizer::getStepMethodName() {
    return this->getStepMethodName(this->stepMethod);
}

void Optimizer::printResults() {
    printf("\n------------------------\n");
    printf("Results:\n");
    printf("  Iterations: %d\n",this->currentIter);
    printf("  Elapsed: %ld ms\n",this->elapsed);
    printf("  Gateways: %d\n",this->network->getGWCount());
    printf("  Used channels: %d\n",this->network->getMinChannels());
    printf("  Average utilization factor: %f\n",this->network->getAvgUF());
    printf("  Coverage: %.2f %% \n",this->network->getEDCoverage()*100.0);
    printf(exitCodes[this->exitCode].c_str(),this->exitCode);
    printf("\n------------------------\n\n");
}

void Optimizer::exportFullResults(char *filename) {
    FILE *file = fopen(filename, "w");
    if(file == NULL){
        printf("Error opening file %s\n", filename);
        return;
    }
    fprintf(file, "Optimization results:\n");
    fprintf(file, "  Network improve method: %s\n", this->getStepMethodName().c_str());
    fprintf(file, "  Iterations performed: %d\n", this->currentIter);
    fprintf(file, "  Elapsed: %lu ms\n", this->elapsed);
    fprintf(file, exitCodes[this->exitCode].c_str(), this->exitCode);
    this->network->printNetworkStatus(file);

    fclose(file);
}

void Optimizer::appendToLog(char *filename) {

    FILE *file = fopen(filename, "r");
    bool printHeader = file == NULL;

    FILE *logfile = fopen(filename, "a");
    if(logfile == NULL){
        printf("Error opening log file.\n");
        return;
    }
    if(printHeader)
        fprintf(logfile, "Method,Pos. distr.,Period distr., Map size, ED, Dens, Max. SF, GW, Channels, Coverage, Avg. UF, Elapsed, Iters, Exit cond.\n");
    
    const unsigned int mapSize = this->network->getMapSize();
    const unsigned int netSize = this->network->getEDCount();

    fprintf(logfile, "%s,%s,%s,%d,%d,%.2f,%d,%d,%d,%.2f,%.2f,%lu,%d,%s\n", 
        this->getStepMethodName().c_str(),
        this->network->getPosDistName().c_str(),
        this->network->getPeriodDistName().c_str(),
        mapSize, 
        netSize, 
        (double) netSize / (double) mapSize / (double) mapSize,
        this->network->getMaxSF(),
        this->network->getGWCount(), 
        this->network->getMinChannels(),
        this->network->getEDCoverage()*100,
        this->network->getAvgUF(),
        this->elapsed,
        this->currentIter,
        this->getExitCodeName().c_str()
    );

    fclose(logfile);
}
