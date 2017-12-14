#ifndef _SSLR_H
#define _SSLR_H
#include<vector>
#include "Car.h"
#include<thread>
#include<mutex>
#include "LNSBase.h"

class SSLR: public LNSBase{  // SSALNS�㷨
public:
    SSLR(vector<Customer*> waitCustomer, vector<Car*> originPlan, float capacity, int maxIter=15000,
            bool verbose=false, int pshaw=6, int pworst=3, float eta=0.025f);
    ~SSALNS();
    void run(vector<Car*> &finalCarSet, float &finalCost, mutex &print_lck);  // �����㷨���൱���㷨��main()����
private:
    vector<Customer*> waitCustomer;   // ������Ĺ˿�
    vector<Customer*> allCustomer;    // ���й˿�
    vector<Car*> originPlan;          // ��ʼ�ƻ�
    Customer depot;
    float capacity;
    int maxIter;
    bool verbose;
};

#endif
