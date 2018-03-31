#include "Route.h"
#include<iostream>
#include<cassert>
#include<vector>
#include<cmath>
#include<limits>
#include<cstdlib>
#include<algorithm>
#include<stdexcept>
#include "../public/PublicFunction.h"
#include "../run/Config.h"

using namespace std;
Route::Route(Spot &headNode, Spot &rearNode, float capacity, float timeDuration):
    capacity(capacity), timeDuration(timeDuration) 
{ 
    // ���캯��
    head = new Spot(headNode);
    rear = new Spot(rearNode);
    head->front = NULL;
    head->next = rear;
    rear->front = head;
    rear->next = NULL;
    size = 0;
    quantity = 0;
}

Route::~Route(){ // ��������
    this->clear();
}

//=============== ����������� ================//
void Route::copy(const Route &L){
    // Ӧ����L��head�ڵ��rear�ڵ㶼����
    // ����֮�⣬ע��private�����е����ݳ�Ա��Ҫ���ƹ���
    this->size = L.size;
    this->capacity = L.capacity;
    this->quantity = L.quantity;
    this->timeDuration = L.timeDuration;
    Spot* originPtr = L.head;
    Spot *copyPtr, *temp;
    while(originPtr!=NULL){
        // ��ͷ�ڵ�һֱ���Ƶ�β�ڵ�
        if(originPtr == L.head){  
            // ���ڸ���head�ڵ�
            copyPtr = new Spot(*L.head);
            copyPtr->front = NULL;
            head = copyPtr;
        } else{
            temp = new Spot(*originPtr);
            temp->front = copyPtr;
            copyPtr->next = temp;
            copyPtr = temp;
    	}
		originPtr = originPtr->next;
    }
    copyPtr->next = NULL;
    rear = copyPtr;
}

Spot& Route::operator[] (int k){
    assert(k>=0 && k<size);
    Spot* temp = head->next;
    for(int i=0; i<k; i++){
        temp = temp->next;
    }
    return *temp;
}

const Spot& Route::operator[] (int k) const{
    assert(k>=0 && k<size);
    Spot* temp = head->next;
    for(int i=0; i<k; i++){
        temp = temp->next;
    }
    return *temp;
}

Route::Route(const Route &L){ // ���ƹ��캯��	
    this->copy(L);
}

Route& Route::operator= (const Route &L){ 
    // ����"="�����������������
    this->clear();  // ��յ�ǰ����	
    this->copy(L);
    return *this;
}

bool Route::isEmpty(){ //�ж������Ƿ�Ϊ��
    return (size==0);
}

void Route::clear(){  
    // �����������head�ڵ��rear�ڵ�
    Spot* ptr1 = head;
    Spot* ptr2;
    while(ptr1!=NULL){
        ptr2 = ptr1->next;
        delete ptr1;
        ptr1 = ptr2;
    }
    head = NULL;
    rear = NULL;
    size = 0;
}

void Route::printRoute(){ // ��ӡ����
    Spot* ptr = head;
    for(; ptr!=NULL; ptr=ptr->next) {
        cout << "id:" << ptr->id << ' ' << "type:" << ' ' << ptr->type << endl;
    }
}


//=============== �����Լ�ɾ���ڵ���� ================//
void Route::insertAfter(Spot *ref, Spot *current) {
    // ��ref�ڵ�������current�ڵ�
    Spot *ptr = head;
    bool mark = false;
    while(ptr!=rear) {
        if(ptr == ref) {
            mark = true;
            break;
        }
		ptr = ptr->next;
    }
    if(mark == false) {
        throw out_of_range("Cannot find the position to insert!");
        return;
    }
    ref->next->front = current;
    current->next = ref->next;
    current->front = ref;
    ref->next = current;
    quantity += current->quantity;
    size++;
    if(checkTimeConstraint() == false) {
        throw out_of_range("In insertAfter, time constraints violated");
    }
}

void Route::insertAtRear(Spot *node) {
    // �ڱ�β����node
    rear->front->next = node;
    node->next = rear;
    node->front = rear->front;
    rear->front = node;
    quantity = quantity + node->quantity;
    size++;
    if(checkTimeConstraint() == false) {
        throw out_of_range("In insertAtRear, time constraints violated");
    }
}

void Route::deleteNode(Spot *node) {
    // ɾ��node�ڵ�
    bool mark = false;
    for(Spot* ptr = head; ptr != rear; ptr = ptr->next) {
        if(ptr == node) {
            mark = true;
            break;
        }
    }
    if(mark == false) {
        throw out_of_range("Cannot find the node to delete!");
    }
    node->front->next = node->next;
    node->next->front = node->front;
    quantity -= node->quantity;
    size--;
    if(checkTimeConstraint() == false) {
        throw out_of_range("In deleteNode, time constraints violated");
    }
    delete node;
}

//=============== ��õ��ڵ���� ================//
Spot* Route::getHeadNode() {
    // ����ͷ���
    return head; 
}

Spot* Route::getRearNode() {
    // ����β�ڵ�
    return rear; 
}

Spot* Route::findCustomer(int id) {
    for(Spot *node=head->next; node!=rear; node=node->next) {
        if(node->id == id) {
            return node;
        }
    }
}

//=============== ��ȡ�������� ================//
int Route::getSize() {
    return this->size;
}

vector<Spot*> Route::getAllCustomer(){  
    // �õ�·�������еĹ˿ͽڵ�(D)
    // ���ص�customer����·���Ľڵ㣬���ⲿ����������
    // Returns:
    //   * customerSet: ·�������еĹ˿ͽڵ㣨��˳��
    vector<Spot*> customerSet;
    for(Spot *ptr=head->next; ptr!=rear; ptr=ptr->next){
            customerSet.push_back(ptr);
    }
    return customerSet;
}

float Route::getLen(float DTpara[], bool artificial){   
    // �õ�·�����ȣ�������Ҫ��ӳͷ���
    // Args:
    //   * DTpara: �Բ�ͬ���͵ĳ�/�˿���ϵĳͷ�����
    //   * artificial: �������ԣ�Ϊtrue��ʾvirtual car
    // Returns:
    //   * len: ·������
    float DTH1, DTH2, DTL1, DTL2;
    float *DTIter = DTpara;
    DTH1 = *(DTIter++);
    DTH2 = *(DTIter++);
    DTL1 = *(DTIter++);
    DTL2 = *(DTIter++);

    float len = 0;
    if(size == 0) {
        // �޹˿ͽڵ�
        return len;
    }
    Spot *pre = head;
    Spot *cur = head->next;
    if(artificial == false) { // real vehicle routing scheme
        while(cur != NULL) {
            float temp1 = 0;
            switch(pre->priority) {
                case 0: {
                    temp1 = 0.0f;
                    break;
                }
                case 1: {
                    temp1 = -DTH1;
                    break;
                }
                case 2: {
                    temp1 = -DTL1;
                    break;
                }
            }
            len += dist(pre, cur);
            len += temp1;
            pre = pre->next;
            cur = cur->next;
        }
        return len;
    } else {
        while(cur != NULL) {
            float temp1 = 0;
            switch(pre->priority) {
                case 0: {
                    temp1 = 0;
                    break;
                }
                case 1: {
                    temp1 = DTH2;
                    break;
                }
                case 2: {
                    temp1 = DTL2;
                    break;
                }       
            }
            len += dist(pre, cur);
            len += temp1;
            pre = pre->next;
            cur = cur->next;
        }
        return len;
    }
}

float Route::getTrueLen(){   
    // �õ���ʵ��·�����ȣ�����ӳͷ���
    // Args:
    //   * DTpara: �Բ�ͬ���͵ĳ�/�˿���ϵĳͷ�����
    //   * artificial: �������ԣ�Ϊtrue��ʾvirtual car
    // Returns:
    //   * len: ·������

    float len = 0;
    if(size == 0) {
        // �޹˿ͽڵ�
        return len;
    }
    Spot *pre = head;
    Spot *cur = head->next;
    while(cur != NULL) {
        len += dist(pre, cur);
        pre = pre->next;
        cur = cur->next;
    }
    return len;

}

float Route::getTimeDuration() {
    // time duration: travel len + serviceTime
    float timeDuration = 0;
    if(size == 0) {
        return 0;
    }
    Spot *pre = head;
    Spot *cur = head->next;
    while(cur!=NULL) {
        timeDuration += dist(pre, cur);
        timeDuration += cur->serviceTime;
        pre = pre->next;
        cur = cur->next;
    }
    return timeDuration;
}

vector<int> Route::getAllID() {
    // �������id������head��rear
    Spot *ptr;
    vector<int> IDs;
    for(ptr=head; ptr!=NULL; ptr=ptr->next) {
        IDs.push_back(ptr->id);
    }
    return IDs;
}

vector<float> Route::getArrivedTime() {
    Spot *ptr;
    vector<float> arrivedTimes;
    for(ptr=head->next; ptr!=rear; ptr=ptr->next) {
        arrivedTimes.push_back(ptr->arrivedTime);
    }
    return arrivedTimes;
}

//=============== �������/ɾ���ڵ���� ================//
vector<float> Route::computeReducedCost(float DTpara[], bool artificial){ 
    // �õ����з����(P-D)���Ƴ����ۣ�ֵԽС��ʾ�Ƴ������Խ�ʡ����Ĵ���
    // Args:
    //   * artificial: Ϊtrue��ʾ��һ����ٵĳ�
    //   * DTpara[]: �Բ�ͬ����Ĺ˿�/�����ĳͷ�ϵ���������Ҫ�õ�
    //               ������reduce cost����ȫ������Ϊ0����
    // Returns:
    //   * costArr: ���з���Ե��Ƴ����ۣ�����·����customer��˳��
    float DTH1, DTH2, DTL1, DTL2;
    float *DTIter = DTpara;
    DTH1 = *(DTIter++);
    DTH2 = *(DTIter++);
    DTL1 = *(DTIter++);
    DTL2 = *(DTIter++);
    vector<float> costArr(0);
    for(Spot* ptr = head->next; ptr != rear; ptr = ptr->next) {
        float cost = -dist(ptr->front, ptr) - dist(ptr, ptr->next)
                     + dist(ptr->front, ptr->next);
        if(artificial == true) {
            switch(ptr->priority){
                case 0: {
                    cost += 0;
                    break;
                }
                case 1: {
                    cost -= DTH2;
                    break;
                }
                case 2: {
                    cost -= DTL2;
                    break;
                }
            }
        } else {
            switch(ptr->priority){
                case 0: {
                    cost += 0;
                    break;
                }
                case 1: {
                    cost += DTH1;
                    break;
                }
                case 2: {
                    cost += DTL1;
                    break;
                }
            }		
        }
        costArr.push_back(cost);
    }
    return costArr;
} 

bool Route::timeWindowJudge(Spot *ref, Spot *cur){
    // �жϽ�cur���뵽ref�����Ƿ��Υ��ʱ�䴰Լ��
    // ͬʱ����timeDuration
    Spot *temp = head;  
    float time = 0;
    float td = 0;
    // ���㵽��ref��ʱ��
    while(temp != ref) {
        if(temp->type == 'D') {
            time = temp->arrivedTime;
            time += temp->serviceTime;
            td += temp->serviceTime;
        } else {
			if(time > temp->endTime) return false;
            if(time < temp->startTime) time = temp->startTime;
            time += temp->serviceTime;
            td += temp->serviceTime;
        }
        time += dist(temp, temp->next);
        td += dist(temp, temp->next);
        if(td > timeDuration) return false;
        temp = temp->next;
    }
    // �����Ƿ�Υ��cur��ʱ�䴰Լ��
    if(time < ref->startTime) time = ref->startTime;
    time += ref->serviceTime;
    td += ref->serviceTime;
    time += dist(ref, cur);
    td += dist(ref, cur);
    if(time > cur->endTime) return false;
    if(time < cur->startTime) time = cur->startTime;
    time += cur->serviceTime;
    td += cur->serviceTime;
    if(td > timeDuration) return false;
    // ��ǰʱ��Ϊ��cur������ʱ�䣬�ж��Ƿ�Υ��ԭ·����ref�Ժ��ʱ�䴰Լ��
    temp = ref->next;
    if(temp != rear) {
        time += dist(cur, temp);
        td += dist(cur, temp);
        if(time > temp->endTime) return false;
        if(time < temp->startTime) time = temp->startTime;
        time += temp->serviceTime;
        td += temp->serviceTime;
        if(td > timeDuration) return false;
        temp = temp->next;
        while(temp != rear) {
            time += dist(temp->front, temp);
            td += dist(temp->front, temp);
            if(time > temp->endTime) return false;
            if(time < temp->startTime) time = temp->startTime;
            time += temp->serviceTime;
            td += temp->serviceTime;
            if(td > timeDuration) return false;
            temp = temp->next;
        }
        td += dist(rear->front, rear);
        if(td > timeDuration) return false;
    } else {
        td += dist(cur, rear);
        if(td > timeDuration) return false;
    }
    return true;
}

void Route::computeInsertCost(Spot* cur, float &minValue, Spot *&ref1, 
        float &secondValue, Spot *&ref2, float randomNoise, bool allowNegativeCost){
    // ����cur��·���е���С������ۺʹ�С�������
    // ���������/�μѲ����ref1/ref2
    // Args:
    //   * allowNegativeCost: Ϊtrue��ʾ�������ȡ�Ǹ�����Ϊfalse��ʾ��ȡ����
    //   * randomNoise: ���������
    // Returns:
    //   * ref1: ��С������۶�Ӧλ�ã���minValue=MAX_FLOAT����ΪNULL
    //   * ref2: ��С������۶�Ӧλ��, ��secondVlue=MAX_FLOAT����ΪNULL
    ref1 = NULL;
    ref2 = NULL;
    minValue = MAX_FLOAT;
    secondValue = MAX_FLOAT;
    if(quantity + cur->quantity > capacity) {
        // ����������Լ�������������
        return;
    }
    for(Spot* ref=head; ref!=rear; ref=ref->next) {
        if(timeWindowJudge(ref, cur) == true) {
            float diff = dist(ref, cur) + dist(cur, ref->next) -
                    dist(ref, ref->next);
            float cost = diff + randomNoise;
            if(allowNegativeCost == false) {
                cost = max(0.0f, cost);
            }
            if(cost <= minValue) {
                // �ҵ����µ���С�ߣ����·���ֵ
                secondValue = minValue;
                ref2 = ref1;
                ref1 = ref;
                minValue = cost;
            }
        }
    }
}

//=============== ���ศ�����ʵĺ��� ================//
bool Route::checkTimeConstraint() {
    // ��鵱ǰ·���Ƿ�����ʱ�䴰Լ��
    if(size == 0) return true;
    Spot *pre = head;
    Spot *cur = head->next;
    float time = 0;
    float td = 0;
    while(cur != rear) {
        time += dist(pre, cur);
        td += dist(pre, cur);
        cur->arrivedTime = time;  // ����arrivedTime
        if(time > cur->endTime) return false;
        if(time < cur->startTime) time = cur->startTime;
        time += cur->serviceTime;
        td += cur->serviceTime;
        if(td > timeDuration) return false;
        pre = pre->next;
        cur = cur->next;
    }
    td += dist(rear, rear->front);
    if(td > timeDuration) return false;
    else return true;
}
