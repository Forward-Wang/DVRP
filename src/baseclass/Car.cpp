#include "Car.h"
#include "../public/PublicFunction.h"
#include "../run/Config.h"
#include<cmath>
#include <stdexcept>

Car::Car(Spot &headNode, Spot &rearNode, float capacity, int index, bool artificial):
    carIndex(index), route(headNode, rearNode, capacity), artificial(artificial)
{
    state = wait;
    nearestDepartureTime = 0;
    nextArriveTime = 0;
    travelDistance = 0;
}

Car::Car(Route &route, int index, bool artificial): carIndex(index), 
    artificial(artificial), route(route)
{
    state = wait;
    nearestDepartureTime = 0;
    nextArriveTime = 0;
    travelDistance = 0;
}

Car::~Car(){  
    // ��Ƕ��������������ᱻ���ã������ڴ˴�delete route
}

Car::Car(const Car& item):route(item.route), artificial(item.artificial){  
    // ���ƹ��캯��
    this->state = item.state;
    this->carIndex = item.carIndex;
    this->state = item.state;
    this->artificial = item.artificial;
    this->nearestDepartureTime = item.nearestDepartureTime;
    this->nextArriveTime = item.nextArriveTime;
    this->travelDistance = item.travelDistance;
}


Car& Car::operator= (Car &item){ 
    // ���ظ�ֵ����
    this->route = item.route;
    this->carIndex = item.carIndex;
    this->state = item.state;
    this->artificial = item.artificial;
    this->nearestDepartureTime = item.nearestDepartureTime;
    this->nextArriveTime = item.nextArriveTime;
    this->travelDistance = item.travelDistance;
    return *this;
}


//================ �õ��������� =================//
Car* Car::getNullCar(vector<Spot*> &removedCustomer){
    // ���ƻ������׽ڵ��β�ڵ��Լ�ʣ������
    // ��Ҫ����choiceָ��depot��customer�����ǲ����Ա�ת�Ƶ���ĳ���
    // ���ص�Car��Ҫ���ⲿ����delete
    Route *emptyRoute = route.getEmptyRoute(removedCustomer);
    Car* newCar = new Car(*emptyRoute, carIndex);
    return newCar;
}

vector<int> Car::getAllID(){
    // ������нڵ��ID����˳��
    return route.getAllID();
}


//================ insert cost��remove cost =================//
void Car::computeInsertCost(Spot *store, Spot *customer, float &minValue, 
        Spot *&refStore1, Spot *&refCustomer1, float &secondValue, Spot *&refStore2,
        Spot *&refCustomer2, float randomNoise, bool allowNegativeCost){
        
    // ��������(store, customer)��·���е���С������ۺʹ�С�������
    // ���������/�μѲ����(refStore, refCustomer)
    // Args:
    //   * randomNoise: �������������
    //   * allowNegativeCost: �Ƿ�������ָ���insertion cost
    // Returns:
    //   * refStore1, refCustomer1: ��minValue=MAX_FLOAT������߾�ΪNULL
    //   * refStorer2, refCustomer2: ��secondValue=MAX_FLOAT������߾�ΪNULL
    route.computeInsertCost(store, customer, minValue, refStore1, refCustomer1,
            secondValue, refStore2, refCustomer2, randomNoise, allowNegativeCost);
}

vector<float> Car::computeReducedCost(float DTpara[]){  
    // �õ�·�������з����(P-D)���Ƴ����ۣ�ֵԽС��ʾ�����Խ�ʡ����Ĵ���
    // Args:
    //   * DTpara: ��ͬ���ȼ��˿ͼ���reduce costʱ��bonus
    return route.computeReducedCost(DTpara, artificial);
}


//================ insert �� delete Customer���� =================//
void Car::insertAtRear(Spot *item){
    // һ�����������ճ��������ڵ㣬��˲����¼���nextArriveTime
    try {
        route.insertAtRear(item);
    } catch (exception &e) {
        throw out_of_range(e.what());
        exit(1);
    }
} 

void Car::insertAtHead(Spot *store, Spot *customer){
    // ֻ�ܵ�����Ϊ��ʱ�ſ�ʹ�ã�Ҳ�����¼���nextArriveTime
    try {
        route.insertAtHead(store, customer);
    } catch (exception &e) {
        throw out_of_range(e.what());
        exit(1);
    }
}

void Car::insertAfter(Spot *refStore, Spot *refCustomer, Spot *store, 
        Spot *customer) {
    // ��refStore�������store����refCustomer�������customer
    // update: �Ƿ���Ҫ����stand�ڵ�
    try {
        route.insertAfter(refStore, refCustomer, store, customer);
    } catch (exception &e) {
        throw out_of_range(e.what());
        exit(1);
    }
    // ����nextArriveTime��������һվĿ�ĵ��Ƿ�ı�
    Spot *lastStop = route.getStand();  // �������פ�㣨��һ�����ʵĽڵ㣩
    // ��ǰʱ�� = ��lastStop������ʱ�� + dist(lastStop, refStore)
    float currentTime = lastStop->arrivedTime + lastStop->serviceTime;
    Spot *nextStop = route.nextPos();
    nextArriveTime = currentTime + dist(lastStop, nextStop);
    // ����nearestDepartureTime
    if(state == departure) {
        nearestDepartureTime = currentTime;
    }
}

void Car::insertAfter(Spot *refStore, Spot *refCustomer, Spot *store,
        Spot *customer, float time) {
    // ��Ҫ����stand�ڵ�İ汾
    updateState(time);  
    try {
        route.insertAfter(refStore, refCustomer, store, customer);
    } catch (exception &e) {
        throw out_of_range(e.what());
        exit(1);
    }
    // ����nextArriveTime��������һվĿ�ĵ��Ƿ�ı�
    Spot *lastStop = route.getStand();  // �������פ�㣨��һ�����ʵĽڵ㣩
    // ��ǰʱ�� = ��lastStop������ʱ�� + dist(lastStop, refStore)
    float currentTime = lastStop->arrivedTime + lastStop->serviceTime;
    Spot *nextStop = route.nextPos();
    nextArriveTime = currentTime + dist(lastStop, nextStop);
    // ����nearestDepartureTime
    if(state == departure) {
        nearestDepartureTime = currentTime;
    }
}

void Car::deleteCustomer(Spot *store, Spot *customer) {
    // ɾ�������(store, customer)
    try {
        route.deleteNode(store, customer);
    } catch (exception &e) {
        throw out_of_range(e.what());
        exit(1);
    }
}


//================ part Route���� =================//
void Car::replaceRoute(Car *newCar, float currentTime){      
    // ��newCar��·�����뵽��ǰ����·����current�ڵ�֮��
    // �˺�����capturePartRoute����ʹ�ã�Ĭ��ʱ��ڵ㲻��
    // ������capture���Ѿ�������stand���������������
    Spot* originNextNode = route.nextPos();  // ԭĿ�ĵ�
    Spot* changedNextNode = newCar->getRoute()->nextPos(); // ���ĺ��Ŀ�ĵ�
    if(state == departure && originNextNode->id != changedNextNode->id) {
        // ���������·;�У����Ҹ���·����Ŀ�ĵظı䣬��Ӧ���޸�nextArriveTime����
        Spot *standPos = route.getStand();
        nextArriveTime = currentTime + dist(standPos, changedNextNode);
    }
    try{
        route.replaceRoute(newCar->getRoute());      // replaceRoute�����Ļ�����״̬
    } catch (exception &e) {
        cout << "Car #" << carIndex << ":" << endl;
        cout << e.what() << endl;
        exit(1);
    }
}

Car* Car::capturePartRoute(float time){   
    // ץȡroute��currentָ��֮���·�������ҷ���һ����
    // timeΪץȡ��ʱ��
    updateState(time);    // �Ƚ�״̬����
    Spot* currentNode = route.currentPos();  // �Ӹõ����
    Spot* nextNode = route.nextPos();        // ��һվĿ�ĵ�
    Spot* startNode = new Spot();         // ���ӵĳ�����
    startNode->id = 0;   
    startNode->type = 'D';
    startNode->priority = 0;
    // ȷ��������λ����Ϣ�Լ�ʱ����Ϣ
    switch(state) {
        case departure: {
            // ������·;�У���������ĳ�ʼ��
            // �õ����λ��λ�ڳ������Ŀ�ĵ������ϵ�ĳһ��
            // �õ��arrivedTime�趨Ϊ��ǰʱ�䣬������ʱ��Ϊ0���Ͳֿ�һ��
            currentNode = route.getStand();   // departure״̬��פ����Ϊ��ǰ��
            float dist = nextArriveTime - nearestDepartureTime;
            startNode->x = (time - nearestDepartureTime) / dist * (nextNode->x - 
                    currentNode->x) + currentNode->x;
            startNode->y = (time - nearestDepartureTime) / dist * (nextNode->y - 
                    currentNode->y) + currentNode->y;
            startNode->arrivedTime = time;
            startNode->serviceTime = 0;
            startNode->startTime = time;
            route.setStand(startNode->x, startNode->y, time);
            nearestDepartureTime = time;
            break;
        }
        case wait: {
            // ���Ӵ��ڵȴ�״̬��ֱ��ȡ��ǰ�ڵ���Ϊ��ʼ��
            // ������ʱ���Գ���������serviceTimeΪ0
            startNode->x = currentNode->x;
            startNode->y = currentNode->y;
            startNode->arrivedTime = time;
            startNode->serviceTime = 0;
            startNode->startTime = time;
            route.setStand(startNode->x, startNode->y, time);
            break;
        }
        case serving: {
            // ���ӵ�ǰ�ڷ���˿ͣ���ʼ��Ϊ��ǰ�����
            // ������ʱ���趨���������ʱ���ȥ��ǰʱ��
            // ע�⵱�����ڵȴ���Ϊ�˿ͷ���ʱ������Ҳ��״̬�趨Ϊserving
            // ע���������˿͵������������nearestDepartureTime��������ǿ�������֮
            Spot *currentPos = route.currentPos();
            startNode->x = currentNode->x;
            startNode->y = currentNode->y;
            startNode->arrivedTime = time;
            startNode->serviceTime = nearestDepartureTime - time;  
            // ����ʱ���Ѿ���ȥ��һ���֣�ע��˿͵����Ӧ��ȷ��arrivedTime
            // time - baseTime��ʾ�Ѿ��������ʱ��
            startNode->startTime = time;
            route.setStand(startNode->x, startNode->y, time, startNode->serviceTime);
            break;
        }
        case offwork: {  
            // �ճ��˵ĳ����ǲ����õ�
            // ��ʱ����һ���ճ�������startNodeû���κ�����
            break;		
        }
    }
    // ��currentָ���Ĺ˿�����newCar�У�ע�����ʣ������leftQuanity
    float leftQuantity = route.getLeftQuantity();  // ����ʣ������
    Spot *depot = route.getRearNode();          // �κ�һ�������յ㶼��depot
    Car *newCar = new Car(*startNode, *depot, leftQuantity, carIndex, false);
    try {
        route.checkArrivedTime();
    } catch(exception &e) {
        cout << "Problem happens before!!" << endl;
    }
    vector<Spot*> nodes = route.capture();
    vector<Spot*>::iterator custIter;
    for(custIter = nodes.begin(); custIter < nodes.end(); custIter++) {
        try {
            newCar->insertAtRear(*custIter);
        } catch (exception &e) {
            cout << "In car #" << carIndex << ":" << endl;
            cerr << "In capture: " << e.what() << endl;
            exit(1);
        }
    }
    return newCar;
}


//================ state��� =================//
void Car::updateState(float time){
    // ���»���״̬
    switch(state){
        case departure: {
            // ԭ״̬�ǳ���������һ״̬�ǻ���ִ�з���
            // ��������»�����nearestDepartureTime
            if(time == nextArriveTime) {
                // ����ǰʱ��������״̬�ı��ʱ�䣬��״̬�ı�
                travelDistance += nextArriveTime - nearestDepartureTime;   // ����travelDistance
                route.moveForward();   // ִ�з��񣬸��ĵ�ǰפ��
                Spot *currentPos = route.currentPos();  // ��ǰפ��
                // *Note*: ��������ʽ���µ�ǰ�˿͵ĵ���ʱ��
                route.currentPos()->arrivedTime = time;
                if(currentPos->id == 0) {
                    state = offwork;   // ����ֿ⣬�ճ�
                } else {
                    Spot *nextPos = route.nextPos();
                    if(currentPos->type == 'C' && time < currentPos->startTime) {
                        // ֻ����customer���ͽڵ��ʱ�䴰����
                        time = currentPos->startTime;
                    }
                    nearestDepartureTime = time + currentPos->serviceTime;
                    if(nearestDepartureTime == time) {  
                        // ��������Ȳ���Ҫ�ȴ���Ҳ����Ҫ�������������
                        route.setStand(currentPos->x, currentPos->y, time);
                        state = departure;
                        nextArriveTime = nearestDepartureTime + dist(currentPos, nextPos);
                    } else {
                        route.setStand(currentPos->x, currentPos->y, time, 
                                currentPos->serviceTime);
                        if(currentPos->type == 'C') {
                            // ������ʣ���ػ�������
                            route.decreaseLeftQuantity(currentPos->quantity);
                        }
                        state = serving;
                    }
                }
            }
            break;
        }
        case serving: {
            // ԭ״̬��ִ�з�������һ״̬�ǻ�������
            Spot* currentPos = route.currentPos();
            Spot* nextPos = route.nextPos();
            if(time == nearestDepartureTime) {
                if(nextPos->type == 'D' && time < LATEST_SERVICE_TIME)  {
                    // �����һվ�ǻص��ֿ����δ���°�ʱ�䣬��ȴ�
                    state = wait;
                    nearestDepartureTime = LATEST_SERVICE_TIME;
                    route.setStand(currentPos->x, currentPos->y, time);
                    cout << "Car #" << carIndex << ": nothing to do, wait" << endl;
                } else {
                    // ��������
                    route.setStand(currentPos->x, currentPos->y, time);
                    state = departure;
                    nextArriveTime = nearestDepartureTime + dist(currentPos, nextPos);
                }
            break;
            }
        }
        case wait: {
            // do nothing now
            if(time == nearestDepartureTime) {
                Spot *currentPos = route.currentPos();
                Spot *nextPos = route.nextPos();
                nextArriveTime = time + dist(currentPos, nextPos);
                route.setStand(currentPos->x, currentPos->y, time);
                state = departure;
            }
            break;
        }
        case offwork: {
            // do nothing now
            // �ճ�֮����Ӧû�к�������
            break;
        }
    }
}

EventElement Car::getCurrentAction(float time){        
    // ��û�����ǰʱ�̵Ķ���
    EventElement event;
    event.carIndex = carIndex;
    updateState(time);   // �ȸ���״̬
    Spot *currentPos = route.currentPos();
    switch(state){
        case departure: {
            Spot *nextPos = route.nextPos();
            event.time = nextArriveTime;
            event.eventType = carArrived;
            event.customerId = nextPos->id;
            break;
        }
        case wait: {
            // do nothing
            event.time = nearestDepartureTime;
            event.eventType = finishedService;
            event.customerId = 0;
            break;
        }
        case serving: {
            Spot *currentPos = route.currentPos();
            event.time = nearestDepartureTime;
            event.customerId = currentPos->id; 
            event.eventType = finishedService;
            break;
        }
        case offwork: {
            // �ճ�
            event.time = time;
            event.customerId = 0;
            event.eventType = carOffWork;
            break;
        }
    }
    return event;
}

EventElement Car::launchCar(float currentTime){         
    // ��������������������wait״̬ʱ��Ч
    EventElement event;
    if(state == wait && route.getSize() != 0) {
        // �������й˿͵�ʱ�Ż�����
        Spot *currentPos = route.currentPos();  // ��ǰפ��
        Spot *nextPos = route.nextPos();        // ��һĿ�ĵ�
        if(nextPos->type == 'D' && currentTime < LATEST_SERVICE_TIME) {
            // ͣ���ȴ����ԣ������һվ�ǲֿⲢ�һ�û�е�ֹͣ����ʱ��
            // ���������ԭ�صȴ������������û�з�������LATEST_SERVICE_TIME
            // ʱ���̷��زֿ⣬�ʷ���һ����Ч�¼�
            state = wait;
            event.time = LATEST_SERVICE_TIME;
            event.eventType = finishedService;
            event.carIndex = carIndex;
            event.customerId = 0;
        } else {
            // ������������
            state = departure;
            currentPos->arrivedTime = currentTime;
            nearestDepartureTime = currentTime;
            float time = currentTime + dist(currentPos, nextPos);
            nextArriveTime = time;
            event.time = time;
            event.eventType = carArrived;
            event.carIndex = carIndex;
            event.customerId = nextPos->id;
        }
    }
    return event;
}

//================ assessment��� =================//
void Car::removeInvalidCustomer(vector<int> validCustomerId, int& retainNum){
    // �Ƴ�·���е���Ч�˿�
    // ��¼���������Ĺ˿���removeCustomerId�е�λ�ã�posVec
    posVec = route.removeInvalidCustomer(validCustomerId, retainNum);
}


void Car::updateTransformMatrix(Matrix<int> &transformMatrix){
    // ��transformMatrix���и���
    // transformMatrix����λ�ö�Ӧ�˿���validCustomerId��ָ��
    assert(posVec.size() != 0);  // Ϊ0��ʾû�н���removeInvalidCustomer�Ĳ���
    vector<int>::iterator intIter;
    for(intIter = posVec.begin(); intIter < posVec.end() - 1; intIter++) {
        int frontPos, backPos;
        frontPos = *(intIter);
        backPos = *(intIter+1);
        int temp = transformMatrix.getElement(frontPos, backPos);
        transformMatrix.setValue(frontPos, backPos, temp+1);
    }
}

int Car::computeScore(Matrix<int> transformMatrix){
    // ���㵱ǰ������·����transformMatrixָ���µĵ÷�
    // transformMatrix����λ�ö�Ӧ�˿���validCustomerId��ָ��
    assert(posVec.size() != 0);  // Ϊ0��ʾû�н���removeInvalidCustomer�Ĳ���
    vector<int>::iterator intIter;
    int score = 0;
    for(intIter = posVec.begin(); intIter < posVec.end() - 1; intIter++) {
        int frontPos, backPos;
        frontPos = *(intIter);
        backPos = *(intIter+1);
        if(frontPos !=0 && backPos !=0) {
            score += transformMatrix.getElement(frontPos, backPos);
        }
    }
    return score;
}
