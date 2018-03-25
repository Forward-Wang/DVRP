#include "Dispatcher.h"
#include<algorithm>
#include "Simulator.h"
#include "../public/PublicFunction.h"
#include "../run/TxtRecorder.h"
#include "../run/Config.h"
#include <stdexcept>

Dispatcher::Dispatcher(vector<Spot*> staticCustomerSet, vector<Spot*> dynamicCustomerSet, 
        vector<Spot*> storeSet, Spot depot, float capacity) {
    this->depot = depot;
    this->capacity = capacity;
    this->storeSet = storeSet;
    this->globalCarIndex = 0;
    int custNum = staticCustomerSet.end() - staticCustomerSet.begin();
    custNum += dynamicCustomerSet.end() - dynamicCustomerSet.begin(); // �ܹ˿���
    servedCustomerId.reserve(custNum);     // �Ѿ�������Ĺ˿�id
    promisedCustomerId.reserve(custNum);
    waitCustomerId.reserve(custNum);
    rejectCustomerId.reserve(custNum);
    vector<Spot*>::iterator custIter = staticCustomerSet.begin();
    for(custIter; custIter < staticCustomerSet.end(); custIter++) {
        // �ڼƻ���ʼǰ�Ѿ��������Ĺ˿Ͷ�����promiseCustomer
        allCustomer.push_back(*custIter);
        promisedCustomerId.push_back((*custIter)->id);
    }
    for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
        allCustomer.push_back(*custIter);	
        dynamicCustomerId.push_back((*custIter)->id);
    }
    sort(allCustomer.begin(), allCustomer.end(), ascendSortForCustId);  // ��id���е�������
    sort(promisedCustomerId.begin(), promisedCustomerId.end());
    sort(dynamicCustomerId.begin(), dynamicCustomerId.end());
}


void Dispatcher::destroy() {
    deleteCustomerSet(allCustomer);
}

void Dispatcher::carFinishTask(int carIndex){       
    // �ճ�
    // ���ǻ��currentPlan��ɾ��idΪcarIndex�Ĺ˿�
    // ���ҽ�����뵽finishedPlan
    vector<Car*>::iterator carIter;
    ostringstream ostr;
    for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
        if((*carIter)->getCarIndex() == carIndex) {
            Car *newCar = new Car(**carIter);
            delete(*carIter);
            currentPlan.erase(carIter);
            finishedPlan.push_back(newCar);
            break;
        }
    }
    if(currentPlan.size() == 0) {
        ostr.str("");
        ostr << "=============== END ================" << endl;
        ostr << "----OH!! Finished!" << endl;
        ostr << "----" << servedCustomerId.size() << " customers get served finally" << endl;
        ostr << "----" << rejectCustomerId.size() << " customers get rejected finally" << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
    } else {
        // �������ʣ��ĳ������ǿճ�
        bool mark = true;
        int num = 0;
        for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
            if((*carIter)->getRoute()->getSize() != 0) {
                mark = false;
                break;
            } else {
                num++;
            }
        }
        if(mark == true) {
            ostr.str("");
            ostr << "=============== END ================" << endl;
            ostr << "----There are " << num << " Cars remains unused!" << endl;
            ostr << "----" << servedCustomerId.size() << " customers get served finally" << endl;
            ostr << "----" << rejectCustomerId.size() << " customers get rejected finally" << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();	
        }
    }
}

void checkFeasible(vector<Car*> carSet, vector<int> promisedCustomerId){
    // �ж�promiseCustomerSet�еĹ˿��Ƿ���carSet��
    vector<int> tempId = promisedCustomerId;  // �õ���promise�Ĺ˿�id
    sort(tempId.begin(), tempId.end());
    vector<Car*>::iterator carIter;
    for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {  
        vector<Spot*> tempCust = (*carIter)->getAllCustomer();
        vector<Spot*>::iterator custIter;
        for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
            vector<int>::iterator intIter = find(tempId.begin(), tempId.end(), (*custIter)->id);
            if(intIter < tempId.end()) {
                // ����ҵ��ˣ���ɾ��
                tempId.erase(intIter);
            }
        }
    }
    if(tempId.size() != 0) {   // if there are promiseCustomers excluded
        throw out_of_range("Not all promise customers are in the car set!");
    }
}

void checkConnection(vector<Car*> carSet) {
    // ���ÿһ������store��customer�������Ƿ�Ե�
    vector<Car*>::iterator carIter;
    for(carIter=carSet.begin(); carIter<carSet.end(); carIter++) {
        vector<Spot*> allSpot = (*carIter)->getRoute()->getAllSpot();
        int count = 0;
        vector<Spot*>::iterator custIter;
        for(custIter=allSpot.begin(); custIter<allSpot.end(); custIter++) {
            if((*custIter)->id >= 1000) {
                // id���ڵ���1000Ŀǰ��store
                count++;
            } else {
                count--;
            }
        }
        if(count != 0) {
            ostringstream ostr;
            ostr << "Car #" << (*carIter)->getCarIndex() << 
                "has unbalanced ids"; 
            throw out_of_range(ostr.str());
            break;
        }
    }
}


vector<EventElement> Dispatcher::handleNewTimeSlot(int slotIndex){ 
    // ��ʱ��ο�ʼ
    cout << "Handle new time slot!" << endl;
    vector<Spot*> promiseCustomerSet;
    vector<Spot*> waitCustomerSet;
    vector<Spot*> dynamicCustomerSet;
    vector<int>::iterator custIdIter;
    vector<Car*>::iterator carIter;
    vector<Car*> updatedPlan;
    for(custIdIter = promisedCustomerId.begin(); custIdIter< promisedCustomerId.end(); custIdIter++) {
        promiseCustomerSet.push_back(allCustomer[*custIdIter - 1]);  // id��1��ʼ���
    }
    for(custIdIter = dynamicCustomerId.begin(); custIdIter < dynamicCustomerId.end(); custIdIter++) {
        dynamicCustomerSet.push_back(allCustomer[*custIdIter - 1]);
    }
    vector<EventElement> newEventList;
    ostringstream ostr;
    if(slotIndex == 1) {  // ·���ƻ���Ҫ��ʼ��
        ostr.str("");
        ostr << "============ Now Initialize the routing plan ===========" << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
        Simulator smu(slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, 
                currentPlan, storeSet);
        depot.arrivedTime = slotIndex * TIME_SLOT_LEN;
        currentPlan = smu.initialPlan(depot, capacity);
        globalCarIndex = currentPlan.size();
        for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
            EventElement newEvent = (*carIter)->launchCar(slotIndex*TIME_SLOT_LEN);  // ����������
            if(newEvent.customerId != 0 && newEvent.customerId != -1) {
                cout << "Car #" << newEvent.carIndex << " has been launched!" << endl;
            }
            newEventList.push_back(newEvent);
        }
        try {
            checkConnection(currentPlan);
        } catch(exception &e) {
            cout << "In initial: " << e.what() << endl;
        }
        ostr.str("");
        ostr << "----Initialization Finished! Now there are " << currentPlan.size() 
            << " cars dispartched!" << endl << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
    } else {
        // ��ȡÿ����δ�߹���·�����з��棬�õ��µ�·���ƻ�
        ostr.str("");
        ostr << "============ Now replan, the time slot is: " << slotIndex << "============" << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
        float currentTime = slotIndex * TIME_SLOT_LEN;
        for(custIdIter = waitCustomerId.begin(); custIdIter < waitCustomerId.end(); custIdIter++) {
            Spot *temp = allCustomer[*custIdIter-1];
            waitCustomerSet.push_back(temp);
        }
        vector<Car*> futurePlan;
        for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
            // ץȡ���ӻ�û���߹��ļƻ�
            Car *tempCar = (*carIter)->capturePartRoute(currentTime);
            futurePlan.push_back(tempCar);
        }
        if (currentPlan.size() != 0) {  // �л�������ʱ���Ž���replan
            Simulator smu(slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, 
                    futurePlan, storeSet);
            vector<int> newservedCustomerId;
            vector<int> newAbandonedCustomerId;
            vector<int> delayCustomerId;
            updatedPlan = smu.replan(newservedCustomerId, newAbandonedCustomerId, 
                    delayCustomerId, capacity);
            withdrawPlan(futurePlan);
            //updatedPlan = smu.no_replan();
            vector<Spot*>::iterator custIter;

            // ����promiseCustomerId, rejectCustomerId�Լ�waitCustomerId
            vector<int>::iterator intIter1, intIter2;
            promisedCustomerId.insert(promisedCustomerId.end(), newservedCustomerId.begin(), 
                    newservedCustomerId.end());
            sort(promisedCustomerId.begin(), promisedCustomerId.end());
            
            // tempVec: ���new served�Լ�new abandoned�˿�
            //          ��ԭ����wait Customer��tempVec����ɵõ����µ�wait Customer
            vector<int> tempVec;
            tempVec.insert(tempVec.end(), newservedCustomerId.begin(), newservedCustomerId.end());

            rejectCustomerId.insert(rejectCustomerId.end(), newAbandonedCustomerId.begin(),
                    newAbandonedCustomerId.end());
            sort(rejectCustomerId.begin(), rejectCustomerId.end());
            tempVec.insert(tempVec.end(), newAbandonedCustomerId.begin(),
                    newAbandonedCustomerId.end());
            
            // ����Ƿ�tempVec��Ԫ�ض���ԭ����wait Customer��
            for(intIter1 = tempVec.begin(); intIter1 < tempVec.end(); intIter1++) {
                intIter2 = find(waitCustomerId.begin(), waitCustomerId.end(), *intIter1);
                if(intIter2 == waitCustomerId.end()) { 
                    // û���ҵ�������
                    cerr << "tempVec not totally in waitCustomerId!!" << endl;
                    exit(1);
                }   
            }

            if(tempVec.size() != 0) {
                // ��ȡ���µ�waitCustomerId
                sort(waitCustomerId.begin(), waitCustomerId.end());
                sort(tempVec.begin(), tempVec.end());
                vector<int> tempVec2(20);
                intIter1 = set_difference(waitCustomerId.begin(), waitCustomerId.end(), 
                        tempVec.begin(), tempVec.end(), tempVec2.begin());
                tempVec2.resize(intIter1 - tempVec2.begin());
                waitCustomerId = tempVec2;
            }
            
            // ��������future plan���嵽currentPlan��Ӧλ��֮��
            int count = 0;
            for (carIter = updatedPlan.begin(); carIter < updatedPlan.end(); carIter++) {
                currentPlan[count]->replaceRoute(*carIter, currentTime);
                EventElement newEvent;
                if (currentPlan[count]->getState() == wait) {
                    // �������ԭ������wait״̬������Ҫ���䷢��
                    newEvent = currentPlan[count]->launchCar(currentTime);
                    if(newEvent.customerId != -1) {
                        cout << "Car #" << newEvent.carIndex << 
                            " has been launched!" << endl;
                    }
                }
                else {
                    newEvent = currentPlan[count]->getCurrentAction(currentTime);
                }
                newEventList.push_back(newEvent);
                count++;
            }
            
            // ֮ǰ���ֹ�replace part route��allCustomer���ݱ��滻
            // ������������������
            for(int i=0; i<allCustomer.size(); i++) {
                if(allCustomer[i]->type != 'C') {
                    cout << i << "th elements in allCustomer is not customer" << endl;
                    exit(1);
                }
            }

            try {
                checkConnection(currentPlan);
            } catch(exception &e) {
                cout << "In replan: " << e.what() << endl;
            }
            ostr.str("");
            ostr << "----Replan Finished! Now there are " << currentPlan.size() 
                << " cars working!" << endl << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();
               
        }
        else {
            ostr.str("");
            ostr << "----no car is applicable!!!" << endl << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();
        }
    }
    return newEventList;
}

int mapID(Spot *node) {
    int id;
    if(node->type == 'D') {
        id = 0;
    } else if (node->type == 'S') {
        id = node->choice->id;
    } else {
        id = node->id;
    }
    return id;
}

Spot* getMappedNode(int id, char type, Car *currentCar) {
    if(type == 'D') {
        return currentCar->getCurrentNode();
    } 
    else {
        Spot *customer = currentCar->findCustomer(id);
        if(type == 'S') {
            return customer->choice;
        } else {
            return customer;
        }
    }
}

EventElement Dispatcher::handleNewCustomer(int slotIndex, Spot *newCustomer){
    // �����¹˿�(dynamic)����
    // Args:
    //    * slotIndex: �����жϹ˿��Ƿ������ĵȵ���һ��slotIndex
    //    * newCustomer: �¹˿ͽڵ�
    // Returns: (����ڳ�Ա������)
    //    * newCar: Ϊ�˷���newCustomer��������Ҫ��ǲ�µ�����
    //    * globalCarIndex: �³���id�������ǲ���³������Լ�1         
    ostringstream ostr;
    ostr.str("");
    ostr<< "----Time: " << newCustomer->startTime << ", Customer with id #" 
        << newCustomer->id << " is arriving..." << endl;
    TxtRecorder::addLine(ostr.str());
    cout << ostr.str();
    Spot *copyNewCustomer = new Spot(*newCustomer);
    Spot *copyNewStore = new Spot(*(newCustomer->choice));
    copyNewCustomer->choice = copyNewStore;
    copyNewStore->choice = copyNewCustomer;
    vector<int>::iterator intIter = find(dynamicCustomerId.begin(), dynamicCustomerId.end(), 
            copyNewCustomer->id);
    dynamicCustomerId.erase(intIter);
    float minInsertCost = MAX_FLOAT;
    // insertPos:
    //    * ��һ��int�ǻ�����ţ���currentPlan�е�λ�ã�
    //    * �ڶ���pair��϶���customer id(��ʹ�������store��Ҳ��¼��customer)
    pair<int, pair<int, int> > insertPos;   
    pair<int, pair<char, char> > insertType;
    vector<Car*>::iterator carIter;
    float currentTime = copyNewCustomer->startTime;       // �˿���������ʱ��������ʱ�䴰��ʼ��ʱ��
    for (carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
        // ��copyNewCustomer��ÿ��route����С�������
        Car *tempCar;
        try {
            tempCar = (*carIter)->capturePartRoute(currentTime);
        } catch (exception &e) {
            cout << "While handling new customer: " << endl;
            cout << e.what() << endl;
        }
        Spot *refStore1, *refCustomer1, *refStore2, *refCustomer2;
        float minValue, secondValue;
        tempCar->computeInsertCost(copyNewCustomer->choice, copyNewCustomer, minValue, 
                refStore1, refCustomer1, secondValue, refStore2, refCustomer2, 
                currentTime);
        if(minValue < minInsertCost) {
            int pos = carIter - currentPlan.begin();  
            minInsertCost = minValue;           
            insertPos = make_pair(pos, make_pair(mapID(refStore1), 
                        mapID(refCustomer1)));
            insertType = make_pair(pos, make_pair(refStore1->type, 
                        refCustomer1->type));
        }
    }
    EventElement newEvent;
    if(minInsertCost == MAX_FLOAT) {
        // û�п��в����
        if(copyNewCustomer->tolerantTime < slotIndex * TIME_SLOT_LEN) { 
            ostr.str("");
            ostr << "He is rejected!" << endl;
            ostr << "His tolerance time is " << copyNewCustomer->tolerantTime << endl;
            ostr << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();
            rejectCustomerId.push_back(copyNewCustomer->id);
            sort(rejectCustomerId.begin(), rejectCustomerId.end());

            //// �Ȳ���replan�������Ȱ����µ�����Ϊ�����
            //Spot *newDepot = new Spot(depot);
            //newDepot->arrivedTime = copyNewCustomer->startTime;
            //Car *newCar = new Car(*newDepot, *newDepot, capacity, globalCarIndex);
            //Spot *refStore1, *refCustomer1, *refStore2, *refCustomer2;
            //float minValue, secondValue;
            //newCar->computeInsertCost(copyNewCustomer->choice, copyNewCustomer, minValue, 
            //        refStore1, refCustomer1, secondValue, refStore2, refCustomer2, 
            //        currentTime);
            //if(minValue == MAX_FLOAT) {
            //    // ����Ѿ����������ͣ���ܾ�Ϊ�����
            //    // ��þ�������tolerantTime-endTimeΪ���ɴ�ʱ��
            //    ostr.str("");
            //    ostr << "He is rejected!" << endl;
            //    ostr << "His tolerance time is " << copyNewCustomer->tolerantTime << endl;
            //    ostr << endl;
            //    TxtRecorder::addLine(ostr.str());
            //    cout << ostr.str();
            //    rejectCustomerId.push_back(copyNewCustomer->id);
            //    sort(rejectCustomerId.begin(), rejectCustomerId.end());
            //    delete newCar;
            //} 
            //else {
            //    // ���򣬽��䰲�Ÿ��µ�����
            //    promisedCustomerId.push_back(copyNewCustomer->id);
            //    sort(promisedCustomerId.begin(), promisedCustomerId.end());
            //    globalCarIndex++;
            //    newCar->insertAtHead(copyNewCustomer->choice, copyNewCustomer);
            //    newEvent = newCar->launchCar(copyNewCustomer->startTime);
            //    currentPlan.push_back(newCar);
            //    cout << "Open new Car #" << newCar->getCarIndex() <<
            //        " to serve him" << endl << endl;
            //}
        } else {  
            // ���򣬽���ȴ��Ĺ˿�����
            ostr.str("");
            ostr << "He will wait for replan!" << endl << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();
            waitCustomerId.push_back(copyNewCustomer->id);  
            sort(waitCustomerId.begin(), waitCustomerId.end());
        }
    } else {
        promisedCustomerId.push_back(copyNewCustomer->id);  // ��Щ�˿�һ����õ�����
        sort(promisedCustomerId.begin(), promisedCustomerId.end());
        int selectedCarPos = insertPos.first;
        int refStoreID = insertPos.second.first;
        int refCustomerID = insertPos.second.second;
        char refStoreType = insertType.second.first;
        char refCustomerType = insertType.second.second;
        Car *selectedCar = currentPlan[selectedCarPos];
        Spot *refStore = getMappedNode(refStoreID, refStoreType, selectedCar);
        Spot *refCustomer = getMappedNode(refCustomerID, refCustomerType, selectedCar);
        try {
            selectedCar->insertAfter(refStore, refCustomer, 
                    copyNewCustomer->choice, copyNewCustomer, currentTime);
        } catch (exception &e) {
            cout << "current id: " << selectedCar->getRoute()->currentPos()->id << endl;
            cout << "refStoreType: " << refStore->type << " refCustomerType: "
                << refCustomer->type << endl;
            cout << e.what() << endl;
            exit(1);
        }
        if(selectedCar->getState() == wait) {  // if the car stays asleep
            newEvent = selectedCar->launchCar(currentTime);
            cout << "launch Car #" << selectedCar->getCarIndex() << endl; 
        } else {
            newEvent = selectedCar->getCurrentAction(currentTime);
        }
        int carIndex = selectedCar->getCarIndex();
        try {
            selectedCar->getRoute()->checkArrivedTime();
        } catch (exception &e) {
            cout << "Car #" << carIndex << " invalid!!!" << endl;
            exit(1);
        }
        ostr.str("");
        ostr << "He is arranged to Car #" << carIndex << endl;
        ostr << "refStore: #" << refStore->id << " refCustomer: #" <<
            refCustomer->id << endl << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
    }
    return newEvent;
}

EventElement Dispatcher::handleCarArrived(float time, int carIndex){                 
    // ������������¼�
    // �ڴ˴����¹˿͵ĵ���ʱ��
    ostringstream ostr;
    vector<Car*>::iterator carIter;
    int pos;  // carIndex��Ӧ�ĳ�����currentPlan�е�λ��
    for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
        if((*carIter)->getCarIndex() == carIndex) {
            pos = carIter - currentPlan.begin();
            break;
        }
    }
    EventElement tempEvent = currentPlan[pos]->getCurrentAction(time);
    if(currentPlan[pos]->getState() == offwork) {
        // �ճ�
        ostr.str("");
        ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex() 
            << " finished its task!" << endl << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
        carFinishTask(tempEvent.carIndex);
    } else {
        // ����newservedCustomerId�Լ�promisedCustomerId
        Spot *currentPos = currentPlan[pos]->getCurrentNode();
        int currentId = currentPos->id;
        if(currentPos->type == 'C'){
            vector<int>::iterator intIter = find(promisedCustomerId.begin(), 
                promisedCustomerId.end(), currentId);
            promisedCustomerId.erase(intIter);
            servedCustomerId.push_back(currentId);
            sort(servedCustomerId.begin(), servedCustomerId.end());
            ostr.str("");
            ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex() 
                << " arrives at customer #" << currentId << endl << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();
        } else {
            ostr.str("");
            ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex()
                << " arrives at store #" << currentId << endl << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();
        }
    }
    return tempEvent;
}

EventElement Dispatcher::handleFinishedService(float time, int carIndex){       
    // ���������ɷ����¼�
    ostringstream ostr;
    EventElement newEvent(-1, carArrived, -1, -1);     // ��Ч�¼�
    vector<Car*>::iterator carIter;
    int pos;  // carIndex��Ӧ�ĳ�����currentPlan�е�λ��
    for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
        if((*carIter)->getCarIndex() == carIndex) {
            pos = carIter - currentPlan.begin();
            break;
        }
    }
    newEvent = currentPlan[pos]->getCurrentAction(time);
    try {
        currentPlan[pos]->getRoute()->checkArrivedTime();
    } catch (exception &e) {
        cout << "Car #" << carIndex << " seems wrong after providing service." << endl;
        exit(1);
    }
    Spot *currentNode = currentPlan[pos]->getCurrentNode();
    int currentId = currentNode->id;
    ostr.str("");
    if(currentNode->type == 'C') {
        ostr << "----Time " << time << ", Car #" << carIndex << 
            " finished service in customer #" << currentId << endl;
    } else {
        ostr << "----Time " << time << ", Car #" << carIndex << 
            " finished service in store #" << currentId << endl;
    }
    ostr << "Its end time for servce is " << currentPlan[pos]->getCurrentNode()->endTime 
        << endl << endl;
    TxtRecorder::addLine(ostr.str());
    cout << ostr.str();
    return newEvent;
}


