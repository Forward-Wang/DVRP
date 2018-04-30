#ifndef _SETBENCH_H
#define _SETBENCH_H

#include "../baseclass/Customer.h"
#include<vector>

using namespace std;

class SetBench{  // �����benchmark���޸ģ����ɶ�̬�˿�����
public:
    SetBench(vector<Customer*> originCustomerSet);  // ���캯��
    ~SetBench(){};  // ��������
    void constructProbInfo(); // ΪprobInfo��ֵ
    void construct(vector<Customer*> &staticCustomerSet, 
            vector<Customer*> &dynamicCustomerSet, Customer depot);   // ����˿�����
private:
    vector<Customer*> originCustomerSet; // ԭʼ�˿�����
};


#endif
