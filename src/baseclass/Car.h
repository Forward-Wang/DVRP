#ifndef _CAR_H
#define _CAR_H
#include "Route.h"
#include "../modules/EventElement.h"
#include "Matrix.h"
#include<vector>

enum State{wait, departure, serving, offwork};

class Car{
public:
    Car(Spot &headNode, Spot &rearNode, float capacity, int index, 
            bool artificial = false);  // ���캯��
    Car(Route &route, int index, bool artificial=false);  // ���캯��
    ~Car();          // ��������
    Car(const Car& item);  //���ƹ��캯��
    Car& operator= (Car &item);       // ���ظ�ֵ����

    bool timeWindowJudge(Spot *refStore, Spot *refCustomer, Spot *store,
            Spot *customer) {
        return route.timeWindowJudge(refStore, refCustomer, store, customer);
    }

    // ��ȡ��������
    int getCarIndex() {return carIndex;}  // �õ��������
    bool judgeArtificial() {return artificial;} // ���س�������
    Route* getRoute(){ return &route;}      // �õ�����·��
    float getCapacity() {return route.getCapacity();}    // ���س�����
    vector<Spot*> getAllCustomer() { return route.getAllCustomer();}
    int getCustomerNum(){ return route.getSize();}       // ��ȡ(P-D)�Ե���Ŀ
    Car* getNullCar(vector<Spot*> &removedCustomer);  // �����еĹ˿�ɾ����������һ���ճ�
    float getTravelDistance() { return travelDistance; } // ��ȡ�����߹�����·��
    float getAddDistance() {
        // ��ȡ����Ϊ����̬�˿͵ĸ���·��
        return travelDistance - route.getOriginLen(); 
    }
    vector<int> getAllID();   // ��ȡ���������нڵ��ID����˳��
    bool checkTimeConstraint() {return route.checkTimeConstraint(); }
    Spot* findCustomer(int id) { return route.findCustomer(id); }

    // ���Ļ�������
    void changeCarIndex(int newIndex) {carIndex = newIndex;}    // ���ĳ������
    void setProperty(bool newProperty) { artificial = newProperty; } // ���û�����������

    // ����insert cost��remove cost
    void computeInsertCost(Spot *store, Spot *customer, float &minValue,
            Spot *&refStore1, Spot *&refCustomer1, float &secondValue, Spot *&refStore2,
            Spot *&refCustomer2, float randomNoise=0, bool allowNegativeCost=false);
    vector<float> computeReducedCost(float DTpara[]);  // �������нڵ���Ƴ�����

    // getCustomer����
    Spot* getHeadNode(){return route.getHeadNode();}    // �õ�������ͷ���
    Spot* getRearNode(){return route.getRearNode();}    // �õ�������β�ڵ�
    Spot* getCurrentNode() {return route.currentPos();} // �õ�currentָ��ָ��Ľڵ�

    // insert �� delete Customer����
    void insertAtRear(Spot *item);   // ��·����β������ڵ�
    void insertAtHead(Spot *store, Spot *customer);     // ��·��ͷ������ڵ�
    void insertAfter(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer);
    void insertAfter(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer,
            float time);
    void deleteCustomer(Spot *store, Spot *customer);

    // part Route����
    // ��newCar��·�����뵽��ǰ����·����current�ڵ�֮��
    void replaceRoute(Car *newCar, float currentTime);      	    
    Car* capturePartRoute(float time);    // ץȡroute��currentָ��֮���·�������ҷ���һ����

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
