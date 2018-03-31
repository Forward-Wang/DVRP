#ifndef _CAR_H
#define _CAR_H
#include "Route.h"
#include "Matrix.h"
#include<vector>

enum State{wait, departure, serving, offwork};

class Car{
public:
    Car(Spot &headNode, Spot &rearNode, int carIndex, int depotIndex, 
        bool artificial = false);  // ���캯��
    ~Car();                // ��������
    Car(const Car& item);  // ���ƹ��캯��
    Car& operator= (Car &item);       // ���ظ�ֵ����

    bool timeWindowJudge(Spot *ref, Spot *cur) {
        return route.timeWindowJudge(ref, cur);
    }

    // ��ȡ��������
    int getCarIndex() { return carIndex; }  // �õ��������
    int getDepotIndex() { return depotIndex; } // �õ���Դ�ֿ���
    bool judgeArtificial() {return artificial;} // ���س�������
    Route* getRoute(){ return &route;}      // �õ�����·��
    float getCapacity() {return route.getCapacity();}    // ���س�����
    vector<Spot*> getAllCustomer() { return route.getAllCustomer();}
    int getCustomerNum(){ return route.getSize();}       // ��ȡ�˿���Ŀ
    vector<int> getAllID();   // ��ȡ���������нڵ��ID����˳��
    bool checkTimeConstraint() {return route.checkTimeConstraint(); }
    Spot* findCustomer(int id) { return route.findCustomer(id); }
	float getTrueLen() { return route.getTrueLen(); }
    float getDemand() { return route.getQuantity(); }
    float getTimeDuration() { return route.getTimeDuration(); }

    // ���Ļ�������
    void changeCarIndex(int newIndex) {carIndex = newIndex;}    // ���ĳ������
    void setProperty(bool newProperty) { artificial = newProperty; } // ���û�����������

    // ����insert cost��remove cost
    void computeInsertCost(Spot *cur, float &minValue, Spot *&ref1, float &secondValue, 
            Spot *&ref2, float randomNoise=0, bool allowNegativeCost=false);
    vector<float> computeReducedCost(float DTpara[]);  // �������нڵ���Ƴ�����

    // getCustomer����
    Spot* getHeadNode(){return route.getHeadNode();}    // �õ�������ͷ���
    Spot* getRearNode(){return route.getRearNode();}    // �õ�������β�ڵ�

    // insert �� delete Customer����
    void insertAtRear(Spot *item);   // ��·����β������ڵ�
    void insertAfter(Spot *ref, Spot *cur);
    void deleteCustomer(Spot *node);
private:
    Route route;    // �ƻ�Ҫ�ߵ�·��
    bool artificial;  // Ϊtrue��ʾ���鹹�ĳ�����false��ʾ��ʵ�ĳ���
    int carIndex;     // �������
    int depotIndex;   // ��Դ�ֿ��id
};

#endif
