#include "SetBench.h"
#include "../public/PublicFunction.h"
#include "Config.h"
#include<algorithm>
#include<cmath>

SetBench::SetBench() {
    // ��������ͼ�ֳ���Ȧ��depotΪ���ģ��̼Ҵ������ڻ������˿ʹ������⻷
    // Args(������Config.h):
    //   * R1, R2, R3: ������depot�ľ���
    //   * NUM_STORE: �̼���Ŀ
    //   * NUM_CUSTOMER: �˿���Ŀ
    //   * NUM_SUBCIRCLE: �˿����򻮷���Ŀ
    //   * LAMBDA: Poisson������̲�����vector���ͣ����ȵ���NUM_SUBCIRCLE
    this->r1 = R1;
    this->r2 = R2;
    this->r3 = R3;
    this->numStore = NUM_STORE;
    this->numSubcircle = NUM_SUBCIRCLE;
    this->lambda = LAMBDA;
    this->currentID;
} // ���캯��

vector<Customer*> SetBench::constructStoreSet() {
    // �����̼Ҽ���
    float innerR = r1;
    float outerR = r2;
    vector<Customer*> storeSet;
    for(int i=0; i<numStore; i++) {
        float r = random(innerR, outerR);
        float theta = random(0, 2*PI);
        Customer store = new Customer;
        store->id = currentID++;
        store->x = r * sin(theta);
        store->y = r * cos(theta);
        store->type = "P";
        store->startTime = 0;
        store->serviceTime = random(0, 10);
        store->prop = 0;
        storeSet.push_back(store);
    }
    return storeSet;
}

vector<Customer*> SetBench::constructCustomerSet() {
    vector<Customer*> customerSet;
    float innerR = r2;
    float outerR = r3;
    int count = 0;
    float timeHorizon = TIME_SLOT_LEN * TIME_SLOT_NUM;
    float deltaT = 10; // �������ʱ��
    float deltaAngle = 2 * PI / numSubcircle;  // ��������н�
    int numSlice = int(timeHorizon/deltaT);
    while(Customer.size() < numCustomer){
        for(int t=0; t<numSlice; t++) {
            for(int j=0; j<numSubcircle; j++) {
                float p = lambda[j] * deltaT * exp(-lambda[j] * deltaT);
                if(p < random(0,1)) {
                    // ���������ɹ˿�
                    float theta = random(deltaAngle*j, deltaAngle*(j+1));
                    float r = random(innerR, outerR);
                    Customer c = new Customer;
                    c->id = currentID++;
                    c->x = r * sin(theta);
                    c->y = r * cos(theta);
                    c->serviceTime = random(0, 10);
                    c->prop = 0;
                    c->choice = int(random(0, numStore));
                    customerSet.push_back(c);
                    if(customerSet.size() == numCustomer) break;
                }
            }
        }
    }
    return customerSet;
}

void SetBench::construct(vector<Customer*> &staticCustomerSet, vector<Customer*> &dynamicCustomerSet){
    // ���ݸ��������������
    constructProbInfo();
    int customerAmount = originCustomerSet.end() - originCustomerSet.begin();
    int i;
    int dynamicNum = (int)floor(customerAmount*DYNAMICISM);  // ��̬����Ĺ˿�����
    vector<int> staticPos;           // ��̬����Ĺ˿ͽڵ���originCustomerSet�еĶ�λ
    // ��̬�����BHs��BHs�����µ�����
    vector<int> dynamicPos = getRandom(0, customerAmount, dynamicNum, staticPos);   	
    vector<Customer*>::iterator iter = originCustomerSet.begin();
    staticCustomerSet.resize(0);
    dynamicCustomerSet.resize(0);
    for(iter; iter<originCustomerSet.end(); iter++) {
        // ��ǰ�˿ͽڵ���originCustomerSet�еĶ�λ
        // ����Ĭ��originCustomerSet�ǰ�id��������
        int count = iter - originCustomerSet.begin();  
        // Ѱ��count�Ƿ���dynamicPos�е�Ԫ��
        vector<int>::iterator iter2 = find(dynamicPos.begin(), dynamicPos.end(), count);
        if(iter2 != dynamicPos.end()) {   
            // ��dynamicPos������
            (*iter)->prop = 1;
            dynamicCustomerSet.push_back(*iter);
        } else {  
            (*iter)->prop = 0;
            staticCustomerSet.push_back(*iter);
        }
        // ���������㷨�����ó��˿Ϳ�����������ʱ���
        int selectSlot = roulette((*iter)->timeProb, TIME_SLOT_NUM);   
        float t1 = selectSlot * TIME_SLOT_LEN;         // ʱ��εĿ�ʼ
        float t2 = (selectSlot+1) * TIME_SLOT_LEN;     // ʱ��εĽ���
        float tempt = random(t1, t2);
        float maxActiveTime = TIME_SLOT_NUM * TIME_SLOT_LEN;  // �����ɹ���������ʱ��
        // ���ٿ���5����serviceTime
        (*iter)->startTime =  min(tempt, maxActiveTime - 5 * (*iter)->serviceTime); 
        float t3 = 3*(*iter)->serviceTime;
        float t4 = 12*(*iter)->serviceTime;
        float timeWindowLen = random(t3, t4);  // ʱ�䴰����
        (*iter)->endTime = min((*iter)->startTime + timeWindowLen, maxActiveTime);
        timeWindowLen = (*iter)->endTime - (*iter)->startTime;
        // �����̵�����õ��𸴵�ʱ�䣬Ϊ0.3-0.6����ʱ�䴰���� + startTime
        (*iter)->tolerantTime = (*iter)->startTime + random(0.6, 0.8) * timeWindowLen;
    }
}
