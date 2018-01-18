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
    this->storeNum = STORE_NUM;
    this->numSubcircle = SUBCIRCLE_NUM;
    this->customerNum = CUSTOMER_NUM;
    this->lambda = LAMBDA;
    this->currentID;
} // ���캯��

void SetBench::constructStoreSet() {
    // �����̼Ҽ���
    float innerR = r1;
    float outerR = r2;
    vector<Spot*> storeSet;
    for(int i=0; i<storeNum; i++) {
        float r = random(innerR, outerR);
        float theta = random(0, 2*PI);
        Spot store = new Spot();
        store->id = currentID++;
        store->x = r * sin(theta);
        store->y = r * cos(theta);
        store->type = "P";
        store->startTime = 0;
        store->serviceTime = random(0, 10);
        store->prop = 0;
        storeSet.push_back(store);
    }
    this->storeSet = storeSet;
}

void SetBench::constructCustomerSet() {
    vector<Spot*> customerSet;
    float innerR = r2;
    float outerR = r3;
    int count = 0;
    float timeHorizon = TIME_SLOT_LEN * TIME_SLOT_NUM; // �����ʱ���᳤��
    float deltaT = 10; // �������ʱ��
    float deltaAngle = 2 * PI / subcircleNum;  // ��������н�
    float alpha = ALPHA;  // ʱ�䴰������dist(�˿ͣ��̼�)�ı���ϵ��
    int numSlice = int(timeHorizon/deltaT);
    while(Spot.size() < customerNum){
        for(int t=0; t<numSlice; t++) {
            for(int j=0; j<numSubcircle; j++) {
                float p = lambda[j] * deltaT * exp(-lambda[j] * deltaT);
                if(p < random(0,1)) {
                    // ���������ɹ˿�
                    float theta = random(deltaAngle*j, deltaAngle*(j+1));
                    float r = random(innerR, outerR);
                    Spot c = new Spot();
                    c->id = currentID++;
                    c->x = r * sin(theta);
                    c->y = r * cos(theta);
                    c->serviceTime = random(0, 10);
                    c->prop = 0;
                    index = int(random(0, storeNum));
                    index = min(storeNum-1, index);
                    c->choice = storeSet[index];
                    distFromCustomerToStore = sqrt(pow(c->x - c->choice->x, 2) + 
                            pow(c->y - c->choice->y, 2));
                    c->startTime = random(0, timeHorizon-alpha*distFromCustomerToStore);
                    c->endTime = random(c->startTime, timeHorizon);
                    c->demand = random(0, MAX_DEMAND);
                    customerSet.push_back(c);
                    if(customerSet.size() == customerNum) break;
                }
            }
        }
    }
    this->customerSet = customerSet;
}

void SetBench::constructDepot() {
    // �ֿ�ڵ�
    Spot depot = new Spot();
    depot->x = 0;
    depot->y = 0;
    depot->id = -1;
    this->depot = depot;
}

void SetBench::construct(vector<Spot*> &staticCustomerSet, vector<Spot*> &dynamicCustomerSet,
        vector<Spot*> &storeSet, Spot &depot){
    // ���ݸ��������������
    constructStoreSet();
    constructCustomerSet();
    constructSpot();
    int i;
    int dynamicNum = (int)floor(customerNum*DYNAMICISM);  // ��̬����Ĺ˿�����
    vector<int> staticPos;           // ��̬����Ĺ˿ͽڵ���customerSet�еĶ�λ
    // ��̬�����BHs��BHs�����µ�����
    vector<int> dynamicPos = getRandom(0, customerNum, dynamicNum, staticPos);   	
    vector<Spot*>::iterator iter = customerSet.begin();
    staticCustomerSet.resize(0);
    dynamicCustomerSet.resize(0);
    for(iter; iter<customerSet.end(); iter++) {
        // ��ǰ�˿ͽڵ���customerSet�еĶ�λ
        // ����Ĭ��customerSet�ǰ�id��������
        int count = iter - customerSet.begin();  
        // Ѱ��count�Ƿ���dynamicPos�е�Ԫ��
        vector<int>::iterator iter2 = find(dynamicPos.begin(), dynamicPos.end(), count);
        if(iter2 != dynamicPos.end()) {   
            // ��dynamicPos������
            dynamicCustomerSet.push_back(*iter);
        } else {  
            staticCustomerSet.push_back(*iter);
        }
        timeWindowLen = (*iter)->endTime - (*iter)->startTime;
        // �����̵�����õ��𸴵�ʱ�䣬Ϊ0.6-0.8����ʱ�䴰���� + startTime
        (*iter)->tolerantTime = (*iter)->startTime + random(0.6, 0.8) * timeWindowLen;
    }
    storeSet = this->storeSet;
    depot = this->depot;
}
