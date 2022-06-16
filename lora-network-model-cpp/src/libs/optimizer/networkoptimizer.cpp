#include "networkoptimizer.h"

using namespace std;
using namespace std::chrono;

NetworkOptimizer::NetworkOptimizer(Builder builder) {
    srand(time(NULL));

    this->enddevices = builder.enddevices;
    this->mapSize = builder.mapSize;
    this->H = builder.H;
    this->lastID = builder.lastID;
    this->maxIter = builder.maxIter;
    this->timeout = builder.timeout;
    this->stepMethod = builder.stepMethod;
    this->updatePeriod = builder.updatePeriod;

    for(int i = 0; i < 16; i++)
        this->availableChannels[i] = true;

    this->currentIter = 0;
    this->elapsed = 0;
}

NetworkOptimizer::~NetworkOptimizer() {
    for (long unsigned int i = 0; i < this->gateways.size(); i++)
        delete this->gateways[i];
    for (long unsigned int i = 0; i < this->enddevices.size(); i++) 
        delete this->enddevices[i];
    this->gateways.clear();
    this->enddevices.clear();
}

NetworkOptimizer::Builder* NetworkOptimizer::Builder::setPositionGenerator(Random *posGenerator) {
    this->posGenerator = posGenerator;
    return this;
}

NetworkOptimizer::Builder* NetworkOptimizer::Builder::setPeriodGenerator(Random *periodGenerator) {
    this->periodGenerator = periodGenerator;
    return this;
}

NetworkOptimizer::Builder* NetworkOptimizer::Builder::setMapSize(unsigned int mapSize) {
    this->mapSize = mapSize;
    return this;
}

NetworkOptimizer::Builder* NetworkOptimizer::Builder::setNetworkSize(unsigned int networkSize) {
    this->networkSize = networkSize;
    return this;
}

NetworkOptimizer::Builder* NetworkOptimizer::Builder::setMaxIter(unsigned int maxIter) {
    this->maxIter = maxIter;
    return this;
}

NetworkOptimizer::Builder* NetworkOptimizer::Builder::setTimeout(int timeout) {
    this->timeout = timeout;
    return this;
}

NetworkOptimizer::Builder* NetworkOptimizer::Builder::setStepMethod(STEP_METHOD stepMethod) {
    this->stepMethod = stepMethod;
    return this;
}

NetworkOptimizer::Builder* NetworkOptimizer::Builder::setUpdatePeriod(int updatePeriod) {
    this->updatePeriod = updatePeriod;
    return this;
}

NetworkOptimizer NetworkOptimizer::Builder::build(){
    // Period values and probabilities for the end devices
    this->H = 1; // Initially 1 and will be updated when the periods are generated
    for (unsigned int i = 0; i < this->networkSize; i++) {
        // End device position
        double x, y; 
        this->posGenerator->setRandom(x, y);        
        // End device period
        int period = this->periodGenerator->randomInt(); 
        // Update hyperperiod
        this->H = lcm(this->H, period);
        // Add end device to the network
        EndDevice *ed = new EndDevice(x, y, this->lastID, period);
        this->enddevices.push_back(ed);
        this->lastID++;
    }
    return NetworkOptimizer(*this);
}

bool NetworkOptimizer::createGateway(double x, double y) {
    for(unsigned char channel = 0; channel < 16; channel++){
        if(this->availableChannels[channel]){
            Gateway *gw = new Gateway(x, y, this->lastID, this->H, channel);
            gateways.push_back(gw);
            this->lastID++;
            this->availableChannels[channel] = false;
            return true;
        }
    }
    printf("No more channels available!\n");
    return false;
}

bool NetworkOptimizer::removeGateway(Gateway *gw) {
    for (unsigned int i = 0; i < this->gateways.size(); i++) {
        if (this->gateways[i] == gw) {
            this->gateways.erase(this->gateways.begin() + i);
            delete this->gateways[i];
            return true;
        }
    }
    return false;
}

void NetworkOptimizer::autoConnect() {
    for(long unsigned int i = 0; i < this->enddevices.size(); i++) {
        // Sort gateways by distance
        sort(this->gateways.begin(), this->gateways.end(), [this, i](Gateway *a, Gateway *b) {
            return a->distanceTo(this->enddevices[i]) < b->distanceTo(this->enddevices[i]);
        });
        for(long unsigned int j = 0; j < this->gateways.size(); j++)
            if(this->gateways[j]->addEndDevice(this->enddevices[i]))
                break;
    }
}

void NetworkOptimizer::disconnect() {
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->disconnect();
}

void NetworkOptimizer::step() {
    switch (this->stepMethod) {
        case RANDOM:
            this->stepRandom();
            break;
        case SPRINGS:
            this->stepSprings();
            break;
        default:
            break;
    }
}

void NetworkOptimizer::stepSprings() {
    //printf("NetworkOptimizer::stepSprings()\n");
    unsigned int nced = 0; // Number of disconnected end devices
    vector<vector<int>> forces; // Attraction forces for each gateway
    for (long unsigned int i = 0; i < this->enddevices.size(); i++) {
        if (!this->enddevices[i]->isConnected()){
            nced++; // Count disconnected end device
            // Sort gateways by distance to get closest
            sort(this->gateways.begin(), this->gateways.end(), [this, i](Gateway *a, Gateway *b) {
                return a->distanceTo(this->enddevices[i]) < b->distanceTo(this->enddevices[i]);
            });
            // Compute attraction force for closest (this->gateways[0])
            int id = this->gateways[0]->getId();
            int x = (int)this->enddevices[i]->getX() - (int)this->gateways[0]->getX();
            int y = (int)this->enddevices[i]->getY() - (int)this->gateways[0]->getY();
            forces.push_back(vector<int>{id, x, y});
        }
    }
    if(nced != 0){
        // Disconnect all nodes
        this->disconnect();
        // Update gateways positions according to forces
        for(long unsigned int i = 0; i < this->gateways.size(); i++){
            // Comoute sum of forces for current gateway
            double sumX = 0, sumY = 0; 
            for(long unsigned int j = 0; j < forces.size(); j++){
                if(forces[j][0] == (int) this->gateways[i]->getId()){
                    sumX += (double) forces[j][1];
                    sumY += (double) forces[j][2];
                }
            }
            // Compute new positions
            double newX = this->gateways[i]->getX() + mclamp(sumX/(double) nced, -100.0, 100.0);
            double newY = this->gateways[i]->getY() + mclamp(sumY/(double) nced, -100.0, 100.0);
            this->gateways[i]->moveTo(newX, newY);
        }
        // Reconnect network
        this->autoConnect();
    }
}

void NetworkOptimizer::stepRandom() {
    //printf("NetworkOptimizer::stepRandom()\n");
    double originalCoverage = this->getEDCoverage();
    // Save original positions
    vector<vector<double>> originalPositions;
    for(long unsigned int i = 0; i < this->gateways.size(); i++){
        double x = this->gateways[i]->getX();
        double y = this->gateways[i]->getY();
        originalPositions.push_back(vector<double>{x, y});
    }
    this->disconnect(); 
    // Randomize positions
    for(long unsigned int i = 0; i < this->gateways.size(); i++){
        Uniform gwPosGenerator = Uniform(-(double)this->mapSize/2, (double)this->mapSize/2);
        double newX, newY;
        gwPosGenerator.setRandom(newX, newY);
        this->gateways[i]->moveTo(newX, newY);
    }
    // Check new coverage
    this->autoConnect();
    double newCoverage = this->getEDCoverage();
    if(newCoverage < originalCoverage){ // If coverage did not improve
        this->disconnect();
        // Restore original positions
        for(long unsigned int i = 0; i < this->gateways.size(); i++){
            double x = originalPositions[i][0];
            double y = originalPositions[i][1];
            this->gateways[i]->moveTo(x, y);
        }
        originalPositions.clear();
        this->autoConnect();
    }
}

double NetworkOptimizer::getEDCoverage() {
    double coverage = 0;
    for (long unsigned int i = 0; i < this->enddevices.size(); i++)
        if (this->enddevices[i]->isConnected())
            coverage++;
    return coverage / (double) this->enddevices.size();
}

void NetworkOptimizer::run( void (*updateCallback)(vector<Gateway*>*, vector<EndDevice*>*) ) {
    steady_clock::time_point begin = steady_clock::now();

    this->exitCode = NOT_RUN;

    // Remove all gateways and leave a single one
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        this->removeGateway(this->gateways[i]);
    Uniform gwPosGenerator = Uniform(-(double)this->mapSize/2, (double)this->mapSize/2);
    double x, y; 
    gwPosGenerator.setRandom(x, y);
    this->createGateway(x, y);
    this->autoConnect();

    unsigned int suboptimalSteps = 0;
    this->currentIter = 0;
    for(unsigned int i = 0; i < this->maxIter; i++){
        this->step();
        
        this->currentIter++;

        if(this->currentIter % this->updatePeriod == 0){
            if(updateCallback != nullptr)
                updateCallback(&(this->gateways), &(this->enddevices));
        }

        // Exit condition: timeout
        steady_clock::time_point end = steady_clock::now();
        this->elapsed = (unsigned int) duration_cast<seconds>(end - begin).count();
        if(this->elapsed > this->timeout){
            this->exitCode = TIMEOUT;
            break;
        }

        // Exit condition: max coverage reached
        double coverage = this->getEDCoverage();
        printf("Iteration %d: Coverage %.2f\n", i, coverage);
        if(coverage > 0.9999){
            this->exitCode = MAX_COVERAGE;
            break;
        }else{
            suboptimalSteps++;
            if(suboptimalSteps > 15){
                gwPosGenerator.setRandom(x, y);
                if(this->createGateway(x, y)){
                    this->autoConnect();
                    suboptimalSteps = 0;
                }else{
                    this->exitCode = MAX_GW;
                    break;
                }
            }
        }
    }
    this->exitCode = MAX_ITER;
}

void NetworkOptimizer::printStatus(char *filename, bool saveLog) {

    FILE *file = fopen(filename, "w");
    if(file == NULL){
        printf("Error opening file %s\n", filename);
        return;
    }
    fprintf(file, "Network status:\n");
    fprintf(file, "  Map size: %d\n", this->mapSize);
    fprintf(file, "  Gateways: %ld\n", this->gateways.size());
    fprintf(file, "  End devices: %ld\n", this->enddevices.size());
    fprintf(file, "  ED coverage: %f\n", this->getEDCoverage()*100);
    // Print gateways positions
    fprintf(file, "Gateways positions and channels:\n");
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        fprintf(file, "  #%d: (%f, %f), channel %d, UF: %f, connected to %d EDs\n", 
            this->gateways[i]->getId(), 
            this->gateways[i]->getX(), 
            this->gateways[i]->getY(),
            this->gateways[i]->getChannel(),
            this->gateways[i]->getUF(),
            this->gateways[i]->connectedEDsCount()
        );

    // Print end devices positions
    fprintf(file, "End devices positions and connections:\n");
    for(long unsigned int i = 0; i < this->enddevices.size(); i++)
        fprintf(file, "  #%d: (%f, %f), period %d, connected to Gateway #%d (at %f mts.) using SF%d \n", 
            this->enddevices[i]->getId(), 
            this->enddevices[i]->getX(), 
            this->enddevices[i]->getY(),
            this->enddevices[i]->getPeriod(),
            this->enddevices[i]->getGatewayId(),
            this->enddevices[i]->getGWDist(),
            this->enddevices[i]->getSF()
        );

    fclose(file);

    if(saveLog){
        FILE *logfile = fopen("summary.csv", "a");
        if(logfile == NULL){
            printf("Error opening log file.\n");
            return;
        }
        fprintf(logfile, "%d,%ld,%ld,%f,%d,%d,%d\n", 
            this->mapSize, 
            this->gateways.size(), 
            this->enddevices.size(), 
            this->getEDCoverage()*100,
            this->elapsed,
            this->currentIter,
            (int) this->exitCode
        );
        fclose(logfile);
    }
}