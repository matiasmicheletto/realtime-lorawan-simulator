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
    this->schedulingMethod = builder.schedulingMethod;
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

Optimizer::Builder* Optimizer::Builder::setSchedulingMethod(SCHED_METHOD schedulingMethod) {
    this->schedulingMethod = schedulingMethod;
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
        printf("  Scheduling method: %s\n", Optimizer::getSchedMethodName(this->schedulingMethod).c_str());
        printf("--------------------------------\n\n");
    #endif

    return Optimizer(*this);
}


void Optimizer::run( void (*progressCallback)(Network *network, unsigned int iter) ) {

    steady_clock::time_point begin = steady_clock::now();
    const unsigned int mapsize = this->network->getMapSize();
    Uniform random( -(double)mapsize/2, (double)mapsize/2);
    double x, y;    
    this->exitCode = MAX_ITER;
    const unsigned long int timeoutms = (unsigned long int) this->timeout * 1000;

    while(true) {
        // Initialize GWs at random positions 

        this->network->removeAllGateways(); 
        for(unsigned int i = 0; i < this->initialGW; i++){
            random.setRandom(x, y);
            this->network->createGateway(x, y);
        }

        this->network->autoConnect();

        unsigned int noProgressStepCounter = 0; // Number of iterations without significative progress
        const long int progressThres = 1 + this->network->getEDCount()/1000; // New connected devices number to be considered as progress
        const unsigned int addGWAfter = 10; // Number of iterations without progress after which a new gateway is added

        #ifdef DEBUG_MODE
            printf("--------------------------------\n");
            printf("Optimizer started:\n");        
            printf("  Max. SF: %d\n", this->network->getMaxSF());
            printf("  Initial number of GW: %d\n", this->initialGW);
            printf("  Initial coverage: %.2f%%\n", this->network->getEDCoverage()*100);
            printf("  Progress thresshold: %ld (min. new connected EDs per iteration)\n", progressThres);
            printf("  Add gateway after: %d steps with no progress\n", addGWAfter);
            printf("--------------------------------\n\n");
        #endif

        unsigned int gwCounter = this->initialGW;
        
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
                progressCallback(this->network, this->currentIter);

            // Exit condition: timeout
            steady_clock::time_point end = steady_clock::now();
            this->elapsed = (unsigned long int) duration_cast<milliseconds>(end - begin).count();
            if(this->elapsed > timeoutms){
                this->exitCode = TIMEOUT;
                break;
            }

            // Objective function: maximize network coverage -> nced = 0
            unsigned int newNCED = this->network->getNCEDCount();
            long int ncedDiff =  (long int) this->nced - (long int) newNCED;
            this->nced = newNCED;

            #if defined(DEBUG_MODE) && !defined(EMSCRIPTEN)
                printf("\r");
                printf(
                    "Iteration %d -- ED left: %d (added: %ld) ", 
                    this->currentIter,                 
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
                    gwCounter++;
                    #ifdef DEBUG_MODE
                        printf("\nGateway #%d added at (%f, %f)\n", gwCounter, x, y);
                    #endif
                }
            }
        }

        // Remove GWs that are not connected to any ED
        #ifdef DEBUG_MODE
            printf("\nRemoving idle gateways...\n");
        #endif
        this->network->removeIdleGateways();
        #ifdef DEBUG_MODE
            printf("Done.\n");
        #endif

        // Apply coloring algorithm to graph of gateways
        #ifdef DEBUG_MODE
            printf("Determining minimum number of cannels...\n");
        #endif
        this->network->configureGWChannels();
        if(this->network->getMinChannels() < 16 || this->network->getMaxSF() == 7){
            #ifdef DEBUG_MODE
                printf("Done. Channels = %d\n", this->network->getMinChannels());
            #endif
            break;
        }else{
            #ifdef DEBUG_MODE
                printf("Number of channels = %d, reducing max. SF to %d\n", this->network->getMinChannels(), this->network->getMaxSF()-1);
            #endif
            this->network->setMaxSF(this->network->getMaxSF() - 1);
        }
    }

    // Check if system is time-feasible
    #ifdef DEBUG_MODE
        printf("Computing schedule...\n");
    #endif
    switch (this->schedulingMethod){
        case EDF:
            this->network->compSchedulerEDF();
            break;
        case PERIOD:
            this->network->compSchedulerMinPeriod();        
        default:
            break;
    }       
    #ifdef DEBUG_MODE
        printf("Done. Unfeasible ED Instances: %d\n", this->network->getNonFeasibleEDs());
    #endif

    if(progressCallback != nullptr)
        progressCallback(this->network, this->currentIter);
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
            return "Springs (Cont.)";
        case RANDOM:
            return "Random (Cont.)";
        default:
            return "Unknown";
    }
}

string Optimizer::getStepMethodName() {
    return this->getStepMethodName(this->stepMethod);
}

string Optimizer::getSchedMethodName(SCHED_METHOD schedMethod) {
    switch(schedMethod){
        case EDF:
            return "EDF";
        case PERIOD:
            return "Min. period";
        default:
            return "Unknown";
    }
}

string Optimizer::getSchedMethodName() {
    return this->getSchedMethodName(this->schedulingMethod);
}

void Optimizer::printResults() {
    printf("\n------------------------\n");
    printf("Results:\n");
    printf("  Iterations: %d\n",this->currentIter);
    printf("  Elapsed: %ld ms\n",this->elapsed);
    printf("  Gateways: %d\n",this->network->getGWCount());
    printf("  Used channels: %d\n",this->network->getMinChannels());
    printf("  Average utilization factor: %f\n",this->network->getAvgUF());
    printf("  Unfeasible ED msg. instances: %d\n",this->network->getNonFeasibleEDs());
    printf("  Coverage: %.2f %% \n",this->network->getEDCoverage()*100.0);
    printf(exitCodes[this->exitCode].c_str(),this->exitCode);
    printf("------------------------\n\n");
}

void Optimizer::exportFullResults(char *filename) {
    FILE *file = fopen(filename, "w");
    if(file == NULL){
        printf("Error opening file %s\n", filename);
        return;
    }
    fprintf(file, "Optimization results:\n");
    fprintf(file, "  Network improve heuristic: %s\n", this->getStepMethodName().c_str());
    fprintf(file, "  Iterations performed: %d\n", this->currentIter);
    fprintf(file, "  Elapsed: %lu ms\n", this->elapsed);
    fprintf(file, exitCodes[this->exitCode].c_str(), this->exitCode);
    this->network->printNetworkStatus(file);

    fclose(file);
}

void Optimizer::appendToLog(const char *filename) {

    FILE *file = fopen(filename, "r");
    bool printHeader = file == NULL;

    FILE *logfile = fopen(filename, "a");
    if(logfile == NULL){
        printf("Error opening log file.\n");
        return;
    }
    if(printHeader)
        fprintf(logfile, "GW Pos. Heuristic,ED Sched. Method,Position distr.,Periods distr.,Map size,ED Number,ED Dens. (1/m^2),Max. SF,GW Number,Channels used,ED Unfeas. Instances,Coverage %%,GW Avg. UF,Elapsed,Iterations,Exit condition\n");
    
    const unsigned int mapSize = this->network->getMapSize();
    const unsigned int netSize = this->network->getEDCount();

    fprintf(logfile, "%s,%s,%s,%s,%d,%d,%.2f,%d,%d,%d,%d,%.2f,%.2f,%lu,%d,%s\n", 
        this->getStepMethodName().c_str(),
        this->getSchedMethodName().c_str(),
        this->network->getPosDistName().c_str(),
        this->network->getPeriodDistName().c_str(),
        mapSize, 
        netSize, 
        (double) netSize / (double) mapSize / (double) mapSize,
        this->network->getMaxSF(),
        this->network->getGWCount(), 
        this->network->getMinChannels(),
        this->network->getNonFeasibleEDs(),
        this->network->getEDCoverage()*100,
        this->network->getAvgUF(),
        this->elapsed,
        this->currentIter,
        this->getExitCodeName().c_str()
    );

    fclose(logfile);
}
