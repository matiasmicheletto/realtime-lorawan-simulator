#include <emscripten.h>
#include "libs/random/uniform.h"
#include "libs/random/custom.h"
#include "libs/network/gateway.h"
#include "libs/network/enddevice.h"
#include "libs/network/network.h"
#include "libs/optimizer/optimizer.h"

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
    unsigned int edgeslen,
    unsigned int gw,
    unsigned int ced,
    unsigned int nced,
    unsigned int iter,
    unsigned char maxSF
),{
    var xBuffer = Module.HEAPF64.subarray(x / 8, (x + nodeslen * 8) / 8);
    var yBuffer = Module.HEAPF64.subarray(y / 8, (y + nodeslen * 8) / 8);
    var idBuffer = Module.HEAPU32.subarray(id / 4, (id + nodeslen * 4) / 4);
    var groupBuffer = Module.HEAPU8.subarray(group, (group + nodeslen));

    var fromBuffer = Module.HEAPU32.subarray(from / 4, (from + edgeslen * 4) / 4);
    var toBuffer = Module.HEAPU32.subarray(to / 4, (to + edgeslen * 4) / 4);
    var sfBuffer = Module.HEAPU8.subarray(sf, (sf + edgeslen));

    var xArray = new Float64Array(xBuffer);
    var yArray = new Float64Array(yBuffer);
    var idArray = new Uint32Array(idBuffer);
    var groupArray = new Uint8Array(groupBuffer);
    var fromArray = new Uint32Array(fromBuffer);
    var toArray = new Uint32Array(toBuffer);
    var sfArray = new Uint8Array(sfBuffer);

    var nodes = [];
    var edges = [];

    for(var i = 0; i < nodeslen; i++) {                    
        nodes[idArray[i]] = {
            x: (xArray[i]/gmapSize+.5),
            y: (yArray[i]/gmapSize+.5),
            group: groupArray[i],
            color: Module.nodeColors[groupArray[i]]
        };
    }

    for(var i = 0; i < edgeslen; i++) {
        edges.push({
            from: fromArray[i],
            to: toArray[i],
            sf: sfArray[i],
            color: Module.edgeColors[sfArray[i]]
        });
    }

    Module.onNetworkUpdate(nodes, edges, gw, ced, nced, iter, maxSF);   

    Module._free(x);
    Module._free(y);
    Module._free(id);
    Module._free(group);
    Module._free(from);
    Module._free(to);
    Module._free(sf);

});

EM_JS(void, updateResults, (
    unsigned int iters,
    unsigned long int elapsed,
    unsigned char exitCode,
    unsigned int gws,
    double coverage,
    int channels
),{
    const exitCodes = [
        "Optimization not started",
        "Timeout",
        "Iterations completed",
        "100% Coverage reached"
    ];    
    Module.onResultsUpdate(
        iters,
        elapsed,
        exitCodes[exitCode],
        gws,
        coverage,
        channels
    );
});


// Callback for the optimizer step
void progressCallback(Network *network, unsigned int iter) {
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
    updateNetwork(x, y, id, group, from, to, sf, nodeslen, edgeslen, gwlen, edgeslen, edlen-edgeslen, iter, network->getMaxSF());
}


extern "C" {
    void runSimulation(
            unsigned int mapSize,
            unsigned int networkSize,
            unsigned int initialGW,
            unsigned char maxSF,
            unsigned char posDist,
            unsigned char periodDist,
            unsigned int maxIter,
            unsigned int timeout,
            unsigned char alg,
            unsigned int updatePeriod
        ) {

        Network::Builder *networkBuilder = Network::Builder()
            .setPositionGenerator((POS_DIST) posDist)
            ->setPeriodGenerator((PERIOD_DIST) periodDist)
            ->setMapSize(mapSize)
            ->setNetworkSize(networkSize)
            ->setMaxSF(maxSF);
        Network* network = new Network(networkBuilder->build());

        Optimizer::Builder *builder = Optimizer::Builder()
            .setNetwork(network)
            ->setInitialGW(initialGW)
            ->setMaxIter(maxIter)
            ->setTimeout(timeout)
            ->setStepMethod((STEP_METHOD) alg)
            ->setUpdatePeriod(updatePeriod); 
        Optimizer* optimizer = new Optimizer(builder->build());
        
        optimizer->run(progressCallback);
        updateResults(
            optimizer->getIterations(),
            optimizer->getElapsed(),
            (unsigned char) optimizer->getExitCode(),
            network->getGWCount(),
            network->getEDCoverage(),
            network->getMinChannels()
        );
        delete optimizer;
        delete network;
    }
}
