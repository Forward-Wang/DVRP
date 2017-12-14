#include "PublicFunction.h"
#include "LNS_rel.h"
#include "ALNS.h"
#include<algorithm>
#include<cassert>
#include<functional>
#include<cmath>

using namespace std;

ALNS::ALNS(vector<Customer*> allCustomer, Customer depot, float capacity, int maxIter,
        bool verbose, int pshaw, int pworst, float eta) {
    this->maxIter = maxIter;
    this->verbose = verbose;
    float randomRange[2] = {0, 1}};
    LNSBase(pshaw, pworst, eta, capacity, randomRange, allCustomer, depot);
}



void ALNS::run(vector<Car*> &finalCarSet, float &finalCost){  
    // �����㷨���൱���㷨��main()����
    int i;

    // ������ʼ�⣬��������Ϊ��ǰȫ�����Ž�
    int customerTotalNum = allCustomer.size();
    vector<Car*> currentCarSet(0);
    Car *initialCar = new Car(depot, depot, capacity, 0);  // ���½�һ����
    currentCarSet.push_back(initialCar);
    greedyInsert(currentCarSet, allCustomer, false);  // ������ʼ·��
    float currentCost = getCost(currentCarSet);
    vector<Car*> globalCarSet = copyPlan(currentCarSet);
    float globalCost = currentCost;

    // ��ϣ��
    vector<size_t> hashTable(0);  
    hashTable.push_back(codeForSolution(currentCarSet));

    // ���ֻ�����ز������趨
    const int removeNum = 3;    // remove heuristic�ĸ���
    const int insertNum = 2;    // insert heuristic�ĸ���
    float removeProb[removeNum];  // ����remove heuristic�ĸ���
    float insertProb[insertNum];  // ����insert heuristic�ĸ���
    float noiseProb[2] = {0.5, 0.5};        // ����ʹ�õĸ���
    for(i=0; i<removeNum; i++){
        removeProb[i] = 1.0f/removeNum;
    }
    for(i=0; i<insertNum; i++){
        insertProb[i] = 1.0f/insertNum;
    }
    float removeWeight[removeNum];  // ����remove heuristic��Ȩ��
    float insertWeight[insertNum];  // ����insert heuristic��Ȩ��
    float noiseWeight[2];   // ������/�������� �ֱ��Ȩ��
    setOne<float>(removeWeight, removeNum);
    setOne<float>(insertWeight, insertNum);
    setOne<float>(noiseWeight, 2);
    int removeFreq[removeNum];      // ����remove heuristicʹ�õ�Ƶ��
    int insertFreq[insertNum];      // ����insert heuristicʹ�õ�Ƶ��
    int noiseFreq[2];               // ����ʹ�õ�Ƶ�ʣ���һ����with noise���ڶ�����without noise
    float haha[3];
    setZero<float>(removeFreq, removeNum);
    setZero<float>(insertFreq, insertNum);
    setZero<float>(noiseFreq, 2);
    int removeScore[removeNum];     // ����remove heuristic�ĵ÷�
    int insertScore[insertNum];     // ����insert heuristic�ĵ÷�
    int noiseScore[2];              // �����÷�
    setZero<float>(removeScore, removeNum);
    setZero<float>(insertScore, insertNum);
    setZero<float>(noiseScore, 2);
    // ����÷��趨
    int sigma1 = 33;
    int sigma2 = 9;
    int sigma3 = 13;
    float r = 0.1f;       // weight��������

    // ������Ĳ���
    int segment = 100;   // ÿ��һ��segment����removeProb, removeWeight�Ȳ���
    float w = 0.05f;      // ��ʼ�¶��趨�йز���
    float T = w * currentCost / (float)log(2);   // ��ʼ�¶�
    float ksi = 0.4f;     // ÿ���Ƴ������ڵ���Ŀռ�ܽڵ����ı���
    float maxd, mind, maxquantity;    // �ڵ�֮����������Լ��ڵ��������������
    computeMax(allCustomer, maxd, mind, maxquantity);
    float noiseAmount = eta * maxd;   // ������
    float c = 0.9998f;    // ��������
    vector<Customer*> removedCustomer(0);                // ���Ƴ��Ľڵ�
    vector<Car*> tempCarSet = copyPlan(currentCarSet);   // ��ʱ��ŵ�ǰ��
    for(int iter=0; iter<maxIter; iter++){
        if(iter%segment == 0){  
            // �µ�segment��ʼ
            if (verbose == true) {
                cout << "...............Segement:" << (int)floor(iter/segment)+1 
                    << "................" << endl;
                cout << "current best cost is:" << globalCost << endl;
                cout << "hash table length is:" << hashTable.size() << endl;
                cout << "shaw   removal:" <<  "(score)-" << removeScore[0] 
                    << '\t' << "(freq)-" << removeFreq[0] << endl;
                cout << "random removal:" <<  "(score)-" << removeScore[1] 
                    << '\t' << "(freq)-" << removeFreq[1] << endl;
                cout << "worst  removal:" <<  "(score)-" << removeScore[2] 
                    << '\t' << "(freq)-" << removeFreq[2] << endl;
                cout << "greedy  insert:" <<  "(score)-" << insertScore[0] 
                    << '\t' << "(freq)-" << insertFreq[0] << endl;
                cout << "regret  insert:" <<  "(score)-" << insertScore[1] 
                    << '\t' << "(freq)-" << insertFreq[1] << endl;
                cout << "noise    addIn:" <<  "(score)-" << noiseScore[0]  
                    << '\t' << "(freq)-" << noiseFreq[0]  << endl;
                cout << endl;
            }
            if(iter != 0){      // ������ǵ�һ��segment
                // ����Ȩ��
                updateWeight(removeFreq, removeWeight, removeScore, r, removeNum);
                updateWeight(insertFreq, insertWeight, insertScore, r, insertNum);
                updateWeight(noiseFreq, noiseWeight, noiseScore, r, 2);
                // ���¸���
                updateProb(removeProb, removeWeight, removeNum);
                updateProb(insertProb, insertWeight, insertNum);
                updateProb(noiseProb, noiseWeight, 2);
                // ������������
                setZero(removeFreq, removeNum);
                setZero(removeScore, removeNum);
                setZero(insertFreq, insertNum);
                setZero(insertScore, insertNum);
                setZero(noiseFreq, 2);
                setZero(noiseScore, 2);
            }
        }

        // ���������ѡȡremove heuristic��insert heuristic
        // �Ը���ѡ��remove heuristic
        float removeSelection = random(0,1);  // ����0-1֮��������
        float sumation = removeProb[0];
        int removeIndex = 0;    // remove heuristic���
        while(sumation < removeSelection){
            sumation += removeProb[++removeIndex];
        }
        removeIndex = min(removeIndex, removeNum-1);   // ��ֹ���
        
        // �Ը���ѡ��insert heurisitc
        float insertSelection = random(0,1);
        sumation = insertProb[0];
        int insertIndex = 0;
        while(sumation < insertSelection){
            sumation += insertProb[++insertIndex];
        }
        insertIndex = min(insertIndex, insertNum-1);   // ��ֹ���
        
        // �Ը���ѡ���Ƿ���������Ӱ��
        float noiseSelection = random(0,1);
        bool noiseAdd = false;
        if(noiseProb[0] > noiseSelection) {
            noiseAdd = true;
        }
        
        // ��Ӧ����ʹ�ô�����һ
        removeFreq[removeIndex]++;
        insertFreq[insertIndex]++;
        noiseFreq[1-(int)noiseAdd]++;
        // ����Ƴ��ڵ���Ŀ
        int maxRemoveNum = min(100, static_cast<int>(floor(ksi*customerAmount)));
        // �����Ƴ��ڵ���Ŀ
        int minRemoveNum = 4;  
        // ��ǰҪ�Ƴ��Ľڵ���Ŀ
        int currentRemoveNum = (int)random(minRemoveNum, maxRemoveNum);
        
        deleteCustomerSet(removedCustomer);  // ���removedCustomer       
        removedCustomer.resize(0);

        // ִ��remove heuristic
        switch(removeIndex) {
            case 0: {
                // ���ȵõ�maxArrivedTime
                float maxArrivedTime = -MAX_FLOAT;
                for(i=0; i<(int)tempCarSet.size(); i++){
                    tempCarSet[i]->getRoute().refreshArrivedTime();	
                    vector<float> temp = tempCarSet[i]->getRoute().getArrivedTime();
                    sort(temp.begin(), temp.end(), greater<float>());
                    if(temp[0] > maxArrivedTime) {
                        maxArrivedTime = temp[0];
                    }
                }
                // �������Աmaxt
                maxt = maxArrivedTime;
                // ִ��removal����
                shawRemoval(tempCarSet, removedCustomer, currentRemoveNum);
                break;
            }
            case 1: {
                randomRemoval(tempCarSet, removedCustomer, currentRemoveNum);
                break;
            }
            case 2:
            {
                worstRemoval(tempCarSet, removedCustomer, currentRemoveNum);
                break;
            }
        }
        // ִ��insert heuristic
        switch(insertIndex) {
            case 0: {
                greedyInsert(tempCarSet, removedCustomer, noiseAdd);
                break;
			}
        case 1: {
                regretInsert(tempCarSet, removedCustomer, noiseAdd);
                break;
            }
        }
        try {
            if (getCustomerNum(tempCarSet) != customerAmount) {
                throw out_of_range("Lose some customers in ALNS!");
            }
        }
        catch (exception &e) {
            cout << e.what() << endl;
        }
        removeNullRoute(tempCarSet);

        // ʹ��ģ���˻��㷨�����Ƿ���ոý�
        float newCost = getCost(tempCarSet, penaltyPara);
        float acceptProb = exp(-(newCost - currentCost)/T);
        bool accept = false;
        if(acceptProb > rand()/(RAND_MAX+1.0f)) {
            accept = true;
        }
        T = T * c;   // ����
        size_t newRouteCode = codeForSolution(tempCarSet);
        
        // �������ж��Ƿ���Ҫ�ӷ�
        // �ӷ�������£�
        // 1. ���õ�һ��ȫ�����Ž�ʱ
        // 2. ���õ�һ����δ�����ܹ��ģ����Ҹ��õĽ�ʱ
        // 3. ���õ�һ����δ�����ܹ��Ľ⣬��Ȼ�����ȵ�ǰ����������ⱻ������
        if(newCost < globalCost){  
            // ���1
            removeScore[removeIndex] += sigma1;
            insertScore[insertIndex] += sigma1;
            noiseScore[1-(int)noiseAdd] += sigma1;
            withdrawPlan(globalCarSet);
            globalCarSet = copyPlan(tempCarSet);
            globalCost = newCost;
        } else {
            vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
            if(tempIter == hashTable.end()){  
                // �ý����û�б����ܹ�
                if(newCost < currentCost){    
                    // ����ȵ�ǰ��Ҫ�ã����2
                    removeScore[removeIndex] += sigma2;
                    insertScore[insertIndex] += sigma2;
                    noiseScore[1-(int)noiseAdd] += sigma2;
                } else {
                    if(accept == true) {       
                        // ���3
                        removeScore[removeIndex] += sigma3;
                        insertScore[insertIndex] += sigma3;
                        noiseScore[1-(int)noiseAdd] += sigma3;						
                    }
                }
            }
        }
        if(accept == true) {    
            // ����������ˣ������currentCarSet�� ����tempCarSet����
            vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
            if(tempIter == hashTable.end()){
                hashTable.push_back(newRouteCode); 
            }
            currentCost = newCost;     // ��������գ�����µ�ǰ��
            withdrawPlan(currentCarSet);
            currentCarSet = copyPlan(tempCarSet);
        } else {    
            // ����tempCarSet�ָ�ΪcurrentCarSet
            withdrawPlan(tempCarSet);
            tempCarSet = copyPlan(currentCarSet);
        }
    }

    // �������
    finalCarSet.clear();
    finalCarSet.resize(globalCarSet.size());
    finalCarSet = copyPlan(globalCarSet);
    withdrawPlan(globalCarSet);
    withdrawPlan(tempCarSet);
    withdrawPlan(currentCarSet);
    deleteCustomerSet(allCustomer);
    hashTable.clear();
    finalCost = globalCost;
}

