#ifndef _LNSBASE_H
#define _LNSBASE_H

#include "../baseclass/Customer.h"
#include "../baseclass/Car.h"
#include <vector>

const float MAX_FLOAT = numeric_limits<float>::max();
const float LARGE_FLOAT = 10000.0f;

using namespace std;

class LNSBase {
    public:
        LNSBase(int pshaw, int pworst, float eta, float capacity, float *randomRange, 
                vector<Customer*> allCustomer, Customer depot, bool herarchicalCar=false, 
                bool allowNegativeCost=false);
        ~LNSBase() { delete DTpara; }
        void resetDTpara(float *DTpara);
        void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
        void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
        void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
        void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, 
                bool noiseAdd);
        void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, 
                bool noiseAdd);
        void reallocateCarIndex(vector<Car*> &originCarSet);
        void removeNullRoute(vector<Car*> &originCarSet, bool mark=false);
        size_t codeForSolution(vector<Car*> originCarSet);
        float getCost(vector<Car*> originCarSet);
        float getTrueCost(vector<Car*> carSet);
        void updateWeight(int *freq, float *weight, int *score, float r, int num);
        void updateProb(float *removedProb, float *removedWeight, int removedNum);

        float maxd, mind, maxt, maxquantity; //节点间最大距离，最大时间窗间隔，最大货物量
    protected:
        int pshaw;  // 增加shaw removal中的随机性
        int pworst; // 增加worst removal的随机性
        float capacity;
        float baseNoise;
        float *DTpara; // 不同优先级顾客的奖惩因子
        float *randomRange; // 随机化噪声量的随机数左值和右值
        vector<Customer*> allCustomer;
        Customer depot;
        // 是否存在virtual car，若存在则插入过程中新开车辆只能是
        // virtual car
        bool hierarchicalCar;
        bool allowNegativeCost; // 是否允许计算insertion cost时有负数
};
#endif
