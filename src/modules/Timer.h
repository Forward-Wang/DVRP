#ifndef _TIMER_H
#define _TIMER_H

#include<vector>
#include "EventElement.h"
#include "../baseclass/Customer.h"
#include "../baseclass/Car.h"

class Timer{  // �¼�������
public:
    Timer(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, 
            float capacity, Customer depot);
    ~Timer(){};  // ��������
    EventElement pop();    // ���¼���ĵ�һ��Ԫ�ص�����
    void addEventElement(EventElement &newEvent);     // ���¼����������¼�
    void updateEventElement(EventElement &newEvent);  // �����¼�
    void deleteEventElement(int carIndex);            // ɾ��eventType���͵��¼�
    // �������Ľ�����������Ҫ��ʼ��Dispatcher
    void run(vector<Car*> &finishedPlan, vector<Customer*> &rejectCustomer, 
            float &travelDistance, float &addAveDistance);
private:
    vector<EventElement> eventList;        // �¼�����
    vector<Customer*> staticCustomerSet;   // ��ǰ��֪�Ĺ˿ͣ���̬�˿ͣ�
    vector<Customer*> dynamicCustomerSet;  // ��̬����Ĺ˿�
    float capacity;
    Customer depot;
    int sampleRate;
    int timeSlotLen;    // ʱ��εĳ���
    int timeSlotNum;    // ʱ��εĸ���
    float iter_percentage;
    int predictMethod;
};

#endif
