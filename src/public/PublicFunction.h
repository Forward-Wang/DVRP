#ifndef _PUBLICFUNCTION_H
#define _PUBLICFUNCTION_H

#include "../baseclass/Car.h"
#include "../baseclass/Spot.h"
#include "../xml/tinystr.h"
#include "../xml/tinyxml.h"
#include<stdexcept>

using namespace std;

float random(float start, float end);
template<class T1, class T2> bool ascendSort(pair<T1, T2> x, pair<T1, T2> y) {
    // ��������
    // �ڶ���Ԫ�ذ�����Ԫ����ԭ�����е�λ��
    return x.first < y.first;
}
template<class T1, class T2>  bool descendSort(pair<T1, T2> x, pair<T1, T2> y) {
    // �ݼ�����
    // �ڶ���Ԫ�ذ�����Ԫ����ԭ�����е�λ��
    return x.first > y.first;
}
vector<float> randomVec(int num);
// ��÷�Χ��lb��ub��m�����ظ�������
// ʣ����������restData��
vector<int> getRandom(int lb, int ub, int m, vector<int> &restData);
// ����probability��Ӧ�������㷨�õ�������������ֵ���ɢֵ
// ��Ƕ��probability���й�һ���ĺ���
int roulette(vector<float> probability);
int poissonSampling(float lambda, float duration);

float dist(Spot* node1, Spot *node2);

// ����probability��Ӧ�������㷨�õ�������������ֵ���ɢֵ
// ������Ǹ��������ͷָ���Լ��ܹ��ĸ��ʷֲ�����
// ��Ƕ��probability���й�һ���ĺ���
int roulette(float *probability, int num);
inline void withdrawPlan(vector<Car*> &Plan);    // ���ټƻ�
inline vector<Car*> copyPlan(vector<Car*> Plan); // ���Ƽƻ�
inline void deleteCustomerSet(vector<Spot*> &customerSet);            // ɾ��customerSet
inline vector<Spot*> copyCustomerSet(vector<Spot*> customerSet);  // ����customerSet
void seperateCustomer(vector<Spot*> originCustomerSet, vector<Spot*> &staticCustomer, 
        vector<Spot*> &dynamicCustomer, float dynamicism);
bool ascendSortForCustId(Spot* item1, Spot* item2);
void computeBest(vector<Car*> carSet, vector<Car*> &bestRoute, float &bestCost);
int getCustomerNum(vector<Car*> originCarSet);
bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2);
bool customerSetEqual(vector<Spot*> c1, vector<Spot*> c2);
vector<Spot*> extractCustomer(vector<Car*> plan);
vector<Spot*> mergeCustomer(vector<Spot*> waitCustomer, vector<Spot*> originCustomer);
template<class T> inline void setZero(T* p, int size);
template<class T> inline void setOne(T *p, int size);
vector<int> getCustomerID(vector<Spot*> customerSet);
vector<int> getCustomerID(vector<Car*> carSet);
void showAllCustomerID(vector<Car*> carSet);
void showAllCustomerID(vector<Spot*> customerSet);
void showAllID(vector<Car*> carSet);
void showDetailForPlan(vector<Car*> carSet);

// ģ�庯��������������ʵ��
inline void withdrawPlan(vector<Car*> &Plan){  
    // ���ټƻ�
    vector<Car*>::iterator carIter;
    for(carIter = Plan.begin(); carIter < Plan.end(); carIter++) {
        delete(*carIter);
    }
    Plan.clear();
}

inline vector<Car*> copyPlan(vector<Car*> Plan) {
    // ���Ƽƻ�
    vector<Car*>::iterator carIter;
    vector<Car*> outputPlan;
    for(carIter = Plan.begin(); carIter < Plan.end(); carIter++) {
        Car* newCar = new Car(**carIter);
        outputPlan.push_back(newCar);
    }
    return outputPlan;
}

inline void deleteCustomerSet(vector<Spot*> &customerSet){   // ɾ��CustomerSet
    vector<Spot*>::iterator iter;
    for(iter = customerSet.begin(); iter < customerSet.end(); iter++) {
        try {
            delete((*iter)->choice);
            delete(*iter);
        } catch (exception &e) {
            throw out_of_range(e.what());
        }
    }
    customerSet.clear();
}

inline vector<Spot*> copyCustomerSet(vector<Spot*> customerSet){  
    // ����customerSet
    // ע��Ҫͬʱ���Ƴ��̼�
    vector<Spot*> outputCust;
    vector<Spot*>::iterator custIter;
    for(custIter = customerSet.begin(); custIter < customerSet.end(); custIter++) {
        Spot *newCust = new Spot(**custIter);
        Spot *newStore = new Spot(*((*custIter)->choice));
        newCust->choice = newStore;
        newStore->choice = newCust;
        outputCust.push_back(newCust);
    }
    return outputCust;
}

template<class T>
inline void setZero(T* p, int size) {
    for (int i=0; i<size; i++) {
        *(p++) = (T)0;
    }
}

template<class T>
inline void setOne(T* p, int size) {
    for (int i=0; i<size; i++) {
        *(p++) = (T)1;
    }
}

#endif
