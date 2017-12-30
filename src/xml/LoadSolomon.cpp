#include "LoadSolomon.h"
#include "tinystr.h"
#include "tinyxml.h"
#include<iostream>

void getFloatFromChildNode(TiXmlHandle parent, string childName, float &value) {
    // ��ȡXML�ļ���parent�ڵ�����ΪchildName����Ϣ����ֵ��value
    TiXmlElement* elem = parent.FirstChild(childName.c_str()).Element();
    value = (float)atof(elem->GetText());
}

bool getData(string filename, vector<Customer*> &allCustomer, 
        Customer &depot, float &capacity){
    // ��ȡxml������allCustomers, depot, capacity��
    TiXmlDocument doc(filename.c_str());     // ����XML�ļ�
    if(!doc.LoadFile()) return false;    // ����޷���ȡ�ļ����򷵻�
    TiXmlHandle hDoc(&doc);         // hDoc��&docָ��Ķ���
    TiXmlElement* pElem;            // ָ��Ԫ�ص�ָ��
    pElem = hDoc.FirstChildElement().Element(); //ָ����ڵ�
    TiXmlHandle hRoot(pElem);       // hRoot�Ǹ��ڵ�
    Customer* customer;
    int tempINT;
    float tempFLOAT;

    // ��ȡx,y�����Ƿ���network->nodes->node�ڵ���
    TiXmlElement* nodeElem = hRoot.FirstChild("network").FirstChild("nodes").
        FirstChild("node").Element(); //��ǰָ����node�ڵ�
    for(nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement()) { // ������ȡnode�ڵ����Ϣ
        customer = new Customer;
        TiXmlHandle node(nodeElem);  // nodeElem��ָ��Ľڵ�
        nodeElem->QueryIntAttribute("id", &tempINT);  //��id�ŵ�temp1�У�����ֵ����
        if(tempINT == 0){  // depot�ڵ�
            depot.id = tempINT;
            depot.prop = 0;
            getFloatFromChildNode(node, "cx", depot.x);
            getFloatFromChildNode(node, "cy", depot.y);
            depot.type = 'D';
            depot.serviceTime = 0;
            depot.arrivedTime = 0;
            depot.startTime = 0;
            depot.priority = 0;
        } else {  // ȡ����
            customer = new Customer;
            customer->id = tempINT;  
            nodeElem->QueryIntAttribute("property", 0);  //�������еĹ˿Ͷ���static��
            customer->prop = tempINT;
            getFloatFromChildNode(node, "cx", depot.x);
            getFloatFromChildNode(node, "cy", depot.y);
            customer->type = 'P';
            customer->priority = 0;
            allCustomer.push_back(customer);
        }
    }

    // ��ȡ������Ϣ
    TiXmlElement* requestElem = hRoot.FirstChild("requests").
        FirstChild("request").Element();   // ָ����request�ڵ�
    int count = 0;
    for(requestElem; requestElem; requestElem = requestElem->NextSiblingElement()) {
        // ��ǰ�˿ͽڵ㣬ע�ⲻ�ܸ�ֵ��һ���µĶ��󣬷������ø��ƹ��캯��
        customer = allCustomer[count];     		
        TiXmlHandle request(requestElem);  // ָ��ָ��Ķ���
        TiXmlHandle twNode = request.FirstChild("tw");
        getFloatFromChildNode(twNode, "start", customer->startTime);
        getFloatFromChildNode(twNode, "end", customer->endTime);
        // quantity
        getFloatFromChildNode(request, "quantity", customer->quantity);
        // service time
        getFloatFromChildNode(request, "service_time", customer->serviceTime);
        TiXmlElement* serviceTimeElem = request.FirstChild("service_time").Element();
        count++;
    }

    // ��ȡcapacity
    TiXmlElement* capacityElem = hRoot.FirstChild("fleet").FirstChild("vehicle_profile").
        FirstChild("capacity").Element();
    tempFLOAT = (float)atof(capacityElem->GetText());
    capacity = tempFLOAT;
    return true;
}

