#include "network.h"

using namespace std;
using namespace std::chrono;

Network::Network(Builder builder) {
    srand((unsigned int) system_clock::now().time_since_epoch().count());

    this->enddevices = builder.enddevices;
    this->mapSize = builder.mapSize;
    this->H = builder.H;
    this->maxSF = builder.maxSF;
    this->lastID = builder.lastID;
    this->posDist = builder.posDist;
    this->periodDist = builder.periodDist;
}

Network::Network(char* filename) {
    // Format: 
    // id,x,y,dist,period,?,?,?\n

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file csv file\n");
        exit(1);
    }
    this->H = 1;
    this->posDist = EXT_POS;
    this->periodDist = EXT_PER;
    this->lastID = 0;
    double minMapSize = 0.0;
    double maxMapSize = 0.0;
    char line[LINE_BUFFER_SIZE];
    while ( fgets(line, LINE_BUFFER_SIZE, file) != NULL ) {
        int col = 0;
        char* token;
        double x, y;
        int period;
        for (token = strtok(line, ","); token != NULL && col < FILE_COLS; col++, token=strtok(NULL, ",")){
            switch (col){
                case 1:
                    x = atof(token);
                    if(x > maxMapSize)
                        maxMapSize = x;
                    if(x < minMapSize)
                        minMapSize = x;
                    break;
                case 2:
                    y = atof(token);
                    if(y > maxMapSize)
                        maxMapSize = y;
                    if(y < minMapSize)
                        minMapSize = y;
                    break;
                case 4:
                    period = atoi(token);
                    break;
                default:
                    break;
            }
        }
        this->H = lcm(this->H, period);
        if(col == FILE_COLS){
            this->H = lcm(this->H, period);
            // Add end device to the network
            EndDevice *ed = new EndDevice(x, y, this->lastID, period);
            this->enddevices.push_back(ed);
            this->lastID++;
        }
    }
    fclose(file);
    this->mapSize = maxMapSize - minMapSize;
    #ifdef DEBUG_MODE
        printf("-----------------------------------\n");
        printf("Configuration file loaded\n");
        printf("End devices: %d\n", this->lastID);
        printf("System hyperperiod: %d\n", this->H);
        printf("Map size: %d\n", this->mapSize);
        printf("-----------------------------------\n");
    #endif
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

Network::Builder* Network::Builder::setMaxSF(unsigned char maxSF) {
    this->maxSF = maxSF;
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
            distBuilder.addValue(16000, 0.25)
                ->addValue(8000, 0.25)
                ->addValue(4000, 0.25)
                ->addValue(3200, 0.25);

            break;
        case MEDIUM:
		    distBuilder.addValue(3600, 0.97)
                ->addValue(1800, 0.01)
                ->addValue(900, 0.02);
            break;
        case HARD:
            distBuilder.addValue(1600, 0.25)
                ->addValue(800, 0.25)
                ->addValue(400, 0.25)
                ->addValue(320, 0.25);
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
        printf("  Map size: %d\n", this->mapSize);
        printf("  End devices: %ld\n", this->enddevices.size());
        printf("  Max. SF: SF%d\n", this->maxSF);
        printf("  ED position generator: %s\n", Network::getPosDistName(this->posDist).c_str());
        printf("  ED period generator: %s\n", Network::getPeriodDistName(this->periodDist).c_str());
        printf("  System hyperperiod: %d\n", this->H);
        printf("--------------------------------\n\n");
    #endif

    // Random generator are no longer needed
    delete posGenerator;
    delete periodGenerator;

    return Network(*this);
}

void Network::autoConnect() {
    for(long unsigned int i = 0; i < this->enddevices.size(); i++) {
        if(!this->enddevices.at(i)->isConnected()){
            // Sort gateways by distance
            sort(this->gateways.begin(), this->gateways.end(), [this, i](Gateway *a, Gateway *b) {
                return a->distanceTo(this->enddevices.at(i)) < b->distanceTo(this->enddevices.at(i));
            });
            for(long unsigned int j = 0; j < this->gateways.size(); j++)
                if(this->gateways.at(j)->addEndDevice(this->enddevices.at(i)))
                    break;
        }
    }
}

void Network::disconnect() {
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->disconnect();
}

void Network::createGateway(double x, double y) {
    Gateway *gw = new Gateway(x, y, this->lastID, this->H, 0, this->maxSF);
    gateways.push_back(gw);
    this->lastID++;
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
    struct force {
        double x = 0;
        double y = 0;
    };
    vector<force> forces(this->gateways.size()); // Forces for each gateway
    const long unsigned int nced = this->getNCEDCount();
    const long unsigned int ted = this->getEDCount();
    for (long unsigned int i = 0; i < this->enddevices.size(); i++) {
        if(this->enddevices[i]->isConnected()){ // Weak attraction force to stabilize gateways in the middle of its end devices
            // Get ID of connected GW
            unsigned int gwID = this->enddevices[i]->getGatewayId();
            // Get index of GW in the vector
            long unsigned int connectedGW = 0;
            for(long unsigned int j = 0; j < this->gateways.size(); j++)
                if(this->gateways[j]->getId() == gwID)
                    connectedGW = j;
            forces[connectedGW].x += (this->enddevices[i]->getX() - this->gateways[connectedGW]->getX())/(double)ted;
            forces[connectedGW].y += (this->enddevices[i]->getY() - this->gateways[connectedGW]->getY())/(double)ted;
        } else { // Strong attraction force to move gateways close to not connected end devices
            // Get closest gateway
            long unsigned int closestGW = 0;
            double minDist = this->gateways[0]->distanceTo(this->enddevices[i]);
            for (long unsigned int j = 1; j < this->gateways.size(); j++) {
                double dist = this->gateways[j]->distanceTo(this->enddevices[i]);
                if (dist < minDist) {
                    minDist = dist;
                    closestGW = j;
                }
            }
            // Add attraction force for closest gw
            forces[closestGW].x += (this->enddevices[i]->getX() - this->gateways[closestGW]->getX())/(double)nced;
            forces[closestGW].y += (this->enddevices[i]->getY() - this->gateways[closestGW]->getY())/(double)nced;
        }
    }
    
    // Disconnect all nodes
    this->disconnect();
    // Update gateways positions according to forces
    for (long unsigned int i = 0; i < this->gateways.size(); i++) {
        double newX = this->gateways[i]->getX() + mclamp(forces[i].x, -100.0, 100.0);
        double newY = this->gateways[i]->getY() + mclamp(forces[i].y, -100.0, 100.0);
        this->gateways[i]->moveTo(newX, newY);
    }
    // Reconnect network
    this->autoConnect();
}

void Network::stepRandom() {
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
    Uniform gwPosGenerator = Uniform(-(double)this->mapSize/2, (double)this->mapSize/2);
    double newX, newY;
    for(long unsigned int i = 0; i < this->gateways.size(); i++){
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

long unsigned int Network::getNCEDCount() {
    long unsigned int count = 0;
    for (long unsigned int i = 0; i < this->enddevices.size(); i++) 
        if(!this->enddevices[i]->isConnected())
            count++;
    return count;
}

void Network::printNetworkStatus(FILE *file) {

    // Print status
    fprintf(file, "Network status:\n");
    fprintf(file, "  Map size: %d\n", this->mapSize);
    fprintf(file, "  End devices: %ld\n", this->enddevices.size());
    fprintf(file, "  Pos. distr.: %s\n", this->getPosDistName(this->posDist).c_str());
    fprintf(file, "  Period dist.: %s\n", this->getPeriodDistName(this->periodDist).c_str());
    fprintf(file, "  Gateways: %ld\n", this->gateways.size());
    fprintf(file, "  ED coverage: %.2f %%\n", this->getEDCoverage()*100);
    fprintf(file, "  Used channels: %d\n", this->minChannels);
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
    for(unsigned int i = 0; i < this->enddevices.size(); i++){
        if(this->enddevices[i]->isConnected())
            fprintf(file, "  #%d: (%.2f, %.2f), period %d, connected to Gateway #%d (at %.2f mts.), using SF%d \n", 
                this->enddevices[i]->getId(), 
                this->enddevices[i]->getX(), 
                this->enddevices[i]->getY(),
                this->enddevices[i]->getPeriod(),
                this->enddevices[i]->getGatewayId(),
                this->enddevices[i]->getGWDist(),
                this->enddevices[i]->getSF()
            );
        else
            fprintf(file, "  #%d: (%.2f, %.2f), period %d, not connected\n", 
                this->enddevices[i]->getId(), 
                this->enddevices[i]->getX(), 
                this->enddevices[i]->getY(),
                this->enddevices[i]->getPeriod()
            );
    }
}

// static
string Network::getPosDistName(POS_DIST posDist) {
    switch(posDist){
        case UNIFORM:
            return "Uniform";
        case NORMAL:
            return "Normal";
        case CLOUDS:
            return "Clouds";
        case EXT_POS:
            return "External";
        default:
            return "Unknown";
    }
}

string Network::getPosDistName() {
    return this->getPosDistName(this->posDist);
}

// static
string Network::getPeriodDistName(PERIOD_DIST periodDist) {
    switch(periodDist){
        case SOFT:
            return "Soft";
        case MEDIUM:
            return "Medium";
        case HARD:
            return "Hard";
        case EXT_PER: 
            return "External";
        default:
            return "Unknown";
    }
}

string Network::getPeriodDistName() {
    return this->getPeriodDistName(this->periodDist);
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

void Network::configureGWChannels() {
    // Compute chromatic number to determine minimum number of channels
    
    // Reset all channels
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->setChannel(0);
    // Compute chromatic number
    this->minChannels = 1;
    list<int> usedChannels;
    for(long unsigned int i = 1; i < this->gateways.size(); i++){ // Start from 1 to avoid GW 0
        // Get a list of channels used by the neighbors of the current gateway
        usedChannels.clear();
        for(long unsigned int j = 0; j < this->gateways.size(); j++){
            if(i == j) continue; // Skip current gateway
            double range = this->gateways[i]->getRange()+this->gateways[j]->getRange();
            if(this->gateways[i]->distanceTo(this->gateways[j]) <= range) // If i and j are neighbors
                usedChannels.push_back(this->gateways[j]->getChannel()); // Add channel of j to the list
        }
        // Traverse the list of used channels and find the minimum available
        int minAvailableChannel = 0;
        for(unsigned long int j = 0; j < this->gateways.size(); j++){
            if(find(usedChannels.begin(), usedChannels.end(), j) == usedChannels.end()){
                minAvailableChannel = j;
                break;
            }
        }
        this->gateways[i]->setChannel(minAvailableChannel);
        // Update minimum number of channels
        if(minAvailableChannel > this->minChannels)
            this->minChannels = minAvailableChannel;
    }
}