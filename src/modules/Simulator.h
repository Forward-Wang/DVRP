#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "../baseclass/Spot.h"
#include "../baseclass/Car.h"
#include<vector>
#include<mutex>

class Simulator{  // ��������
public:
    Simulator(int slotIndex, vector<Spot*> promiseCustomerSet, vector<Spot*> waitCustomerSet,
            vector<Spot*> dynamicCustomerSet, vector<Car*> currentPlan, vector<Spot*> storeSet); // ���캯��
    ~Simulator();  // �������� 
    vector<Car*> initialPlan(Spot depot, float capacity);     // ���ò����ƶ���ʼ�ƻ�
    vector<Car*> replan(vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId, 
            vector<int> &delayCustomerId, float capacity);
    vector<Spot*> generateScenario(Spot depot);  // ������̬�˿͵��龰
    vector<Car*> no_replan();
    bool checkFeasible(vector<Car*> carSet);
private:
    int slotIndex;      // ��ǰ�ǵڼ���slot
    // �������Ĺ˿ͣ���һ��Ҫ�ڼƻ����У�
    vector<Spot*> promiseCustomerSet;   
    // ����Ҫ����Ĺ˿ͣ����Բ������ڼƻ����У����Ǳ�����tolerantTime֮ǰ�ظ��ܷ����
    vector<Spot*> waitCustomerSet;       
    vector<Spot*> dynamicCustomerSet;     // δ֪�Ĺ˿ͼ�
    // ��ǰ�ƻ�
    // �ڳ�ʼ����ʱ�򣬲�û�е�ǰ�ƻ�����ʱcurrentPlan������һ���ճ���������������ȡcapacity, depot��Ϣ
    vector<Car*> currentPlan;  
    vector<Spot*> storeSet;
};

#endif
