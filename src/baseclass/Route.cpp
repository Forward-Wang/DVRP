#include "Route.h"
#include "Customer.h"
#include "../public/PublicFunction.h"
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
Route::Route(Customer &headNode, Customer &rearNode, float capacity):capacity(capacity)
{ 
    // ���캯��
    head = new Customer;
    *head = headNode;  // ���ƽڵ�
    head->type = 'D';
    rear = new Customer;
    *rear = rearNode;
    rear->type = 'D';
    stand = new Customer;
    *stand = headNode;
    stand->type = 'D';
    stand->front = head;
    stand->next = rear;
    head->front = NULL;
    head->next = rear;
    rear->front = head;
    rear->next = NULL;
    current = head;  // ��ʼ��currentָ��ָ��head�ڵ�
    size = 0;
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
    Customer* originPtr = L.head;
    Customer* copyPtr = head;
    Customer* temp = NULL;
    while(originPtr!=NULL){
        // ��ͷ�ڵ�һֱ���Ƶ�β�ڵ�
        if(originPtr == L.head){  // ���ڸ��Ƶ�һ���ڵ�
            copyPtr = new Customer;
            copyPtr->front = NULL;
            head = copyPtr;
            *copyPtr = *L.head;
        } else{
            temp = new Customer;
            *temp = *originPtr;
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
    // standָ����趨
    stand = new Customer;
    *stand = *L.stand;
    stand->front = current;
    stand->next = current->next;
}

Customer& Route::operator[] (int k){
    assert(k>=0 && k<size);
    Customer* temp = head->next;
    for(int i=0; i<k; i++){
        temp = temp->next;
    }
    return *temp;
}

const Customer& Route::operator[] (int k) const{
    assert(k>=0 && k<size);
    Customer* temp = head->next;
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
    Customer* ptr1 = head;
    Customer* ptr2;
    while(ptr1!=NULL){
        ptr2 = ptr1->next;
        delete ptr1;
        ptr1 = ptr2;
    }
    head = NULL;
    rear = NULL;
    current = NULL;
    delete stand;
    stand = NULL;
    size = 0;
}

void Route::printRoute(){ // ��ӡ����
    Customer* ptr = head;
    for(; ptr!=NULL; ptr=ptr->next) {
        cout << "id:" << ptr->id << ' ' << "type:" << ' ' << ptr->type << endl;
    }
}


//=============== �����Լ�ɾ���ڵ���� ================//
void Route::insertAfter(Customer item1, Customer item2){
    // ����������item1��ͬ�Ľڵ�������ڵ�item2
    Customer* temp = new Customer;
    *temp = item2;
    Customer* ptr = head;
    while(ptr!=rear){
        if (ptr->id == item1.id){  // ����id���ж������ڵ��Ƿ���ͬ
            break;
        }
        ptr = ptr->next;
    }
    if(ptr == rear) {
        // û���ҵ�������false
        delete temp;
        throw out_of_range("Cannot find the position to insert!");
    } else{
        if(ptr->id == stand->front->id) {
            // ˵��item2�ڵ���뵽stand�ڵ����
            stand->next = temp;
        }
        quantity = quantity + item2.quantity;
        temp->next = ptr->next;
        ptr->next->front = temp;
        temp->front = ptr;
        ptr->next = temp;
        size++;
        try{
            checkArrivedTime();
        } catch (exception &e) {
            throw out_of_range("In insertAfter: " + string(e.what()));
        }
    }
}

void Route::insertAtHead(Customer item){ 
    // �ڱ�ͷ����item
    // ֻ�е�currentָ��Ϊheadʱ����true
    if(current == head) {
        Customer *temp = new Customer;
        *temp = item;
        temp->next = head->next;
        head->next->front = temp;
        head->next = temp;
        temp->front = head;
        stand->next = temp;
        quantity = quantity + item.quantity;
        size++;
        try {
            checkArrivedTime();
        } catch(exception &e) {
            throw out_of_range("In insertAtHead: " + string(e.what()));
        }
    }
    else{
        throw out_of_range("The car has departured, cannot insert node after head!");
    }
}

void Route::insertAtRear(Customer item){   
    // �ڱ�β����item
    // ֻ�е���β����current�ڵ�ʱ����true
    if(current != rear) {
        Customer *temp = new Customer;
        *temp = item;
        temp->next = rear;
	    temp->front = rear->front;
	    rear->front->next = temp;
	    rear->front = temp;
        stand->next = current->next;
	    quantity = quantity + item.quantity;
	    size++;
        try {
            checkArrivedTime();
        } catch (exception &e) {
            throw out_of_range("In insert at rear: " + string(e.what()));
        }
	} else {
        throw out_of_range("Has reached the end node, cannot insert any nodes!");
    }
}

void Route::deleteNode(Customer item){
    // ɾ����������item��ͬ�Ľڵ�
    // ֻ��ɾ��currentָ�����Ľڵ�
    if(current == rear) {
        // �Ѿ�������·���е����нڵ㣬��ֹɾ��
        throw out_of_range("Forbid deleting for we have finished the route!");
    }
    Customer* temp1 = current->next;

    if (current == NULL) {
        throw out_of_range("The current node is NULL!");
    }
    if (temp1 == NULL) {
        throw out_of_range("We have reached the rear!");
    }

    while(temp1!=rear) {
        if(temp1->id == item.id) {
            break;
        }
        temp1 = temp1->next;
	}
	if(temp1 == rear) {  // û���ҵ�
        throw out_of_range("We want to delete inexistent customer!");
	} else {
        if(stand->next->id == temp1->id) stand->next = temp1->next;
        Customer* nextNode = temp1->next;
        Customer* frontNode = temp1->front;
        frontNode->next = nextNode;
        nextNode->front = frontNode;
        delete temp1;
        size--;
        quantity = quantity - item.quantity;
        try{
            checkArrivedTime();
        } catch (exception &e) {
            throw out_of_range("In deleteNode: " + string(e.what()));
        }
    }
}


//=============== ��õ��ڵ���� ================//
Customer& Route::currentPos(){ // ���ص�ǰλ��
    return *current;
}

Customer& Route::getHeadNode() {
    Customer* newCust = new Customer(*head);
    return *newCust; 
}

Customer& Route::getRearNode() {
    Customer* newCust = new Customer(*rear);
    return *newCust; 
}


//=============== ��ȡ�������� ================//
int Route::getSize() {
    return this->size;
}

vector<Customer*> Route::getAllCustomer(){  // �õ�·�������еĹ˿ͽڵ�
    // ���ص�customer����new�����ĶѶ�������ڴ�������ע��˴�
    vector<Customer*> customerSet(size);
    Customer* ptr = head->next;
    Customer* ptr2;
    for(int i=0; i<size; i++){
        ptr2 = new Customer;
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

    Customer *ptr1 = head;
    Customer *ptr2 = head->next;
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
            len = len + dist(ptr1, ptr2);
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
            len = len + dist(ptr1, ptr2);
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
    Customer* front = head;         // ��������ʼ�ڵ�
    Customer* back = front->next;   // ��һ���ڵ�
    float originLen = 0;
    while(back != NULL) {
        // ��β�ڵ㣬���ֿ⣬�ڴ˼��㷶Χ֮��
        if(back->prop != 0) {
            back = back->next;
        } 
        else {
            originLen += dist(front, back);
            front = back;
            back = back->next;
        }
    }
    return originLen;
}


vector<float> Route::getArrivedTime(){     // �õ��������нڵ��arrivedTime
    Customer *temp = head->next;
    vector<float> arrivedTimes;
    while(temp != rear) {
        arrivedTimes.push_back(temp->arrivedTime);
        temp = temp->next;
    }
    return arrivedTimes;
}


//=============== �޸��������� ================//
bool Route::moveForward(){
    current = current->next;
    stand->x = current->x;
    stand->y = current->y;
    stand->front = current;
    stand->next = current->next;
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
    Customer *ptr1 = head;   // ǰ�ڵ�
    Customer *ptr2, *ptr3;
    for(int i=0; i<size; i++){
        ptr2 = ptr1->next;  // ��ǰ�ڵ�
        ptr3 = ptr2->next;  // ��ڵ�
        float temp = -dist(ptr1, ptr2) - dist(ptr2, ptr3) + dist(ptr1, ptr3); 
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

bool Route::timeWindowJudge(Customer *pre, Customer item){  
    // �����item���뵽pre�����Ƿ��Υ��ʱ�䴰Լ��
    // ��ʱ�����ǲֿ�Ĺز�ʱ��
    // pos��pre��λ��, 0��ʾ�ֿ�
    float time = stand->arrivedTime;
    time += stand->serviceTime;
    Customer *temp, *temp2;
    // ��current��pre
    if(pre != current) {
        temp = current->next;
        time += dist(stand, temp);
        if(time < temp->startTime) time = temp->startTime;
        time += temp->serviceTime;
        temp = temp->next;
        while(temp != pre && temp != NULL) {
            // temp == NULL֤��pre�����ڣ�һ�㲻����
            time += dist(temp->front, temp);
            if(time > temp->endTime) return false;
            if(time < temp->startTime) time = temp->startTime;
            time += temp->serviceTime;
            time += temp->serviceTime;
            temp = temp->next;
        }
        if(temp == NULL) return false;
        time += dist(temp->front, temp);
        if(time > pre->endTime) return false;
        if(time < pre->startTime) time = pre->startTime;
        time += temp->serviceTime;
    }

    // ����time�Ǵ�pre������ʱ��
    // ���������жϲ���item��᲻��Υ��item�Լ����̽ڵ��ʱ�䴰Լ��
    time = time + dist(pre, &item);
    if(time > item.endTime) return false;
    if(time < item.startTime) time = item.startTime;
    time = time + item.serviceTime;
    // �Ƿ�Ӱ��pre����һ���ڵ�
    temp = pre->next;
    if(temp == rear) return true;
    time = time + dist(temp, &item);
    if(time > temp->endTime) return false;
    if(time < temp->startTime) time = temp->startTime;
    time = time + temp->serviceTime;

    // Ȼ���жϻ᲻��Υ��������Ľڵ��ʱ�䴰Լ��
    temp = pre->next;
    temp2 = temp->next;
    while(temp2 !=rear){ 
        time = time + dist(temp, temp2);
        if(time > temp2->endTime) return false;
        if(time < temp2->startTime) time = temp2->startTime;
        time = time + temp2->serviceTime;
        temp = temp->next;
        temp2 = temp2->next;
    }
    return true;
}

void Route::computeInsertCost(Customer item, float &minValue, Customer &customer1, 
        float &secondValue, Customer &customer2,
        float randomNoise, bool allowNegativeCost){
    // ����item�ڵ���·���е���С������ۺʹ�С�������
    // ���������/�μѲ����ǰ��Ĺ˿ͽڵ�
    // pertubation: �Ŷ���������
    // allowNegativeCost: Ϊtrue��ʾ�������ȡ�Ǹ�����Ϊfalse��ʾ��ȡ����
    // randomNoise: ���������
    Customer *pre;
    minValue = MAX_FLOAT;
    secondValue = MAX_FLOAT;
    customer1.id = -1;
    customer2.id = -1;
    for(pre=current; pre!=rear; pre=pre->next) {  // һ����size+1��λ�ÿ��Կ��ǲ���
        if(quantity + item.quantity <= capacity){   // ����Լ��
            if(timeWindowJudge(pre, item) == true) { // ����ʱ�䴰Լ��
                float cost = dist(pre, &item) + dist(&item, pre->next) -
                    dist(pre, pre->next);
                cost += randomNoise;
                if(allowNegativeCost == false) {
                    cost = max(0.0f, cost);
                }
                if(cost <= minValue){  // �ҵ��˸�С�ģ�����minValue��secondValue
                    secondValue = minValue;
                    customer2 = customer1;
                    minValue = cost;
                    customer1 = *pre;
                }
            }
        }
    }
}

void Route::checkArrivedTime() {
    float time = stand->arrivedTime;
    time += stand->serviceTime;
    Customer *pre = stand;
    Customer *cur = stand->next;
    while(cur != rear) {
        time += dist(pre, cur);
        cur->arrivedTime = time;
        if(time > cur->endTime) {
            cout << "Problem in: " << cur->id << " type is: " << cur->type << endl;
            cout << "Now time is: " << time << " end time for him: " <<
                cur->endTime << endl;
            Customer *temp = head->next;
            cout << "Now ids are: " << endl;
            for(temp; temp != rear; temp = temp->next) {
                cout << temp->id << "\t";
            }
            cout << endl;
            throw out_of_range("Violating time constraints");
        }
        if(time < cur->startTime) time = cur->startTime;
        time += cur->serviceTime;
        pre = pre->next;
        cur = cur->next;
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
    Customer *ptr2 = current->next;
    Customer *ptr3 = NULL;
    Customer *ptr4 = NULL;
    ptr4 = ptr1->head;
    while(ptr2 != rear) {
        ptr3 = new Customer;
        *ptr3 = *ptr2;
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
    return *ptr1;
}

void Route::replaceRoute(const Route &route) {  // ��route�滻��currentָ����·��
    Customer* ptr1;
    Customer *ptr2, *ptr3;
    if(current->next != rear) { // current���滹�нڵ㣬��Ҫ�����ԭ��·��
        ptr2 = current->next;
        // ���ԭ·����currentָ������Ԫ��
        // ��������rear�ڵ�����
        while(ptr2 != rear) {
            quantity -= ptr2->quantity;
            ptr3 = ptr2->next;
            delete ptr2;
            ptr2 = ptr3;
            size--;
        }
    }
    current->next = rear;
    rear->front = current;
    stand->next = rear;
    // ��route�г�head��rear��Ľڵ㶼���Ƶ�currentָ���
    ptr1 = route.head->next;
    while(ptr1 != route.rear) {
        ptr2 = ptr1->next;
        try{
            insertAtRear(*ptr1);
        } catch (exception &e) {
            throw out_of_range("While replace route: " + string(e.what()));
        }
        ptr1 = ptr2;
    }
    return;
}

//=============== ���ศ�����ʵĺ��� ================//
bool Route::checkPassRoute(){
    // ������߹���·���Ƿ�Υ��ʱ�䴰Լ��
    // ����ֻ��鵽����һ���ڵ��ʱ���Ƿ�С��ǰһ���ڵ��ʱ�䴰��ʼʱ��
    Customer* ptr1 = head;
    Customer* ptr2 = head->next;
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
    Customer* ptr1 = head->next;
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
