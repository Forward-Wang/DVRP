#include "SSLR.h"
#include "../baseclass/Matrix.h"
#include "../run/TxtRecorder.h"
#include<cmath>
#include<stdexcept>
#include<algorithm>
#include<cassert>
#include<functional>

using namespace std;

float RANDOM_RANGE_SSLR[2] = {-1, 1};

SSLR::SSLR(vector<Customer*> waitCustomer, vector<Car*> originPlan, float capacity, int maxIter,
        bool verbose, int pshaw, int pworst, float eta): LNSBase(pshaw, 
        pworst, eta, capacity, RANDOM_RANGE_SSLR, mergeCustomer(waitCustomer, 
        extractCustomer(originPlan)), originPlan[0]->getRearNode(), true, true) 
    {
        this->maxIter = maxIter;
        this->verbose = verbose;
        vector<Customer*>::iterator custPtr;
        // ��waitCustomer�������ȼ���Ϊ2
        for(custPtr = waitCustomer.begin(); custPtr < waitCustomer.end(); custPtr++){
            Customer* newCust = new Customer(**custPtr);
            newCust->priority = 2;
            this->waitCustomer.push_back(newCust);
        }
        this->originPlan = copyPlan(originPlan);
    }

SSLR::~SSLR() {}

bool judgeFeasible(vector<Car*> carSet, int &infeasibleNum) {
	// �ж�carSet�Ƿ����
    bool mark = true;
    vector<Car*>::iterator carIter;
    for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
        //����Ƿ��пճ�
        if ((*carIter) == NULL) {            
            throw out_of_range("Empty cars in the solution!");
        }                                                              

        if((*carIter)->judgeArtificial() == true) {
            // �ж�artificial�����Ƿ���priorityΪ1�Ľڵ�
            vector<Customer*> tempCust = (*carIter)->getAllCustomer();
            for(vector<Customer*>::iterator custIter = tempCust.begin(); custIter < tempCust.end(); 
                    custIter++) {
                if((*custIter)->priority == 1) {
                    mark = false;
                    infeasibleNum++;
                }
            }
            deleteCustomerSet(tempCust);    
        }
    }
    return mark;
}

float* computeDTpara(vector<Customer*> allCustomer, vector<Customer*> waitCustomer, Customer depot, float maxd,
        float mind){
    // ����Բ�ͬ���ȼ��˿͵Ľ���ϵ��
    // Args:
    //   * maxd: ���й˿�֮���������
    //   * allCustomer: ���й˿ͽڵ㣨�������в�ͬ���ȼ��˿ͣ�
    //   * waitCustomer: �����ȼ��˿�
    int PR2Num = (int) waitCustomer.size();
    int PR1Num = (int) allCustomer.size() - PR2Num;
    vector<Customer*>::iterator custPtr;
    float DTH1, DTH2, DTL1, DTL2;
    float distToDepot = 0;    // �����˿ͽڵ㵽�ֿ�ľ���
    for(custPtr = allCustomer.begin(); custPtr < allCustomer.end(); custPtr++) {
        distToDepot += sqrt(pow((*custPtr)->x - depot.x, 2) + pow((*custPtr)->y - depot.y, 2));
    }
    DTL2 = 50;
    DTL1 = 2*maxd + 1;
    DTH2 = 80;
    float tempsigma1 = 2*maxd + DTH2;
    //float tempsigma2 = 2*(PR1NUM + PR2NUM + PR3NUM) * maxd + PR2NUM * DT22 + PR3NUM * DT32 - 
    //	(PR1NUM + PR2NUM + PR3NUM) * mind + PR2NUM * DT21 + PR3NUM * DT31 - DT12;
    float tempsigma2 = 2*distToDepot - DTH2 + PR2Num * (DTL1 + DTL2) - (PR1Num + PR2Num + 1) * mind;
    DTH1 = max(tempsigma1, tempsigma2) + 1;
    float *DTpara = new float[4];
    DTpara[0] = DTH1;
    DTpara[1] = DTH2;
    DTpara[2] = DTL1;
    DTpara[3] = DTL2;
    return DTpara;
}

void SSLR::run(vector<Car*> &finalCarSet, float &finalCost, mutex &print_lck){  
    // �����㷨���൱���㷨��main()����
    int i;
    int customerTotalNum = (int)allCustomer.size();  // �ܵĹ˿���
    int originCarNum = (int)originPlan.size();   // ��ʼӵ�еĻ�������
    vector<Customer*>::iterator custPtr;
    vector<Car*>::iterator carIter;

    // ����ƻ���û�й˿ͣ����׳�warning
    if (customerTotalNum == 0) {                                 
        cout << "WARNNING: In replan, but no customers!" << endl;            
    }                                                              

    // ����Բ�ͬ���ȼ��˿͵Ľ���ϵ��
    float *DTpara = computeDTpara(allCustomer, waitCustomer, depot, maxd, mind);
    resetDTpara(DTpara);
	
    // ����base solution
    vector<Car*> baseCarSet = copyPlan(originPlan);
    if (waitCustomer.size() != 0) {       
        // ֻ�е�waitCustomer��Ϊ��ʱ����"replan"�ļ�ֵ
        // ʹ��virtual carȥװ��waitCustomer
        vector<Car*> tempCarSet1;
        Car *tcar = new Car(depot, depot, capacity, 100, true);
        tempCarSet1.push_back(tcar);
        greedyInsert(tempCarSet1, waitCustomer, false);
        for (carIter = tempCarSet1.begin(); carIter < tempCarSet1.end(); carIter++) {
            Car *tcar = new Car(**carIter);
            baseCarSet.push_back(tcar);
        }
        withdrawPlan(tempCarSet1);
    }
    // ��׼���ۣ�����õ��Ľ���������⣬��һ������
    // һ����˵����������Ľ�ʱ�����е�
    float baseCost = getCost(baseCarSet);   
	
    // �����ʼȫ�����Ž�
    vector<Car*> currentCarSet(0);
    for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
        // ����ԭ�еĳ�������¼������Լ��յ��Լ�ʣ����������׼ʱ��
        Car *newCar = new Car((*carIter)->getNullCar());
        currentCarSet.push_back(newCar);
    }
    // �Ե�ǰ��ӵ�е�working carΪ�����������ʼ�⣨��ȫ���¹��죩
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
    float eta = 0.025f;   // ����ϵ��
    float noiseAmount = eta * maxd;   // ������
    float c = 0.9998f;    // ��������
    vector<Customer*> removedCustomer(0);    // ���Ƴ��Ľڵ�
    vector<Car*> tempCarSet = copyPlan(currentCarSet);      // ��ʱ��ŵ�ǰ��

    pair<bool, int> removalSelectTrend = make_pair(false, 0);
    for(int iter=0; iter<maxIter; iter++){
        if(iter%segment == 0){  // �µ�segment��ʼ
            if(verbose == true) {
                cout << "...............Segement:" << (int)floor(iter/segment)+1 << 
                    "................" << endl;
                cout << "base cost is: " << baseCost << endl;
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
        if(removalSelectTrend.first == false) {
            float removeSelection = random(0,1);  // ����0-1֮��������
            sumation = removeProb[0];
            removeIndex = 0;    // remove heuristic���
            while(sumation < removeSelection){
                sumation += removeProb[++removeIndex];
            }
            removeIndex = min(removeIndex, removeNum-1);  // ��ֹ���
        }
        else{
            removeIndex = removalSelectTrend.second;
        }
        removalSelectTrend.first = false;
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

        deleteCustomerSet(removedCustomer);  // ���removedCustomer
        removedCustomer.resize(0);

        // ִ��remove heuristic
        switch(removeIndex) {
            case 0: {
                // ���ȵõ�maxArrivedTime
                float maxArrivedTime = -MAX_FLOAT;
                for(i=0; i<(int)tempCarSet.size(); i++){
                    // tempCarSet[i]->getRoute().refreshArrivedTime;
                    vector<float> temp = tempCarSet[i]->getRoute().getArrivedTime();
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

        try {
            if (getCustomerNum(tempCarSet) != customerTotalNum) {
                throw out_of_range("Lose some customers in SSLR!");
            }
        } 
        catch (exception &e) {
            cout << e.what() << endl;
        }

        // �Ƴ���·��
        removeNullRoute(tempCarSet, true);
        
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
                        if(newCost > baseCost) {
                            // ��������˸���Ľ⣬�����ӶԵ�ǰ����Ŷ�
                            ksi = 0.8f;   				
                            // ��ʱǿ��ʹ��random removal���ƻ���ǰ�Ľ�
                            removalSelectTrend.first = true;   	
                            removalSelectTrend.second = 1;     // random removal
                        }
                        else {
                            // û�н��ܸ���Ľ⣬��ʹ���еȵ�remove�˿���
                            ksi = 0.6f;
                        }
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
    withdrawPlan(finalCarSet);
    finalCarSet.reserve(originPlan.size());
    ostringstream ostr;
    ostr.str("");
    print_lck.lock();
    // unique_lock<mutex> lck(print_lck);
    int infeasibleNum;
	
    if(judgeFeasible(globalCarSet, infeasibleNum) == false) {
        // ��������������õĽ⣬��ά��ԭ���Ľ�
        ostr << "SSALNS: we should use the origin plan, there are " << infeasibleNum << 
            "high priority customers left in virtual vehicles." << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
        print_lck.unlock();
        finalCarSet = copyPlan(originPlan);
    } else {
        ostr << "SSALNS: we will use the new plan" << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
        print_lck.unlock();
        for (carIter = globalCarSet.begin(); carIter < globalCarSet.end(); carIter++) {
            if ((*carIter)->judgeArtificial() == false) {
                Car *tempCar = new Car(**carIter);
                finalCarSet.push_back(tempCar);
            }
        }
    }

    try {
        if(judgeFeasible(finalCarSet, infeasibleNum) == false) {
            throw out_of_range("In SSLR: The final carset is infeasible!!");
        }
    } 
    catch (exception &e) {
        cout << e.what() << endl;
    }

    delete [] DTpara;
    finalCost = globalCost;
    deleteCustomerSet(waitCustomer);
    deleteCustomerSet(allCustomer);
    withdrawPlan(originPlan);
    withdrawPlan(baseCarSet);
    withdrawPlan(tempCarSet);
    withdrawPlan(globalCarSet);
    hashTable.clear();
}

