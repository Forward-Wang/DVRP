#include "SetBench.h"
#include "../public/PublicFunction.h"
#include "Config.h"
#include<algorithm>
#include<cmath>

void SetBench::constructProbInfo(vector<Customer*> originCustomerSet){ 
    // ���ø����ڵ�ĸ�����Ϣ
    vector<int> BHsPos(0); // BHs��λ��
    int i;
    // float temp[6] = {0.4, 0.2, 0.2, 0.1, 0.1, 0};
    vector<Customer*>::iterator iter = originCustomerSet.begin();
    for(iter; iter < originCustomerSet.end(); iter++) {
        vector<float> dist = randomVec(TIME_SLOT_NUM);   // �ڸ���slot�������ĸ���
        // vector<float> dist(temp, temp+TIME_SLOT_NUM);
        // int index = random(0, TIME_SLOT_NUM-1);
        for(i=0; i<TIME_SLOT_NUM; i++) {
            // if(i == index) {
            //     (*iter)->timeProb[i] = 0.5;
            // } else {
            //     (*iter)->timeProb[i] = 0.5/TIME_SLOT_NUM;
            // }
            (*iter)->timeProb[i] = dist[i];
        }
    }
}

void SetBench::changeTWL(vector<Customer*> customerSet, Customer depot, float alpha) {
    float timeHorizon = LATEST_SERVICE_TIME;  // �����ɹ���������ʱ��
    for(int i=0; i<customerSet.size(); i++) {
        Customer *c = customerSet[i];
        float minTimeWindowLen = dist(&depot, c);
        c->endTime = random(c->startTime + ALPHA * minTimeWindowLen,
                timeHorizon);
        if(c->tolerantTime > c->endTime) {
            c->endTime = c->tolerantTime;
        }
    }
}

void SetBench::changeDYN(vector<Customer*> originCustomerSet, Customer depot, float dynamicism,
        vector<Customer*> &staticCustomer, vector<Customer*> &dynamicCustomer) {
    // ���˿ͼ��ֳ�static��dynamic��������
    // Args:
    //   * originCustomerSet: ���еĹ˿ͼ���
    //   * dynamicism: ��̬�˿�ռ��
    // Returns:
    //   * staticCustomer: ��̬�˿ͼ���
    //   * dynamicCustomer: ��̬�˿ͼ���
    sort(originCustomerSet.begin(), originCustomerSet.end(), ascendSortForCustId);
    int customerAmount = originCustomerSet.size();
    int dynamicNum = (int)floor(customerAmount*dynamicism);  // ��̬����Ĺ˿�����
    // dynamicPos: ��̬����Ĺ˿���OriginCustomerSet�еĶ�λ
    // staticPos:  ��̬����Ĺ˿ͽڵ���originCustomerSet�еĶ�λ
    vector<int> staticPos;          	
    // ��̬�����BHs��BHs�����µ�����
    vector<int> dynamicPos = getRandom(0, customerAmount, dynamicNum, staticPos);
    vector<Customer*>::iterator iter;
    for (iter=originCustomerSet.begin(); iter < originCustomerSet.end(); iter++) {
        // ��ǰ�˿ͽڵ���originCustomerSet�еĶ�λ
        // ����Ĭ��originCustomerSet�ǰ�id��������
        int count = iter - originCustomerSet.begin();  				
        // Ѱ��count�Ƿ���dynamicPos��Ԫ��
        vector<int>::iterator iter2 = find(dynamicPos.begin(), dynamicPos.end(), count);
        if (iter2 != dynamicPos.end()) {   // ��dynamicPos������
            (*iter)->prop = 1;
            dynamicCustomer.push_back(*iter);
        }
        else {
            (*iter)->prop = 0;
            staticCustomer.push_back(*iter);
        }
    }
}

void SetBench::construct(vector<Customer*> originCustomerSet, vector<Customer*> &staticCustomerSet, 
        vector<Customer*> &dynamicCustomerSet, Customer depot){
    // ���ݸ��������������
    constructProbInfo(originCustomerSet);
    int customerAmount = originCustomerSet.end() - originCustomerSet.begin();
    int i;
    int dynamicNum = (int)floor(customerAmount*DYNAMICISM);  // ��̬����Ĺ˿�����
    vector<int> staticPos;           // ��̬����Ĺ˿ͽڵ���originCustomerSet�еĶ�λ
    // ��̬�����BHs��BHs�����µ�����
    vector<int> dynamicPos = getRandom(0, customerAmount, dynamicNum, staticPos);   	
    vector<Customer*>::iterator iter = originCustomerSet.begin();
    staticCustomerSet.resize(0);
    dynamicCustomerSet.resize(0);
    float timeSlotLen = REPLAN_END_TIME / TIME_SLOT_NUM;
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
        float t1 = selectSlot * timeSlotLen;         // ʱ��εĿ�ʼ
        float t2 = (selectSlot+1) * timeSlotLen;     // ʱ��εĽ���
        float tempt = random(t1, t2);
        float maxActiveTime = LATEST_SERVICE_TIME;  // �����ɹ���������ʱ��
        float minTimeWindowLen = dist(&depot, *iter);
        (*iter)->startTime =  min(tempt, maxActiveTime - ALPHA * minTimeWindowLen); 
        (*iter)->endTime = random((*iter)->startTime + ALPHA * minTimeWindowLen,
                maxActiveTime);

        float timeWindowLen = (*iter)->endTime - (*iter)->startTime;  // ʱ�䴰����
        // �����̵�����õ��𸴵�ʱ�䣬Ϊ0.3-0.6����ʱ�䴰���� + startTime
        (*iter)->tolerantTime = (*iter)->startTime + random(0.6, 0.8) * timeWindowLen;
    }
}
