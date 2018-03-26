#include "SSLR.h"
#include "../baseclass/Matrix.h"
#include "../run/TxtRecorder.h"
#include<cmath>
#include<stdexcept>
#include<algorithm>
#include<cassert>
#include<functional>

using namespace std;

float RANDOM_RANGE_SSLR[2] = {0, 1};

SSLR::SSLR(vector<Spot*> allCustomer, vector<Spot*> depots, int maxIter, bool verbose, 
            int pshaw, int pworst, float eta): LNSBase(pshaw,  pworst, eta, 
            RANDOM_RANGE_SSLR, allCustomer, depots, true, true) 
{
    this->maxIter = maxIter;
    this->verbose = verbose;
}

SSLR::~SSLR() {}

bool judgeFeasible(vector<Car*> carSet, vector<Car*> refCarSet, int &infeasibleNum) {
	// �ж�carSet�Ƿ����
    // ��Ҫ�ж�refCarSet�еĹ˿��Ƿ���carSet��working vehicle��
    infeasibleNum = 0;
    bool mark = true;
    vector<Car*>::iterator carIter;
    vector<int> refIDs = getCustomerID(refCarSet);
    vector<int>::iterator iter, iter1;
    sort(refIDs.begin(), refIDs.end());
    vector<Car*> workingCarSet;
    for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
        if((*carIter)->judgeArtificial() == false) {
            workingCarSet.push_back(*carIter);
        }
    }
    vector<int> currentIDs = getCustomerID(workingCarSet);
    for(iter = currentIDs.begin(); iter < currentIDs.end(); iter++) {
        iter1 = find(refIDs.begin(), refIDs.end(), *iter);
        if(iter1 < refIDs.end()) {
            refIDs.erase(iter1);
        }
        sort(refIDs.begin(), refIDs.end());
    }
    infeasibleNum = refIDs.size();
    mark = (infeasibleNum==0);
    return mark;
}

float* computeDTpara(vector<Spot*> allCustomer, vector<Spot*> depots,
        float maxd, float mind){
    // ����Բ�ͬ���ȼ��˿͵Ľ���ϵ��
    // Args:
    //   * maxd: ���й˿�֮���������
    //   * allCustomer: ���й˿ͽڵ㣨������ͬ���ȼ���
    int customerNum = allCustomer.size();
    float DTH1, DTH2;
    DTH2 = 10;
    vector<Spot*>::iterator custPtr;
    float distToDepot = 0;    // �����˿ͽڵ㵽�ֿ�ľ���
    Spot *depot = depots[0];  // ���ѡһ��depot
    for(custPtr = allCustomer.begin(); custPtr < allCustomer.end(); custPtr++) {
        distToDepot += dist(*custPtr, depot);
    }
    float sigma1 = 2*maxd + DTH2;
    float sigma2 = 2 * distToDepot - (customerNum+1) * mind - DTH2;
    DTH1 = max(sigma1, sigma2) + 1;

    float *DTpara = new float[4];
    DTpara[0] = DTH1;
    DTpara[1] = DTH2;
    // û�е����ȼ��˿�
    DTpara[2] = 0;
    DTpara[3] = 0;
    //cout << "DTH1: " << DTpara[0] << " DTH2: " << DTpara[1] << " DTL1: " <<
    //    DTpara[2] << " DTL2:" << DTpara[3] << endl;
    return DTpara;
}

void SSLR::run(vector<Car*> &finalCarSet, float &finalCost){  
    // �����㷨���൱���㷨��main()����
    int i;
    int customerTotalNum = (int)allCustomer.size();  // �ܵĹ˿���
    vector<Spot*>::iterator custPtr;
    vector<Car*>::iterator carIter;

    // ����ƻ���û�й˿ͣ����׳�warning
    if (customerTotalNum == 0) {                                 
        cout << "WARNNING: In replan, but no customers!" << endl;            
    }                                                              

    // ����Բ�ͬ���ȼ��˿͵Ľ���ϵ��
    float *DTpara = computeDTpara(allCustomer, depots, maxd, mind);
    resetDTpara(DTpara);
	
    // �����ʼȫ�����Ž�
    vector<Car*> currentCarSet(0);
    Car *newCar = new Car(*depots[0], *depots[0], 0, depots[0]->id);
    currentCarSet.push_back(newCar);
    greedyInsert(currentCarSet, allCustomer, false);
    // ȫ�����Ž⣬��ʼ���뵱ǰ����ͬ
    vector<Car*> globalCarSet = copyPlan(currentCarSet);        
    float currentCost = getCost(currentCarSet);
    float globalCost = currentCost;

    vector<size_t> hashTable(0);  // ��ϣ��
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
    setZero<int>(removeFreq, removeNum);
    setZero<int>(insertFreq, insertNum);
    setZero<int>(noiseFreq, 2);
    int removeScore[removeNum];     // ����remove heuristic�ĵ÷�
    int insertScore[insertNum];     // ����insert heuristic�ĵ÷�
    int noiseScore[2];              // �����÷�
    setZero<int>(removeScore, removeNum);
    setZero<int>(insertScore, insertNum);
    setZero<int>(noiseScore, 2);
    // ����÷��趨
    int sigma1 = 33;
    int sigma2 = 9;
    int sigma3 = 13;
    float r = 0.1f;       // weight��������

    // ������Ĳ���
    int segment = 100;   // ÿ��һ��segment����removeProb, removeWeight�Ȳ���
    float w = 0.05f;      // ��ʼ�¶��趨�йز���
    float T = w * abs(currentCost) / (float)log(2);   // ��ʼ�¶�
    float ksi = 0.8f;    // ÿ���Ƴ������ڵ���Ŀռ�ܽڵ����ı���
    float c = 0.9998f;    // ��������
    vector<Spot*> removedCustomer(0);    // ���Ƴ��Ľڵ�
    vector<Car*> tempCarSet = copyPlan(currentCarSet);      // ��ʱ��ŵ�ǰ��

    for(int iter=0; iter<maxIter; iter++){
        if(iter%segment == 0){  // �µ�segment��ʼ
            if(verbose == true) {
                cout << "...............Segment:" << (int)floor(iter/segment)+1 << 
                    "................" << endl;
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
                setZero<int>(removeFreq, removeNum);
                setZero<int>(removeScore, removeNum);
                setZero<int>(insertFreq, insertNum);
                setZero<int>(insertScore, insertNum);
                setZero<int>(noiseFreq, 2);
                setZero<int>(noiseScore, 2);
            }
        }

        // ���������ѡȡremove heuristic��insert heuristic
        // �Ը���ѡ��remove heuristic
        int removeIndex;
        float sumation;
        float removeSelection = random(0,1);  // ����0-1֮��������
        sumation = removeProb[0];
        removeIndex = 0;    // remove heuristic���
        while(sumation < removeSelection){
            sumation += removeProb[++removeIndex];
        }
        removeIndex = min(removeIndex, removeNum-1);  // ��ֹ���
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

        // decide the number to remove
        int currentRemoveNum;  
        // ����Ƴ��Ľڵ���
        int maxRemoveNum = min(100, static_cast<int>(floor(ksi*customerTotalNum)));  
        // �����Ƴ��Ľڵ���
        int minRemoveNum = 4;
        // ��ǰ��Ҫ�Ƴ��Ľڵ���Ŀ
        currentRemoveNum = (int)floor(random(minRemoveNum, maxRemoveNum)); 

        removedCustomer.clear();
        removedCustomer.resize(0);

        // ִ��remove heuristic
        switch(removeIndex) {
            case 0: {
                // ���ȵõ�maxArrivedTime
                float maxArrivedTime = -MAX_FLOAT;
                for(i=0; i<(int)tempCarSet.size(); i++){
                    // tempCarSet[i]->getRoute().refreshArrivedTime;
                    vector<float> temp = tempCarSet[i]->getRoute()->getArrivedTime();
                    sort(temp.begin(), temp.end(), greater<float>());
                    if(temp[0] > maxArrivedTime) {
                        maxArrivedTime = temp[0];
                    }
                }
                // �������Աmaxt
                this->maxt = maxArrivedTime;
                // ����removal����
                shawRemoval(tempCarSet, removedCustomer, currentRemoveNum);
                break;
            }
            case 1: {
                randomRemoval(tempCarSet, removedCustomer, currentRemoveNum);
                break;
            }
            case 2: {
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

        if (getCustomerNum(tempCarSet) != customerTotalNum) {
            cout << "Lose some customers in SSLR!" << endl;
            exit(1);
        }

        // �Ƴ���·��
        removeNullRoute(tempCarSet);
        
        // ʹ��ģ���˻��㷨�����Ƿ���ոý�
        bool accept = false;
        float newCost = getCost(tempCarSet);
        float acceptProb = exp(-(newCost - currentCost)/T);
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
        if(newCost < globalCost){  // ���1
            // �õ���ȫ�����Ž⣬���Ǽ���remove�Ĺ˿���
            ksi = 0.4f;
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
                    // ����õ��˸��õĽ⣬�����remove�Ĺ˿���
                    ksi = 0.4f;  
                    removeScore[removeIndex] += sigma2;
                    insertScore[insertIndex] += sigma2;
                    noiseScore[1-(int)noiseAdd] += sigma2;
                } 
                else {      
                    if(accept == true) {       
                        // ���3
                        ksi = 0.6f;
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
        } 
        else {    
            // ����tempCarSet�ָ�ΪcurrentCarSet
            withdrawPlan(tempCarSet);
            tempCarSet = copyPlan(currentCarSet);
        }
    }    

    finalCarSet = globalCarSet;   
    finalCost = globalCost;
    withdrawPlan(tempCarSet);
    withdrawPlan(globalCarSet);
    withdrawPlan(currentCarSet);
    hashTable.clear();
}

