#ifndef _CAR_H
#define _CAR_H
#include "Route.h"
#include "../modules/EventElement.h"
#include "Matrix.h"
#include<vector>

enum State{wait, departure, serving, offwork};

class Car{
public:
    Car(Customer &headNode, Customer &rearNode, float capacity, int index, 
            bool artificial = false);  // ���캯��
    ~Car();          // ��������
    Car(const Car& item);  //���ƹ��캯��
    Car& operator= (Car &item);       // ���ظ�ֵ����

    // ��ȡ��������
    int getCarIndex() {return carIndex;}  // �õ��������
    bool judgeArtificial() {return artificial;} // ���س�������
    Route getRoute(){ return route;}      // �õ�����·��
    float getCapacity() {return route.getCapacity();}    // ���س�����
    vector<Customer*> getAllCustomer() { return route.getAllCustomer();}
    int getCustomerNum(){ return route.getSize();}       // ��ȡ�˿���Ŀ
    Car getNullCar();  // �����еĹ˿�ɾ����������һ���ճ�
    float getTravelDistance() { return travelDistance; } // ��ȡ�����߹�����·��
    float getAddDistance() {
        // ��ȡ����Ϊ����̬�˿͵ĸ���·��
        return travelDistance - route.getOriginLen(); 
    } 

    // ���Ļ�������
    void changeCarIndex(int newIndex) {carIndex = newIndex;}    // ���ĳ������
    void setProperty(bool newProperty) { artificial = newProperty; } // ���û�����������

    // ����insert cost��remove cost
    void computeInsertCost(Customer item, float &minValue, Customer &customer1, 
            float &secondValue, Customer &customer2, float randomNose=0, 
            bool allowNegativeCost=false);
    vector<float> computeReducedCost(float DTpara[]);  // �������нڵ���Ƴ�����

    // getCustomer����
    Customer& getHeadNode(){return route.getHeadNode();}    // �õ�������ͷ���
    Customer& getRearNode(){return route.getRearNode();}    // �õ�������β�ڵ�
    Customer& getCurrentNode() {return route.currentPos();} // �õ�currentָ��ָ��Ľڵ�

    // insert �� delete Customer����
    void insertAtRear(Customer item);   // ��·����β������ڵ�
    void insertAtHead(Customer item);   // ��·��ͷ������ڵ�
    void insertAfter(Customer item1, Customer item2);     //  ��item1�����item2
    void insertAfter(Customer item1, Customer item2, float time);
    void deleteCustomer(Customer item); // ��·����ɾ��item�ڵ�

    // part Route����
    // ��newCar��·�����뵽��ǰ����·����current�ڵ�֮��
    void replaceRoute(Car &newCar, float currentTime);      	    
    Car capturePartRoute(float time);    // ץȡroute��currentָ��֮���·�������ҷ���һ����

    // state���
    void updateState(float time);        // ����״̬
    EventElement getCurrentAction(float time);        // ��û�����ǰʱ�̵Ķ���
    State getState(){ return state;}     // ���ػ�����ǰ��״̬
    EventElement launchCar(float currentTime);         // ��������������������wait״̬ʱ��Ч

    // assessment���
    void removeInvalidCustomer(vector<int> validCustomerId, int &retainNum);    // �Ƴ�·���е���Ч�˿�
    void updateTransformMatrix(Matrix<int> &transformMatrix);
    int computeScore(Matrix<int> transformMatrix);
private:
    State state;    // ����״̬�������н����ȴ�����
    Route route;    // �ƻ�Ҫ�ߵ�·��
    float nearestDepartureTime;   // ���µĳ���ʱ��
    float nextArriveTime;        // ��һվ�ĵ���ʱ��
    bool artificial;  // Ϊtrue��ʾ���鹹�ĳ�����false��ʾ��ʵ�ĳ���
    int carIndex;     // �������
    float travelDistance;   // ������ʻ�������
    vector<int> posVec;   // ���Ƴ�����Ч�ڵ�󣬼�¼·���нڵ���validCustomerId��λ�� 
};

#endif
