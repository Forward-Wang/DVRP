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
Route::Route(Spot &headNode, Spot &rearNode, float capacity):capacity(capacity)
{ 
    // ���캯��
    head = new Spot(headNode);
    rear = new Spot(rearNode);
    stand = new Spot(headNode);
    stand->front = head;
    stand->next = rear;
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
            copyPtr = new Spot(*L.head);
            copyPtr->front = NULL;
            head = copyPtr;
        } else{
            temp = new Spot(*originPtr);
            temp->front = copyPtr;
            copyPtr->next = temp;
            copyPtr = temp;
            if(originPtr->type == 'C') {
                // ׷����ָ����̻�(D)
                temp = L.head;
                int i = 0;
                while(temp != L.rear) {
                    if(temp == originPtr->choice) break;
                    temp = temp->next;
                    i++;
                }
                temp = head;
                for(int j=0; j<i; j++) {
                    temp = temp->next;
                }
                if(temp->type == 'D') {
                    // �̵���depot����ֻ��Ҫ��������
                    copyPtr->choice = temp;
                } else {
                    // ˫������
                    copyPtr->choice = temp;
                    temp->choice = copyPtr;
                }
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
    // standָ����趨
    stand = new Spot(*L.stand);
    stand->front = current;
    stand->next = current->next;
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
    delete stand;
    stand = NULL;
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
    // ֻ�ܲ��뵽stand�ڵ����
    Spot *ptr = current;
    if(DEBUG) {
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
    }
    if(ref == stand->front) {
        // ˵��current�ڵ���뵽stand�ڵ����
        stand->next = current;
    }
    ref->next->front = current;
    current->next = ref->next;
    current->front = ref;
    ref->next = current;
    if(current->type == 'C') {
        quantity += current->quantity;
    }
    size++;
    try{
        checkArrivedTime();
    } catch (exception &e) {
        cout << "In insertAfter: " << e.what() << endl;
        exit(1);
    }
}

void Route::insertAfter(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer){
    // ��������refStoreָ��ָ��Ľڵ�������storeָ��ָ��ڵ�
    // ��������refCustomerָ��ָ��Ľڵ�������customerָ��ڵ�
    assert(store->type == 'S' && customer->type == 'C');
    if(DEBUG) {
        Spot *ptr = current;
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
            cout << "refStore: " << refStore->type << " refCustomer: " << refCustomer->type
                << endl;
            throw out_of_range("Cannot find the position to insert!");
            return;
        }
    }
    if(refStore == stand->front) {
        stand->next = store;
    } 
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
    try {
        checkArrivedTime();  // ����ڵ�󣬼��arrivedTime
    } catch (exception &e) {
        cout << "refStore: " << refStore->id << " refCustomer: " << refCustomer->id << endl;
        cout << "While inserting: store: " << store->id << " customer: " 
            << customer->id << endl;
        throw out_of_range("In insertAfter: " + string(e.what()));
    }
}

void Route::insertAtHead(Spot *store, Spot *customer){ 
    // �ڱ�ͷ����store��customer
    // ע��store����customerǰ�棨����pickup-delivery���⣬���������ã�
    // ֻ�е�currentָ��Ϊheadʱ����true
    assert(store->type == 'S' && customer->type == 'C');
    if(current == head && size == 0) {
        // Ҫ��·������Ϊ�ղſ������ַ�ʽ����
        if(timeWindowJudge(head, head, store, customer) == false) {
            throw out_of_range("Invalid inserting!");
        }
        head->next = store;
        store->next = customer;
        store->front = head;
        rear->front = customer;
        customer->next = rear;
        customer->front = store;
        stand->next = store;
        quantity = quantity + customer->quantity;
        size++;
        try {
            checkArrivedTime();  // ����ڵ�󣬸���arrivedTime
        } catch (exception &e) {
            cout << "store id: " << store->id << " customer id: "
                << customer->id << endl;
            cout << "In insertAtHead: " << e.what() << endl;
            exit(1);
        }
    }
    else{
        throw out_of_range("Cannot insert node after head!");
    }
}

void Route::insertAtRear(Spot *node) {
    // �ڱ�β����node��ע�����ﲻ������Ϸ���
    // ������ԣ�Ĭ��currentָ��λ��head
    // ��Ҫ���û��Լ���֤�ڵ��ǿ��Դճ�(P-D)��
    rear->front->next = node;
    node->next = rear;
    node->front = rear->front;
    rear->front = node;
    if(node->type == 'C') {
        quantity = quantity + node->quantity;
        size++;
        if(node->choice->type == 'D') {
            node->choice = head;
        }
    }
    stand->next = current->next;
    try{
        checkArrivedTime();  // ����ڵ�󣬼��arrivedTime
    } catch (exception &e) {
        throw out_of_range("In insert at rear: " + string(e.what()));
    }
}

void Route::deleteNode(Spot *node) {
    // ɾ��node�ڵ�
    if(DEBUG) {
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
    }
    if(stand->next == node) stand->next = node->next;
    node->front->next = node->next;
    node->next->front = node->front;
    if(node->type == 'C') {
        quantity -= node->quantity;
        size--;
    }
    try{
        checkArrivedTime();
    } catch (exception &e) {
        cout << "In deleteNode: " << e.what() << endl;
        exit(1);
    }
    delete node;
}

void Route::deleteNode(Spot *store, Spot *customer){
    // ɾ��������ָ��ֵ��store��customer��ͬ�Ľڵ�
    // ֻ��ɾ��currentָ�����Ľڵ�
    assert(store->type == 'S' || store->type == 'D');
    assert(customer->type == 'C');
    if(current == rear) {
        // �Ѿ�������·���е����нڵ㣬��ֹɾ��
        throw out_of_range("Forbid deleting for we have finished the route!");
    }

    if (current == NULL) {
        throw out_of_range("The current node is NULL!");
    }

    if(DEBUG) {
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
            return;
        }
    }
    if(stand->next == store) {
        if(store->next == customer) stand->next = customer->next;
        else stand->next = store->next;
    }
    store->front->next = store->next;
    store->next->front = store->front;
    customer->front->next = customer->next;
    customer->next->front = customer->front;
    delete store;
    delete customer;
    size--;
    quantity = quantity - customer->quantity;
    try{
        checkArrivedTime();  // ɾ���ڵ�󣬸���arrivedTime
    } catch (exception &e) {
        cout << "In deleteNode: " << e.what() << endl;
        exit(1);
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
    for(Spot *ptr=head; ptr!=rear; ptr=ptr->next){
        if(ptr->type == 'C') {
            customerSet.push_back(ptr);
        }
    }
    return customerSet;
}

vector<Spot*> Route::getAllSpot() {
    // �õ�head��rear(����������)�����нڵ�
    vector<Spot*> spotSet;
    for(Spot *ptr=head->next; ptr!=rear; ptr=ptr->next) {
        spotSet.push_back(ptr);
    }
    return spotSet;

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
    if(artificial == false) { // real vehicle routing scheme
        for(Spot *ptr = head->next; ptr->next != NULL; ptr = ptr->next) {
            Spot *pre = ptr->front;
            Spot *next = ptr->next;
            float cost = 0.0f;
            try{
                cost = dist(pre, ptr) + dist(ptr, next);
            } catch (exception &e) {
                cerr << "In getLen, Route.cpp: " << e.what() << endl;
                exit(1);
            }
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
        for(Spot *ptr = head->next; ptr->next != NULL; ptr = ptr->next) {
            Spot *pre = ptr->front;
            Spot *next = ptr->next;
            float cost = 0.0f;
            try {
                cost = dist(pre, ptr) + dist(ptr, next);
            } catch (exception &e) {
                cerr << "In getLen, Route.cpp: " << e.what() << endl;
                exit(1);
            }
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
            originLen = originLen + dist(front, back);
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
    current = current->next;
    stand->x = current->x;
    stand->y = current->y;
    stand->front = current;
    stand->next = current->next;
    current->visit = true;
    if(current == NULL) {  // �Ѿ��������
        return false;
    } else {
        return true;
    }
}

void Route::setStand(float x, float y, float arrivedTime, float serviceTime) {
    stand->x = x;
    stand->y = y;
    stand->arrivedTime = arrivedTime;
    stand->serviceTime = serviceTime;
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
            if(ptr->choice->type == 'D') {
                // ����choiceΪDepot�Ĺ˿ͽڵ㣬��removeCostΪ�����
                // ��ʾ����ڵ㲻���Ա�remove
                costArr.push_back(LARGE_FLOAT);
            } else {
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
        }
    }
    return costArr;
} 

bool Route::timeWindowJudge(Spot *refStore, Spot *refCustomer, Spot *store, Spot *customer){
    // �жϽ�store���뵽refStore���沢�ҽ�customer���뵽refCustomer�����Ƿ��Υ��ʱ�䴰Լ��
    // ע��refStore��refCustomer��������"store"����"customer"
    // ����refStore�ض���refCustomerǰ��
    assert(store->type == 'S' && customer->type == 'C');
    Spot *temp;    // temp�ڵ�Ϊstore����㣬������פ��
    float time = stand->arrivedTime; // פ�㵱ǰʱ��Ϊ��׼ʱ��
    time += stand->serviceTime;
    temp = stand;
    // ��refStore������time�����ж�current��refStore��ʱ�䴰Լ��
    // temp��Ϊ�����ϵ�refStore������λ��storeǰ��
    if(refStore != current) {
        temp = current->next;
        time += dist(stand, temp);
        if(temp->type == 'C' && time < temp->startTime) {
            time = temp->startTime;
        }
        time += temp->serviceTime;
        temp = temp->next;
        while(temp != refStore && temp != NULL) {
            // temp==NULL˵��refStore�����ڣ�һ�㲻�����
            time += dist(temp->front, temp);
            if(temp->type == 'C' && time < temp->startTime) {
                time = temp->startTime;
            }
            time += temp->serviceTime;
            temp = temp->next;
        }
        if(temp == NULL) return false;
        time += dist(temp->front, temp);
        if(refStore->type == 'C') {
            if(time > refStore->endTime) return false;
            if(time < refStore->startTime) time = refStore->startTime;
        }
        time += temp->serviceTime;
    }

    // ����timeΪ��refStore�ڵ�ĳ���ʱ��
    // ע��refStore�п��ܻ���stand
    time += dist(temp, store);
    time += store->serviceTime;
    // preָ��store����customer(��refStore�Ƿ�����ΪrefCustomer����)
    // curָ��refStore����һ���ڵ�
    Spot *pre, *cur; // �����ж�customer֮���ʱ�䴰Լ���Ƿ�����
    
    // �жϴ�store��customerΪֹ��ʱ�䴰Լ���Ƿ�����
    if(refStore == refCustomer) {
        // ����refStore==refCustomer������������⴦��
        time += dist(store, customer);
        if(time > customer->endTime) return false;
        if(time < customer->startTime) time = customer->startTime;
        time += customer->serviceTime;
        pre = customer;
        cur = temp->next;
    } else {
        pre = store;
        cur = temp->next;
    }
    
    if(time > OFF_WORK_TIME) return false;
    // �������ж�customer֮���ʱ�䴰Լ���Ƿ�����
    // ����refCustomer�Ѿ���������stand��ֻ����·���еĽڵ�
    while(true) {
        if(pre == refCustomer) {
            // ǰһ�ڵ���refCustomer����ô��һ�ڵ�Ӧ����customer
            cur = customer;
        }
        if(cur == rear) break;
        time += dist(pre, cur);
        if(cur->type == 'C') {
            if(time > cur->endTime) return false;
            if(time < cur->startTime) time = cur->startTime;
        }
        time += cur->serviceTime;
        if(time > OFF_WORK_TIME) return false;
        if(cur == customer) {
            // pre��customer��ʱ���棬���ǲ������ؽ�customer����
            // ��ʱcurrentָ��ָ�����·����ʵ�ʴ��ڵĽڵ㣨customer֮��
            cur = pre->next;
            pre = customer;
        } else {
            pre = cur;
            cur = cur->next;
        }
    }
    return true;
}

void Route::computeInsertCost(Spot *store, Spot* customer, float &minValue, 
        Spot *&refStore1, Spot *&refCustomer1, float &secondValue, Spot *&refStore2,
        Spot *&refCustomer2, float randomNoise, bool allowNegativeCost){
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
    refStore1 = NULL;
    refCustomer1 = NULL;
    refStore2 = NULL;
    refCustomer2 = NULL;
    minValue = MAX_FLOAT;
    secondValue = MAX_FLOAT;
    if(quantity + customer->quantity > capacity) {
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

void Route::checkArrivedTime(){   
    // ����һ�¸����ڵ�ĵ���ʱ��
    // ͷ����arrivedTime + serviceTime����Ϊ��׼ʱ��
    float time = stand->arrivedTime;
    time += stand->serviceTime;
    Spot *pre = stand;
    Spot *cur = stand->next;
    while(cur != rear) {
        time += dist(pre, cur);
        cur->arrivedTime = time;
        if(cur->type == 'C') {
            if(time > cur->endTime) {
                cout << "problem in: " << cur->id << endl;
                cout << "Now time is: " << time << " end time for him: " <<
                    cur->endTime << endl;
                Spot *temp = head->next;
                cout << "Now ids are: " << endl;
                for(temp; temp != rear; temp = temp->next) {
                    cout << temp->id << "\t";
                }
                cout << endl;
                throw out_of_range("Violating time constraints");
            }
            if(time < cur->startTime) time = cur->startTime;
        }
        time += cur->serviceTime;
        if(time > OFF_WORK_TIME) {
            cout << "Time is: " << time << endl;
            throw out_of_range("Exceeding the off work time!");
        }
        pre = pre->next;
        cur = cur->next;
    }
}


//=============== ·�����滻����ȡ ================//
Route* Route::getEmptyRoute(vector<Spot*> &removedCustomer) {
    // ��ȡ�յ�·����������Ҫ����·����choiceΪdepot��customer
    // removedCustomer: currentָ��֮��Ĺ˿ͽڵ� - �����Ĺ˿ͽڵ�
    Route *newRoute = new Route(*head, *rear, leftQuantity);
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
                Spot *customer = new Spot(*temp);
                customer->choice = newRoute->head;
                newRoute->insertAtRear(customer);
            }
        }
        temp = temp->next;
    }
    return newRoute;
}

vector<Spot*> Route::capture(){ 
    // Intro:
    //   * ץȡcurrentָ�������нڵ�
    //   * �����Ѿ����ʹ���Ӧpickup�ڵ��delivery�ڵ㣬��ѡ���store��ʱΪ
    //   * ץȡ·����head�ڵ�
    vector<Spot*> output;
    if(current->next == rear) { // currentָ����Ѿ�û��·��
        return output;
    }
    for(Spot* ptr=current->next; ptr != rear; ptr = ptr->next) {
        Spot *temp = new Spot(*ptr);
        if(temp->type == 'C') {
            if(temp->choice->visit == true) {
                // customer��Ӧ��store�Ѿ������ʹ�
                temp->choice = rear;
            } else {
                // ���ڽڵ��Ǹ��Ƶģ����choice��Ϣ��ʧ����Ҫ
                // ����ԭ�е�λ�ù�ϵȷ��choiceָ��
                int count = 0;
                Spot* temp2 = current->next;
                while(temp2->choice->id != ptr->id) {
                    temp2 = temp2->next;
                    count++;
                }
                output[count]->choice = temp;
                temp->choice = output[count];
            }
        }
        output.push_back(temp);
    }
    return output;
}

void Route::replaceRoute(Route *route) {  
    // ��route�滻��currentָ����·��
    // ����route��choiceΪdepot��customer����Ҫ�һ���ԭ��ָ����̵�
    vector<Spot*> customerPool(CUSTOMER_NUM);
    Spot *ptr1, *ptr2;
    // ��ձ�·����currentָ�����Ľڵ� 
    if(current->next != rear) { // current���滹�нڵ�
        // ���ԭ·����currentָ������Ԫ��
        // ��������rear�ڵ�����
        ptr1 = current->next;
        while(ptr1 != rear) {
            if(ptr1->type == 'C') {
                customerPool[ptr1->id] = ptr1->choice;
            }    
            ptr2 = ptr1->next;
            deleteNode(ptr1);
            ptr1 = ptr2;
        }
    }
    // �޸�route��choiceΪdepot��customer��ѡ����̵�
    ptr1 = route->head->next;
    while(ptr1 != NULL) {
        if(ptr1->type == 'C' && ptr1->choice->type == 'D') {
            Spot *store = customerPool[ptr1->id];
            store->choice = ptr1;
            ptr1->choice = store;
        }
        ptr1 = ptr1->next;
    }

    // ��route�г�head��rear��Ľڵ㶼���Ƶ�currentָ���
    ptr1 = route->head->next;
    while(ptr1 != route->rear) {
        ptr2 = ptr1->next;
        try {
            insertAtRear(ptr1);
        } catch (exception &e) {
            throw out_of_range("While replace route: " + string(e.what()));
        }
        ptr1 = ptr2; 
    }
    // ��ձ���
    customerPool.clear();
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

bool Route::checkTimeConstraint() {
    // ��鵱ǰ·���Ƿ�����ʱ�䴰Լ��
    if(size == 0) return true;
    Spot *pre = head;
    Spot *cur = head->next;
    float time = 0;
    while(cur != rear) {
        time += dist(pre, cur);
        if(cur->type == 'C') {
            if(time < cur->startTime) {
                time = cur->startTime;
            }
            if(time > cur->endTime) {
                return false;
            }
        }
        time += cur->serviceTime;
        pre = pre->next;
        cur = cur->next;
    }
    return true;
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
                delete ptr2;
                ptr2 = ptr1->next;
                delete ptr1;
                ptr1 = ptr2;
            } else {
                retainNum++;
                int pos = intIter - validCustomerId.begin();
                posVec.push_back(pos);
                ptr1 = ptr1->next;
            } 
        } else {
            ptr1 = ptr1->next;
        }
    }
    posVec.push_back(0);  // �ֿ�ڵ�λ��
    return posVec;
}
