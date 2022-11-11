#include "network.h"

using namespace std;
using namespace std::chrono;

Network::Network(Builder builder) {
    srand((unsigned int) system_clock::now().time_since_epoch().count());

    this->enddevices = builder.enddevices;
    this->mapSize = builder.mapSize;
    this->H = builder.H;
    this->maxSF = builder.maxSF;
    this->gwCnt = builder.gwCnt;
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
    this->lastID = 1; // 0 is reserved
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
        this->H = (unsigned int) lcm(this->H, period);
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
        printf("End devices: %ld\n", this->enddevices.size());
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

Network::Builder* Network::Builder::setGWCnt(unsigned int gwCnt) {
    this->gwCnt = gwCnt;
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
		    distBuilder.addValue(8000, 0.25)
                ->addValue(4000, 0.25)
                ->addValue(2000, 0.25)
                ->addValue(1600, 0.25);
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
    // Set initial ID
    this->lastID = 1; // 0 is reserved
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
            const double gwRange = Gateway::getRange(this->maxSF);
            double maxDist = gwRange;
            double minDist = 0;
            long unsigned int candidateGW = 0;
            while(minDist < maxDist){
                for(long unsigned int j = 0; j < this->gateways.size(); j++){
                    double dist = this->gateways.at(j)->distanceTo(this->enddevices.at(i));
                    if(dist < maxDist && dist > minDist){
                        maxDist = dist;
                        candidateGW = j;
                    }
                }
                // Try to connect ed to the gateway
                if(this->gateways.at(candidateGW)->addEndDevice(this->enddevices.at(i)))
                    break;
                else{ // If not, adjust ranges and search next gw
                    minDist = maxDist;
                    maxDist = gwRange;
                }
            }
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

void Network::removeIdleGateways() {
    for (unsigned int i = 0; i < this->gateways.size(); i++) {
        if (this->gateways[i]->connectedEDsCount() == 0) {
            delete this->gateways[i];
            this->gateways.erase(this->gateways.begin() + i);
            i--;
        }
    }
}

void Network::removeAllGateways() {
    this->disconnect();
    for(long unsigned int i = 0; i < this->gateways.size(); i++)
        delete this->gateways[i];
    this->gateways.clear();
}

void Network::stepSprings() {
    /*struct force {
        double x = 0;
        double y = 0;
    };*/    
    const long unsigned int nced = this->getNCEDCount();
    const long unsigned int ted = this->getEDCount();
    for (long unsigned int i = 0; i < this->enddevices.size(); i++) {
        if(this->enddevices[i]->isConnected()){ // Weak attraction force to stabilize gateways in the middle of its end devices
            Gateway *gw = this->enddevices[i]->getGateway();
            double vx = (this->enddevices[i]->getX() - gw->getX())/(double)ted;
            double vy = (this->enddevices[i]->getY() - gw->getY())/(double)ted;
            gw->addToVel(vx, vy);
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
            double vx = (this->enddevices[i]->getX() - this->gateways[closestGW]->getX())/(double)nced;
            double vy = (this->enddevices[i]->getY() - this->gateways[closestGW]->getY())/(double)nced;
            this->gateways[closestGW]->addToVel(vx, vy);
        }
    }
    
    // Disconnect all nodes
    this->disconnect();
    // Update gateways positions according to forces
    double flimit = (double) this->mapSize/ 4.0; // Maximum force to apply to each gateway
    for (long unsigned int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->updatePos(flimit);
    // Reconnect network
    this->autoConnect();
}

void Network::stepRandom() {
    // Save original positions and coverage
    double originalCoverage = this->getEDCoverage();
    vector<vector<double>> originalPositions;
    for(long unsigned int i = 0; i < this->gateways.size(); i++){
        double x = this->gateways[i]->getX();
        double y = this->gateways[i]->getY();
        originalPositions.push_back(vector<double>{x, y});
    }
    // Randomize gw positions
    this->disconnect(); 
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
        // Restore original positions
        this->disconnect();
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

double Network::getAvgUF() {
    double avgUF = 0;
    for (long unsigned int i = 0; i < this->gateways.size(); i++)
        avgUF += this->gateways[i]->getUF();
    return avgUF / (double) this->gateways.size();
}

void Network::setMaxSF(unsigned char maxSF) {
    if(maxSF >= 7 && maxSF <= 12)
        this->maxSF = maxSF;    
}

void Network::printNetworkStatus(FILE *file) {

    // Print status
    fprintf(file, "Network status:\n");
    fprintf(file, "  Map size: %d\n", this->mapSize);
    fprintf(file, "  End devices: %ld\n", this->enddevices.size());
    fprintf(file, "  End devices density: %.4f\n", (double) this->enddevices.size() / (double) this->mapSize / (double) this->mapSize);
    fprintf(file, "  Pos. distr.: %s\n", this->getPosDistName(this->posDist).c_str());
    fprintf(file, "  Period dist.: %s\n", this->getPeriodDistName(this->periodDist).c_str());
    fprintf(file, "  Gateways: %ld\n", this->gateways.size());
    fprintf(file, "  Average UF: %.4f\n", this->getAvgUF());
    fprintf(file, "  Max. SF: %d\n", this->maxSF);
    fprintf(file, "  Not feasible EDs: %ld\n", this->timeUnfeasibleEDs.size());
    fprintf(file, "  ED coverage: %.4f %%\n", this->getEDCoverage()*100);
    fprintf(file, "  Used channels: %d\n", this->minChannels);
    // Print gateways positions
    fprintf(file, "Gateways positions and connections details:\n");
    for(unsigned int i = 0; i < this->gateways.size(); i++){
        fprintf(file, "  #%d: (%.2f, %.2f), channel %d, UF: %.2f, connected to %d EDs. ", 
            this->gateways[i]->getId(), 
            this->gateways[i]->getX(), 
            this->gateways[i]->getY(),
            this->gateways[i]->getChannel(),
            this->gateways[i]->getUF(),
            this->gateways[i]->connectedEDsCount()
        );
        vector<double>uf = this->gateways[i]->getUFbySF();
        for(int j = 0; j < (int) uf.size(); j++)
            fprintf(file, "UF%d: %.2f, ", j+7, uf[j]);
        fprintf(file, "\n");
    }

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

void Network::exportNetworkCSV(char *filename) {

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

void Network::exportEDsCSV(char *filename) {
    FILE *file = fopen(filename, "w");
    if(file == NULL){
        printf("Error opening file %s\n", filename);
        return;
    }

    // Print end devices positions
    //fprintf(file, "ID,X,Y,Period\n");
    for(long unsigned int i = 0; i < this->enddevices.size(); i++){
        fprintf(file, "%d,%.2f,%.2f,%d\n", 
            this->enddevices[i]->getId(), 
            this->enddevices[i]->getX(), 
            this->enddevices[i]->getY(),
            this->enddevices[i]->getPeriod()
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
            const double rangei = this->gateways[i]->getRange();
            const double rangej = this->gateways[j]->getRange();
            const double distance = this->gateways[i]->distanceTo(this->gateways[j]);                        
            if(distance < rangei+rangej){ // i and j are neighbors
                usedChannels.push_back(this->gateways[j]->getChannel()); // Add channel of j to the list
            }
        }
        // Traverse the list of used channels and find the minimum available
        int minAvailableChannel = 0;
        for(unsigned long int j = 0; j < this->gateways.size(); j++){ // Worst case is one channel per gateway
            if(find(usedChannels.begin(), usedChannels.end(), j) == usedChannels.end()){ // if channel j not found
                minAvailableChannel = j; // If j is not used, then is the minimum available channel
                break; // Stop search
            }
        }
        this->gateways[i]->setChannel(minAvailableChannel);
        // Update minimum number of channels
        if(minAvailableChannel+1 > this->minChannels)
            this->minChannels = minAvailableChannel+1;
    }    
}

void Network::compSchedulerMinPeriod() {
    // initialize scheduler with dimenstions GW x SF x H    
    this->scheduler.resize(this->gateways.size());
    for(unsigned int i = 0; i < this->gateways.size(); i++){
        this->scheduler[i].resize(6);
        for(unsigned int j = 0; j < 6; j++){
            this->scheduler[i][j].resize(this->H/pow(2, j));
            for(unsigned int k = 0; k < this->H/pow(2, j); k++){
                this->scheduler[i][j][k] = 0; // initialize scheduler with all slots empty
            }
        }
    }
    
    if(this->getNCEDCount() > 0){
        printf("Error: There are not connected End Devices. Coverage should equals 100%% to run scheduler.\n");
        return;
    }
    
    this->timeUnfeasibleEDs.clear(); // Clear list of unfeasible EDs  

    // Sort endevices vector by ascending period
    sort(this->enddevices.begin(), this->enddevices.end(), [](EndDevice *a, EndDevice *b){
        return a->getPeriod() < b->getPeriod();
    });

    // Schedule by ED with fixed priorities
    for(unsigned int ed = 0; ed < this->enddevices.size(); ed++){
        // The ID is used to identify the ED in the scheduler slots
        const unsigned int edId = this->enddevices[ed]->getId(); // IDs (begins at 1)

        // Get GW index (i) of current ED
        const unsigned int gwId = this->enddevices[ed]->getGatewayId();
        unsigned int i = 0; // GW index
        for(unsigned int gw = 0; gw < this->gateways.size(); gw++) {
            if(this->gateways[gw]->getId() == gwId) {
                i = gw; // found
                break;
            }
        }

        // Get SF index (j)
        const unsigned int j = (unsigned int) this->enddevices[ed]->getSF()-7; // SF index (0..5)        

        // Allocate slots for current ED
        unsigned int period = this->enddevices[ed]->getPeriod()/pow(2, j); // End device period scaled to SF
        unsigned int instances = this->H/this->enddevices[ed]->getPeriod(); // Instance number is constant for all SF
        for(unsigned int inst = 0; inst < instances; inst++){            
            bool allocated = false;
            for(unsigned int k = inst*period; k < (inst+1)*period; k++){                
                if(this->scheduler[i][j][k] == 0){ // if slot is empty
                    this->scheduler[i][j][k] = edId;
                    allocated = true;
                    break;
                }
            }
            if(!allocated) // Could not find free slot for some instance of current ED
                this->timeUnfeasibleEDs.push_back(ed);
        }
    }
}

void Network::compSchedulerEDF() {
    // initialize scheduler with dimenstions GW x SF x H
    this->scheduler.resize(this->gateways.size());
    for(unsigned int i = 0; i < this->gateways.size(); i++){
        this->scheduler[i].resize(6);
        for(unsigned int j = 0; j < 6; j++){
            this->scheduler[i][j].resize(this->H/pow(2, j));
            for(unsigned int k = 0; k < this->H/pow(2, j); k++){
                this->scheduler[i][j][k] = 0; // initialize scheduler with all slots empty
            }
        }
    }
    
    if(this->getNCEDCount() > 0){
        printf("Error: There are not connected End Devices. Coverage should equals 100%% to run scheduler.\n");
        return;
    }

    // Schedule with dynamic priorities (early deadline first)
    this->timeUnfeasibleEDs.clear(); // Clear list of unfeasible EDs  
    for(unsigned int i = 0; i < this->gateways.size(); i++){ // For each GW schedule        
        for(unsigned int j = 0; j < 6; j++){ // For each SF            
            vector<EndDevice*> schED = this->gateways[i]->getConnectedEDs(j+7); // List of ED to schedule for current GW and SF
            if(schED.size() > 0){
                vector<EndDevice*> readyEDs; // List of EDs ready to be scheduled on each slot                
                const unsigned int Hsf = this->H/pow(2, j); // Number of slots of current SF
                unsigned int k = 0;
                while(k < Hsf){ // For each slot of current GW and SF
                    // Get list of ready EDs for current slot
                    for(unsigned int ed = 0; ed < schED.size(); ed++){
                        const unsigned int period = schED[ed]->getPeriod()/pow(2, j);
                        if(k % period == 0)
                            readyEDs.push_back(schED[ed]);
                    }
                    if(readyEDs.size() > 0){ // If there are ready EDs, schedule them
                        // Find ED with earliest deadline
                        unsigned int minDeadline = Hsf+1;
                        EndDevice* minED = NULL;
                        for(unsigned int ed = 0; ed < readyEDs.size(); ed++){
                            const unsigned int period = readyEDs[ed]->getPeriod()/pow(2, j);
                            const unsigned int deadline = (k / period + 1)*period; // Deadline slot
                            if(deadline < minDeadline){
                                minDeadline = deadline;
                                minED = readyEDs[ed];
                            }
                        }
                        // Schedule ED
                        bool allocated = false;
                        while(k < minDeadline){
                            if(this->scheduler[i][j][k] == 0){
                                this->scheduler[i][j][k] = minED->getId();
                                // Remove ED from list of ready EDs
                                for(unsigned int ed = 0; ed < readyEDs.size(); ed++){
                                    if(readyEDs[ed]->getId() == minED->getId()){
                                        readyEDs.erase(readyEDs.begin()+ed);
                                    }
                                }
                                allocated = true;
                                break;
                            }else{
                                k++;
                            }
                        }
                        if(!allocated){ // Could not find free slot for some instance of current ED
                            this->timeUnfeasibleEDs.push_back(minED->getId());
                            
                            /*
                            printf("Error: Could not find free slot for ED %d.\n", minED->getId());
                            printf("Period is %d, SF is %d, k is %d, minDeadline is %d.\n", minED->getPeriod(), j+7, k, minDeadline);
                            return;
                            */
                            
                            k++;
                        }
                    }else{
                        k++;
                    }                    
                }
            }
        }
    }
}

void Network::exportGWGrid(char *filename) {
    // Build a regular grid of GW and print adjacency matrix
    if(filename != NULL){
        FILE *file = fopen(filename, "w");

        /// Build GW grid
        const int side = sqrt(this->gwCnt);
        const int offset = this->mapSize/2;
        const int step = this->mapSize/(side-1);
        for(int row = 0; row < side; row++) {
            for(int col = 0; col < side; col++){
                double x = row * step - offset;
                double y = col * step - offset;
                this->createGateway(x, y);
            }
        }

        /// Print data
        fprintf(file, "%ld %d\n", this->enddevices.size(), this->gwCnt); // Header of file
        for(long unsigned int i = 0; i < this->enddevices.size(); i++) {
            for(unsigned int j = 0; j < this->gwCnt; j++) {
                const double dist = this->gateways[j]->distanceTo(this->enddevices[i]);
                const int minSF = this->gateways[j]->getMinSF(dist);
                const int maxSF = this->gateways[j]->getMaxSF(this->enddevices[i]->getPeriod());
                fprintf(file, "%d ", (minSF > 12 || minSF > maxSF) ? 100 : minSF);
            }
            fprintf(file, "%d\n", this->enddevices[i]->getPeriod());
        }

        fclose(file);
    }
}

void Network::printScheduler(char *filename) {
    // Print scheduler to file if filename is not null
    if(filename != NULL){
        FILE *file = fopen(filename, "w");

        // Print unfeasible EDs if any
        if(this->timeUnfeasibleEDs.size() > 0){
            fprintf(file, "Unfeasible EDs\n");
            fprintf(file, "ID, period, SF, GW ID\n");
            for(unsigned int ed = 0; ed < this->timeUnfeasibleEDs.size(); ed++){
                const unsigned int edIdx = this->timeUnfeasibleEDs[ed];
                EndDevice* uED = this->enddevices[edIdx];
                fprintf(file, "%d,%d,%d,%d\n", uED->getId(), uED->getPeriod(), uED->getSF(), uED->getGateway()->getId());
            }
        }

        // Print scheduling table for each GW
        fprintf(file, "\nScheduling table\n");
        for(unsigned int i = 0; i < this->gateways.size(); i++){
            fprintf(file, "Scheduling table for GW %d\n,", this->gateways[i]->getId());
            for(unsigned int k = 0; k < this->H; k++)
                fprintf(file, "%d,", k);
            fprintf(file, "\n");
            for(unsigned int j = 0; j < 6; j++){
                fprintf(file, "SF%d:,", j+7);
                for(unsigned int k = 0; k < this->H/pow(2, j); k++){    
                    fprintf(file, "%d,", this->scheduler[i][j][k]);
                }
                fprintf(file, "\n");
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
}

unsigned int Network::getNonFeasibleEDs() {
    return this->timeUnfeasibleEDs.size();
}