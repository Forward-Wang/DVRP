#ifndef _ROUTE_H
#define _ROUTE_H

#include<iostream>
#include<vector>
#include "Spot.h"

using namespace std;


class Route{
public:
    Route(Spot &headNode, Spot &rearNode, float capacity);      // ���캯��
    ~Route();   // ��������
    Route(const Route &L);  // ���ƹ��캯��
    Route& operator= (const Route &L);  // ���ظ�ֵ����� 
    Spot& operator[] (int k);     // �õ������еĵ�k��Ԫ��
    const Spot& operator[] (int k) const; // ��̬����
    bool isEmpty();   // �ж������Ƿ�Ϊ��
    void printRoute();   // ��ӡ·��
    void clear();     // �������

    // �����Լ�ɾ���ڵ����
    void insertAfter(Spot *pre, Spot *current);    
    void insertAfter(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer); 
    void insertAtHead(Spot *store, Spot *customer);
    void insertAtRear(Spot *node);
    void deleteNode(Spot *node);
    void deleteNode(Spot *store, Spot *customer);

    // ��õ��ڵ����
    Spot* currentPos();   // ���ص�ǰλ��
    Spot* nextPos() {return current->next;}      // ��һ���ڵ��λ��
    Spot* pastPos() {return current->front;}     // ǰһ���ڵ��λ��
    Spot* getHeadNode();    // �õ�ͷ���
    Spot* getRearNode();    // �õ�β�ڵ�
    Spot* findCustomer(int id);

    // ��ȡ��������
    float getQuantity() {return this->quantity;}            // �õ�������ʹ�õ�����(װ�ع˿�)
    float getLeftQuantity() {return this->leftQuantity;}    // �õ�����ʣ���������복����ʻ�����أ�
    float getCapacity() {return this->capacity;}            // ���ر����ĳ�����
    float getLen(float DTpara[], bool artificial = false);  // �õ�·������
    float getOriginLen();    // ����·���е�������̬�˿ͽڵ������ѵ�·������
    int getSize();           // �õ���ǰ����Ĵ�С
    vector<float> getArrivedTime();      // �õ��������нڵ��arrivedTime
    vector<Spot*> getAllCustomer();  // �õ�·�������еĹ˿ͽڵ�
    vector<Spot*> getAllSpot();      // �õ����еĽڵ�
    vector<int> getAllID();          // ��ȡ���нڵ��ID����˳��

    // �޸���������
    void decreaseLeftQuantity(float amount) {leftQuantity -= amount;}   
    // ����ʣ��ĳ��������������µĹ˿ͣ�
    void setLeftQuantity(float newValue) {leftQuantity = newValue;}
    void setCurrentArrivedTime(float time) {current->arrivedTime = time;}      
    // ���õ�ǰ�ڵ�ĵ���ʱ��
    bool moveForward();      // ��ǰ��

    // �������/ɾ���ڵ����
    // �������нڵ���Ƴ�����
    vector<float> computeReducedCost(float DTpara[], bool artificial = false);
    // ����item�ڵ���·���е���С������ۺʹ�С�������
    // ���������/�μѲ����ǰ��Ĺ˿ͽڵ�
    // penaltyParaΪ�ͷ�ϵ����������Ҫ�ͷ���penaltyPara = 0
    void computeInsertCost(Spot *store, Spot *customer, float &minValue, Spot *&refStore1,
            Spot *&refCustomer1, float &secondValue, Spot *&refStore2, Spot *&refCustomer2,
            float randomNoise = 0.0f, bool allowNegativeCost = true);  

    // �����item���뵽pre�����Ƿ��Υ��ʱ�䴰Լ��
    bool timeWindowJudge(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer);
    
    // ����һ�¸����ڵ�ĵ���ʱ��
    void refreshArrivedTime();   

    // ·�����滻����ȡ
    // ��route�滻��currentָ����·��
    Route* getEmptyRoute(vector<Spot*> &removedCustomer);
    void replaceRoute(Route *route);  
    // ץȡcurrentָ����·��
    vector<Spot*> capture();  

    // ���ศ�����ʵĺ���
    vector<int> removeInvalidCustomer(vector<int> validCustomerId, int &retainNum);
    bool checkPassRoute();
    bool checkTimeConstraint();
private:
    Spot *head, *current, *rear;  // ��ͷ����β�͵�ǰָ�룬��ǰָ��ָ�������ǰ��פ��
    int size;         // ����ĳ���
    float quantity;   // ��ǰ��ʹ�õ�����(��װ�صĹ˿������й�)
    float leftQuantity;   // ʣ���������복����ʻ����йأ�
    float capacity;       // �������������ﱣ��
    void copy(const Route& L);  // �������������ƹ��캯�������ء�=������ʹ��
    vector<float> arrivedTime;  // �ڸ����ڵ�ĵ���ʱ��
};


#endif
