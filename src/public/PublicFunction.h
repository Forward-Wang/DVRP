#ifndef _PUBLICFUNCTION_H
#define _PUBLICFUNCTION_H

#include "../baseclass/Car.h"
#include "../baseclass/Spot.h"
#include<stdexcept>

using namespace std;

const float MAX_FLOAT = numeric_limits<float>::max();
const float LARGE_FLOAT = 100000000.0f;

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
float dist(Spot* node1, Spot *node2);
inline void withdrawPlan(vector<Car*> &Plan);    // ���ټƻ�
inline vector<Car*> copyPlan(vector<Car*> Plan); // ���Ƽƻ�
inline void deleteCustomerSet(vector<Spot*> &customerSet);            // ɾ��customerSet
inline vector<Spot*> copyCustomerSet(vector<Spot*> customerSet);  // ����customerSet
bool ascendSortForCustId(Spot* item1, Spot* item2);
int getCustomerNum(vector<Car*> originCarSet);
bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2);
bool customerSetEqual(vector<Spot*> c1, vector<Spot*> c2);
vector<Spot*> extractCustomer(vector<Car*> plan);
template<class T> inline void setZero(T* p, int size);
template<class T> inline void setOne(T *p, int size);
vector<int> getCustomerID(vector<Spot*> customerSet);
vector<int> getCustomerID(vector<Car*> carSet);
void showAllCustomerID(vector<Car*> carSet);
void showAllCustomerID(vector<Spot*> customerSet);
void showDetailForPlan(vector<Car*> carSet);
vector<float> getDemands(vector<Car*> carSet);
vector<float> getTimeDurations(vector<Car*> carSet);

// ģ�庯��������������ʵ��
inline void withdrawPlan(vector<Car*> &Plan){  
    // ���ټƻ�
    vector<Car*>::iterator carIter;
    for(carIter = Plan.begin(); carIter < Plan.end(); carIter++) {
        delete(*carIter);
    }
    Plan.clear();
	Plan.resize(0);
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
    vector<Spot*>::iterator iter, iter1;
    for(iter = customerSet.begin(); iter < customerSet.end();) {
        try {
            iter1 = iter+1;
            delete(*iter);
            iter = iter1;
        } catch (exception &e) {
            throw out_of_range(e.what());
        }
    }
    customerSet.clear();
	customerSet.resize(0);
}

inline vector<Spot*> copyCustomerSet(vector<Spot*> customerSet){  
    // ����customerSet
    vector<Spot*> outputCust;
    vector<Spot*>::iterator custIter;
    for(custIter = customerSet.begin(); custIter < customerSet.end(); custIter++) {
        Spot *newCust = new Spot(**custIter);
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
