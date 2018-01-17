#ifndef _TIMER_H
#define _TIMER_H

#include<vector>
#include "EventElement.h"
#include "../baseclass/Spot.h"
#include "../baseclass/Car.h"

class Timer{  // �¼�������
public:
    Timer(vector<Spot*> staticCustomerSet, vector<Spot*> dynamicCustomerSet, 
            float capacity, Spot depot);
    ~Timer(){};  // ��������
    EventElement pop();    // ���¼���ĵ�һ��Ԫ�ص�����
    void addEventElement(EventElement &newEvent);     // ���¼����������¼�
    void updateEventElement(EventElement &newEvent);  // �����¼�
    void deleteEventElement(int carIndex);            // ɾ��eventType���͵��¼�
    // �������Ľ�����������Ҫ��ʼ��Dispatcher
    void run(vector<Car*> &finishedPlan, vector<Spot*> &rejectCustomer, 
            float &travelDistance, float &addAveDistance);
private:
    vector<EventElement> eventList;        // �¼�����
    vector<Spot*> staticCustomerSet;   // ��ǰ��֪�Ĺ˿ͣ���̬�˿ͣ�
    vector<Spot*> dynamicCustomerSet;  // ��̬����Ĺ˿�
    float capacity;
    Spot depot;
    int sampleRate;
    int timeSlotLen;    // ʱ��εĳ���
    int timeSlotNum;    // ʱ��εĸ���
    float iter_percentage;
    int predictMethod;
};

#endif
