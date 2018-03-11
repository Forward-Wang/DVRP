#ifndef _DISPATCHER_H
#define _DISPATHCER_H

#include "EventElement.h"
#include "../baseclass/Car.h"
#include<vector>

class Dispatcher{   // ����������
public:
    Dispatcher(vector<Spot*> staticCustomerSet, vector<Spot*> dynamicCustomerSet, 
            vector<Spot*> storeSet, Spot depot, float capacity);
    ~Dispatcher(){}; // ��������
    void destroy();  // ����Dispatcher
    EventElement handleNewCustomer(int slotIndex, Spot* newCustomer);    // �����¹˿�(dynamic)����
    EventElement handleCarArrived(float time, int carIndex);            // ������������¼�
    EventElement handleFinishedService(float time, int carIndex);       // ���������ɷ����¼�
    // EventElement handleDepature(float time, int carIndex);              // ������������¼�
    vector<EventElement> handleNewTimeSlot(int slotIndex); // ��ʱ��ο�ʼ 
    void carFinishTask(int carIndex);       // �ճ� 
    vector<int> getRejectCustomerId() {return rejectCustomerId; }
    vector<Car*> getFinishedPlan() {return finishedPlan; }
private:
    vector<Spot*> allCustomer;    // ���еĹ˿�
    vector<Spot*> storeSet;
    vector<int> dynamicCustomerId;    // ��̬����Ĺ˿͵�id
    vector<int> servedCustomerId;     // �Ѿ�������Ĺ˿�id
    vector<int> promisedCustomerId;   // (δ�������)�Ѿ��õ�'OK' promise�Ĺ˿�id
    vector<int> waitCustomerId;       // (δ�����������֪��)���ڵȴ�'OK' promise�Ĺ˿�id
    vector<int> rejectCustomerId;     // (δ�����������֪��)�õ���'NO' promise�Ĺ˿�id
    vector<Car*> currentPlan;         // ��ǰ�ƻ�
    vector<Car*> finishedPlan;        // ����ɼƻ�    
    Spot depot;
    float capacity;
    int timeSlotLen;   // ʱ��εĳ���
    int timeSlotNum;   // ʱ��εĸ���
    int globalCarIndex;
    int samplingRate;  // ������
    float iter_percentage;
    int predictMethod;
};

#endif
