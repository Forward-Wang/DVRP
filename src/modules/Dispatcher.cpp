#include "Dispatcher.h"
#include<algorithm>
#include "Simulator.h"
#include "../public/PublicFunction.h"
#include "../run/TxtRecorder.h"
#include "../run/Config.h"
#include <stdexcept>

const float MAX_FLOAT = numeric_limits<float>::max();

Dispatcher::Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, 
        Customer depot, float capacity) {
    this->depot = depot;
    this->capacity = capacity;
    int custNum = staticCustomerSet.end() - staticCustomerSet.begin();
    custNum += dynamicCustomerSet.end() - dynamicCustomerSet.begin(); // 总顾客数
    servedCustomerId.reserve(custNum);     // 已经服务过的顾客id
    promisedCustomerId.reserve(custNum);
    waitCustomerId.reserve(custNum);
    rejectCustomerId.reserve(custNum);
    vector<Customer*>::iterator custIter = staticCustomerSet.begin();
    for(custIter; custIter < staticCustomerSet.end(); custIter++) {
        // 在计划开始前已经提出需求的顾客都属于promiseCustomer
        Customer* newCust = new Customer(**custIter);
        allCustomer.push_back(newCust);
        promisedCustomerId.push_back(newCust->id);
    }
    for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
        Customer* newCust = new Customer(**custIter);
        allCustomer.push_back(newCust);	
        dynamicCustomerId.push_back(newCust->id);
    }
    sort(allCustomer.begin(), allCustomer.end(), ascendSortForCustId);  // 按id进行递增排序
    sort(promisedCustomerId.begin(), promisedCustomerId.end());
    sort(dynamicCustomerId.begin(), dynamicCustomerId.end());
}


void Dispatcher::destroy() {
    deleteCustomerSet(allCustomer);
}

void Dispatcher::carFinishTask(int carIndex){       
    // 收车
    // 我们会从currentPlan中删除id为carIndex的顾客
    // 并且将其加入到finishedPlan
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
        // 如果所有剩余的车辆都处于wait状态，则也算是完成了任务
        bool mark = true;
        int num = 0;
        for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
            if((*carIter)->getState() != wait) {
                mark = false;
                break;
            } else {
                num++;
            }
        }
        if(mark == true) {
            ostr.str("");
            ostr << "=============== END ================" << endl;
            ostr << "----There are " << num << " cars remains unused!" << endl;
            ostr << "----" << servedCustomerId.size() << " customers get served finally" << endl;
            ostr << "----" << rejectCustomerId.size() << " customers get rejected finally" << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();	
        }
    }
}

void checkFeasible(vector<Car*> carSet, vector<int> promisedCustomerId){
    // 判断promiseCustomerSet中的顾客是否都在carSet中
    vector<int> tempId = promisedCustomerId;  // 得到了promise的顾客id
    sort(tempId.begin(), tempId.end());
    vector<Car*>::iterator carIter;
    for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {  
        vector<Customer*> tempCust = (*carIter)->getAllCustomer();
        vector<Customer*>::iterator custIter;
        for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
            vector<int>::iterator intIter = find(tempId.begin(), tempId.end(), (*custIter)->id);
            if(intIter < tempId.end()) {
                // 如果找到了，就删掉
                tempId.erase(intIter);
            }
        }
        deleteCustomerSet(tempCust);
    }
    if(tempId.size() != 0) {   // if there are promiseCustomers excluded
        throw out_of_range("Not all promise customers are in the car set!");
    }
}


vector<EventElement> Dispatcher::handleNewTimeSlot(int slotIndex){ 
    // 新时间段开始
    vector<Customer*> promiseCustomerSet;
    vector<Customer*> waitCustomerSet;
    vector<Customer*> dynamicCustomerSet;
    vector<int>::iterator custIdIter;
    vector<Car*>::iterator carIter;
    vector<Car*> updatedPlan;
    for(custIdIter = promisedCustomerId.begin(); custIdIter< promisedCustomerId.end(); custIdIter++) {
        promiseCustomerSet.push_back(allCustomer[*custIdIter - 1]);  // id从1开始编号
    }
    for(custIdIter = dynamicCustomerId.begin(); custIdIter < dynamicCustomerId.end(); custIdIter++) {
        dynamicCustomerSet.push_back(allCustomer[*custIdIter - 1]);
    }
    vector<EventElement> newEventList;
    ostringstream ostr;
    if(slotIndex == 0) {  // 路径计划需要初始化
        ostr.str("");
        ostr << "============ Now Initialize the routing plan ===========" << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
        Simulator smu(slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, currentPlan);
        updatedPlan = smu.initialPlan(depot, capacity);
        currentPlan = copyPlan(updatedPlan);
        withdrawPlan(updatedPlan);
        vector<Car*>::iterator carIter;
        for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
            EventElement newEvent = (*carIter)->launchCar(0);  // 将车辆发动
            newEventList.push_back(newEvent);
        }
        ostr.str("");
        ostr << "----Initialization Finished! Now there are " << currentPlan.size() 
            << " cars dispatched!" << endl << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
    } else {
        // 调取每辆车未走过的路径进行仿真，得到新的路径计划
        ostr.str("");
        ostr << "============ Now replan, the time slot is: " << slotIndex << "============" << endl;
        TxtRecorder::addLine(ostr.str());
        cout << ostr.str();
        float currentTime = slotIndex * REPLAN_END_TIME / TIME_SLOT_NUM / SPLIT;
        for(custIdIter = waitCustomerId.begin(); custIdIter < waitCustomerId.end(); custIdIter++) {
            Customer *temp = new Customer;
            *temp = *allCustomer[*custIdIter - 1];
            waitCustomerSet.push_back(temp);
        }
        vector<Car*> futurePlan;
        for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
            // 抓取车子还没有走过的计划
            Car *tempCar = new Car((*carIter)->capturePartRoute(currentTime));
            futurePlan.push_back(tempCar);
        }
        if (currentPlan.size() != 0 && REPLAN) {  // 有货车可派时，才进行replan
            Simulator smu(slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, 
                    futurePlan);
            vector<int> newservedCustomerId;
            vector<int> newAbandonedCustomerId;
            vector<int> delayCustomerId;
            updatedPlan = smu.replan(newservedCustomerId, newAbandonedCustomerId, 
                    delayCustomerId, capacity);
            withdrawPlan(futurePlan);
            vector<Customer*>::iterator custIter;

            // 更新promiseCustomerId, rejectCustomerId以及waitCustomerId
            vector<int>::iterator intIter;
            // tempVec: 存放new served以及new abandoned顾客
            //          将原来的wait Customer和tempVec作差即可得到最新的wait Customer
            vector<int> tempVec;
            tempVec.insert(tempVec.end(), newservedCustomerId.begin(), newservedCustomerId.end());
            // newServedCustomer将会成为新的promisedCustomer
            promisedCustomerId.insert(promisedCustomerId.end(), newservedCustomerId.begin(), newservedCustomerId.end());
            sort(promisedCustomerId.begin(), promisedCustomerId.end());
            
            tempVec.insert(tempVec.end(), newAbandonedCustomerId.begin(), newAbandonedCustomerId.end());
            rejectCustomerId.insert(rejectCustomerId.end(), newAbandonedCustomerId.begin(), newAbandonedCustomerId.end());
            sort(rejectCustomerId.begin(), rejectCustomerId.end());
            
            // 检查是否tempVec的元素都在原来的wait Customer中
            for(intIter = tempVec.begin(); intIter < tempVec.end(); intIter++) {
                vector<int>::iterator intIter2 = find(waitCustomerId.begin(), waitCustomerId.end(), 
                        *intIter);
                if(intIter2 == waitCustomerId.end()) { 
                    // 没有找到，报错
                    throw out_of_range("tempVec not totally in waitCustomerId!!");
                }   
            }

            if(tempVec.size() != 0) {
                // 提取最新的waitCustomerId
                sort(waitCustomerId.begin(), waitCustomerId.end());
                sort(tempVec.begin(), tempVec.end());
                vector<int> tempVec2(waitCustomerId.size());
                vector<int>::iterator iterxx = set_difference(waitCustomerId.begin(), 
                        waitCustomerId.end(), tempVec.begin(), tempVec.end(), tempVec2.begin());
                tempVec2.resize(iterxx - tempVec2.begin());
                waitCustomerId = tempVec2;
            }
            
            if(slotIndex >= TIME_SLOT_NUM * SPLIT) {
                // 后面已经没有replan
                ostr.str("");
                ostr << "No replan after current time, reject all waiting customer, the number is:" << waitCustomerId.size() << endl;
                TxtRecorder::addLine(ostr.str());
                cout << ostr.str();
                rejectCustomerId.insert(rejectCustomerId.end(), waitCustomerId.begin(), 
                        waitCustomerId.end());
                waitCustomerId.clear();
            }
            
            // 将变更后的future plan安插到currentPlan对应位置之后
            int count = 0;
            for (carIter = updatedPlan.begin(); carIter < updatedPlan.end(); carIter++) {
                currentPlan[count]->replaceRoute(**carIter, currentTime);
                EventElement newEvent;
                if (currentPlan[count]->getState() == wait) {
                    // 如果货车原来处于wait状态，则需要将其发动
                    newEvent = currentPlan[count]->launchCar(currentTime);
                }
                else {
                    newEvent = currentPlan[count]->getCurrentAction(currentTime);
                }
                newEventList.push_back(newEvent);
                count++;
            }
            withdrawPlan(updatedPlan);
            deleteCustomerSet(waitCustomerSet);
            ostr.str("");
            ostr << "----Replan Finished! Now there are " << currentPlan.size() 
                << " cars working!" << endl << endl;
            TxtRecorder::addLine(ostr.str());
            cout << ostr.str();
        }
        else {
            if(currentPlan.size() == 0) {
                ostr.str("");
                ostr << "----no car is applicable!!!" << endl << endl;
                TxtRecorder::addLine(ostr.str());
                cout << ostr.str();
            } else {
                if(!REPLAN) {
                    ostr.str("");
                    ostr << "----Not use replan!!" << endl << endl;
                    TxtRecorder::addLine(ostr.str());
                    cout << ostr.str();
                }
            }
        }
    }
    return newEventList;
} 

EventElement Dispatcher::handleNewCustomer(int slotIndex, const Customer& newCustomer){  
    // 处理新顾客到达
    ostringstream ostr;
    ostr.str("");
    ostr<< "----Customer with id #" << newCustomer.id << " is arriving..." << endl;
    TxtRecorder::addLine(ostr.str());
    if(SHOW_DETAIL) cout << ostr.str();
    vector<int>::iterator intIter = find(dynamicCustomerId.begin(), dynamicCustomerId.end(), 
            newCustomer.id);
    dynamicCustomerId.erase(intIter);
    float minInsertCost = MAX_FLOAT;
    // 第一个int是货车编号（于currentPlan中的位置），第二个Customer是插入点前面的顾客
    pair<int, Customer> insertPos;   
    vector<Car*>::iterator carIter;
    float currentTime = newCustomer.startTime;       // 顾客提出需求的时间正好是时间窗开始的时间
    for (carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
        // 求newCustomer在每条route的最小插入代价
        Car tempCar = (*carIter)->capturePartRoute(currentTime);
        Customer customer1, customer2;
        float minValue, secondValue;
        tempCar.computeInsertCost(newCustomer, minValue, customer1, secondValue, customer2);
        if(minValue < minInsertCost) {
            int pos = carIter - currentPlan.begin();  
            minInsertCost = minValue;
            insertPos = make_pair(pos, customer1);
        }
    }
    EventElement newEvent;
    if(minInsertCost == MAX_FLOAT) {
        // 没有可行插入点
        if(newCustomer.tolerantTime < slotIndex * REPLAN_END_TIME/TIME_SLOT_NUM/SPLIT || 
                !REPLAN || currentTime > REPLAN_END_TIME) { 
            // 等不到replan，则reject
            ostr.str("");
            ostr << "He is rejected!" << endl;
            ostr << "His tolerance time is " << newCustomer.tolerantTime << endl;
            ostr << endl;
            TxtRecorder::addLine(ostr.str());
            if(SHOW_DETAIL) cout << ostr.str();
            rejectCustomerId.push_back(newCustomer.id);
        } else {  // 否则，进入等待的顾客序列
            ostr.str("");
            ostr << "He will wait for replan!" << endl << endl;
            TxtRecorder::addLine(ostr.str());
            if(SHOW_DETAIL) cout << ostr.str();
            waitCustomerId.push_back(newCustomer.id);  
            sort(waitCustomerId.begin(), waitCustomerId.end());
        }
    } else {
        promisedCustomerId.push_back(newCustomer.id);  // 这些顾客一定会得到服务
        sort(promisedCustomerId.begin(), promisedCustomerId.end());
        int selectedCarPos = insertPos.first;
        Customer selectedCustomer = insertPos.second;
        try {
            currentPlan[selectedCarPos]->insertAfter(selectedCustomer, newCustomer,
                    currentTime);
        } catch (exception &e) {
            throw out_of_range(e.what());
        }
        if(currentPlan[selectedCarPos]->getState() == wait) {  // if the car stays asleep
            newEvent = currentPlan[selectedCarPos]->launchCar(currentTime);
        } else {
            newEvent = currentPlan[selectedCarPos]->getCurrentAction(currentTime);
        }
        int carIndex = currentPlan[selectedCarPos]->getCarIndex();
        ostr.str("");
        ostr << "He is arranged to car #" << carIndex << endl << endl;
        TxtRecorder::addLine(ostr.str());
        if(SHOW_DETAIL) cout << ostr.str();
    }
    return newEvent;
}

EventElement Dispatcher::handleCarArrived(float time, int carIndex){                 
    // 处理货车到达事件
    // 在此处更新顾客的到达时间
    ostringstream ostr;
    vector<Car*>::iterator carIter;
    int pos;  // carIndex对应的车辆在currentPlan中的位置
    for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
        if((*carIter)->getCarIndex() == carIndex) {
            pos = carIter - currentPlan.begin();
            break;
        }
    }
    EventElement tempEvent = currentPlan[pos]->getCurrentAction(time);
    if(currentPlan[pos]->getState() == offwork) {
        // 收车
        ostr.str("");
        ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex() 
            << " finished its task!" << endl << endl;
        TxtRecorder::addLine(ostr.str());
        if(SHOW_DETAIL) cout << ostr.str();
        carFinishTask(tempEvent.carIndex);
    } else {
        // 更新newservedCustomerId以及promisedCustomerId
        int currentId = currentPlan[pos]->getCurrentNode().id;
        vector<int>::iterator intIter = find(promisedCustomerId.begin(), 
                promisedCustomerId.end(), currentId);
        promisedCustomerId.erase(intIter);
        servedCustomerId.push_back(currentId);
        sort(servedCustomerId.begin(), servedCustomerId.end());
        ostr.str("");
        ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex() 
            << " arrives at customer #" << currentId << endl << endl;
        TxtRecorder::addLine(ostr.str());
        if(SHOW_DETAIL) cout << ostr.str();

    }
    return tempEvent;
}

EventElement Dispatcher::handleFinishedService(float time, int carIndex){       
    // 处理货车完成服务事件
    ostringstream ostr;
    EventElement newEvent(-1, carArrived, -1, -1);     // 无效事件
    vector<Car*>::iterator carIter;
    int pos;  // carIndex对应的车辆在currentPlan中的位置
    for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
        if((*carIter)->getCarIndex() == carIndex) {
            pos = carIter - currentPlan.begin();
            break;
        }
    }
    newEvent = currentPlan[pos]->getCurrentAction(time);
    int currentId = currentPlan[pos]->getCurrentNode().id;
    ostr.str("");
    ostr << "----Time " << time << ", car #" << carIndex << 
        " finished service in customer #" << currentId << endl;
    ostr << "Its end time for servce is " << currentPlan[pos]->getCurrentNode().endTime 
        << endl << endl;;
    TxtRecorder::addLine(ostr.str());
    if(SHOW_DETAIL) cout << ostr.str();
    return newEvent;
}


