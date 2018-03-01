#include "Car.h"
#include "../public/PublicFunction.h"
#include<cmath>
#include <stdexcept>

Car::Car(Spot &headNode, Spot &rearNode, float capacity, int index, bool artificial):
    carIndex(index), route(headNode, rearNode, capacity), artificial(artificial)
{
    state = wait;
    nearestDepatureTime = 0;
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
    this->nearestDepatureTime = item.nearestDepatureTime;
    this->nextArriveTime = item.nextArriveTime;
    this->travelDistance = item.travelDistance;
}


Car& Car::operator= (Car &item){ 
    // ���ظ�ֵ����
    this->route = item.route;
    this->carIndex = item.carIndex;
    this->state = item.state;
    this->artificial = item.artificial;
    this->nearestDepatureTime = item.nearestDepatureTime;
    this->nextArriveTime = item.nextArriveTime;
    this->travelDistance = item.travelDistance;
    return *this;
}


//================ �õ��������� =================//
Car* Car::getNullCar(){
    // ���ƻ������׽ڵ��β�ڵ��Լ�ʣ������
    // ��Ҫ���ⲿ����delet
    float leftCapacity = route.getLeftQuantity();
    Car* newCar = new Car(getHeadNode(), getRearNode(), leftCapacity, carIndex);
    return newCar;
}

vector<int> getAllID(){
    // ������нڵ��ID����˳��
    return route.getAllID();
}


//================ insert cost��remove cost =================//
void Car::computeInsertCost(Spot *store, Spot *customer, float &minValue, 
        Spot *refStore1, Spot *refCustomer1, float &secondValue, Spot *refStore2,
        Spot *refCustomer2, float randomNoise, bool allowNegativeCost){
        
    // ��������(stoer, customer)��·���е���С������ۺʹ�С�������
    // ���������/�μѲ����(refStore, refCustomer)
    // Args:
    //   * randomNoise: �������������
    //   * allowNegativeCost: �Ƿ�������ָ���insertion cost
    // Returns:
    //   * refStore1, refCustomer1: ��minValue=MAX_FLOAT������߾�ΪNULL
    //   * refStorer2, refCustomer2: ��secondValue=MAX_FLOAT������߾�ΪNULL
    route.computeInsertCost(stor, customer, minValue, refStore1, refCustomerr1,
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
    try {
        route.insertAtRear(item);
    } catch (exception &e) {
        throw out_of_range(e.what());
        exit(1);
    }
} 

void Car::insertAtHead(Spot *store, Spot *customer){    
    try {
        route.insertAtHead(store, customer);
    } catch (exception &e) {
        throw out_of_range(e.what());
        exit(1);
    }
}

void Car::insertAfter(Spot *ref, Spot *current) {
    // ��ref�ڵ�������current�ڵ�
    // ���ڵ��ڵ���룬��ʱֻ��Route.capture()������ʹ��
    try {
        route.insertAfter(ref, current);
    } catch (exception &e) {
        throw out_of_range(e.what());
    }
}

void Car::insertAfter(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer) {
    // ��refStore�������store����refCustomer�������customer
    Spot *tempSpot1 = refStore;
    Spot *tempSpot2 = refCustomer;
    if(refStore->id == 0) {
        // ˵��refStore�ڵ����鹹�ģ����������������һ���ڵ��ѱ�����
        // ����ʱ�ķ����(store, customer)��Ҫ���뵽ԭ������һ���ڵ㵽��һ���ڵ�֮��
        // ��˵��������·;�У����Ҹ���·����Ŀ�ĵظı䣬��Ӧ���޸�nextArriveTime����
        Spot *lastStop = route.currentPos();  // �������פ�㣨��һ�����ʵĽڵ㣩
        // ��ǰʱ�� = ��lastStop������ʱ�� + dist(lastStop, refStore)
        float currentTime = nearestDepatureTime + dist(lastStop, refStore);
        nextArriveTime = currentTime + dist(refStore, store)
        tempSpot1 = lastStop;
	}
    if(refCustomer == refStore) {
        // store��customer֮��û�б�Ľڵ�
        tempSpot2 = tempSpot1;
    }
    try {
        route.insertAfter(tempSpot1, tempSpot2, store, customer);
    } catch (exception &e) {
        throw out_of_rang0e(e.what());
        exit(1);
    }
}

void Car::deleteCustomer(Spot *item) {
    // ɾ�����ڵ����
    try {
        route.deleteNode(item);
    } catch (exception &e) {
        throw out_of_range(e.what());
        exit(1);
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
    updateState(currentTime);  // �ȸ���״̬
    Spot* currentNode = route.currentPos();  // ���������
    Spot* originNextNode = route.nextPos();  // ԭĿ�ĵ�
    route.replaceRoute(newCar->getRoute());      // replaceRoute�����Ļ�����״̬
    Spot* changedNextNode = route.nextPos(); // ���ĺ��Ŀ�ĵ�
    if(state == depature && originNextNode->id != changedNextNode->id) {
        // ���������·;�У����Ҹ���·����Ŀ�ĵظı䣬��Ӧ���޸�nextArriveTime����
        float dist = nextArriveTime - nearestDepatureTime;
        // x,y�ǻ�����ǰλ��
        float x = (currentTime - nearestDepatureTime) / dist * (originNextNode.x - 
                currentNode.x) + currentNode.x;
        float y = (currentTime - nearestDepatureTime) / dist * (originNextNode.y - 
                currentNode.y) + currentNode.y;
        nextArriveTime = currentTime + sqrt(pow(x - changedNextNode.x, 2) + 
                pow(y - changedNextNode.y, 2));
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
        case depature: {
            // ������·;�У���������ĳ�ʼ��
            // �õ����λ��λ�ڳ������Ŀ�ĵ������ϵ�ĳһ��
            // �õ��arrivedTime�趨Ϊ��ǰʱ�䣬������ʱ��Ϊ0���Ͳֿ�һ��
            float dist = nextArriveTime - nearestDepatureTime;
            startNode->x = (time - nearestDepatureTime) / dist * (nextNode.x - 
                    currentNode.x) + currentNode.x;
            startNode->y = (time - nearestDepatureTime) / dist * (nextNode.y - 
                    currentNode.y) + currentNode.y;
            startNode->arrivedTime = time;
            startNode->serviceTime = 0;
            startNode->startTime = time;
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
            break;
        }
        case serving: {
            // ���ӵ�ǰ�ڷ���˿ͣ���ʼ��Ϊ��ǰ�����
            // ������ʱ���趨���������ʱ���ȥ��ǰʱ��
            // ע�⵱�����ڵȴ���Ϊ�˿ͷ���ʱ������Ҳ��״̬�趨Ϊserving
            // ע���������˿͵������������nearestDepatureTime��������ǿ�������֮
            Spot currentPos = route.currentPos();
            startNode->x = currentNode.x;
            startNode->y = currentNode.y;
            startNode->arrivedTime = time;
            startNode->serviceTime = nearestDepatureTime - time;  
            // ����ʱ���Ѿ���ȥ��һ���֣�ע��˿͵����Ӧ��ȷ��arrivedTime
            // time - baseTime��ʾ�Ѿ��������ʱ��
            startNode->startTime = time;
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
    Spot depot = route.getRearNode();          // �κ�һ�������յ㶼��depot
    Car *newCar = new Car(*startNode, depot, leftQuantity, carIndex, false);
    Route* tempRoute = route.capture();                       // ץȡcurrentָ����·��
    vector<Spot*> tempCust = tempRoute->getAllCustomer();     // ���currentָ�������й˿�
    vector<Spot*>::iterator custIter;
    for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
        try {
            newCar->insertAtRear(**custIter);
        } catch (exception &e) {
            cerr << e.what() << endl;
            exit(1);
        }
    }
    deleteCustomerSet(tempCust);
    return newCar;
}


//================ state��� =================//
void Car::updateState(float time){
    // ���»���״̬
    switch(state){
        case depature: {
            // ԭ״̬�ǳ���������һ״̬�ǻ���ִ�з���
            // ��������»�����nearestDepartureTime
            if(time == nextArriveTime) {
                // ����ǰʱ��������״̬�ı��ʱ�䣬��״̬�ı�
                travelDistance += nextArriveTime - nearestDepatureTime;   // ����travelDistance
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
                    nearestDepatureTime = time + currentPos->serviceTime;
                    if(nearestDepatureTime == time) {  
                        // ��������Ȳ���Ҫ�ȴ���Ҳ����Ҫ�������������
                        state = depature;
                        nextArriveTime = nearestDepatureTime + dist(currentPos, nextPos);
                    } else {
                        // ������ʣ���ػ�������
                        route.decreaseLeftQuantity(currentPos->quantity);
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
            if(time == nearestDepatureTime) {
                // ���Խ���״̬ת��
                state = depature;
                nextArriveTime = nearestDepatureTime + dist(currrentPos, nextPos);
            }
            break;
        }
        case wait: {
            // do nothing now
            // �����������˻���֮����ܽ���״̬ת��
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
    Spot currentPos = route.currentPos();
    switch(state){
        case depature: {
            Spot *nextPos = route.nextPos();
            event.time = nextArriveTime;
            event.eventType = carArrived;
            event.customerId = nextPos->id;
            break;
        }
        case wait: {
            // do nothing	
            // ������Ч�¼���˵��������û������
            break;
        }
        case serving: {
            Spot *currentPos = route.currentPos();
            event.time = nearestDepatureTime;
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
        state = depature;
        Spot *currentPos = route.currentPos();  // ��ǰפ��
        Spot *nextPos = route.nextPos();        // ��һĿ�ĵ�
        nearestDepatureTime = currentTime;
        float time = currentTime + dist(currentPos, nextPos);
        nextArriveTime = time;
        event.time = time;
        event.eventType = carArrived;
        event.carIndex = carIndex;
        event.customerId = nextPos.id;
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
