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
    artificial(artificial), route(route) {
}

Car::~Car(){  
    // ��Ƕ��������������ᱻ���ã������ڴ˴�delete route
}

Car::Car(const Car& item):route(item.route), artificial(item.artificial){  
    // ���ƹ��캯��
    this->carIndex = item.carIndex;
}


Car& Car::operator= (Car &item){ 
    // ���ظ�ֵ����
    this->route = item.route;
    this->carIndex = item.carIndex;
    this->artificial = item.artificial;
    return *this;
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
