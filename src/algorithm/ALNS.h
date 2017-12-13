#ifndef _ALNS_H
#define _ALNS_H
#include "Car.h"
#include<vector>

class ALNS{  // �㷨��
public:
	ALNS(vector<Customer*> allCustomer, Customer depot, float capacity, int maxIter,
            bool verbose=false, int pshaw=6, int pworst=3, float eta=0.025f);
	~ALNS(){};
	void run(vector<Car*> &finalCarSet, float &finalCost);  // �����㷨���൱���㷨��main()����
private:
	int maxIter;
    bool verbose;
};

#endif
