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
private:
    Route route;    // �ƻ�Ҫ�ߵ�·��
    bool artificial;  // Ϊtrue��ʾ���鹹�ĳ�����false��ʾ��ʵ�ĳ���
    int carIndex;     // �������
};

#endif
