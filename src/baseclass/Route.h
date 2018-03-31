#ifndef _ROUTE_H
#define _ROUTE_H

#include<iostream>
#include<vector>
#include "Spot.h"

using namespace std;


class Route{
public:
    Route(Spot &headNode, Spot &rearNode, float capacity, float timeDuration); // ���캯��
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
    void insertAtRear(Spot *node);
    void deleteNode(Spot *node);

    // ��õ��ڵ����
    Spot* getHeadNode();    // �õ�ͷ���
    Spot* getRearNode();    // �õ�β�ڵ�
    Spot* findCustomer(int id);

    // ��ȡ��������
    float getQuantity() {return this->quantity;}            // �õ�������ʹ�õ�����(װ�ع˿�)
    float getCapacity() {return this->capacity;}            // ���ر����ĳ�����
    float getLen(float DTpara[], bool artificial = false);  // �õ�·������(�ӳͷ�)
	float getTrueLen();      // �õ���ʵ·������(���ӳͷ�)
    int getSize();           // �õ���ǰ����Ĵ�С
    vector<Spot*> getAllCustomer();  // �õ�·�������еĹ˿ͽڵ�
    vector<int> getAllID();
    vector<float> getArrivedTime();
    float getTimeDuration();

    // �������/ɾ���ڵ����
    // �������нڵ���Ƴ�����
    vector<float> computeReducedCost(float DTpara[], bool artificial = false);
    // ����item�ڵ���·���е���С������ۺʹ�С�������
    // ���������/�μѲ����ǰ��Ĺ˿ͽڵ�
    // penaltyParaΪ�ͷ�ϵ����������Ҫ�ͷ���penaltyPara = 0
    void computeInsertCost(Spot *cur, float &minValue, Spot *&ref1, float &secondValue, Spot *&ref2, 
        float randomNoise = 0.0f, bool allowNegativeCost = true);  

    // �����item���뵽pre�����Ƿ��Υ��ʱ�䴰Լ��
    bool timeWindowJudge(Spot *ref, Spot *cur);
    // ���ศ�����ʵĺ���
    bool checkTimeConstraint();
private:
    // ��ͷ����β����ǰָ���פ��
    // ��ǰָ��ָ��ǰһ�γ����أ���δ������Ϊ�ֿ⣩
    // פ��ָ������ǰ���ڵ�
    Spot *head, *rear;
    int size;         // ����ĳ���
    float timeDuration; // ������ʻʱ�����ֵ
    float quantity;   // ��ǰ��ʹ�õ�����(��װ�صĹ˿������й�)
    float capacity;       // �������������ﱣ��
    void copy(const Route& L);  // �������������ƹ��캯�������ء�=������ʹ��
};

#endif
