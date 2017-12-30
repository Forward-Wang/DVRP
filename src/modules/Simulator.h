#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "../baseclass/Customer.h"
#include "../baseclass/Car.h"
#include<vector>

class Simulator{  // ��������
public:
    Simulator(int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
            vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan); // ���캯��
    ~Simulator();  // �������� 
    vector<Car*> initialPlan(Customer depot, float capacity);     // ���ò����ƶ���ʼ�ƻ�
    vector<Car*> replan(vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId, 
            vector<int> &delayCustomerId, float capacity);
    vector<Customer*> generateScenario();  // ������̬�˿͵��龰
    vector<Car*> no_replan();
    bool checkFeasible(vector<Car*> carSet);
private:
    int slotIndex;      // ��ǰ�ǵڼ���slot
    // �������Ĺ˿ͣ���һ��Ҫ�ڼƻ����У�
    vector<Customer*> promiseCustomerSet;   
    // ����Ҫ����Ĺ˿ͣ����Բ������ڼƻ����У����Ǳ�����tolerantTime֮ǰ�ظ��ܷ����
    vector<Customer*> waitCustomerSet;       
    vector<Customer*> dynamicCustomerSet;     // δ֪�Ĺ˿ͼ�
    // ��ǰ�ƻ�
    // �ڳ�ʼ����ʱ�򣬲�û�е�ǰ�ƻ�����ʱcurrentPlan������һ���ճ���������������ȡcapacity, depot��Ϣ
    vector<Car*> currentPlan;  
};

#endif
