#ifndef _SSLR_H
#define _SSLR_H
#include<vector>
#include "../baseclass/Car.h"
#include "../baseclass/Spot.h"
#include "../public/PublicFunction.h"
#include<thread>
#include<mutex>
#include "LNSBase.h"


class SSLR: public LNSBase{  // SSALNS�㷨
public:
    SSLR(vector<Spot*> waitCustomer, vector<Car*> originPlan, float capacity, int maxIter=15000,
        bool verbose=false, int pshaw=6, int pworst=3, float eta=0.0f); 
    ~SSLR();
    void run(vector<Car*> &finalCarSet, float &finalCost, mutex &print_lck);
private:
    vector<Spot*> waitCustomer;   // ������Ĺ˿�
    vector<Car*> originPlan;          // ��ʼ�ƻ�
    int maxIter;
    bool verbose;
};

#endif
