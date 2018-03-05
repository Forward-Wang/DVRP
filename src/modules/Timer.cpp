#include "Timer.h"
#include "Dispatcher.h"
#include<cassert>
#include<algorithm>
#include "../run/Config.h"

bool ascendSortEvent(const EventElement &a, const EventElement &b){  // ��������
    return a.time < b.time;
}

EventElement Timer::pop(){  
    // ���¼��Ƴ�
    assert(eventList.size() > 0);
    vector<EventElement>::iterator iter = eventList.begin(); 
    vector<EventElement>::iterator iter2;
    EventElement newElement;
    if(iter->carIndex == -1 && eventList.size() > 1) {
        // carIndexΪ-1��ʾ"newTimeSlot"�¼�
        // �������������¼��Ƿ�Ҳ��ͬһ�¼�����������,
        // �������¼����и������ȼ��������¼����ȼ���ͬ��
        iter2 = iter + 1;
        if(iter2->time == iter->time) {
            newElement = *iter2;
            eventList.erase(iter2);
        } 
        else {
            newElement = *iter;
            eventList.erase(iter);
        }
    } 
    else {
        newElement = *iter;
        eventList.erase(iter);
    }
    return newElement;
}

Timer::Timer(vector<Spot*> staticCustomerSet, vector<Spot*> dynamicCustomerSet, 
        vector<Spot*> storeSet, float capacity, Spot depot) {   
    // ���캯�����������Ϊ���й˿ͣ��Լ���ʱ��ο�ʼֵ
    this->staticCustomerSet = staticCustomerSet;
    this->dynamicCustomerSet = dynamicCustomerSet;
    this->capacity = capacity;
    this->depot = depot;
    this->storeSet = storeSet;
    EventElement newEvent;
    int i;
    for(i=0; i<TIME_SLOT_NUM; i++) {  
        // ���ӡ�ʱ��ε���¼�
        newEvent = EventElement(i*TIME_SLOT_LEN, newTimeSlot, -1, -1);
        eventList.push_back(newEvent);
    }
    vector<Spot*>::iterator iter = dynamicCustomerSet.begin();
    for(iter; iter< dynamicCustomerSet.end(); iter++) {  
        // ���ӡ��¹˿͵����¼���
        newEvent = EventElement((*iter)->startTime, newCustomer, -1, (*iter)->id);
        eventList.push_back(newEvent);
    }
    sort(eventList.begin(), eventList.end(), ascendSortEvent);
}

void Timer::addEventElement(EventElement &newEvent){  
    // ���¼����������¼�
    eventList.push_back(newEvent);
    sort(eventList.begin(), eventList.end(), ascendSortEvent);
}

void Timer::deleteEventElement(int carIndex){     
    // ɾ����carIndex��ص��¼�
    // "carIndex=-1"��ʾ"newTimeSlot"�¼���������ɾ��
    if(carIndex == -1) return;
    vector<EventElement>::iterator iter = eventList.begin();
    for(iter; iter<eventList.end(); ){
        if((*iter).carIndex == carIndex) {
            EventElement *p = &(*iter);
            delete(p);
            iter = eventList.erase(iter);
            break;
        } else {
            iter++;
        }
    }
}

void searchCustomer(int customerId, vector<Spot*> customerSet, Spot* customer) {
    // ����customerId��customerSet��Ѱ�ҹ˿�
    bool mark = false;
    vector<Spot*>::iterator custIter;
    for(custIter = customerSet.begin(); custIter < customerSet.end(); custIter++) {
        if((*custIter)->id == customerId) {
            mark = true;
            customer = *custIter;
            break;
        }
    }
    if (mark == false) {    
        throw out_of_range("Cannot search the customer, it may be missed!");    
    }
}

void Timer::updateEventElement(EventElement &newEvent){  
    // �����¼�
    vector<EventElement>::iterator iter = eventList.begin();
    if(newEvent.time == -1) {
        // ������Ч�¼���������
        return;	
    } else {
        bool found = false;
        for(iter; iter<eventList.end(); iter++){
            if((*iter).carIndex == newEvent.carIndex) {
                // �жϸ����¼���λ�ã�������֮
                // ע�����ÿһ���������¼����н����һ���¼�
                found = true;
                (*iter).time = newEvent.time;
                (*iter).eventType = newEvent.eventType;
                (*iter).customerId = newEvent.customerId;
            }
        }
        if(found == false) {
            // �Ҳ�������Ϊ�����ڴ�֮ǰû�н�ʱ���¼��
            // ������������ڻ����ڲֿ�ȴ���һ��ʱ�����յ���������
            addEventElement(newEvent);
        }
    }
}

// enum EventType{newCustomer, carArrived, finishedService, carDepature, newTimeSlot, carOffWork};

void Timer::run(vector<Car*> &finishedPlan, vector<Spot*> &rejectCustomer, 
        float &travelDistance, float &addAveDistance) {
    // �������ĳ�ʼ��
    Dispatcher disp(staticCustomerSet, dynamicCustomerSet, storeSet, depot, capacity);
    int slotIndex = 0;  // ��0��ʱ���
    while(eventList.size() != 0) {
        EventElement currentEvent = pop();  // ��������¼�
        switch(currentEvent.eventType) {
            case newCustomer: {   // �¹˿͵���
                Spot* cust;
                try {
                    searchCustomer(currentEvent.customerId, dynamicCustomerSet, cust);
                }
                catch (exception &e) {
                    cerr << "In event \"newCustomer\": " << e.what() << endl;
                    exit(1);
                }
                EventElement newEvent = disp.handleNewCustomer(slotIndex, cust);
                updateEventElement(newEvent);
                break;
            }
            case carArrived: {
                EventElement newEvent = disp.handleCarArrived(currentEvent.time, 
                        currentEvent.carIndex);
                updateEventElement(newEvent);
                break;
            }
            case finishedService: {
                EventElement newEvent = disp.handleFinishedService(currentEvent.time, 
                        currentEvent.carIndex);
                updateEventElement(newEvent);
                break;
            }
            case newTimeSlot: {
                vector<EventElement> newEventList = disp.handleNewTimeSlot(slotIndex);
                vector<EventElement>::iterator eventIter;
                for(eventIter = newEventList.begin(); eventIter < newEventList.end(); eventIter++) {
                    // ��������¼��б���
                    updateEventElement(*eventIter);
                }
                slotIndex++;
                break;
            }
            case carOffWork: {
                // do nothing now
                break;
            }
        }
    }
    vector<Spot*> allCustomer;
    vector<Spot*>::iterator custIter;
    for(custIter = staticCustomerSet.begin(); custIter < staticCustomerSet.end(); custIter++) {
        allCustomer.push_back(*custIter);
    }
    for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
        allCustomer.push_back(*custIter);
    }
    sort(allCustomer.begin(), allCustomer.end());
    vector<int> rejectCustomerId = disp.getRejectCustomerId();
    vector<int>::iterator intIter;
    for(intIter = rejectCustomerId.begin(); intIter < rejectCustomerId.end(); intIter++) {
        // �˿ͽڵ�id��1��ʼ
        rejectCustomer.push_back(allCustomer[*intIter-1]);
    }
    finishedPlan = disp.getFinishedPlan();
    vector<Car*>::iterator carIter;
    travelDistance = 0;
    addAveDistance = 0;
    for(carIter = finishedPlan.begin(); carIter < finishedPlan.end(); carIter++) {
        travelDistance += (*carIter)->getTravelDistance();
        addAveDistance += (*carIter)->getAddDistance();
    }
    // Ϊ����̬�˿����������ӵ�ƽ��·��
    addAveDistance /= (dynamicCustomerSet.size() - rejectCustomer.size());
    disp.destroy();
}
