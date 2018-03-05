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
    void construct(vector<Spot*> &staticCustomerSet, vector<Spot*> &dynamicCustomerSet,
            vector<Spot*> &storeSet, Spot &depot);   // ����˿�����
private:
    float r1, r2, r3;
    int storeNum, subcircleNum, customerNum;
    float *lambda;
    vector<Spot*> storeSet;
    vector<Spot*> customerSet;
    Spot *depot;
};

#endif
