#ifndef _PUBLICFUNCTION_H
#define _PUBLICFUNCTION_H

#include "../baseclass/Car.h"
#include "../xml/tinystr.h"
#include "../xml/tinyxml.h"

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

// ����probability��Ӧ�������㷨�õ�������������ֵ���ɢֵ
// ������Ǹ��������ͷָ���Լ��ܹ��ĸ��ʷֲ�����
// ��Ƕ��probability���й�һ���ĺ���
int roulette(float *probability, int num);
inline void withdrawPlan(vector<Car*> &Plan);    // ���ټƻ�
inline vector<Car*> copyPlan(vector<Car*> Plan); // ���Ƽƻ�
inline void deleteCustomerSet(vector<Customer*> &customerSet);            // ɾ��customerSet
inline vector<Customer*> copyCustomerSet(vector<Customer*> customerSet);  // ����customerSet
void seperateCustomer(vector<Customer*> originCustomerSet, vector<Customer*> &staticCustomer, 
        vector<Customer*> &dynamicCustomer, float dynamicism);
bool ascendSortForCustId(Customer* item1, Customer* item2);
void computeBest(vector<Car*> carSet, vector<Car*> &bestRoute, float &bestCost);
void showAllCustomer(vector<Car*> carSet);
int getCustomerNum(vector<Car*> originCarSet);
bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2);
bool customerSetEqual(vector<Customer*> c1, vector<Customer*> c2);
vector<Customer*> extractCustomer(vector<Car*> plan);
template<class T> inline void setZero(T* p, int size);
template<class T> inline void setOne(T *p, int size);

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

inline void deleteCustomerSet(vector<Customer*> &customerSet){   // ɾ��CustomerSet
    vector<Customer*>::iterator iter;
    for(iter = customerSet.begin(); iter < customerSet.end(); iter++) {
        delete(*iter);
    }
    customerSet.clear();
}

inline vector<Customer*> copyCustomerSet(vector<Customer*> customerSet){  // ����customerSet
    vector<Customer*> outputCust;
    vector<Customer*>::iterator custIter;
    for(custIter = customerSet.begin(); custIter < customerSet.end(); custIter++) {
        Customer *newCust = new Customer(**custIter);
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
