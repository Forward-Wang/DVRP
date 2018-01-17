#ifndef _SETBENCH_H
#define _SETBENCH_H

#include "../baseclass/Customer.h"
#include<vector>

using namespace std;

class SetBench{  // �����benchmark���޸ģ����ɶ�̬�˿�����
public:
    SetBench();  // ���캯��
    ~SetBench(){};  // ��������
    void constructProbInfo(); // ΪprobInfo��ֵ
    void construct(vector<Customer*> &staticCustomerSet, 
            vector<Customer*> &dynamicCustomerSet);   // ����˿�����
private:
    float r1, r2, r3;
    int numStore, numSubcircle;
    vector<float> lambda;
    int currentID;
};


#endif
