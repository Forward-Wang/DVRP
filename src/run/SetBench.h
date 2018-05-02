#ifndef _SETBENCH_H
#define _SETBENCH_H

#include "../baseclass/Spot.h"
#include<vector>

using namespace std;

class SetBench{  // �����benchmark���޸ģ����ɶ�̬�˿�����
public:
    SetBench();  // ���캯��
    ~SetBench(){};  // ��������
    void constructStoreSet();
    void constructCustomerSet();
    void constructDepot();
    void changeTWL(vector<Spot*> customerSet, Spot *depot, float newAlpha);
    void changeDYN(vector<Spot*> customerSet, Spot *depot, int beginIndex,
            vector<Spot*> &staticCustomer, vector<Spot*> &dynamicCustomer);
    void construct(vector<Spot*> &staticCustomerSet, vector<Spot*> &dynamicCustomerSet,
            vector<Spot*> &storeSet, Spot &depot, float currentTime);   // ����˿�����
private:
    vector<Spot*> storeSet;
    vector<Spot*> customerSet;
    Spot *depot;
};

#endif
