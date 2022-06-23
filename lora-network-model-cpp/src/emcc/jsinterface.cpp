#include "jsinterface.h"

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
    unsigned char exitCode,
    unsigned int gws,
    double coverage,
    int channels
), {
    // This callback should be defined in the JS side
    onResultsUpdate(iters, elapsed, exitCode, gws, coverage, channels); 
});

JsInterface::JsInterface(
    unsigned int mapSize,
    unsigned int networkSize,
    unsigned char posDist,
    unsigned char periodDist,
    unsigned int maxIter,
    unsigned int timeout,
    unsigned char alg,
    unsigned int updatePeriod) {

    Network::Builder *networkBuilder = Network::Builder()
        .setPositionGenerator((POS_DIST) posDist)
        ->setPeriodGenerator((PERIOD_DIST) periodDist)
        ->setMapSize(mapSize)
        ->setNetworkSize(networkSize);
    this->network = new Network(networkBuilder->build());

    Optimizer::Builder *builder = Optimizer::Builder()
        .setNetwork(this->network)
        ->setMaxIter(maxIter)
        ->setTimeout(timeout)
        ->setStepMethod((STEP_METHOD) alg)
        ->setUpdatePeriod(updatePeriod); 
    this->optimizer = new Optimizer(builder->build());
}

JsInterface::~JsInterface() {
    delete this->network;
    delete this->optimizer;
}

void JsInterface::run() {
    this->optimizer->run(this->progressCallback);
    int channels = this->network->configureGWChannels();
    updateResults(
        this->optimizer->getIterations(),
        this->optimizer->getElapsed(),
        (unsigned char)this->optimizer->getExitCode(),
        this->network->getGWCount(),
        this->network->getEDCoverage(),
        channels
    );
}

// Callback for the optimizer step
void JsInterface::progressCallback(Network *network) {
    // Compute number of nodes and edges
    vector<Gateway*>* gws = network->getGWs();
    vector<EndDevice*>* eds = network->getEDs();

    unsigned int gwlen = gws->size();
    unsigned int edlen = eds->size();
    unsigned int nodeslen = gwlen + edlen;
    unsigned int edgeslen = 0;
    for(unsigned int i = 0; i < edlen; i++)
        if(eds->at(i)->isConnected())
            edgeslen++;
    
    // Allocate memory for node arrays
    double *x = (double*) malloc(sizeof(double) * nodeslen);
    double *y = (double*) malloc(sizeof(double) * nodeslen);
    unsigned int *id = (unsigned int*) malloc(sizeof(unsigned int) * nodeslen);
    unsigned char *group = (unsigned char*) malloc(sizeof(unsigned char) * nodeslen);
    // Allocate memory for edge arrays
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