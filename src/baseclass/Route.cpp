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
    Spot *copyPtr, *temp;
    while(originPtr!=NULL){
        // ��ͷ�ڵ�һֱ���Ƶ�β�ڵ�
        if(originPtr == L.head){  
            // ���ڸ���head�ڵ�
            copyPtr = new Spot(L.head);
            copyPtr->front = NULL;
            head = copyPtr;
        } else{
            temp = new Spot(*originPtr);
            temp->front = copyPtr;
            copyPtr->next = temp;
            copyPtr = temp;
            if(originPtr->type == 'C') {
                // ׷����ָ����̻�(P)
                temp = L.head->next;
                int i = 0;
                while(temp != L.rear) {
                    if(temp == originPtr->choice) break;
                    temp = temp->next;
                    i++;
                }
                temp = head->next;
                for(int j=0; j<i; j++) {
                    temp = temp->next;
                }
                // ˫������
                copyPtr->choice = temp;
                temp->choice = copyPtr;
            }
		}
        if(L.current == originPtr){
            // currentָ��ĸ���
            current = copyPtr;
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
void Route::insertAfter(Spot *ref, Spot *current) {
    // ��ref�ڵ�������current�ڵ�
    Spot *ptr = head;
    bool mark = false;
    while(ptr!=rear) {
        if(ptr == ref) {
            mark = true;
            break;
        }
    }
    if(mark == false) {
        throw out_of_range("Cannot find the position to insert!");
        return;
    }
    ref->next->front = current;
    current->next = ref->next;
    current->front = ref;
    ref->next = current;
    if(current->type == 'C') {
        quantity += current->quantity;
    }
    size++;
    refreshArrivedTime()
}

void Route::insertAfter(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer){
    // ��������refStoreָ��ָ��Ľڵ�������storeָ��ָ��ڵ�
    // ��������refCustomerָ��ָ��Ľڵ�������customerָ��ڵ�
    assert(store->type == 'S' && customer->type == 'C');
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
        refStore->next->front = store;
        store->next = refStore->next;
        refStore->next = store;
        store->front = refStore;
        // ������Ҫ�������refStore��refCustomer��ͬһ���ڵ������
        if(refStore == refCustomer) {
            store->next->front = customer;
            customer->next = store->next;
            customer->front = store;
            store->next = customer;
        } else {
            refCustomer->next->front = customer;
            customer->next = refCustomer->next;
            refCustomer->next = customer;
            customer->front = refCustomer;
        }
        size++;
        refreshArrivedTime();  // ����ڵ�󣬸���arrivedTime
    }
}

void Route::insertAtHead(Spot *store, Spot *customer){ 
    // �ڱ�ͷ����store��customer
    // ע��store����customerǰ�棨����pickup-delivery���⣬���������ã�
    // ֻ�е�currentָ��Ϊheadʱ����true
    assert(store->type == 'S' && customer->type == 'C');
    if(current == head && size == 0) {
        // Ҫ��·������Ϊ�ղſ������ַ�ʽ����
        head->next = store;
        store->next = customer;
        store->front = head;
        rear->front = customer;
        customer->next = rear;
        quantity = quantity + customer->quantity;
        size++;
        refreshArrivedTime();  // ����ڵ�󣬸���arrivedTime
    }
    else{
        throw out_of_range("Cannot insert node after head!");
    }
}

void Route::insertAtRear(Spot *node) {
    // �ڱ�β����node��ע�����ﲻ������Ϸ���
    // ��Ҫ���û��Լ���֤�ڵ��ǿ��Դճ�(P-D)��
    rear->front->next = node;
    node->next = rear;
    if(node->type == 'C') {
        quantity = quantity + node->quantity;
        size++;
        refreshArrivedTime();  // ����ڵ�󣬸���arrivedTime
    }
}


void Route::deleteNode(Spot *node) {
    // ɾ��node�ڵ�
    bool mark = false;
    for(Spot* ptr = current; ptr != rear; ptr = ptr->next) {
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
    if(node->type == 'C') {
        quantity -= node->quantity;
        size--;
    }
    refreshArrivedTime();
    delete node;
}

void Route::deleteNode(Spot *store, Spot *customer){
    // ɾ��������ָ��ֵ��store��customer��ͬ�Ľڵ�
    // ֻ��ɾ��currentָ�����Ľڵ�
    assert(store->type == 'S' && customer->type == 'C');
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
        if(count == 0) break;
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
    // �õ�·�������еĹ˿ͽڵ�(D)
    // ���ص�customer����·���Ľڵ㣬���ⲿ����������
    // Returns:
    //   * customerSet: ·�������еĹ˿ͽڵ㣨��˳��
    vector<Spot*> customerSet(size);
    Spot* ptr = head->next;
    Spot* ptr2;
    for(Spot *ptr=head; ptr!=rear; ptr=ptr->next){
        if(ptr->type == 'C') {
            customerSet.push_back(ptr);
        }
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

    Spot *ptr1 = head;
    Spot *ptr2 = head->next;
    float len = 0;
    if(artificial == false) { // real vehicle routing scheme
        for(Spot *ptr = head->next; ptr != rear; ptr = ptr->next) {
            Spot *pre = ptr->front;
            Spot *next = ptr->next;
            float cost = dist(pre, ptr) + dist(ptr, next);
            if(ptr->type == 'C') {
                switch(ptr->priority){
                    case 0: {
                        cost += 0.0f;
                        break;
                    }
                    case 1: {
                        cost -= DTH1;
                        break;
                    }
                    case 2: {
                        cost -= DTL1;
                        break;
                    }
                }
            }
            len += cost;
        }
    } else {
        for(Spot *ptr = head->next; ptr != rear; ptr = ptr->next) {
            Spot *pre = ptr->front;
            Spot *next = ptr->next;
            float cost = dist(pre, ptr) + dist(ptr, next);
            if(ptr->type == 'C') {
                switch(ptr->priority){
                    case 0: {
                        cost += 0.0f;
                        break;
                    }
                    case 1: {
                        cost += DTH2;
                        break;
                    }
                    case 2: {
                        cost += DTL2;
                        break;
                    }
                }
            }
            len += cost;
        }
    }
    return len;
}

float Route::getOriginLen() {  
    // �õ�����̬�ڵ��·������
    // ��property��ʶ�˿����ԣ���propertyΪ0ʱ��ʾ��̬��Ϊ1��ʾ��̬
    Spot* front = head;         // ��������ʼ�ڵ�
    Spot* back = front->next;   // ��һ���ڵ�
    float originLen = 0;
    while(back != NULL) {
        // ֻ���㾲̬�ڵ���������
        if(back->prop != 0) {
            back = back->next;
        }
        else {
            origenLen = originLen + dist(front, back);
            front = back;
            back = back -> next;
        }
    }
    return originLen;
}


vector<float> Route::getArrivedTime(){     // �õ��������нڵ��arrivedTime
    return arrivedTime;
}

vector<int> Route::getAllID() {
    // ��ȡ·�������е�ID������P��D
    vector<int> IDs;
    for(Spot *temp = head->next; temp != rear; temp = temp->next) {
        IDs.push_back(temp->id);
    }
    return IDs;
}

//=============== �޸��������� ================//
bool Route::moveForward(){
    current->visit = true;
    current = current->next;
    if(current == NULL) {  // �Ѿ��������
        return false;
    } else {
        return true;
    }
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
    for(Spot* ptr = head; ptr != rear; ptr = ptr->next) {
        if(ptr->type == 'C') {
            // ��customerѰ���ҵ�store
            Spot *customer = ptr;
            Spot *customerPre = customer->front;
            Spot *customerNext = customer->next;
            Spot *store = ptr->choice;
            Spot *storePre = store->front;
            Spot *storeNext = store->next;
            float diff1 = -dist(customerPre, customer) - dist(customer, 
                    customerNext) + dist(customerPre, customerNext);
            float diff2 = -dist(storePre, store) - dist(store, storeNext) + 
                    dist(storePre, storeNext);
            float cost = diff1 + diff2;
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
                switch(ptr1->priority){
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
    }
    return costArr;
} 

bool Route::timeWindowJudge(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer){
    // �жϽ�store���뵽refStore���沢�ҽ�customer���뵽refCustomer�����Ƿ��Υ��ʱ�䴰Լ��
    // ע��refStore��refCustomer��������"store"����"customer"
    // ����refStore�ض���refCustomerǰ��
    assert(store->type == 'S' && customer->type == 'C');
    int pos = 0;
    for(Spot *temp=head; temp!=refStore; temp=temp->next) {
        // �ҵ�refStore��·���е�λ�ã�����ȡarrivedTime��
        pos++;
    }
    float time = arrivedTime[pos];
    Spot *ptr1, *ptr2;

    // ���������жϲ���store�Լ�customer���Υ��ʱ�䴰Լ��
    if(refStore->type == 'C' && time < refStore->startTime){   
        // arrivedTime[pos]ֻ�ӵ���refStore��arrived time��û���ж��Ƿ���ǰ����
        // ֻ����customer�ڵ��ʱ�䴰
        time = refStore->startTime;
    }
    // �ж��Ƿ�Υ��store�����ʱ�䴰Լ��
    // ע��store����û��ʱ�䴰Լ��
    time += store->serviceTime;
    Spot *pre, *current;
    // �п���refStore��refCustomer��ͬһ���ڵ㣬�Դ������⴦��
    if(refStore == refCustomer) {
        // store -> customer
        float travelLen = dist(store, customer);
        time += travelLen;
        if(time < customer->startTime) {
            time = customer->startTime;
        }
        if(time > customer->endTime) {
            return false;
        }
        time += customer->serviceTime;
        // customer -> store->next
        if(refStore->next == rear) {
            // �Ѿ���store, customer������·��β��
            return true;
        } else {
            float travelLen = dist(customer, refStore->next);
            time += travelLen;
            if(refStore->next->type == 'C') {
                if(time < refStore->next->startTime) {
                    time = refStore->next->startTime;
                }
                if(time > refStore->next->endTime) {
                    return false;
                }
            }
            time += refStore->next->serviceTime;
        }
        pre = refStore->next;
        current = pre->next;
    } else {
        pre = store;
        current = refStore->next;
    }
    bool mark = true; 
    while(true) {
        if(current == rear) break;
        if(pre == refCustomer) {
            // ǰһ�ڵ���refCustomer����ô��һ�ڵ�Ӧ����customer
            current = customer;
        }
        float travelLen = dist(pre, current);
        time += travelLen;
        if(current->type == 'C') {
            if(time > current->endTime) {
                mark = false;
                break;
            }
            if(time < current->startTime) {
                time = current->startTime;
            }
        }
        time += current->serviceTime;
        if(pre == refCustomer) {
            // pre��customer��ʱ���棬���ǲ������ؽ�customer����
            // ��ʱcurrentָ��ָ�����·����ʵ�ʴ��ڵĽڵ㣨customer֮��
            current = pre->next;
            pre = customer;
        } else {
            pre = current;
            current = current->next;
        }
    }
    return mark;
}

void Route::computeInsertCost(Spot *store, Spot* customer, float &minValue, 
        Spot *refStore1, Spot refCustomer1, float &secondValue, Spot *refStore2,
        Spot *refCustomer2, float randomNoise, bool allowNegativeCost){
    // ��������(store, customer)��·���е���С������ۺʹ�С�������
    // ���������/�μѲ����(refStore, refCustomer)
    // ���store == head����refStoreû�����壨������������ڵ������Ѿ������˿�
    // ��Cָ����store��·�����¹滮�����ʱ�������ڵأ�head�ڵ㣩���䵱store�Ľ�ɫ��
    // Args:
    //   * pertubation: �Ŷ���������
    //   * allowNegativeCost: Ϊtrue��ʾ�������ȡ�Ǹ�����Ϊfalse��ʾ��ȡ����
    //   * randomNoise: ���������
    // Returns:
    //   * refStore1, refCustomer1: ��minValue=MAX_FLOAT������߾�ΪNULL
    //   * refStore2, refCustomer2: ��secondVlue=MAX_FLOAT������߾�ΪNULL
    assert(store->type == 'S' && customer->type == 'C');
    refStore1 = NULL;
    refCustomer1 = NULL;
    refStore2 = NULL;
    refCustomer2 = NULL;
    minValue = MAX_FLOAT;
    secondValue = MAX_FLOAT;
    if(quantity + refCustomer->quantity > capacity) {
        // ����������Լ�������������
        return;
    }
    if(store == head) {
        for(Spot* refCustomer=current; refCustomer != rear; refCustomer = 
                refCustomer->next) {
            if(timeWindowJudge(head, refCustomer, store, customer) == true) {
                float diff = dist(refCustomer, customer) + dist(customer, 
                        refCustomer->next) - dist(refCustomer, refCustomer->next);
                float cost = diff + randomNoise;
                if(allowNegativeCost == false) {
                    cost = max(0.0f, cost);
                }
                if(cost <= minValue) {
                    // �ҵ����µ���С�ߣ����·���ֵ
                    secondValue = minValue;
                    refCustomer2 = refCustomer1;
                    refStore2 = NULL;
                    refCustomer1 = refCustomer;
                    refStore1 = NULL;
                    minValue = cost;
                }
            }
        }
    } else {
        for(Spot* refStore=current; refStore != rear; refStore=refStore->next) {
            for(Spot* refCustomer=refStore; refCustomer != rear; 
                    refCustomer = refCustomer->next) {
                if(timeWindowJudge(refStore, refCustomer, store, customer) 
                        == true) {
                    // ����ʱ�䴰Լ��
                    float diff1 = dist(refStore, store) + dist(store, 
                            refStore->next) - dist(refStore, refStore->next);
                    float diff2 = dist(refCustomer, customer) + dist(customer, 
                        refCustomer->next) - dist(refCustomer, refCustomer->next);
                    float cost = diff1 + diff2 + randomNoise;
                    if(allowNegativeCost == false) {
                        cost = max(0.0f, cost);
                    }
                    if(cost <= minValue) {
                        // �ҵ����µ���С�ߣ����·���ֵ
                        secondValue = minValue;
                        refCustomer2 = refCustomer1;
                        refStore2 = refStore1;
                        refCustomer1 = refCustomer;
                        refStore1 = refStore;
                        minValue = cost;
                    }
                }
            }
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
        time = time + dist(tfront, tcurrent);
        arrivedTime.push_back(time);
        tcurrent->arrivedTime = time;
        // tcurrent->arrivedTime = time;
        if(tcurrent->type == 'C' && time < tcurrent->startTime){
            // ֻ�й˿ͽڵ��С�ʱ�䴰��
            time = tcurrent->startTime;
        }
        time = time + tcurrent->serviceTime;
        tfront = tfront->next;
        tcurrent = tcurrent->next;
    }
}


//=============== ·�����滻����ȡ ================//
Route* Route::getEmptyRoute(vector<Spot*> &removedCustomer) {
    // ��ȡ�յ�·����������Ҫ����·����choiceΪdepot��customer
    // removedCustomer: currentָ��֮��Ĺ˿ͽڵ� - �����Ĺ˿ͽڵ�
    Route *newRoute = new Route(head, rear, leftQuantity);
    Spot *temp = current->next;
    while(temp != NULL) {
        if(temp->type == 'C') {
            if(temp->choice->type != 'D') {
                Spot *customer = new Spot(*temp);
                Spot *store = new Spot(*(customer->choice));
                customer->choice = store;
                store->choice = customer;
                removedCustomer.push_back(customer);
            }
            else {
                Spot *customer = newSpot(*temp);
                customer->choice = newRoute->head;
                newRoute->insertAtRear(customer);
            }
        }
        temp = temp->next;
    }
    return newRoute;
}

Route* Route::capture(){ 
    // ץȡcurrentָ����·��
    // currentָ�뵱ǰ�ڵ㽫��Ϊhead�ڵ�
    // ����ǰ·����capacity��leftQuantityԭ������
    // �����Ѿ����ʹ���Ӧpickup�ڵ��delivery�ڵ㣬��ѡ���store��ʱΪ
    // ץȡ·����head�ڵ�
    Route* newRoute = new Route(*current, *rear, capacity);
    if(current->next == rear) { // currentָ����Ѿ�û��·��
        return *newRoute;
    }
    for(Spot* ptr=current->next; ptr != rear; ptr = ptr->next) {
        Spot *temp = new Spot(*ptr);
        if(temp->type == 'C' && temp->choice->visit == true) {
            // customer��Ӧ��store�Ѿ������ʹ�
            temp->choice = newRoute->getHeadNode();
            newRoute->insertAtRear(temp);
        }
    }
    return newRoute;
}

void Route::replaceRoute(Route &route) {  
    // ��route�滻��currentָ����·��
    // ����route��choiceΪdepot��customer����Ҫ�һ���ԭ��ָ����̵�
    vector<Spot*> customerPool(NUM_OF_CUSTOMER);
    Spot *ptr1, *ptr2, *ptr3;
    // ��ձ�·����currentָ�����Ľڵ� 
    if(current->next != rear) { // current���滹�нڵ�
        // ���ԭ·����currentָ������Ԫ��
        // ��������rear�ڵ�����
        ptr1 = current->next;
        while(ptr1 != rear) {
            if(ptr1->type == 'C') {
                customerPool[ptr1->id] = ptr1->choice;
                ptr2 = ptr1->next;
                deleteNode(ptr1);
                ptr1 = ptr2;
            }
        }
    }
    // �޸�route��choiceΪdepot��customer��ѡ����̵�
    ptr1 = route.current->next;
    while(ptr1 != NULL) {
        if(ptr1->type == 'C' && ptr1->choice->type == 'D') {
            Spot *store = customerPool[ptr1->id];
            store->choice = ptr1;
            ptr1->choice = store;
        }
    }

    // ��route�г�head��rear��Ľڵ㶼���Ƶ�currentָ���
    ptr1 = route.head->next;
    while(ptr1 != route.rear) {
        try {
            insertAtRear(ptr1);
        } catch (exception &e) {
            cout << "While replace route: " << e.what() << endl;
        }
        ptr1 = ptr1->next; 
    }
    // ��ձ���
    customerPool.clear();
    route.clear();
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
    // ������id��validCustomerId�е�customer�ڵ��Ӧ�ķ����
    // ע��ͨ��customer->choice���Եõ��˿�ѡȡ���̵�
    // Returns: 
    //   * retainNum: route��ӵ�е�valid customer����
    //   * posVec: ���������Ĺ˿ͽڵ���validCustomerId�е�λ����ɵ�����
    vector<int> posVec;
    posVec.push_back(0);   // �ֿ�ڵ�λ��
    Spot* ptr1 = head->next;
    while(ptr1 != rear) {
        if(ptr1->type == 'C') {
            int currentId = ptr1->id;
            vector<int>::iterator intIter = find(validCustomerId.begin(), 
                    validCustomerId.end(), currentId);
            if(intIter == validCustomerId.end()) {
                // ����Ҳ�����˵���ýڵ���invalid��ɾ��֮
                quantity -= ptr1->quantity;
                size--;
                ptr1->front->next = ptr1->next;
                ptr1->next->front = ptr1->front;
                Spot *ptr2 = ptr1->choice;  // �ù˿�ѡ����̵꣬ҲҪһ��ɾ��
                ptr2->front->next = ptr2->next;
                ptr2->next->front = ptr2->front;
            } else {
                retainNum++;
                int pos = intIter - validCustomerId.begin();
                posVec.push_back(pos);
            } 
            ptr1 = ptr1->next;
        }
    }
    posVec.push_back(0);  // �ֿ�ڵ�λ��
    return posVec;
}
