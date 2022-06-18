#include "network.h"

using namespace std;
using namespace std::chrono;

Network::Network(Builder builder) {
    srand((unsigned int) system_clock::now().time_since_epoch().count());

    this->enddevices = builder.enddevices;
    this->mapSize = builder.mapSize;
    this->H = builder.H;
    this->lastID = builder.lastID;
    this->posDist = builder.posDist;
    this->periodDist = builder.periodDist;

    for(int i = 0; i < 16; i++)
        this->availableChannels[i] = true;
}

Network::~Network() {
    for (unsigned int i = 0; i < this->gateways.size(); i++)
        delete this->gateways[i];
    for (unsigned int i = 0; i < this->enddevices.size(); i++) 
        delete this->enddevices[i];
    this->gateways.clear();
    this->enddevices.clear();
}

Network::Builder* Network::Builder::setPositionGenerator(POS_DIST posDist) {
    this->posDist = posDist;
    return this;
}

Network::Builder* Network::Builder::setPeriodGenerator(PERIOD_DIST periodDist) {
    this->periodDist = periodDist;
    return this;
}

Network::Builder* Network::Builder::setMapSize(unsigned int mapSize) {
    this->mapSize = mapSize;
    return this;
}

Network::Builder* Network::Builder::setNetworkSize(unsigned int networkSize) {
    this->networkSize = networkSize;
    return this;
}

Network Network::Builder::build(){

    Random* posGenerator;
    // Create the position generator function
    switch (this->posDist) {
        case UNIFORM: // Positions have a uniform distribution
            posGenerator = new Uniform(-(double)this->mapSize/2, (double)this->mapSize/2);
            break;
        case NORMAL: // Positions have normal distribution
            posGenerator = new Normal(-(double)this->mapSize/2, (double)this->mapSize/2);
            break;
        case CLOUDS:{ // Positions fall with a uniform distribution of 3 normal distributions 
            posGenerator = new Clouds(-(double)this->mapSize/2, (double)this->mapSize/2, 5);
            break;
        }
        default:
            printf("Warning: Network::Builder: Invalid position distribution");
            break;
    }

    CustomDist::Builder distBuilder = CustomDist::Builder();
    switch (this->periodDist){
        case SOFT:
            distBuilder.addValue(3600, 0.97)
                ->addValue(1800, 0.01)
                ->addValue(900, 0.02);
            break;
        case MEDIUM:
            distBuilder.addValue(3600, 0.9)
                ->addValue(1800, 0.05)
                ->addValue(900, 0.05);
            break;
        case HARD:
            distBuilder.addValue(3600, 0.80)
                ->addValue(1800, 0.1)
                ->addValue(900, 0.05)
                ->addValue(600, 0.05);
            break;
        default:
            printf("Warning: Network::Builder: Invalid period distribution");
            break;
        break;
    }
    CustomDist *periodGenerator = new CustomDist(distBuilder.build());

    // Period values and probabilities for the end devices
    this->H = 1; // Initially 1 and will be updated when the periods are generated
    for (unsigned int i = 0; i < this->networkSize; i++) {
        // End device position
        double x, y; 
        posGenerator->setRandom(x, y);        
        // End device period
        int period = periodGenerator->randomInt(); 
        // Update hyperperiod
        this->H = lcm(this->H, period);
        // Add end device to the network
        EndDevice *ed = new EndDevice(x, y, this->lastID, period);
        this->enddevices.push_back(ed);
        this->lastID++;
    }

    #ifdef DEBUG_MODE
        printf("--------------------------------\n");
        printf("Network configuration:\n");
        printf("Map size: %d\n", this->mapSize);
        printf("End devices: %ld\n", this->enddevices.size());
        printf("ED position generator: %d (0->uniform, 1->normal, 2->clouds)\n", (int) this->posDist);
        printf("ED period generator: %d (0->soft, 1->medium, 2->hard)\n", (int) this->periodDist);
        printf("System hyperperiod: %d\n", this->H);
        printf("--------------------------------\n\n");
    #endif

    // Random generator are no longer needed
    delete posGenerator;
    delete periodGenerator;

    return Network(*this);
}

void Network::autoConnect() {
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

void Network::disconnect() {
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->disconnect();
}

bool Network::createGateway(double x, double y) {
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

bool Network::removeGateway(unsigned int id) {
    for (unsigned int i = 0; i < this->gateways.size(); i++) {
        if (this->gateways[i]->getId() == id) {
            this->gateways.erase(this->gateways.begin() + i);
            delete this->gateways[i];
            return true;
        }
    }
    return false;
}

void Network::removeAllGateaways() {
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        delete this->gateways[i];
    this->gateways.clear();
}

void Network::stepSprings() {
    //printf("Network::stepSprings()\n");
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

void Network::stepRandom() {
    //printf("Network::stepRandom()\n");
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

double Network::getEDCoverage() {
    double coverage = 0;
    for (long unsigned int i = 0; i < this->enddevices.size(); i++)
        if (this->enddevices[i]->isConnected())
            coverage++;
    return coverage / (double) this->enddevices.size();
}

void Network::printNetworkStatus(FILE *file) {

    // Print status
    fprintf(file, "Network status:\n");
    fprintf(file, "  Map size: %d\n", this->mapSize);
    fprintf(file, "  End devices: %ld\n", this->enddevices.size());
    fprintf(file, "  Pos. distr.: %s\n", this->getPosDistName().c_str());
    fprintf(file, "  Period dist.: %s\n", this->getPeriodDistName().c_str());
    fprintf(file, "  Gateways: %ld\n", this->gateways.size());
    fprintf(file, "  ED coverage: %.2f %%\n", this->getEDCoverage()*100);
    // Print gateways positions
    fprintf(file, "Gateways positions and connections details:\n");
    for(unsigned int i = 0; i < this->gateways.size(); i++)
        fprintf(file, "  #%d: (%.2f, %.2f), channel %d, UF: %.2f, connected to %d EDs\n", 
            this->gateways[i]->getId(), 
            this->gateways[i]->getX(), 
            this->gateways[i]->getY(),
            this->gateways[i]->getChannel(),
            this->gateways[i]->getUF(),
            this->gateways[i]->connectedEDsCount()
        );
    // Print end devices positions
    fprintf(file, "End devices positions and connection details:\n");
    for(unsigned int i = 0; i < this->enddevices.size(); i++)
        fprintf(file, "  #%d: (%.2f, %.2f), period %d, connected to Gateway #%d (at %.2f mts.), using SF%d \n", 
            this->enddevices[i]->getId(), 
            this->enddevices[i]->getX(), 
            this->enddevices[i]->getY(),
            this->enddevices[i]->getPeriod(),
            this->enddevices[i]->getGatewayId(),
            this->enddevices[i]->getGWDist(),
            this->enddevices[i]->getSF()
        );
}

string Network::getPosDistName() {
    switch(this->posDist){
        case UNIFORM:
            return "uniform";
        case NORMAL:
            return "normal ";
        case CLOUDS:
            return "clouds ";
        default:
            return "unknown";
    }
}

string Network::getPeriodDistName() {
    switch(this->periodDist){
        case SOFT:
            return "soft";
        case MEDIUM:
            return "medium ";
        case HARD:
            return "hard   ";
        default:
            return "unknown";
    }
}

void Network::exportNodesCSV(char *filename) {

    FILE *file = fopen(filename, "w");
    if(file == NULL){
        printf("Error opening file %s\n", filename);
        return;
    }

    // Print gateways data
    fprintf(file, "ID,X,Y,Channel,UF,Connected EDs\n");
    for(long unsigned int i = 0; i < this->gateways.size(); i++){
        fprintf(file, "%d,%.2f,%.2f,%d,%.2f,%d\n", 
            this->gateways[i]->getId(), 
            this->gateways[i]->getX(), 
            this->gateways[i]->getY(),
            this->gateways[i]->getChannel(),
            this->gateways[i]->getUF(),
            this->gateways[i]->connectedEDsCount()
        );
    }

    // Print end devices positions
    fprintf(file, "ID,X,Y,Period,GW ID,Dist. GW,SF\n");
    for(long unsigned int i = 0; i < this->enddevices.size(); i++){
        fprintf(file, "%d,%.2f,%.2f,%d,%d,%.2f,%d\n", 
            this->enddevices[i]->getId(), 
            this->enddevices[i]->getX(), 
            this->enddevices[i]->getY(),
            this->enddevices[i]->getPeriod(),
            this->enddevices[i]->getGatewayId(),
            this->enddevices[i]->getGWDist(),
            this->enddevices[i]->getSF()
        );
    }

    fclose(file);
}
