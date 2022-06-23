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
            distBuilder.addValue(3600, 0.7)
                ->addValue(1800, 0.1)
                ->addValue(900, 0.1)
                ->addValue(600, 0.1);
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
        printf("ED position generator: %s\n", Network::getPosDistName(this->posDist).c_str());
        printf("ED period generator: %s\n", Network::getPeriodDistName(this->periodDist).c_str());
        printf("System hyperperiod: %d\n", this->H);
        printf("--------------------------------\n\n");
    #endif

    // Random generator are no longer needed
    delete posGenerator;
    delete periodGenerator;

    return Network(*this);
}

void Network::autoConnect() {
    //Alternative --> for_each(this->enddevices.begin(),this->enddevices.end(),[this](EndDevice *ed) {
    for(long unsigned int i = 0; i < this->enddevices.size(); i++) {
        if(!this->enddevices[i]->isConnected()){
            // Sort gateways by distance
            sort(this->gateways.begin(), this->gateways.end(), [this, i](Gateway *a, Gateway *b) {
                return a->distanceTo(this->enddevices[i]) < b->distanceTo(this->enddevices[i]);
            });
            for(long unsigned int j = 0; j < this->gateways.size(); j++)
                if(this->gateways[j]->addEndDevice(this->enddevices[i]))
                    break;
        }
    }
}

void Network::disconnect() {
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->disconnect();
}

void Network::createGateway(double x, double y) {
    Gateway *gw = new Gateway(x, y, this->lastID, this->H);
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

void Network::stepRandomPreserve() {
    double originalCoverage = this->getEDCoverage();
    // Save original positions and find the GW with greater UF
    vector<vector<double>> originalPositions;
    double maxUF = 0;
    long unsigned int maxUFIndex = 0;
    for(long unsigned int i = 0; i < this->gateways.size(); i++){
        double x = this->gateways[i]->getX();
        double y = this->gateways[i]->getY();
        originalPositions.push_back(vector<double>{x, y});
        if(this->gateways[i]->getUF() > maxUF){
            maxUF = this->gateways[i]->getUF();
            maxUFIndex = i;
        }
    }
    // Disconnect all GWs except the one with greater UF and randomize positions
    Uniform gwPosGenerator = Uniform(-(double)this->mapSize/2, (double)this->mapSize/2);
    double newX, newY;
    for(long unsigned int i = 0; i < this->gateways.size(); i++){
        if(i != maxUFIndex){
            gwPosGenerator.setRandom(newX, newY);
            this->gateways[i]->disconnect();
            this->gateways[i]->moveTo(newX, newY);
        }
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

int Network::configureGWChannels() {
    // Compute chromatic number to determine minimum number of channels
    
    // Reset all channels
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->setChannel(0);
    // Compute chromatic number
    int minChannels = 1;
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
        if(minAvailableChannel > minChannels)
            minChannels = minAvailableChannel;
    }
    return minChannels;
}