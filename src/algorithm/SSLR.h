#ifndef _SSLR_H
#define _SSLR_H
#include<vector>
#include "../baseclass/Car.h"
#include "../baseclass/Customer.h"
#include "../public/PublicFunction.h"
#include<thread>
#include<mutex>
#include "LNSBase.h"

float RANDOM_RANGE[2] = {-1, 1};

class SSLR: public LNSBase{  // SSALNS�㷨
public:
    SSLR(vector<Customer*> waitCustomer, vector<Car*> originPlan, float capacity, int maxIter=15000,
        bool verbose=false, int pshaw=6, int pworst=3, float eta=0.025f): LNSBase(pshaw, 
        pworst, eta, capacity, RANDOM_RANGE, mergeCustomer(waitCustomer, 
        extractCustomer(originPlan)), originPlan[0]->getRearNode(), true, true) 
    {
        this->maxIter = maxIter;
        this->verbose = verbose;
        vector<Customer*>::iterator custPtr;
        // ��waitCustomer�������ȼ���Ϊ2
        for(custPtr = waitCustomer.begin(); custPtr < waitCustomer.end(); custPtr++){
            Customer* newCust = new Customer(**custPtr);
            newCust->priority = 2;
            this->waitCustomer.push_back(newCust);
        }
        this->originPlan = copyPlan(originPlan);
    }
    ~SSLR();
    void run(vector<Car*> &finalCarSet, float &finalCost, mutex &print_lck);
private:
    vector<Customer*> waitCustomer;   // ������Ĺ˿�
    vector<Car*> originPlan;          // ��ʼ�ƻ�
    int maxIter;
    bool verbose;
};

#endif
