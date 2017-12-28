#include "PublicFunction.h"
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<vector>
#include<numeric>
#include<sstream>
#include<iostream>
#include<limits>
#include<map>
#include<cmath>
#include<ctime>
#include<string>
#include<functional>
#include<cstddef>
#include "Matrix.h"
#include "ALNS.h"

bool ascendSortForCustId(Customer* item1, Customer* item2) {
    return item1->id < item2->id;
}

float random(float start, float end){
    // ����start��end֮��������
    // some times using (int)output will get the boundary value "end" !!
    return (float)(start+(end-start)*rand()/(RAND_MAX+1.0));
}

vector<float> randomVec(int num){  
    // ����num������������ǵĺ�Ϊ1
    float rest = 1;  // ��ʼ����Ϊ1
    vector<float> output;
    if(num == 1) {
        output.push_back(1);
    } else {
        for(int i=0; i<num - 1; i++) {
            float temp = random(0, rest); // ���������
            output.push_back(temp);
            rest -= temp;
        }
        output.push_back(rest);
    }
    return output;
}

vector<int> getRandom(int lb, int ub, int m, vector<int> &restData){
    // ����m����ͬ�ģ���ΧΪ[lb, ub)�������
    // restData, ���˷���ֵ��ʣ�����ֵ
    restData.resize(0);
    for(int i=0; i<ub-lb; i++) {
        restData.push_back(i+lb);
    }
    int total = ub-lb;
    vector<int> outputData(0);
    for(int j=0; j<m; j++) {
        vector<int>::iterator iter = restData.begin();
        int num = (int)rand() % total; // 0-total-1, if normal 
        num = min(num, total - 1);     // avoid exceeding the right side
        iter += num;
        int temp = *(iter);
        outputData.push_back(temp);
        restData.erase(iter);
        total--;
    }
    return outputData;
}

int roulette(vector<float> probability) {
    // �����㷨
    // ���ѡ��һ������k (from 0 to |probability|)��
    vector<float>::iterator floatIter;
    float sumation1 = accumulate(probability.begin(), probability.end(), 0.0f); // ���
    for(floatIter = probability.begin(); floatIter < probability.end(); floatIter++) {
        *floatIter /= sumation1;  // ��һ��
    }
    int totalNum = probability.end() - probability.begin();  // ����Ŀ
    int k = 0;
    float sumation = 0;
    float randFloat = rand()/(RAND_MAX + 1.0f);
    floatIter = probability.begin();
    while((sumation < randFloat) && (floatIter < probability.end())) {
        k++;
        sumation += *(floatIter++);
    }
    k = min(k-1, totalNum-1); // avoid k is larger than totalNum
    k = max(k, 0);            // when randFloat = 0, here k < 0 will happen
    return k;
}

int roulette(float *probability, int num) {
    // �����㷨
    // ���ѡ��һ������k (from 0 to |probability|)��
    // һ����num�����ʷֲ�
    int i;
    float sumation1 = 0; // ���
    for(i=0; i<num; i++) {
        sumation1 += probability[i];
    }
    for(i=0; i<num; i++) {
        probability[i] /= sumation1;  // ��һ��
    }
    int k = 0;
    float sumation = 0;
    float randFloat = rand()/(RAND_MAX + 1.0f);
    while((sumation < randFloat) && (k < num)) {
        k++;
        sumation += probability[k];
    }
    k = min(k - 1, num - 1);   // avoid k is larger than num
    k = max(k, 0);             // when randFloat = 0, here k < 0 will happen
    return k;
}



void seperateCustomer(vector<Customer*> originCustomerSet, vector<Customer*> &staticCustomer, 
        vector<Customer*> &dynamicCustomer, float dynamicism) {
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
    staticCustomer.resize(0);
    dynamicCustomer.resize(0);
    vector<Customer*> originCopy = copyCustomerSet(originCustomerSet);
    for (iter=originCopy.begin(); iter < originCopy.end(); iter++) {
        // ��ǰ�˿ͽڵ���originCustomerSet�еĶ�λ
        // ����Ĭ��originCustomerSet�ǰ�id��������
        int count = iter - originCopy.begin();  				
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


void computeBest(vector<Car*> carSet, vector<Car*> &bestRoute, float &bestCost){
    // �����CarSet�й˿͵���ѷ���·��
    // Args:
    //   * carSet: ������һ���ȽϴֲڵĽ⣬�������й˿ͽڵ����Ϣ
    // Returns:
    //   * bestRoute: ��ѷ���·�ߣ���ALNS���
    //   * bestCost: ��С����
    Customer depot = carSet[0]->getRearNode();
    float capacity = carSet[0]->getCapacity();
    vector<Customer*> allCustomer;
    vector<Car*>::iterator carIter;
    vector<Customer*>::iterator custIter;
    for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
        vector<Customer*> tempCust = (*carIter)->getAllCustomer();
        cout << "Loading the " << carIter - carSet.begin() << "car data!" << endl;
        for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
            allCustomer.push_back(*custIter);
        }
    }
    depot.priority = 0;
    depot.startTime = 0;
    depot.serviceTime = 0;
    depot.arrivedTime = 0;
    depot.prop = 0;
    ALNS alg(allCustomer, depot, capacity, 25000);
    alg.run(bestRoute, bestCost);
    deleteCustomerSet(allCustomer);
}

void showAllCustomer(vector<Car*> carSet) {
    // ��ʾcarSet�����й˿͵�id��Ϣ
    vector<int> allCustomerId;
    for(int i=0; i<(int)carSet.size(); i++) {
        vector<Customer*> tempCust = carSet[i]->getAllCustomer();
        vector<Customer*>::iterator custIter;
        for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
            allCustomerId.push_back((*custIter)->id);
        }
        deleteCustomerSet(tempCust);
    }
    sort(allCustomerId.begin(), allCustomerId.end());
    vector<int>::iterator intIter;
    int count = 0;
    for(intIter = allCustomerId.begin(); intIter < allCustomerId.end(); intIter++) {
        if (count % 8 == 0) {
            cout << endl;
        }
        cout << (*intIter) << '\t';
        count++;
    }
    cout << endl;
}

int getCustomerNum(vector<Car*> originCarSet) {
    // ���·�����й˿ͽڵ����Ŀ
    int customerNum = 0;
    for (int i = 0; i<(int)originCarSet.size(); i++) {
        customerNum += originCarSet[i]->getRoute().getSize();
    }
    return customerNum;
}

bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2){
    // �ж�carSet1��carSet2�Ƿ�һ��(���������еĹ˿�id˳���ж�)
    if(carSet1.size() != carSet2.size()) {return false;}
    bool mark = true;
    for(int i=0; i<(int)carSet1.size(); i++){
        vector<Customer*> cust1 = carSet1[i]->getRoute().getAllCustomer();
        vector<Customer*> cust2 = carSet2[i]->getRoute().getAllCustomer();
        if(cust1.size() != cust2.size()) {mark = false; break;}
        for(int j=0; j<(int)cust1.size(); j++) {
            if(cust1[j]->id != cust2[j]->id) {
                mark = false; 
                break;
            }
        }
        deleteCustomerSet(cust1);
        deleteCustomerSet(cust2);
    }
    return mark;
}

bool customerSetEqual(vector<Customer*>c1, vector<Customer*>c2){
    // �ж�customer����c1��c2�Ƿ�һ��������id�жϣ�
    if(c1.size() != c2.size()) {return false;}
    bool mark = true;
    for(int i=0; i<(int)c1.size(); i++) {
        if(c1[i]->id != c2[i]->id) {
            mark = false; 
            break;
        }
    }
    return mark;

}

vector<Customer*> extractCustomer(vector<Car*> plan) {
    // ��plan�еĹ˿ͽڵ�ȫ����ȡ����
    vector<Customer*> allCustomer;
    vector<Customer*>::iterator custIter;
    vector<Car*>::iterator carIter;
    for (carIter=plan.begin(); carIter<plan.end(); carIter++) {
        vector<Customer*> temp = (*carIter)->getAllCustomer();
        for (carIter=temp.begin(); carIter<temp.end(); carIter++) {
            allCustomer.push_back(*custIter);
        }
    }
    return allCustomer;
}

vector<Customer*> mergeCustomer(vector<Customer*> waitCustomer, vector<Customer*>originCustomer) {
    // ��waitCustomer��originCustomer�ں�Ϊһ������
    vector<Customer*> allCustomer;
    vector<Customer*> custIter;
    for(custIter = waitCustomer.begin(); custIter < waitCustomer.end(); custIter++) {
        allCustomer.push_back(new(**custIter));
    }
    for(custIter = originCustomer.begin(); custIter < originCustomer.end(); custIter++) {
        allCustomer.push_back(new(**custIter));
    }
    return allCustomer;
}

