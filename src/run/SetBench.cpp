#include "SetBench.h"
#include "../public/PublicFunction.h"
#include "Config.h"
#include<algorithm>
#include<cmath>

SetBench::SetBench() {
    // ��������ͼ�ֳ���Ȧ��depotΪ���ģ��̼Ҵ������ڻ������˿ʹ������⻷
    // Args(������Config.h):
    //   * R1, R2: �̼���������
    //   * R3, R4: �˿���������
    //   * NUM_STORE: �̼���Ŀ
    //   * NUM_SUBCIRCLE: �˿����򻮷���Ŀ
    //   * LAMBDA: Poisson������̲�����vector���ͣ����ȵ���NUM_SUBCIRCLE
} // ���캯��

void SetBench::constructStoreSet() {
    // �����̼Ҽ���
    float innerR = R1;
    float outerR = R2;
    vector<Spot*> storeSet;
    for(int i=0; i<STORE_NUM; i++) {
        float r = random(innerR, outerR);
        float theta = random(0, 2*PI);
        Spot *store = new Spot();
        // �̵�id��1000��ʼ
        store->id = 1000 + storeSet.size() + 1;
        store->x = r * sin(theta);
        store->y = r * cos(theta);
        store->type = 'S';
        store->startTime = 0;
        store->serviceTime = random(0, 5);
        store->prop = 0;
        storeSet.push_back(store);
    }
    this->storeSet = storeSet;
}

void SetBench::constructCustomerSet() {
    vector<Spot*> customerSet(0);
    float innerR = R3;
    float outerR = R4;
    //float timeHorizon = (float)TIME_SLOT_LEN * TIME_SLOT_NUM; // �����ʱ���᳤��
    float timeHorizon = LATEST_SERVICE_TIME;
    float deltaT = 1; // �������ʱ��
    float deltaAngle = 2 * PI / SUBCIRCLE_NUM;  // ��������н�
    bool mark = true;
    int count = 0; 
    for(int j=0; j<SUBCIRCLE_NUM; j++) {
        for(int i=0; i<TIME_SLOT_NUM-1; i++) {
            int customerNum = poissonSampling(LAMBDA[j], TIME_SLOT_LEN);
            for(int x=0; x<customerNum; x++) {
                // ���������ɹ˿�
                float theta = random(deltaAngle*j, deltaAngle*(j+1));
                float r = random(innerR, outerR);
                Spot *c = new Spot();
                // �˿͵�id��1��ʼ
                c->id =  ++count;
                c->x = r * sin(theta);
                c->y = r * cos(theta);
                c->serviceTime = random(0, 5);
                c->prop = 0;
                c->type = 'C';
                // ���ѡ���̵�
                int index = int(random(0, STORE_NUM));
                index = min(STORE_NUM-1, index);
                Spot *store = new Spot(*storeSet[index]);
                store->type = 'S';
                c->choice = store;
                store->choice = c;
                float distFromCustomerToStore = dist(c, c->choice);
                float distFromDepotToStore = dist(depot, c->choice);
                float minTimeLen = distFromCustomerToStore + distFromDepotToStore;
                if(i*TIME_SLOT_LEN+ALPHA*minTimeLen > timeHorizon) {
                    count--;
                    continue;
                } else {
                    // ��֤�㹻����ʱ�䴰
                    c->startTime = random(i*TIME_SLOT_LEN, (i+1)*TIME_SLOT_LEN);
                    c->startTime = min(c->startTime, timeHorizon-ALPHA*minTimeLen);
                    c->endTime = random(c->startTime+ALPHA*minTimeLen, timeHorizon);
                    float windowLen = c->endTime - c->startTime;
                    c->tolerantTime = c->startTime + random(0.6*windowLen, 0.8*windowLen);
                    c->quantity = random(0, MAX_DEMAND);
                    customerSet.push_back(c);
                }
            }
        }
    }
    this->customerSet = customerSet;
    cout << "The size of customerSet: " << this->customerSet.size() << endl;
}

void SetBench::constructDepot() {
    // �ֿ�ڵ�
    Spot *depot = new Spot();
    depot->x = 0;
    depot->y = 0;
    depot->id = 0;
    depot->type = 'D';
    this->depot = depot;
}

void SetBench::construct(vector<Spot*> &staticCustomerSet, vector<Spot*> &dynamicCustomerSet,
        vector<Spot*> &storeSet, Spot &depot, float currentTime){
    // ��������
    // currentTime: startTime < currentTime��Ϊstatic, ����Ϊdynamic
    // ��Ҫ��д������߼�
    constructDepot();
    constructStoreSet();
    constructCustomerSet();
    vector<Spot*>::iterator iter = customerSet.begin();
    for(iter = customerSet.begin(); iter < customerSet.end(); iter++) {
        if((*iter)->startTime <= currentTime) {
            staticCustomerSet.push_back(*iter);
        } else {
            dynamicCustomerSet.push_back(*iter);
        }
    }
    depot = *this->depot;
    storeSet = this->storeSet;
    cout << "static customer number: " << staticCustomerSet.size() << endl;
    cout << "dynamic customer number: " << dynamicCustomerSet.size() << endl;
}

