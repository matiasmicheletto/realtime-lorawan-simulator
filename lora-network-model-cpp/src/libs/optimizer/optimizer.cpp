#include "optimizer.h"

using namespace std;
using namespace std::chrono;

const string exitCodes[6] = {
    "  Exit code [%d]: Optimization not started\n",
    "  Exit code [%d]: Timeout\n",
    "  Exit code [%d]: 100%% coverage reached\n",
    "  Exit code [%d]: Iterations completed\n",
    "  Exit code [%d]: Maximum gateways reached\n",
    "  Exit code [%d]: Unknown code\n",
};

Optimizer::Optimizer(Builder builder) {

    // Import parameters from builder
    this->network = builder.network;
    this->maxIter = builder.maxIter;
    this->timeout = builder.timeout;
    this->stepMethod = builder.stepMethod;
    this->updatePeriod = builder.updatePeriod;

    // Initialize local variables
    this->currentIter = 0;
    this->elapsed = 0;
    this->nced = this->network->getEDCount();
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

    // Begin with only one gateway
    this->network->removeAllGateaways(); 
    this->network->createGateway();

    const unsigned long int timeoutms = (unsigned long int) this->timeout * 1000;
    unsigned int suboptimalSteps = 0;
    
    this->exitCode = MAX_ITER;
    for(this->currentIter = 0; this->currentIter < this->maxIter; this->currentIter++){
        
        switch (this->stepMethod) {
            case SPRINGS:
                this->network->stepSprings();
                break;
            case RANDOM:
                this->network->stepRandom();
                break;
            case RANDOM_PRESERVE:
                this->network->stepRandomPreserve();
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
        unsigned int newNCED = this->network->getNCEDCount();
        long int ncedDiff = newNCED - this->nced;
        this->nced = newNCED;

        #ifdef DEBUG_MODE
            printf("\r");
            printf(
                "Iteration %d (%.2f %%) -- NCED: %u (change: %ld)", 
                this->currentIter, 
                this->currentIter/(double)this->maxIter*100, 
                newNCED,
                ncedDiff
            );
        #endif

        if(newNCED == 0){
            this->exitCode = MAX_COVERAGE;
            break;
        }else{
            if(ncedDiff < 50)
                suboptimalSteps++;
            if(suboptimalSteps > 15){  
                this->network->createGateway();
                suboptimalSteps = 0;
            }
        }
    }

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
        case RANDOM_PRESERVE:
            return "Random (preserve)";
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
    FILE *logfile = fopen(filename, "a");
    if(logfile == NULL){
        printf("Error opening log file.\n");
        return;
    }
    fprintf(logfile, "%s,%s,%s,%d,%d,%d,%.2f,%lu,%d,%s\n", 
        this->getStepMethodName().c_str(),
        this->network->getPosDistName().c_str(),
        this->network->getPeriodDistName().c_str(),
        this->network->getMapSize(), 
        this->network->getGWCount(), 
        this->network->getEDCount(), 
        this->network->getEDCoverage()*100,
        this->elapsed,
        this->currentIter,
        this->getExitCodeName().c_str()
    );
    fclose(logfile);
}
