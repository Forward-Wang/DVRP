#include "Car.h"
#include "../public/PublicFunction.h"
#include "../run/Config.h"
#include<cmath>
#include <stdexcept>

Car::Car(Spot &headNode, Spot &rearNode, int index, 
        bool artificial): route(headNode, rearNode)
{
    this->carIndex = index;
    this->artificial = artificial;
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
void Car::computeInsertCost(Spot *cur, float &minValue, Spot *&ref1, float &secondValue, 
        Spot *&ref2, float randomNoise, bool allowNegativeCost){
        
    // ����cur�ڵ���·���е���С������ۺʹ�С�������
    // ���������/�μѲ����ref1/ref2
    // Args:
    //   * randomNoise: �������������
    //   * allowNegativeCost: �Ƿ�������ָ���insertion cost
    // Returns:
    //   * ref1: ������С�Ĳ���㣬��minValue=MAX_FLOAT����ΪNULL
    //   * ref2: ���۴�С�Ĳ���㣬��secondValue=MAX_FLOAT����ΪNULL
    route.computeInsertCost(cur, minValue, ref1, secondValue, ref2, randomNoise, allowNegativeCost);
}

vector<float> Car::computeReducedCost(float DTpara[]){  
    // �õ�·�������з����(P-D)���Ƴ����ۣ�ֵԽС��ʾ�����Խ�ʡ����Ĵ���
    // Args:
    //   * DTpara: ��ͬ���ȼ��˿ͼ���reduce costʱ��bonus
    return route.computeReducedCost(DTpara, artificial);
}


//================ insert �� delete Customer���� =================//
void Car::insertAtRear(Spot *item){
    route.insertAtRear(item);
} 

void Car::insertAfter(Spot *ref, Spot *cur) {
    // ��ref�������cur
    route.insertAfter(refStore, refCustomer, store, customer);
}

void Car::deleteCustomer(Spot *node) {
    // ɾ���ڵ�node
    route.deleteNode(node);
}
