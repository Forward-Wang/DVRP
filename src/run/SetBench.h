#ifndef _SETBENCH_H
#define _SETBENCH_H

#include "../baseclass/Customer.h"
#include<vector>

using namespace std;

class SetBench{  // �����benchmark���޸ģ����ɶ�̬�˿�����
public:
    SetBench(){};
    ~SetBench(){};  // ��������
    void constructProbInfo(vector<Customer*> originCustomerSet); // ΪprobInfo��ֵ
    void changeTWL(vector<Customer*> customerSet, Customer depot, float alpha);

    void changeDYN(vector<Customer*> originCustomerSet, Customer depot, float dynamicism,
    vector<Customer*> &staticCustomer, vector<Customer*> &dynamicCustomer);
    void construct(vector<Customer*> originCustomerSet, vector<Customer*> &staticCustomerSet, 
            vector<Customer*> &dynamicCustomerSet, Customer depot);   // ����˿�����
};


#endif
