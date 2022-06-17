#include "optimizerinterface.h"

// JavaScript side callbacks

EM_JS(void, updateNetwork, (
    double* x, 
    double* y, 
    unsigned int* id, 
    unsigned char* group,
    unsigned int* to,
    unsigned int* from,
    unsigned char* sf,
    unsigned int nodeslen,
    unsigned int edgeslen
), {
    // This callback should be defined in the JS side
    onNetworkUpdate(x, y, id, group, from, to, sf, nodeslen, edgeslen); 
});

EM_JS(void, updateResults, (
    unsigned int iters,
    unsigned long int elapsed,
    double coverage,
    unsigned char exitCode
), {
    // This callback should be defined in the JS side
    onResultsUpdate(iters, elapsed, coverage, exitCode); 
});

// Callback for the optimizer step
void updateCallback(vector<Gateway*>* gws, vector<EndDevice*>* eds) {
    // Compute number of nodes and edges
    unsigned int gwlen = gws->size();
    unsigned int edlen = eds->size();
    unsigned int nodeslen = gwlen + edlen;
    unsigned int edgeslen = 0;
    for(unsigned int i = 0; i < edlen; i++)
        if(eds->at(i)->isConnected())
            edgeslen++;
    
    // Allocate memory for arrays
    double *x = (double*) malloc(sizeof(double) * nodeslen);
    double *y = (double*) malloc(sizeof(double) * nodeslen);
    unsigned int *id = (unsigned int*) malloc(sizeof(unsigned int) * nodeslen);
    unsigned char *group = (unsigned char*) malloc(sizeof(unsigned char) * nodeslen);
    unsigned int *from = (unsigned int*) malloc(sizeof(unsigned int) * edgeslen);
    unsigned int *to = (unsigned int*) malloc(sizeof(unsigned int) * edgeslen);
    unsigned char *sf = (unsigned char*) malloc(sizeof(unsigned char) * edgeslen);

    // Fill arrays
    unsigned int j = 0;
    for (unsigned int i = 0; i < edlen; i++) {
        // For nodes
        x[i] = eds->at(i)->getX();
        y[i] = eds->at(i)->getY();
        id[i] = eds->at(i)->getId();
        group[i] = eds->at(i)->isConnected() ? 1 : 0; // 0 = NCED, 1 = ED, 2 = GW
        // For edges
        if(eds->at(i)->isConnected()) {
            from[j] = eds->at(i)->getId();
            to[j] = eds->at(i)->getGatewayId();
            sf[j] = eds->at(i)->getSF();
            j++;
        }
    }
    for(unsigned int i = 0; i < gwlen; i++) {
        // For nodes
        x[i + edlen] = gws->at(i)->getX();
        y[i + edlen] = gws->at(i)->getY();
        id[i + edlen] = gws->at(i)->getId();
        group[i + edlen] = 2;
    }
    updateNetwork(x, y, id, group, from, to, sf, nodeslen, edgeslen);
}


OptimizerInterface::OptimizerInterface(
    unsigned int mapSize,
    unsigned int networkSize,
    unsigned int maxIter,
    unsigned int timeout,
    unsigned char alg,
    int updatePeriod) {
    
    Uniform posDist = Uniform(-(double) mapSize/2, (double) mapSize/2);
    CustomDist periodDist = CustomDist::Builder()
        .addValue(3600, 0.97)
        ->addValue(1800, 0.01)
        ->addValue(900, 0.02)
        ->build();

    NetworkOptimizer::Builder *builder = NetworkOptimizer::Builder()
        .setPositionGenerator(&posDist)
        ->setPeriodGenerator(&periodDist)
        ->setMapSize(mapSize)
        ->setNetworkSize(networkSize)
        ->setMaxIter(maxIter)
        ->setTimeout(timeout)
        ->setStepMethod((STEP_METHOD) (alg-1))
        ->setUpdatePeriod(updatePeriod); 
    
    this->opt = new NetworkOptimizer(builder->build());
}

OptimizerInterface::~OptimizerInterface() {
    delete this->opt;
}

void OptimizerInterface::run() {
    this->opt->run(updateCallback);
    updateResults(
        this->opt->getIterations(),
        this->opt->getElapsed(),
        this->opt->getEDCoverage(),
        (unsigned char)this->opt->getExitCode()
    );
}
