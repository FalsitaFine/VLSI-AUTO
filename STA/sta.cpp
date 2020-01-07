#include <iostream> 
#include <fstream> 
#include <sstream>  
#include <stdio.h>  
#include <string.h>

#include <vector> 
#include <list> 
#include <iterator> 
#include <array>
#include <tuple>
#include <map> 
#include <queue>

#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <time.h>

using namespace std; 
double lutlookup(string gatetype, double index_1, double index_2, string outtype);

//Struct of Gate
struct Gate
{
    string name;

    double capacitance = 0;

    double delay_input_slew_index[7];
    double delay_output_load_index[7];
    double slew_input_slew_index[7];
    double slew_output_load_index[7];

    double cell_delays[7][7];
    double output_slew[7][7];
};


struct Node
{
    string name;
    string type = "DEFAULT(NOT DEFINED)";
    string secondaryType = "NORMAL";
    vector<int> fanouts;
    vector<int> fanins;

    double delay;
    double slew;

    double arrivalTimeOut;
    double outputSlewOut;

    int inDegree = 0;
    int inDegreeRemain = 0;

    int loadCapacitanceflag = 0;
    double loadCapacitance = 0;
    double requiredArrivalTime = 0;
    double slack = 0;

    int outDegree = 0;
    int outDegreeRemain = 0;

    int visited = 0;

};

list <int> loadlist;
vector<Node> circuitNodes; 
vector<list<int> > circuitVector; 
map<int, string> numbertotype; 
map<string, Gate> typetogate; 
int circuitSize = 1000;
queue <int> front;
queue <int> frontBack;
queue <int> outGates;

double totalDelay = 0;
double requiredTime = 0;
list <int> criticalPath;

//From Canvas
struct ParenCommaEq_is_space : std::ctype<char>{
ParenCommaEq_is_space():std::ctype<char>(get_table()){}
static mask const* get_table()
{
static mask rc[table_size];
rc['(']=std::ctype_base::space;
rc[')']=std::ctype_base::space;
rc[',']=std::ctype_base::space;
rc['=']=std::ctype_base::space;
rc[' ']=std::ctype_base::space;
rc['\t']=std::ctype_base::space;
rc['\r']=std::ctype_base::space;

return &rc[0];
}
};


double forwardTraversal(){
    int popGate = 0;
    int popCount = 0;
    double tempDelay = 0;
    double tempSlew = 0;
    while(!front.empty()){
        popGate = front.front();
        front.pop();
        popCount++;
        //cout << popGate << endl;


        for(int i=0; i<circuitNodes.at(popGate).outDegree;++i){
            circuitNodes.at(popGate).loadCapacitance += typetogate[circuitNodes.at(circuitNodes.at(popGate).fanouts[i]).type].capacitance;
            circuitNodes.at(circuitNodes.at(popGate).fanouts[i]).inDegreeRemain--;
            
            /*
            cout<<"FANOUT "<<circuitNodes.at(popGate).fanouts[i]<< " TYPE "<<circuitNodes.at(circuitNodes.at(popGate).fanouts[i]).type<< " REMAIN INDEGREE " << circuitNodes.at(circuitNodes.at(popGate).fanouts[i]).inDegreeRemain << endl;
            
            if(circuitNodes.at((circuitNodes.at(popGate).fanouts[i])).secondaryType == "OUTPUT"){
                cout << "THIS ONE IS OUTPUT" << endl;
            }
            */

            if(circuitNodes.at((circuitNodes.at(popGate).fanouts[i])).inDegreeRemain == 0){
                front.push(circuitNodes.at(popGate).fanouts[i]);
                circuitNodes.at((circuitNodes.at(popGate).fanouts[i])).visited = 1;
            }
        }

        if (circuitNodes.at(popGate).secondaryType == "OUTPUT" && circuitNodes.at(popGate).outDegree == 0){
            //cout <<"SIMP OUT" << endl;
            circuitNodes.at(popGate).loadCapacitance = typetogate["INV"].capacitance * 4.0; //The load capacitance of the output gates is equal to four times the capacitance of an inverter
        }

        tempDelay = 0;
        tempSlew = 0;


        //COMPUTE DELAY AND SLEW
        if (circuitNodes.at(popGate).type != "INPUT"){
            for(int j=0; j< circuitNodes.at(popGate).inDegree; ++j){
                if (circuitNodes.at(popGate).inDegree > 2){
                    tempDelay = lutlookup(circuitNodes.at(popGate).type, circuitNodes.at(circuitNodes.at(popGate).fanins[j]).outputSlewOut ,circuitNodes.at(popGate).loadCapacitance, "DELAY") * double(circuitNodes.at(popGate).inDegree) / 2.0;
                    tempSlew = lutlookup(circuitNodes.at(popGate).type, circuitNodes.at(circuitNodes.at(popGate).fanins[j]).outputSlewOut ,circuitNodes.at(popGate).loadCapacitance, "SLEW") * double(circuitNodes.at(popGate).inDegree) / 2.0;
                    //tempDelay = lutlookup(circuitNodes.at(popGate).type, circuitNodes.at(circuitNodes.at(popGate).fanins[j]).outputSlewOut  * circuitNodes.at(popGate).inDegree / 2,circuitNodes.at(popGate).loadCapacitance * circuitNodes.at(popGate).inDegree / 2, "DELAY");
                    //tempSlew = lutlookup(circuitNodes.at(popGate).type, circuitNodes.at(circuitNodes.at(popGate).fanins[j]).outputSlewOut * circuitNodes.at(popGate).inDegree / 2 ,circuitNodes.at(popGate).loadCapacitance * circuitNodes.at(popGate).inDegree / 2, "SLEW");

                }else{
                    tempDelay = lutlookup(circuitNodes.at(popGate).type, circuitNodes.at(circuitNodes.at(popGate).fanins[j]).outputSlewOut ,circuitNodes.at(popGate).loadCapacitance, "DELAY");
                    tempSlew = lutlookup(circuitNodes.at(popGate).type, circuitNodes.at(circuitNodes.at(popGate).fanins[j]).outputSlewOut ,circuitNodes.at(popGate).loadCapacitance, "SLEW");
                }
                //cout << tempDelay << ", " <<tempSlew<<endl;
                if (circuitNodes.at(popGate).arrivalTimeOut <= circuitNodes.at(circuitNodes.at(popGate).fanins[j]).arrivalTimeOut + tempDelay){
                    circuitNodes.at(popGate).arrivalTimeOut = circuitNodes.at(circuitNodes.at(popGate).fanins[j]).arrivalTimeOut + tempDelay;
                    circuitNodes.at(popGate).outputSlewOut = tempSlew;

                }

            }
        }else{
                circuitNodes.at(popGate).outputSlewOut = 0.002;
                circuitNodes.at(popGate).arrivalTimeOut = 0;

                //tempDelay = lutlookup(circuitNodes.at(popGate).type, 0.002 ,circuitNodes.at(popGate).loadCapacitance, "DELAY");
                //circuitNodes.at(popGate).arrivalTimeOut = tempDelay;
                //circuitNodes.at(popGate).arrivalTimeOut = circuitNodes.at(circuitNodes.at(popGate).fanins[j]).arrivalTimeOut + tempDelay;
                //circuitNodes.at(popGate).outputSlewOut = tempSlew;
        }
        if(circuitNodes.at(popGate).arrivalTimeOut > totalDelay){
            totalDelay = circuitNodes.at(popGate).arrivalTimeOut;
        }
        circuitNodes.at(popGate).delay = tempDelay;

        //cout << "LOAD CAPACITANCE, arrivalTimeOut and outputSlew OF GATE " << popGate <<" "<< circuitNodes.at(popGate).type<< " "<<circuitNodes.at(popGate).secondaryType<< " : "<< circuitNodes.at(popGate).loadCapacitance <<", "<< circuitNodes.at(popGate).arrivalTimeOut<<", "<<circuitNodes.at(popGate).outputSlewOut<<endl;


    }
//cout <<"TOTAL TRAVERSED GATE: " << popCount <<endl;
//totalDelay = totalDelay * 1.1;
return totalDelay;

}


double backTraversal(){
    ofstream fpOut;
    fpOut.open("./ckt_traversal.txt", ios::out | ios::app);
    int popGate = 0;
    int popCount = 0;
    double minRequired = 9999;
    double currentRequired = 0;
    while(!frontBack.empty()){
        popGate = frontBack.front();
        frontBack.pop();
        popCount++;
        minRequired = 9999;
        currentRequired = 0;
        //cout << popGate << endl;
        if (circuitNodes.at(popGate).secondaryType == "OUTPUT"){
            circuitNodes.at(popGate).requiredArrivalTime = requiredTime;
        }else{
            for(int i=0; i<circuitNodes.at(popGate).outDegree; ++i){
                currentRequired = circuitNodes.at(circuitNodes.at(popGate).fanouts[i]).requiredArrivalTime - circuitNodes.at(circuitNodes.at(popGate).fanouts[i]).delay;
                if (currentRequired < minRequired){
                    minRequired = currentRequired;
                }
            }
            circuitNodes.at(popGate).requiredArrivalTime = minRequired;
        }

        for(int i=0; i<circuitNodes.at(popGate).inDegree;++i){
            circuitNodes.at(circuitNodes.at(popGate).fanins[i]).outDegreeRemain--;
            //cout<<"FANIN "<<circuitNodes.at(popGate).fanins[i]<< " TYPE "<<circuitNodes.at(circuitNodes.at(popGate).fanins[i]).type<< " REMAIN OUTDEGREE " << circuitNodes.at(circuitNodes.at(popGate).fanins[i]).outDegreeRemain << endl;
            
            if(circuitNodes.at((circuitNodes.at(popGate).fanins[i])).type == "INPUT"){
                //cout << "THIS ONE IS INPUT" << endl;
            }
            if(circuitNodes.at((circuitNodes.at(popGate).fanins[i])).outDegreeRemain == 0){
                frontBack.push(circuitNodes.at(popGate).fanins[i]);
            }
        }

        circuitNodes.at(popGate).slack =   circuitNodes.at(popGate).requiredArrivalTime - circuitNodes.at(popGate).arrivalTimeOut;

        //cout << "arrival, required and slack OF GATE " << popGate <<" "<< circuitNodes.at(popGate).secondaryType<<" "<< circuitNodes.at(popGate).arrivalTimeOut<< " "<<circuitNodes.at(popGate).requiredArrivalTime<< " "<< circuitNodes.at(popGate).slack<<endl;
        if (circuitNodes.at(popGate).secondaryType != "OUTPUT"){
         fpOut << circuitNodes.at(popGate).type << "-" << popGate <<": " << circuitNodes.at(popGate).slack * 1000 <<"ps" << endl;
         //fpOut << circuitNodes.at(popGate).type << "-" << popGate <<": " << circuitNodes.at(popGate).slack * 1000 <<"ps" << "---DeBUG " <<circuitNodes.at(popGate).arrivalTimeOut<<"-"<<circuitNodes.at(popGate).inDegree<<"-" <<circuitNodes.at(popGate).outDegree<<endl;
        
         //for(int i=0; i< circuitNodes.at(popGate).inDegree;++i){
         //   fpOut << circuitNodes.at(popGate).fanins[i] << endl;
         //}

        }else{
         fpOut << circuitNodes.at(popGate).secondaryType<<"-" << circuitNodes.at(popGate).type << "-" << popGate <<": " << circuitNodes.at(popGate).slack * 1000 <<"ps" <<endl;
         //fpOut << circuitNodes.at(popGate).secondaryType<<"-" << circuitNodes.at(popGate).type << "-" << popGate <<": " << circuitNodes.at(popGate).slack * 1000 <<"ps" << "---DeBUG " <<circuitNodes.at(popGate).arrivalTimeOut<<"-"<<circuitNodes.at(popGate).inDegree<<"-" <<circuitNodes.at(popGate).outDegree<<endl;
        
         //for(int i=0; i< circuitNodes.at(popGate).inDegree;++i){
         //   fpOut << circuitNodes.at(popGate).fanins[i] << endl;
         // }

        }

    }
//cout <<"TOTAL TRAVERSED GATE: " << popCount <<endl;
fpOut.close();
}

void findCritical(){
    srand(time(NULL));
    int popGate = 0;
    int currentGate = 0;
    int randomValue = 0;
    int nextGate = 0;
    double minSlack = 9999;
    while(!outGates.empty()){
        popGate = outGates.front();
        outGates.pop();
        if (circuitNodes.at(popGate).slack < minSlack){
            minSlack = circuitNodes.at(popGate).slack;
            currentGate = popGate;
        }
        else if (circuitNodes.at(popGate).slack == minSlack){ //Have several same output, choose one randomly
            randomValue = rand() % 10;
            if (randomValue >= 5){ 
            minSlack = circuitNodes.at(popGate).slack;
            currentGate = popGate;
        }
        }
    }
    //cout << "CRITICAL->" << currentGate << " WITH SLACK "<< minSlack <<endl;
    criticalPath.push_back(currentGate);
    nextGate = currentGate;
    while(circuitNodes.at(currentGate).type!="INPUT"){
        minSlack = 9999;
        for(int i=0; i<circuitNodes.at(currentGate).inDegree; ++i){
            if (circuitNodes.at(circuitNodes.at(currentGate).fanins[i]).slack < minSlack){
                minSlack = circuitNodes.at(circuitNodes.at(currentGate).fanins[i]).slack;
                nextGate = circuitNodes.at(currentGate).fanins[i];
            }
        }
        currentGate = nextGate;
    //cout << "CRITICAL->" << currentGate << " WITH SLACK "<< minSlack <<endl;
    criticalPath.push_back(currentGate);
    }

}



double lutlookup(string gatetype, double index_1, double index_2, string outtype){
    int index1flag = 0;
    int index2flag = 0;
    int index1flagl = 0;
    int index2flagl = 0;
    int index1_l = 0;
    int index1_s = 0;
    int index1_e = 0;

    int index2_l = 0;
    int index2_s = 0;
    int index2_e = 0;   
   
if (outtype == "DELAY"){
    for(int i = 0; i < 7; i++){
        if (index_1 == typetogate[gatetype].delay_input_slew_index[i]){
            index1flag = 1;
            index1_e = i;
        }
        if (index_2 == typetogate[gatetype].delay_output_load_index[i]){
            index2flag = 1;
            index2_e = i;
        }
        if (index_1 >= typetogate[gatetype].delay_input_slew_index[i]){
            index1_s = i;
        }
        if (index_2 >= typetogate[gatetype].delay_output_load_index[i]){
            index2_s = i;
        }
        if (index_1 < typetogate[gatetype].delay_input_slew_index[i] && index1flagl == 0){
            index1flagl = 1;
            index1_l = i;
        }
        if (index_2 < typetogate[gatetype].delay_output_load_index[i] && index2flagl == 0){
            index2flagl = 1;
            index2_l = i;
        }
    }

    if (index1flag == 1 && index2flag == 1){
        return typetogate[gatetype].cell_delays[index1_e][index2_e];
    }else{

        double v11 = typetogate[gatetype].cell_delays[index1_s][index2_s];
        double v12 = typetogate[gatetype].cell_delays[index1_s][index2_l];
        double v21 = typetogate[gatetype].cell_delays[index1_l][index2_s];
        double v22 = typetogate[gatetype].cell_delays[index1_l][index2_l];
        double t = index_1;
        double t1 = typetogate[gatetype].delay_input_slew_index[index1_s];
        double t2 = typetogate[gatetype].delay_input_slew_index[index1_l];
        double C = index_2;
        double C1 = typetogate[gatetype].delay_output_load_index[index2_s];
        double C2 = typetogate[gatetype].delay_output_load_index[index2_l];

        double tempResult = 0;
        tempResult = (v11*(C2-C)*(t2-t) + v12*(C-C1)*(t2-t) + v21*(C2-C)*(t-t1) + v22*(C-C1)*(t-t1))/((C2-C1)*(t2-t1));
        //cout <<"RETURNED DELAY: " << tempResult <<endl;

        return tempResult;

    }
}else { // SLEW
    for(int i = 0; i < 7; i++){
        if (index_1 == typetogate[gatetype].slew_input_slew_index[i]){
            index1flag = 1;
            index1_e = i;
        }
        if (index_2 == typetogate[gatetype].slew_output_load_index[i]){
            index2flag = 1;
            index2_e = i;
        }
        if (index_1 > typetogate[gatetype].slew_input_slew_index[i]){
            index1_s = i;
        }
        if (index_2 > typetogate[gatetype].slew_output_load_index[i]){
            index2_s = i;
        }
        if (index_1 < typetogate[gatetype].slew_input_slew_index[i] && index1flagl == 0){
            index1flagl = 1;
            index1_l = i;
        }
        if (index_2 < typetogate[gatetype].slew_output_load_index[i] && index2flagl == 0){
            index2flagl = 1;
            index2_l = i;
        }
    }

    if (index1flag == 1 && index2flag == 1){
        return typetogate[gatetype].output_slew[index1_e][index2_e];
    }else{

        double v11 = typetogate[gatetype].output_slew[index1_s][index2_s];
        double v12 = typetogate[gatetype].output_slew[index1_s][index2_l];
        double v21 = typetogate[gatetype].output_slew[index1_l][index2_s];
        double v22 = typetogate[gatetype].output_slew[index1_l][index2_l];
        double t = index_1;
        double t1 = typetogate[gatetype].slew_input_slew_index [index1_s];
        double t2 = typetogate[gatetype].slew_input_slew_index[index1_l];
        double C = index_2;
        double C1 = typetogate[gatetype].slew_output_load_index[index2_s];
        double C2 = typetogate[gatetype].slew_output_load_index[index2_l];

        double tempResult = 0;
        tempResult = (v11*(C2-C)*(t2-t) + v12*(C-C1)*(t2-t) + v21*(C2-C)*(t-t1) + v22*(C-C1)*(t-t1))/((C2-C1)*(t2-t1));
        //cout <<"RETURNED SLEW: " << tempResult <<endl;
        return tempResult;

    }

}

}


//Reduce the useless part in a string
void reduce(char* str,char ch)
{
    char *p = str;
    char *q = str;
    while(*q)
    {
        if (*q !=ch)
        {
            *p++ = *q;
        }
        q++;
    }
    *p='\0';
}


//Give output
void generateOutput(int gateToShow)
{
ofstream fpOut;
fpOut.open("./cuixx327.txt", ios::out | ios::app);

auto itr = numbertotype.find(gateToShow);
auto itrv = typetogate.find("DEFAULT(NOT DEFIND)");
string typeOut = "";

if(itr!= numbertotype.end()){
    typeOut = numbertotype[gateToShow];
    itrv = typetogate.find(typeOut);
    //cout<< typeOut << endl;
}
else{
    cout << "WRONG GATE #" << gateToShow <<" : THIS GATE# CANNOT BE FOUND IN THE CIRCUIT" << endl;
    fpOut<< "WRONG GATE #" << gateToShow <<" : THIS GATE# CANNOT BE FOUND IN THE CIRCUIT" << endl;
    exit(0);
}
    

    //cout<<(typeOut.compare("INPUT")==0)<<endl;

    if (typeOut.compare("INPUT")==0 || typeOut.compare("OUTPUT")==0){
            cout << gateToShow << " " << numbertotype[gateToShow] << endl;
            fpOut << gateToShow << " " << numbertotype[gateToShow] << endl;
    }else if(itrv != typetogate.end())
    {
            cout <<  gateToShow << " " << circuitNodes.at(gateToShow).type <<  " " << circuitNodes.at(gateToShow).fanins.front()  << " " <<  circuitNodes.at(gateToShow).fanins.back() << " " << typetogate[numbertotype[gateToShow]].output_slew[2][1]<<endl;
            fpOut <<  gateToShow << " " << numbertotype[gateToShow] <<  " " << circuitNodes.at(gateToShow).fanins.front()  << " " <<  circuitNodes.at(gateToShow).fanins.back() << " " << typetogate[numbertotype[gateToShow]].output_slew[2][1]<<endl;

    }
    else{
            cout <<  gateToShow <<  " " << numbertotype[gateToShow] << " " << circuitNodes.at(gateToShow).fanins.front()  << " " <<  circuitNodes.at(gateToShow).fanins.back() << " " << typetogate["DEFAULT(NOT DEFINED)"].output_slew[2][1]<< " NOTE: THIS TYPE IS NOT DEFINED IN THE LIBRARY " << endl;  
            fpOut <<  gateToShow <<  " " << numbertotype[gateToShow] << " " << circuitNodes.at(gateToShow).fanins.front()  << " " <<  circuitNodes.at(gateToShow).fanins.back() << " " << typetogate["DEFAULT(NOT DEFINED)"].output_slew[2][1]<< " NOTE: THIS TYPE IS NOT DEFINED IN THE LIBRARY " << endl;  

    }

cout<<"DEGREE: "<< circuitNodes.at(gateToShow).outDegree<<endl;
    for(int i = 0; i < circuitNodes.at(gateToShow).outDegree; ++i){
        cout << "FANOUT: " << circuitNodes.at(gateToShow).fanouts.at(i) << endl;
    }
    
fpOut.close();
}



//Give output
void generateCRT()
{
ofstream fpOut;
fpOut.open("./ckt_traversal.txt", ios::out | ios::app);
fpOut << endl;

fpOut << "Critical Path: "<< endl;

    int popGate = -1;
    int firstflag = 0;
    while(!criticalPath.empty()){
        popGate = criticalPath.back();
        criticalPath.pop_back();
        if (firstflag != 0){
            fpOut << ",";
        }else{
            firstflag = 1;
        }
        if (circuitNodes.at(popGate).secondaryType != "OUTPUT"){
         fpOut << circuitNodes.at(popGate).type << "-" << popGate;
        }else{
         fpOut << circuitNodes.at(popGate).secondaryType<<"-"<< circuitNodes.at(popGate).type << "-" << popGate;
        }
    }
fpOut << endl;

fpOut << "-------------------------------------------------------" << endl;
fpOut.close();
}



//Modified from the canvas, from PA0A

//===========LOAD LIBRARY============

//get the number of gates
int getGateNum(char *fName)
{
    FILE *fp;
    FILE *fpOut;

    char lineBuf[1024];
    char *cellName = NULL;
    char *valueDelay = NULL;
    char *nextToken = NULL;
    int gateNum = 0;
    
    // open the file
    fp = fopen(fName, "r");

    if (fp == NULL) {
        //cout << "Error opening file " << fName << endl;
        return -1;
    }
    
    while (!feof(fp)) {
        int flag = 0;

        fgets(lineBuf, 1023, fp); // get one whole line
        char *bufPtr = strstr(lineBuf, "cell "); // can you find "cell" in lineBuf? 


        if (bufPtr != NULL)
            flag = 1;
        else
            continue;

        if (flag == 1){
        //cout << "found cell ";

        // continue scanning lineBuf to get to '('
        while (*bufPtr != '(' && *bufPtr != NULL)
            bufPtr++;

        if (*bufPtr == NULL) {
            //cout << " ERROR! Found cell, but didn't find '('" << endl;
            return -1;
        }

        gateNum++;
        }



    }

    return gateNum;
}

int parseFileCFormat(char *fName, struct Gate* gatearr[])
{
    //cout <<"Begin"<<endl;
    FILE *fp;
    FILE *fpOut;

    char lineBuf[1024];
    char *cellName = NULL;
    char *valueDelay = NULL;
    char *nextToken = NULL;
    char *tempstring = NULL;
    int gateNum = 0;
    //cout << "Parsing input file " << fName << " using C syntax." << endl;
    
    // open the file
    fp = fopen(fName, "r");
    //fpOut = fopen("./cuixx327.txt","a+");

    if (fp == NULL) {
        //cout << "Error opening file " << fName << endl;
        return -1;
    }
    

    int flag = 0; // flag == 1: cell name; flag == 0: value 
    int isdelay = 0;
    int index = -1;
    int inx = 0;
    int iny = 0;
    int ini = 0;
    int loadtype = 0;
    int initialflag = 0;
    // look for the keyword "cell"
    while (!feof(fp)) {

        fgets(lineBuf, 1023, fp); 
        char *bufPtr = strstr(lineBuf, "cell "); 
        char *bufPtrValue = strstr(lineBuf, "values"); 
        char *bufPtrDelay = strstr(lineBuf, "cell_delay"); 
        char *bufPtrSlew = strstr(lineBuf, "output_slew"); 
        char *bufPtrIndx1 = strstr(lineBuf, "index_1"); 
        char *bufPtrIndx2 = strstr(lineBuf, "index_2"); 
        char *bufPtrCap = strstr(lineBuf, "capacitance"); 

        //cout << lineBuf;

        if (bufPtrDelay != NULL){
            loadtype = 1;
            isdelay = 1;
        }

        if (bufPtrSlew != NULL){
            loadtype = 2;
            isdelay = 1;
        }

        flag = 0;
        //   bufPtr will be a char pointer to a char in lineBuf at the start of cell

        if (bufPtr != NULL){
            flag = 1;
            initialflag = 1;
        }

        if (bufPtrValue != NULL)
            flag = 2;

        if (flag == 0 && isdelay != 2 && bufPtrIndx1 == NULL && bufPtrIndx2 == NULL && bufPtrCap == NULL)
            continue;

        if (flag == 1){
        //cout << "found cell ";

        // continue scanning lineBuf to get to '('
        while (*bufPtr != '(' && *bufPtr != NULL)
            bufPtr++;

        if (*bufPtr == NULL) {
            //cout << " ERROR! Found cell, but didn't find '('" << endl;
            return -1;
        }

        // we found '('. Now we are at the start of the cell name
        cellName = strtok(bufPtr, "() ");
        // if we wanted to read another token, we would have used:
        // cellName2 = strtok(NULL, "() ");
        //cout << cellName << "MODIFIED" << endl;
        //fprintf(fpOut, "%s\n",cellName);
        //cout << cellName<<endl;
        index++;

        (*gatearr)[index].name = cellName;


        gateNum++;
        isdelay = 1;
        }

        //cout << initialflag << endl;
        //cout<<lineBuf<<endl;
        if(initialflag == 1 && bufPtrCap != NULL){
            //cout << "CAP"<<endl;
            valueDelay = strstr(lineBuf, "capacitance");
            reduce(valueDelay,'\\');
            reduce(valueDelay,')');
            reduce(valueDelay,'"');
            reduce(valueDelay,';');
            reduce(valueDelay,' ');
            reduce(valueDelay,'\t');
            reduce(valueDelay,'\n');

            valueDelay = strtok(valueDelay,":");
            //ini = -1;
            valueDelay = strtok(NULL,";");

            //cout << valueDelay << endl;
            (*gatearr)[index].capacitance= stod(valueDelay);
            

        }


        if(initialflag == 1 && bufPtrIndx1 != NULL){
            //cout << "INDEX1"<<endl;
            valueDelay = strstr(lineBuf, "index_1");
            reduce(valueDelay,'\\');
            reduce(valueDelay,')');
            reduce(valueDelay,'"');
            reduce(valueDelay,';');
            reduce(valueDelay,' ');
            reduce(valueDelay,'\t');
            reduce(valueDelay,'\n');

            valueDelay = strtok(valueDelay,"()");
            ini = -1;

            while(valueDelay != NULL){
            //cout << valueDelay << "," << ini << endl;
            if (ini >= 0){
            if (loadtype == 1){
            (*gatearr)[index].delay_input_slew_index[ini] = stod(valueDelay);
            }else{
            (*gatearr)[index].slew_input_slew_index[ini] = stod(valueDelay);
            }
            }
            ini++;
            valueDelay = strtok(NULL,",");
            }
        }

        if(initialflag == 1 && bufPtrIndx2 != NULL){
            //cout << "INDEX2"<<endl;
            valueDelay = strstr(lineBuf, "index_2");
            reduce(valueDelay,'\\');
            reduce(valueDelay,')');
            reduce(valueDelay,'"');
            reduce(valueDelay,';');
            reduce(valueDelay,' ');
            reduce(valueDelay,'\t');
            reduce(valueDelay,'\n');


            valueDelay = strtok(valueDelay,"()");
            ini = -1;

            while(valueDelay != NULL){
            //cout << valueDelay << endl;
            if (ini >=0){
            if (loadtype == 1){
            (*gatearr)[index].delay_output_load_index[ini] = stod(valueDelay);
            }else{
            (*gatearr)[index].slew_output_load_index[ini] = stod(valueDelay);
            }
            }
            ini++;
            valueDelay = strtok(NULL,",");

            }
        }

        if (flag == 2){
        if (isdelay == 1){
        inx = 0;
        //cout << "found value ";
        // continue scanning lineBuf to get to '('
        while (*bufPtrValue != '(' && *bufPtrValue != NULL)
            bufPtrValue++;

        if (*bufPtrValue == NULL) {
            //cout << " ERROR! Found value, but didn't find '('" << endl;
            return -1;
        }


        // we found '('. Now we are at the start of the cell name
        //valueDelay = strtok(bufPtrValue, "() ");
        valueDelay = bufPtrValue;
            reduce(valueDelay,'\\');
            reduce(valueDelay,')');
            reduce(valueDelay,'"');
            reduce(valueDelay,';');
            reduce(valueDelay,'(');
            reduce(valueDelay,' ');
            reduce(valueDelay,'\t');
            reduce(valueDelay,'\n');


            valueDelay = strtok(valueDelay,",");

            iny = 0;
            while(valueDelay != NULL){
                //cout << valueDelay;
                //fprintf(fpOut, "%s",valueDelay);
            
                if (loadtype == 1){
                ((*gatearr)[index].cell_delays)[inx][iny] = stod(valueDelay);
                }

                if (loadtype == 2){
                //cout <<"output_slew of " << (*gatearr)[index].name <<" has been set to "<< inx << " , "<< iny<< "-->" << stod(valueDelay)<< endl;
                ((*gatearr)[index].output_slew)[inx][iny] = stod(valueDelay);
                }
    

                //cout<<cellName<<endl;
                //cout<< stod(valueDelay) << " stored in " << inx << "," << iny << endl; 

                valueDelay = strtok(NULL,",");
                /*
                if (valueDelay){
                    fprintf(fpOut,"%s", ", ");

                }

                else{
                    fprintf(fpOut,"%s\n", ";");
                }
                */
                iny++;


            }

        // if we wanted to read another token, we would have used:
        //     cellName2 = strtok(NULL, "() ");
        //cout << valueDelay << endl;
        isdelay = 2;
        inx++;

        continue;
    }
}

        if (isdelay == 2){
            char *bufPtrValue = strstr(lineBuf, ")"); // can you find "cell" in lineBuf? 
            if (bufPtrValue != NULL)
                isdelay = 0;

            bufPtrValue = lineBuf;

            valueDelay = bufPtrValue;
            reduce(valueDelay,'\\');
            reduce(valueDelay,')');
            reduce(valueDelay,'"');
            reduce(valueDelay,';');
            reduce(valueDelay,' ');
            reduce(valueDelay,'\t');
            reduce(valueDelay,'\n');


            valueDelay = strtok(valueDelay,",");
            iny = 0;

            while(valueDelay != NULL){
                //cout << valueDelay;
                //fprintf(fpOut, "%s",valueDelay);
                

                if (loadtype == 1){
                ((*gatearr)[index].cell_delays)[inx][iny] = stod(valueDelay);
                }
                if (loadtype == 2){
                //cout <<"output_slew of " << (*gatearr)[index].name <<" has been set to "<< inx << " , "<< iny<< "-->" << stod(valueDelay)<< endl;

                ((*gatearr)[index].output_slew)[inx][iny] = stod(valueDelay);
                }
    

                //cout<< stod(valueDelay) << " stored in " << inx << "," << iny << endl; 
                valueDelay = strtok(NULL,",");
                /*
                if (valueDelay){
                    fprintf(fpOut,"%s", ", ");

                }

                else{
                    fprintf(fpOut,"%s\n", ";");
                }
                */
                iny++;


            }
            // if we wanted to read another token, we would have used:
            //     cellName2 = strtok(NULL, "() ");
            //cout << valueDelay << endl;
            inx++;

            if (inx == 7){
                if (iny == 7){
                    typetogate.erase((*gatearr)[index].name);
                    typetogate.insert(pair<string, Gate>((*gatearr)[index].name, (*gatearr)[index])); 
                    //cout << endl << (*gatearr)[index].name << " bind to " << index << endl;
                    //cout <<(*gatearr)[index].output_slew[1][1]<<endl;
                }
            }
        
    }


    }

        // if one were to extract numbers, then they could use atoi, or sscanf(cellName, "%f", &fNumber), etc.

    fclose(fp);
    //fclose(fpOut);
    return gateNum;
}



//===========LOAD LIBRARY FINISHED============






//LOAD CIRCUIT
int circuitLoad(char *fName)
{

    FILE *fp;
    FILE *fpOut;

    char lineBuf[1024];
    char *cellName = NULL;
    char *valueDelay = NULL;
    char *nextToken = NULL;
    int gateNum = 0;
    //cout << "Parsing input file " << fName << " using C syntax." << endl;
    // open the file
    fp = fopen(fName, "r");

    if (fp == NULL) {
        //cout << "Error opening file " << fName << endl;
        return -1;
    }
    

    int flag = 0; // flag == 1: cell name; flag == 0: value 
    int isdelay = 0;
    int index = -1;
    int inx = 0;
    int iny = 0;
    int loadtype = 0;
    int findfanin = 0;
    int findfanout = 0;
    // look for the keyword "cell"
    while (!feof(fp)) {

            fgets(lineBuf, 1023, fp); 
            char *bufPtr = strstr(lineBuf, "cell "); 
            char *inputPtr = strstr(lineBuf, "INPUT"); 
            char *outputPtr = strstr(lineBuf, "OUTPUT"); 
            char *veriPtr = strstr(lineBuf, "="); 
            char *commitPtr = strstr(lineBuf, "#");



            //cout << lineBuf;

            //cout<<loadtype;
            //cout << lineBuf << endl;

            if (inputPtr != NULL){
                loadtype = 1;
            }

            if (outputPtr != NULL){
                loadtype = 2;
            }


            if (veriPtr != NULL){
                if(commitPtr == NULL)
                loadtype = 3;
            }


            if (loadtype == 1 || loadtype == 2){

                istringstream iss(lineBuf);
                iss.imbue(locale(cin.getloc(),new ParenCommaEq_is_space));
                //cout << "..." << endl;
                string firstword;
                iss >> firstword;
                int gateNo = 0;
                string gateName;
                while(iss){
                    if(firstword.find_first_of("0123456789") != std::string::npos){
                        if (loadtype == 1){
                            gateNo = stoi(firstword);

                            if (gateNo >= circuitSize){
                                circuitSize = ceil((double)gateNo/1000.0) * 1000 + 1;
                                //circuitVector.resize(circuitSize);
                                circuitNodes.resize(circuitSize);
                            }



                            numbertotype.insert(pair<int, string>(gateNo,"INPUT"));
                            circuitNodes.at(gateNo).type = "INPUT";


                            front.push(gateNo);

                        }
                        if (loadtype == 2){
                            gateNo = stoi(firstword);

                            if (gateNo >= circuitSize){
                                circuitSize = ceil((double)gateNo/1000.0) * 1000 + 1;
                                //circuitVector.resize(circuitSize);
                                circuitNodes.resize(circuitSize);

                            }


                            numbertotype.insert(pair<int, string>(gateNo,"OUTPUT"));
                            circuitNodes.at(gateNo).secondaryType = "OUTPUT";
                            frontBack.push(gateNo);
                            outGates.push(gateNo);


                        }
                        
                    }
                    //cout << gateNo << endl;
                    //}
                    
                    iss >> firstword;
                }
            }


            if (loadtype == 3){

                istringstream iss(lineBuf);//assuminglineStrisastring//containingonelineoftextfromthefile.Seethe//examplepostedforPA0a.
                iss.imbue(locale(cin.getloc(),new ParenCommaEq_is_space));
                //cout << "..." << endl;
                string firstword;
                iss >> firstword;
                int gateNo = 0;
                int gateflag = 0;
                string gateName;
                while(iss){
                    if(firstword.find_first_of("0123456789") != std::string::npos){
                        if (gateflag == 0){
                            gateNo = stoi(firstword);

                            if (gateNo >= circuitSize){
                                circuitSize = ceil((double)gateNo/1000.0) * 1000 + 1;
                                //circuitVector.resize(circuitSize);
                                circuitNodes.resize(circuitSize);

                            }

                            gateflag = 1;
                        }
                        else{


                            if (stoi(firstword) >= circuitSize){
                                circuitSize = ceil((double)stoi(firstword)/1000.0) * 1000 + 1;
                                //circuitVector.resize(circuitSize);
                                circuitNodes.resize(circuitSize);

                            }

                            findfanin = 0;
                            findfanout = 0;

                            //circuitVector.at(gateNo).push_back(stoi(firstword));
                            if(circuitNodes.at(gateNo).fanins.empty()){
                            circuitNodes.at(gateNo).fanins.push_back(stoi(firstword));
                            circuitNodes.at(gateNo).inDegree++;
                            circuitNodes.at(gateNo).inDegreeRemain++;
                            }
                            else{
                                for(int itri=0;itri<circuitNodes.at(gateNo).inDegree;++itri){
                                    if (circuitNodes.at(gateNo).fanins[itri] == stoi(firstword)){
                                        findfanin = 1;
                                    }
                                }

                                if (findfanin == 0){
                                        circuitNodes.at(gateNo).fanins.push_back(stoi(firstword));
                                        circuitNodes.at(gateNo).inDegree++;
                                        circuitNodes.at(gateNo).inDegreeRemain++; 
                                }
                            }



                            if (circuitNodes.at(stoi(firstword)).fanouts.empty()){
                            circuitNodes.at(stoi(firstword)).fanouts.push_back(gateNo);
                            circuitNodes.at(stoi(firstword)).outDegree++;
                            circuitNodes.at(stoi(firstword)).outDegreeRemain++;
                            }
                            else{
                                for(int itro=0;itro<circuitNodes.at(stoi(firstword)).outDegree;++itro){
                                    if (circuitNodes.at(stoi(firstword)).fanouts[itro] == gateNo){
                                        findfanout = 1;
                                    }
                                }
                                if(findfanout == 0){
                                    circuitNodes.at(stoi(firstword)).fanouts.push_back(gateNo);
                                    circuitNodes.at(stoi(firstword)).outDegree++;
                                    circuitNodes.at(stoi(firstword)).outDegreeRemain++;
                                }

                            }
                            
                            /*

                            //circuitVector.at(gateNo).push_back(stoi(firstword));
                            if(circuitNodes.at(gateNo).fanins.empty()){
                            circuitNodes.at(gateNo).fanins.push_back(stoi(firstword));
                            circuitNodes.at(gateNo).inDegree++;
                            circuitNodes.at(gateNo).inDegreeRemain++;
                            }
                            else if (circuitNodes.at(gateNo).fanins.back() != stoi(firstword)){
                            circuitNodes.at(gateNo).fanins.push_back(stoi(firstword));
                            circuitNodes.at(gateNo).inDegree++;
                            circuitNodes.at(gateNo).inDegreeRemain++;
                            }
                            if (circuitNodes.at(stoi(firstword)).fanouts.empty()){
                            circuitNodes.at(stoi(firstword)).fanouts.push_back(gateNo);
                            circuitNodes.at(stoi(firstword)).outDegree++;
                            circuitNodes.at(stoi(firstword)).outDegreeRemain++;
                            }
                            else if (circuitNodes.at(stoi(firstword)).fanouts.back() != gateNo){
                            circuitNodes.at(stoi(firstword)).fanouts.push_back(gateNo);
                            circuitNodes.at(stoi(firstword)).outDegree++;
                            circuitNodes.at(stoi(firstword)).outDegreeRemain++;
                            }
*/

                            //cout << firstword << " add to " << gateNo << endl;

                        }

                    }

                    else{
                        if(firstword!="="){
                            if(firstword!= "\n"){
                        gateName = firstword;
                        transform(gateName.begin(),gateName.end(),gateName.begin(),::toupper);

                        numbertotype.insert(pair<int, string>(gateNo,gateName));
                        circuitNodes.at(gateNo).type = gateName;


                        //cout << gateName << " bind to " << gateNo << endl;
                        //cout << "load gate name: " << gateName << gateName.size()<< endl;
                    }
                    }
                    }



                        //cout << gateNo << endl;
                    //}
                    
                    iss >> firstword;
                }

                }

            // if we wanted to read another token, we would have used:
            //     cellName2 = strtok(NULL, "() ");
            //cout << valueDelay << endl;

            continue;
        
}

        // if one were to extract numbers, then they could use atoi, or sscanf(cellName, "%f", &fNumber), etc.

    fclose(fp);
    return gateNum;
}







// Driver code 
int main(int argc, char *argv[])
{ 
    //int V = 5; 
  
  /*
    if (argc < 4){
        cout << "I need at least 4 args" << endl;
        exit(0);
    }
*/
    int gateNum = 0;
    circuitNodes.resize(circuitSize); //Initialize as 1000

    char* inputfile = argv[1];

    gateNum = getGateNum(inputfile);

    Gate* gatearr = new Gate[gateNum+4];

    //cout << gateNum << endl;


    int abdgateNum = 0;
    
    abdgateNum = parseFileCFormat(inputfile,&gatearr);
    
    //cout <<abdgateNum<< endl;


// SET DEFAULT VALUES
    gatearr[gateNum+1].name = "INPUT";
    gatearr[gateNum+2].name = "OUTPUT";
    gatearr[gateNum+3].name = "DEFAULT";

    //cout << gatearr[0].name << endl;
    //cout << typetogate["AND"].name<< " ";

    for(int i = 0; i < 7; ++ i) {
        for(int j = 0; j<7; ++j){
    //cout << typetogate["AND"].cell_delays[i][j]<< " ";
    gatearr[gateNum+1].cell_delays[i][j] = 0;
    gatearr[gateNum+1].output_slew[i][j] = 0;

    gatearr[gateNum+2].cell_delays[i][j] = 0;
    gatearr[gateNum+2].output_slew[i][j] = 0;

    gatearr[gateNum+3].cell_delays[i][j] = 0;
    gatearr[gateNum+3].output_slew[i][j] = 0;

}
//cout<<endl;
}

    typetogate.insert(pair<string, Gate>("INPUT", gatearr[gateNum+1])); 
    typetogate.insert(pair<string, Gate>("OUTPUT", gatearr[gateNum+2])); 
    typetogate.insert(pair<string, Gate>("DEFAULT(NOT DEFINED)", gatearr[gateNum+3])); 






//LOAD CIRCUIT
    gateNum = circuitLoad(argv[2]);
    //cout << typetogate["BUF"].output_slew[0][0]<<endl;

//TEST OUT
    //generateOutput(1916);
    //generateOutput(2150);
    //generateOutput(33);
    //cout << "SIZE of vector:" << circuitSize << endl;

//TEST OUT

//int itera = 3;
//while(itera < argc){
//cout << stoi(argv[itera]) << endl;
//generateOutput(stoi(argv[itera]));
//itera++;
//}

//string example_type = "NAND";
//cout<< typetogate["NAND"].name<< endl;

//cout<< typetogate["NAND"].slew_output_load_index[2]<< endl;


//double lutsample = 0;
//lutsample = lutlookup("NAND", 0.0323, 0.996, "DELAY");
//cout <<"RESULT: "<<lutsample<<endl;

totalDelay = forwardTraversal();
//cout<< "CIRCUIT DELAY: " <<totalDelay<<endl;

ofstream fpOut;
fpOut.open("./ckt_traversal.txt", ios::out | ios::app);
fpOut << "-----------------------------------------------------" << endl;
fpOut << "Circuit Delay: "<< totalDelay*1000 <<"ps"<< endl;
fpOut << endl;
fpOut << "Gate Slacks: "<< endl;

requiredTime = totalDelay * 1.1;
backTraversal();
findCritical();
generateCRT();

return 0; 
} 
