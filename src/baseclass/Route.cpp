#include "Route.h"
#include<iostream>
#include<cassert>
#include<vector>
#include<cmath>
#include<limits>
#include<cstdlib>
#include<algorithm>
#include<stdexcept>

const float MAX_FLOAT = numeric_limits<float>::max();

using namespace std;
Route::Route(Spot &headNode, Spot &rearNode, float capacity):capacity(capacity)
{ 
    // ���캯��
    head = new Spot(headNode);
    rear = new Spot(rearNode);
    head->front = NULL;
    head->next = rear;
    rear->front = head;
    rear->next = NULL;
    current = head;  // ��ʼ��currentָ��ָ��head�ڵ�
    size = 0;
    arrivedTime.push_back(head->arrivedTime);
    quantity = 0;
    leftQuantity = capacity;
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
    this->leftQuantity = L.leftQuantity;
    this->arrivedTime = L.arrivedTime;
    Spot* originPtr = L.head;
    Spot* copyPtr = head;
    Spot* temp = NULL;
    while(originPtr!=NULL){
        // ��ͷ�ڵ�һֱ���Ƶ�β�ڵ�
        if(originPtr == L.head){  // ���ڸ��Ƶ�һ���ڵ�
            copyPtr = new Spot();
            copyPtr->front = NULL;
            head = copyPtr;
            *copyPtr = *L.head;
        } else{
            temp = new Spot(*originPtr);
            temp->front = copyPtr;
            copyPtr->next = temp;
            copyPtr = temp;
		}
        if(L.current == originPtr){
            // currentָ��ĸ���
            current = copyPtr;
        }
		originPtr = originPtr->next;
    }
    temp->next = NULL;
    rear = temp;
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

void Route::clear(){  // ������������head�ڵ��rear�ڵ�?
    Spot* ptr1 = head;
    Spot* ptr2;
    while(ptr1!=NULL){
        ptr2 = ptr1->next;
        delete ptr1;
        ptr1 = ptr2;
    }
    head = NULL;
    rear = NULL;
    current = NULL;
    size = 0;
}

void Route::printRoute(){ // ��ӡ����
    Spot* ptr = head;
    for(; ptr!=NULL; ptr=ptr->next) {
        cout << "id:" << ptr->id << ' ' << "type:" << ' ' << ptr->type << endl;
    }
}


//=============== �����Լ�ɾ���ڵ���� ================//
void Route::insertAfter(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer){
    // ��������refStoreָ��ָ��Ľڵ�������storeָ��ָ��ڵ�
    // ��������refCustomerָ��ָ��Ľڵ�������customerָ��ڵ�
    Spot *tempstore = new Spot(store);
    Spot *tempCustomer = new Spot(customer)
    Spot *ptr = head;
    int count = 2;   // ��������ref�ڵ㶼�ҵ�
    while(ptr != rear){
        if (ptr == refStore){  
            count--;
        }
        if (ptr == refCustomer) {
            count--;
        }
        if(count == 0) {
            break;
        }
        ptr = ptr->next;
    }
    if(count > 0) {
        // û����ȫ�ҵ�������false
        throw out_of_range("Cannot find the position to insert!");
    } else{
        // ����quantity��ֵ�����Ҳ���store�Լ�customer
        quantity += customer->quantity;
        refStore->next->front = tempStore;
        tempStore->next = refStore->next;
        refStore->next = tempStore;
        tempstore->front = refStore;
        refCustomer->next->front = tempCustomer;
        tempCustomer->next = refCustomer->next;
        refCustomer->next = refCustomer;
        tempCustomer->front = refCustomer;
        size++;
        refreshArrivedTime();  // ����ڵ�󣬸���arrivedTime
    }
}

void Route::insertAtHead(Spot *store, Spot *customer){ 
    // �ڱ�ͷ����store��customer
    // ע��store����customerǰ�棨����pickup-delivery���⣬���������ã�
    // ֻ�е�currentָ��Ϊheadʱ����true
    if(current == head && size == 0) {
        // Ҫ��·������Ϊ�ղſ������ַ�ʽ����
        Spot *tempStore = new Spot(*store);
        Spot *tempCustomer = new Spot(*customer);
        head->next = tempStore;
        tempStore->next = tempCustomer;
        tempStore->front = head;
        rear->front = tempCustomer;
        tempCustomer->next = rear;
        quantity = quantity + tempCustomer->quantity;
        size++;
        refreshArrivedTime();  // ����ڵ�󣬸���arrivedTime
    }
    else{
        throw out_of_range("Cannot insert node after head!");
    }
}


void Route::deleteNode(Spot *store, Spot *customer){
    // ɾ��������ָ��ֵ��store��customer��ͬ�Ľڵ�
    // ֻ��ɾ��currentָ�����Ľڵ�
    if(current == rear) {
        // �Ѿ�������·���е����нڵ㣬��ֹɾ��
        throw out_of_range("Forbid deleting for we have finished the route!");
    }

    if (current == NULL) {
        throw out_of_range("The current node is NULL!");
    }

    Spot* temp1 = current->next;
    int count = 2;  // ��Ҫͬʱ�ҵ�store��customer�ſ�ɾ��
    while(temp1!=rear) {
        if(temp1 == store) {
            count--;
        }
        if(temp1 == customer) {
            count--;
        }
        if(count == 0) breakl
        temp1 = temp1->next;
	}
	if(count > 0) {  
        // û����ȫ�ҵ�
        throw out_of_range("We want to delete inexistent customer!");
	} else {
        store->front->next = store->next;
        store->next->front = store->front;
        customer->front->next = customer->next;
        customer->next->front = customer->front;
        delete store;
        delete customer;
        size--;
        quantity = quantity - customer.quantity;
        refreshArrivedTime();  // ɾ���ڵ�󣬸���arrivedTime
    }
}


//=============== ��õ��ڵ���� ================//
Spot* Route::currentPos(){ 
    // ���ص�ǰλ��
    return current;
}

Spot* Route::getHeadNode() {
    // ����ͷ���
    return head; 
}

Spot* Route::getRearNode() {
    // ����β�ڵ�
    return rear; 
}


//=============== ��ȡ�������� ================//
int Route::getSize() {
    return this->size;
}

vector<Spot*> Route::getAllCustomer(){  
    // �õ�·�������еĹ˿ͽڵ�
    // ���ص�customer����new�����ĶѶ�������ڴ�������ע��˴�
    vector<Spot*> customerSet(size);
    Spot* ptr = head->next;
    Spot* ptr2;
    for(int i=0; i<size; i++){
        ptr2 = new Spot();
        *ptr2 = *ptr;
        customerSet[i] = ptr2;
        ptr = ptr->next;
    }
    return customerSet;
}

float Route::getLen(float DTpara[], bool artificial){   // �õ�·������
    // ����ֵΪʵ�ʵ�·�����ȼ��ϳͷ�����
    // ��ȡDTpara
    float DTH1, DTH2, DTL1, DTL2;
    float *DTIter = DTpara;
    DTH1 = *(DTIter++);
    DTH2 = *(DTIter++);
    DTL1 = *(DTIter++);
    DTL2 = *(DTIter++);

    Spot *ptr1 = head;
    Spot *ptr2 = head->next;
    if(artificial == false) { // real vehicle routing scheme
        float len = 0;
        while(ptr2 != NULL){
            float temp1 = 0;
            switch(ptr1->priority){
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
            len = len + sqrt(pow(ptr1->x - ptr2->x, 2)+pow(ptr1->y - ptr2->y, 2));
            len += temp1;
            ptr2 = ptr2->next;
            ptr1 = ptr1->next;
        }
        return len;
    } else {
        float len = 0;
        while(ptr2 != NULL){
            float temp1 = 1.0f;
            switch(ptr1->priority){
                case 0: {
                    temp1 = 0.0f;
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
            len = len + sqrt(pow(ptr1->x - ptr2->x, 2)+pow(ptr1->y - ptr2->y, 2));
            len += temp1;
            ptr2 = ptr2->next;
            ptr1 = ptr1->next;
        }
        return len;		
    }
}

float Route::getOriginLen() {  
    // �õ�����̬�ڵ��·������
    // ע�⣬��property��ʶ�˿����ԣ���propertyΪ0ʱ��ʾ��̬��Ϊ1��ʾ��̬
    Spot* front = head;         // ��������ʼ�ڵ�
    Spot* back = front->next;   // ��һ���ڵ�
    float originLen = 0;
    while(back != NULL) {
        // ��β�ڵ㣬���ֿ⣬�ڴ˼��㷶Χ֮��
        if(back->prop != 0) {
            back = back->next;
        } 
        else {
            originLen += sqrt(pow(front->x - back->x, 2) + pow(front->y - back->y, 2));
            front = back;
            back = back->next;
        }
        //originLen += sqrt(pow(front->x - back->x, 2) + pow(front->y - back->y, 2));
        //front = back;
        //back = back->next;
    }
    return originLen;
}


vector<float> Route::getArrivedTime(){     // �õ��������нڵ��arrivedTime
    return arrivedTime;
}


//=============== �޸��������� ================//
bool Route::moveForward(){
    current = current->next;
    if(current == NULL) {  // �Ѿ��������
        return false;
    } else {
        return true;
    }
}


//=============== �������/ɾ���ڵ���� ================//
vector<float> Route::computeReducedCost(float DTpara[], bool artificial){ 
    // �õ����й˿ͽڵ���Ƴ�����
    // ֵԽС��ʾ�Ƴ������Խ�ʡ����Ĵ���
    // artificial: Ϊtrue��ʾ��һ����ٵĳ�
    // �����Ҫ�������Ƴ����ۣ��������DT��ȡΪ0����
    float DTH1, DTH2, DTL1, DTL2;
    float *DTIter = DTpara;
    DTH1 = *(DTIter++);
    DTH2 = *(DTIter++);
    DTL1 = *(DTIter++);
    DTL2 = *(DTIter++);
    vector<float> costArr(0);
    Spot *ptr1 = head;   // ǰ�ڵ�
    Spot *ptr2, *ptr3;
    for(int i=0; i<size; i++){
        ptr2 = ptr1->next;  // ��ǰ�ڵ�
        ptr3 = ptr2->next;  // ��ڵ�
        float temp =  -sqrt(pow(ptr1->x - ptr2->x, 2) + pow(ptr1->y - ptr2->y, 2)) - 
            sqrt(pow(ptr2->x - ptr3->x, 2) + pow(ptr2->y - ptr3->y, 2)) +
            sqrt(pow(ptr1->x - ptr3->x, 2) + pow(ptr1->y - ptr3->y, 2));
        float temp1 = 0;
        if(artificial == true) {
            switch(ptr1->priority){
                case 0: {
                    temp1 = 0;
                    break;
                }
                case 1: {
                    temp1 = -DTH2;
                    break;
                }
                case 2: {
                    temp1 = -DTL2;
                    break;
                }
            }
        } else {
            switch(ptr1->priority){
                case 0: {
                    temp1 = 0;
                    break;
                }
                case 1: {
                    temp1 = DTH1;
                    break;
                }
                case 2: {
                    temp1 = DTL1;
                    break;
                }
            }		
        }
        temp += temp1;
        costArr.push_back(temp);
        ptr1 = ptr1->next;
    }
    return costArr;
} 

bool Route::timeWindowJudge(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer){
    // �жϽ�store���뵽refStore���沢�ҽ�customer���뵽refCustomer�����Ƿ��Υ��ʱ�䴰Լ��
    // ע��refStore��refCustomer��������"store"����"customer"
    float time = arrivedTime[pos];
    Spot *ptr1, *ptr2;

    // ���������жϲ���item��᲻��Υ��item�������̽ڵ��ʱ�䴰Լ��
    if(pre->type == 'C' && time < pre->startTime){   
        // arrivedTime[pos]ֻ�ӵ���pre��arrived time��û���ж��Ƿ���ǰ����
        // ֻ����customer�ڵ��ʱ�䴰
        time = pre->startTime;
    }
    time += pre->serviceTime;
    time = time + sqrt(pow(pre->x - item.x, 2) + pow(pre->y - item.y, 2));
    if(time > item.endTime && item.type=="C") {  
        // Υ����ʱ�䴰Լ��
        // ֻ����customer�ڵ��ʱ�䴰
        return false;
    } else{
        if(time < item.startTimei && item.type=="C") {
            time = item.startTime;
        }
        time = time + item.serviceTime;
        ptr2 = pre->next;
        if(ptr2 == rear){  // item��������յ㣬��ʱ������
            return true;
        } else {
            time = time + sqrt(pow(ptr2->x - item.x, 2) + pow(ptr2->y - item.y, 2));
            if(time > ptr2->endTime) {
                return false;
            } else {
                if(time < ptr2->startTime) {
                    time = ptr2->startTime;
                }
                time = time + ptr2->serviceTime;
            }
        }
    }

    // Ȼ���жϻ᲻��Υ��������Ľڵ��ʱ�䴰Լ��
    bool mark = true;
    ptr1 = pre->next;
    ptr2 = ptr1->next;
    while(mark == true && ptr2 !=rear){ 
        time = time + sqrt(pow(ptr1->x - ptr2->x, 2) + pow(ptr1->y - ptr2->y, 2));	
        if(time > ptr2->endTime){
            mark = false;
            break;
        } else {
            if(time < ptr2->startTime){
                time = ptr2->startTime;
            }
            time = time + ptr2->serviceTime;
        }
        ptr1 = ptr1->next;
        ptr2 = ptr2->next;
    }
    return mark;
}

void Route::computeInsertCost(Spot item, float &minValue, Spot &customer1, 
        float &secondValue, Spot &customer2,
        float randomNoise, bool allowNegativeCost){
    // ����item�ڵ���·���е���С������ۺʹ�С�������
    // ���������/�μѲ����ǰ��Ĺ˿ͽڵ�
    // pertubation: �Ŷ���������
    // allowNegativeCost: Ϊtrue��ʾ�������ȡ�Ǹ�����Ϊfalse��ʾ��ȡ����
    // randomNoise: ���������
    Spot *pre = current;   // ֻ�ܲ��뵽δ�߹��Ľڵ�ǰ
    Spot *succ = pre->next;
    minValue = MAX_FLOAT;
    secondValue = MAX_FLOAT;
    customer1.id = -1;
    customer2.id = -1;
    int startPos = 0;
    Spot* temp = head;
    while(temp!= pre) {
        temp = temp->next;
        startPos++;
    }
    for(int i=startPos; i<=size; i++) {  // һ����size+1��λ�ÿ��Կ��ǲ���
        if(quantity + item.quantity <= capacity){   // ����Լ��
            if(timeWindowJudge(pre, i, item) == true) { // ����ʱ�䴰Լ��
                float temp = sqrt(pow(pre->x - item.x, 2) + pow(pre->y - item.y, 2)) +
                    sqrt(pow(item.x - succ->x, 2) + pow(item.y - succ->y, 2)) -
                    sqrt(pow(pre->x - succ->x, 2) + pow(pre->y - succ->y, 2));
                temp += randomNoise;
                if(allowNegativeCost == false) {
                    temp = max(0.0f, temp);
                }
                if(temp <= minValue){  // �ҵ��˸�С�ģ�����minValue��secondValue
                    secondValue = minValue;
                    customer2 = customer1;
                    minValue = temp;
                    customer1 = *pre;
                }
            }
        }
        pre = pre->next;
        if(succ != rear){
            succ = succ->next;
        }
    }
}

void Route::refreshArrivedTime(){   
    // ����һ�¸����ڵ�ĵ���ʱ��
    // ͷ����arrivedTime + serviceTime����Ϊ��׼ʱ��
    arrivedTime.clear();
    Spot* tfront = head;
    while(tfront != current->next){
        // ��ͷ��㵽current�ڵ�֮ǰ��arrivedTime������Ҫ���¼���
        arrivedTime.push_back(tfront->arrivedTime);
        tfront = tfront->next;
    }
    tfront = current;
    Spot* tcurrent = current->next;
    float time = current->arrivedTime + current->serviceTime;
    while(tcurrent != rear){
        // current�ڵ�����arrivedTime��Ҫ���¼���
        time = time + sqrt(pow(tfront->x - tcurrent->x, 2) + pow(tfront->y - tcurrent->y, 2));
        arrivedTime.push_back(time);
        // tcurrent->arrivedTime = time;
        if(tcurrent->type == "C" && time < tcurrent->startTime){
            // ֻ�й˿ͽڵ��С�ʱ�䴰��
            time = tcurrent->startTime;
        }
        time = time + tcurrent->serviceTime;
        tfront = tfront->next;
        tcurrent = tcurrent->next;
    }
}


//=============== ·�����滻����ȡ ================//
Route& Route::capture(){ 
    // ץȡcurrentָ����·��
    // currentָ�뵱ǰ�ڵ㽫��Ϊhead�ڵ�
    // ����ǰ·����capacity��leftQuantityԭ������
    Route* ptr1 = new Route(*current, *rear, capacity);
    if(current->next == rear) { // currentָ����Ѿ�û��·��
        return *ptr1;
    }
    Spot *ptr2 = current->next;
    Spot *ptr3 = NULL;
    Spot *ptr4 = NULL;
    ptr4 = ptr1->head;
    while(ptr2 != rear) {
        ptr3 = new Spot(*ptr2);
        ptr4->next = ptr3;
        ptr3->front = ptr4;
        ptr4 = ptr3;
        ptr1->quantity = ptr1->quantity + ptr2->quantity;
        ptr2 = ptr2->next;
        ptr1->size++;
    }
    ptr3->next = ptr1->rear;
    ptr1->rear->front = ptr3;
    ptr1->setLeftQuantity(leftQuantity);
    ptr1->refreshArrivedTime();   // ����part route��arrivedTime
    return *ptr1;
}

void Route::replaceRoute(const Route &route) {  // ��route�滻��currentָ����·��
    Spot* ptr1;
    Spot *ptr2, *ptr3;
    if(current->next != rear) { // current���滹�нڵ㣬��Ҫ�����ԭ��·��
        ptr2 = current->next;
        // ���ԭ·����currentָ������Ԫ��
        // ��������rear�ڵ�����
        while(ptr2 != rear) { 
            quantity = quantity - ptr2->quantity;
            ptr3 = ptr2->next;
            delete ptr2;
            ptr2 = ptr3;
            size--;
        }
    }
    // ��route�г�head��rear��Ľڵ㶼���Ƶ�currentָ���
    ptr1 = route.head->next;
    ptr3 = current;
    while(ptr1 != route.rear){  
        quantity = quantity + ptr1->quantity;
        ptr2 = new Spot();
        *ptr2 = *ptr1;
        ptr3->next = ptr2;
        ptr2->front = ptr3;
        ptr3 = ptr2;
        ptr1 = ptr1->next;
        size++;
    }
    ptr3->next = rear;
    rear->front = ptr3;
    refreshArrivedTime();   // ������·����refreshһ��arrivedTime
    return;
}

//=============== ���ศ�����ʵĺ��� ================//
bool Route::checkPassRoute(){
    // ������߹���·���Ƿ�Υ��ʱ�䴰Լ��
    // ����ֻ��鵽����һ���ڵ��ʱ���Ƿ�С��ǰһ���ڵ��ʱ�䴰��ʼʱ��
    Spot* ptr1 = head;
    Spot* ptr2 = head->next;
    bool mark = true;
    if(current == head) {  // ���ӻ�û�Ӳֿ������������
        return true;
    } else {
        while(ptr2 != current->next) {
            // һֱ��鵽current�ڵ�
            float leastArriveTime = ptr1->startTime;  // ������һ���ڵ�����ʱ��
            if(ptr2->arrivedTime < leastArriveTime) {
                mark = false;
            }
            ptr1 = ptr1 -> next;
            ptr2 = ptr2 -> next;
        }
        return mark;
    }
}

vector<int> Route::removeInvalidCustomer(vector<int> validCustomerId, int &retainNum){
    // ������id��validCustomerId�еĽڵ�
    // ���ر����Ľڵ���validCustomerId�е�λ��
    vector<int> posVec;
    posVec.push_back(0);   // �ֿ�ڵ�λ��
    Spot* ptr1 = head->next;
    while(ptr1 != rear) {
        int currentId = ptr1->id;
        vector<int>::iterator intIter = find(validCustomerId.begin(), 
                validCustomerId.end(), currentId);
        if(intIter == validCustomerId.end()) {
            // ����Ҳ�����˵���ýڵ���invalid��ɾ��֮
            quantity -= ptr1->quantity;
            size--;
            ptr1->front->next = ptr1->next;
            ptr1->next->front = ptr1->front; 
        } else {
            retainNum++;
            int pos = intIter - validCustomerId.begin();
            posVec.push_back(pos);
        } 
        ptr1 = ptr1->next;
    }
    posVec.push_back(0);  // �ֿ�ڵ�λ��
    return posVec;
}
