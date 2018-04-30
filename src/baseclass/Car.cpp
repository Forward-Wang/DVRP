#include "Car.h"
#include "../public/PublicFunction.h"
#include<cmath>
#include <stdexcept>

Car::Car(Customer &headNode, Customer &rearNode, float capacity, int index, bool artificial):
    carIndex(index), route(headNode, rearNode, capacity), artificial(artificial)
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
Car Car::getNullCar(){
    // ���ƻ������׽ڵ��β�ڵ��Լ�ʣ������
    float leftCapacity = route.getLeftQuantity();
    Car newCar(getHeadNode(), getRearNode(), leftCapacity, carIndex);
    return newCar;
}


//================ insert cost��remove cost =================//
void Car::computeInsertCost(Customer item, float &minValue, Customer &customer1, 
        float &secondValue, Customer &customer2, float randomNoise, bool allowNegativeCost){
    // ����item�ڵ㵽����·������С������ۺʹ�С�������
    // randomNoise: �������������
    // allowNegativeCost: �Ƿ�������ָ���insertion cost
    route.computeInsertCost(item, minValue, customer1, secondValue, customer2, 
            randomNoise, allowNegativeCost);
}

vector<float> Car::computeReducedCost(float DTpara[]){  
    // ���㳵��Ҫ��������нڵ���Ƴ�����
    // DTpara: ��ͬ���ȼ��˿ͼ���reduce costʱ��bonus
    return route.computeReducedCost(DTpara, artificial);
}


//================ insert �� delete Customer���� =================//
void Car::insertAtRear(Customer item){
    try {
        route.insertAtRear(item);
    } catch (exception &e) {
        throw out_of_range(e.what());
    }
} 

void Car::insertAtHead(Customer item){    
    try {
        route.insertAtHead(item);
    } catch (exception &e) {
        throw out_of_range(e.what());
    }
}

void Car::insertAfter(Customer item1, Customer item2) {
    // �򵥵���item1�������item2
    Customer itema, itemb;
    itema = item1;
    itemb = item2;
    if(item1.id == 0) {
        // ˵�����뵽����ʼ�����
        // ����ʼ����Ķ����capturePartRoute()
        // ������Ҫ����һ��nextArriveTime
        // ���������·;�У����Ҹ���·����Ŀ�ĵظı䣬��Ӧ���޸�nextArriveTime����
        if(state != wait) {
            Customer itemx = route.getStand();  // �������פ�㣨��ʵ��·;�У�פ��ָ���ǹ˿͵㣩
            nextArriveTime = nearestDepartureTime + dist(&itemx, &item2);
        }
        itema = route.currentPos();
	}
    try {
        route.insertAfter(itema, itemb);
    } catch (exception &e) {
        throw out_of_range(e.what());
    }
    if(state == departure) {
        nearestDepartureTime = route.getStand().arrivedTime + route.getStand().serviceTime;
    }
}

void Car::insertAfter(Customer item1, Customer item2, float time) {
    // ��Ҫupdate state�İ汾
    updateState(time);
    Customer itema, itemb;
    itema = item1;
    itemb = item2;
    if(item1.id == 0) {
        // ��һվĿ�ĵظı�ʱ������nextArriveTime
        if(state != wait) {
            Customer itemx = route.getStand();
            nextArriveTime = nearestDepartureTime + dist(&itemx, &item2);
        }
        itema = route.currentPos();
    }
    try {
        route.insertAfter(itema, itemb);
    } catch (exception &e) {
        throw out_of_range(e.what());
    }
} 

void Car::deleteCustomer(Customer item) {
    try {
        route.deleteNode(item);
    } catch (exception &e) {
        throw out_of_range(e.what());
    }
}


//================ part Route���� =================//
void Car::replaceRoute(Car &newCar, float currentTime){      
    // ��newCar��·�����뵽��ǰ����·����current�ڵ�֮��
    updateState(currentTime);  // �ȸ���״̬
    Customer currentNode = route.currentPos();  // ���������
    Customer originNextNode = route.nextPos();  // ԭĿ�ĵ�
    try {
        route.replaceRoute(newCar.getRoute());
    } catch (exception &e) {
        cout << "Car id is #" << carIndex << endl;
        throw out_of_range(e.what());
    }
    Customer changedNextNode = route.nextPos(); // ���ĺ��Ŀ�ĵ�
    if(state == departure && originNextNode.id != changedNextNode.id) {
        // ���������·;�У����Ҹ���·����Ŀ�ĵظı䣬��Ӧ���޸�nextArriveTime����
        Customer standPos = route.getStand();
        nextArriveTime = currentTime + dist(&standPos, &changedNextNode);
    } 
}

Car Car::capturePartRoute(float time){   
    // ץȡroute��currentָ��֮���·�������ҷ���һ����
    // timeΪץȡ��ʱ��
    updateState(time);    // �Ƚ�״̬����
    Customer *startNode = new Customer;         // ���ӵĳ�����
    *startNode = route.getStand();
    startNode->id = 0;   
    startNode->type = 'D';
    startNode->priority = 0;
    // ��currentָ���Ĺ˿�����newCar�У�ע�����ʣ������leftQuanity
    float leftQuantity = route.getLeftQuantity();  // ����ʣ������
    Customer depot = route.getRearNode();          // �κ�һ�������յ㶼��depot
    Car newCar(*startNode, depot, leftQuantity, carIndex, false);
    Route tempRoute = route.capture();                         // ץȡcurrentָ����·��
    vector<Customer*> tempCust = tempRoute.getAllCustomer();   // ���currentָ�������й˿�
    vector<Customer*>::iterator custIter;
    for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
        try {
            newCar.insertAtRear(**custIter);
        } catch (exception &e) {
            throw out_of_range("In capture part route: " + string(e.what()));
        }
    }
    deleteCustomerSet(tempCust);
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
                Customer currentPos = route.currentPos();  // ��ǰפ��
                route.currentPos().arrivedTime = time;     // ���µ�ǰ�˿͵ĵ���ʱ��
                if(currentPos.id == 0) {
                    state = offwork;   // ����ֿ⣬�ճ�
                } else {
                    Customer nextPos = route.nextPos();
                    if(time < currentPos.startTime) {
                        time = currentPos.startTime;
                    }
                    nearestDepartureTime = time + currentPos.serviceTime;
                    if(nearestDepartureTime == time) {  
                        // ��������Ȳ���Ҫ�ȴ���Ҳ����Ҫ�������������
                        state = departure;
                        nextArriveTime = nearestDepartureTime + sqrt(pow(currentPos.x - nextPos.x, 2) 
                                + pow(currentPos.y - nextPos.y, 2));
                    } else {
                        route.setStand(currentPos.x, currentPos.y, time, currentPos.serviceTime);
                        route.decreaseLeftQuantity(currentPos.quantity);
                        state = serving;
                    }
                }
            } else {
                // ��ά��departure״̬��ֻ����stand�ڵ������Ϣ
                Customer currentPos = route.getStand();
                Customer nextPos = route.nextPos();
                float distance = nextArriveTime - nearestDepartureTime;
                float x = (time - nearestDepartureTime) / distance * (nextPos.x -
                        currentPos.x) + currentPos.x;
                float y = (time - nearestDepartureTime) / distance * (nextPos.y -
                        currentPos.y) + currentPos.y;
                route.setStand(x, y, time);
                travelDistance = travelDistance + time - nearestDepartureTime;
                nearestDepartureTime = time;
            }
            break;
        }
        case serving: {
            // ԭ״̬��ִ�з�������һ״̬�ǻ�������
            Customer currentPos = route.currentPos();
            Customer nextPos = route.nextPos();
            if(time == nearestDepartureTime) {
                // ���Խ���״̬ת��
                route.setStand(currentPos.x, currentPos.y, time);
                state = departure;
                nextArriveTime = nearestDepartureTime + sqrt(pow(currentPos.x - nextPos.x, 2) 
                        + pow(currentPos.y - nextPos.y, 2));
            } else {
                // ��ά��serving״̬������stand�����Ϣ
                route.setStand(currentPos.x, currentPos.y, time, nearestDepartureTime-time);
            }
            break;
        }
        case wait: {
            // do nothing now
            // �����������˻���֮����ܽ���״̬ת��
            // �Ա���wait״̬������stand�����Ϣ
            Customer currentPos = route.currentPos();
            route.setStand(currentPos.x, currentPos.y, time);
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
    Customer currentPos = route.currentPos();
    switch(state){
        case departure: {
            Customer nextPos = route.nextPos();
            event.time = nextArriveTime;
            event.eventType = carArrived;
            event.customerId = nextPos.id;
            break;
        }
        case wait: {
            // do nothing	
            // ������Ч�¼���˵��������û������
            break;
        }
        case serving: {
            Customer currentPos = route.currentPos();
            event.time = nearestDepartureTime;
            event.customerId = currentPos.id; 
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
        state = departure;
        Customer currentPos = route.currentPos();  // ��ǰפ��
        Customer nextPos = route.nextPos();        // ��һĿ�ĵ�
        nearestDepartureTime = currentTime;
        float time = currentTime + sqrt(pow(currentPos.x - nextPos.x, 2) 
                + pow(currentPos.y - nextPos.y, 2));
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
