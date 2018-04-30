#include "SetBench.h"
#include "../public/PublicFunction.h"
#include "Config.h"
#include<algorithm>
#include<cmath>

SetBench::SetBench(vector<Customer*> originCustomerSet) {
    this->originCustomerSet = copyCustomerSet(originCustomerSet);
} // ���캯��

void SetBench::constructProbInfo(){ 
    // ���ø����ڵ�ĸ�����Ϣ
    vector<int> BHsPos(0); // BHs��λ��
    int i;
    // float temp[6] = {0.4, 0.2, 0.2, 0.1, 0.1, 0};
    vector<Customer*>::iterator iter = originCustomerSet.begin();
    for(iter; iter < originCustomerSet.end(); iter++) {
        // vector<float> dist = randomVec(timeSlotNum);   // �ڸ���slot�������ĸ���
        // vector<float> dist(temp, temp+6);
        int index = random(0, TIME_SLOT_NUM-1);
        for(i=0; i<TIME_SLOT_NUM; i++) {
            if(i == index) {
                (*iter)->timeProb[i] = 0.5;
            } else if(i == TIME_SLOT_NUM - 1) {
                (*iter)->timeProb[i] = 0;
            } else {
                (*iter)->timeProb[i] = 0.5/(TIME_SLOT_NUM - 1);
            }
            //(*iter)->timeProb[i] = dist[i];
        }
    }
}

void SetBench::construct(vector<Customer*> &staticCustomerSet, vector<Customer*> &dynamicCustomerSet, Customer depot){
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
        float minTimeWindowLen = dist(&depot, *iter);
        (*iter)->startTime =  min(tempt, maxActiveTime - ALPHA * minTimeWindowLen); 
        (*iter)->endTime = random((*iter)->startTime + ALPHA * minTimeWindowLen,
                maxActiveTime);

        float timeWindowLen = (*iter)->endTime - (*iter)->startTime;  // ʱ�䴰����
        // �����̵�����õ��𸴵�ʱ�䣬Ϊ0.3-0.6����ʱ�䴰���� + startTime
        (*iter)->tolerantTime = (*iter)->startTime + random(0.6, 0.8) * timeWindowLen;
    }
}
