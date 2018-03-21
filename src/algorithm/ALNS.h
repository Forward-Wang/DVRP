#ifndef _ALNS_H
#define _ALNS_H
#include "../baseclass/Car.h"
#include "LNSBase.h"
#include<vector>

class ALNS: public LNSBase {  // �㷨��
public:
    ALNS(vector<Spot*> allCustomer, Spot depot, float capacity, int maxIter=15000, 
            bool verbose=false, int pshaw=6, int pworst=3, float eta=0.025);
    ~ALNS(){};
    void run(vector<Car*> &finalCarSet, float &finalCost);  // �����㷨���൱���㷨��main()����
private:
    int maxIter;
    bool verbose;
};

#endif
