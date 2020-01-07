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
#include <ctime>

using namespace std; 



//Random Function(Used by Shuffle)
int randomFun(int range){
	return std::rand() % range;
}

int acceptMove(double temperature, double deltaCost, double k){
	if (deltaCost < 0){
		return 1;
	}
	else{
		double boltz = exp(-deltaCost/(k*temperature));
		double r = ((double) rand() / (RAND_MAX));
		if (r < boltz){
			return 1;
		}else{
			return 0;
		}
	}
}

//
struct HyperNet
{
	int no = 0;
	int degree = 0;

	vector<int> connectedblocks;
};

struct Block
{
	int no = 0;
	int width = 0;
	int height = 0;

	int X = 0;
	int Y = 0;

	int isBeginningH = 1;
	int inDegreeH = 0;
	int inDegreeRemainH = 0;
	vector<int> outputsH;
	int outDegreeH = 0;
	int outDegreeRemainH = 0;

	int isBeginningV = 1;
	int inDegreeV = 0;
	int inDegreeRemainV = 0;
	vector<int> outputsV;
	int outDegreeV = 0;
	int outDegreeRemainV = 0;
};


vector<HyperNet> hyperNets;
vector<Block> blocks;
vector<Block> blocksBack;

vector<int> sequencePositive;
vector<int> sequenceNegative;
vector<int> sequencePositivePos;
vector<int> sequenceNegativePos;
queue <int> frontH;
queue <int> frontV;

vector<int> sequencePositiveBack;
vector<int> sequenceNegativeBack;
vector<int> sequencePositivePosBack;
vector<int> sequenceNegativePosBack;


double temperature = 40000;

vector<vector<int> > constraintGraphH,constraintGraphV;

//From Canvas, Struct for Load
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


int graphChecking(int blockNum, int blockflag){
	if (blockflag == 0){
		cout << "Current" << endl;

		cout <<"Positive: ";
		for(int i = 0; i<blockNum; ++i){
			cout << sequencePositive.at(i) << " ";
		}
		cout << endl;
		cout <<"Negative: ";
		for(int i = 0; i<blockNum; ++i){
			cout << sequenceNegative.at(i) << " ";
		}
		cout << endl;
		cout <<"Positive Pos: ";
		for(int i = 0; i<blockNum; ++i){
			cout << sequencePositivePos.at(i) << " ";
		}
		cout << endl;
		cout <<"NegativePos: ";
		for(int i = 0; i<blockNum; ++i){
			cout << sequenceNegativePos.at(i) << " ";
		}
		cout << endl;
	}else{
		cout << "BackUp" << endl;

		cout <<"Positive: ";
		for(int i = 0; i<blockNum; ++i){
			cout << sequencePositiveBack.at(i) << " ";
		}
		cout << endl;
		cout <<"Negative: ";
		for(int i = 0; i<blockNum; ++i){
			cout << sequenceNegativeBack.at(i) << " ";
		}
		cout << endl;
		cout <<"Positive Pos: ";
		for(int i = 0; i<blockNum; ++i){
			cout << sequencePositivePosBack.at(i) << " ";
		}
		cout << endl;
		cout <<"NegativePos: ";
		for(int i = 0; i<blockNum; ++i){
			cout << sequenceNegativePosBack.at(i) << " ";
		}
		cout << endl;

	}

}

float getHPWL(int netNum){
	HyperNet tempNet;
	float xLeft = 0;
	float xRight = 0;
	float yDown = 0;
	float yUp = 0;
	float tempX = 0;
	float tempY = 0;
	float hpwl = 0;
	float hpwlSum = 0;

	for(int i=0; i< netNum; ++i){
		tempNet = hyperNets.at(i);
		xLeft = blocks.at(tempNet.connectedblocks.at(0)).X + blocks.at(tempNet.connectedblocks.at(0)).width/2;
		yDown = blocks.at(tempNet.connectedblocks.at(0)).Y + blocks.at(tempNet.connectedblocks.at(0)).height/2;
		xRight = blocks.at(tempNet.connectedblocks.at(0)).X + blocks.at(tempNet.connectedblocks.at(0)).width/2;
		yUp = blocks.at(tempNet.connectedblocks.at(0)).Y + blocks.at(tempNet.connectedblocks.at(0)).height/2;



		/*
		xLeft = blocks.at(tempNet.connectedblocks.at(0)).X;
		yDown = blocks.at(tempNet.connectedblocks.at(0)).Y;
		xRight = blocks.at(tempNet.connectedblocks.at(0)).X;
		yUp = blocks.at(tempNet.connectedblocks.at(0)).Y;
		*/

		//cout << "For Hypernet " << i << endl; 
		for(int j=0; j<tempNet.degree; ++j){
			//cout<< tempNet.connectedblocks.at(j) << ":" << blocks.at((tempNet.connectedblocks.at(j))).X << "," << blocks.at((tempNet.connectedblocks.at(j))).Y<< endl;
			tempX = blocks.at((tempNet.connectedblocks.at(j))).X + blocks.at((tempNet.connectedblocks.at(j))).width/2;
			tempY = blocks.at((tempNet.connectedblocks.at(j))).Y + blocks.at((tempNet.connectedblocks.at(j))).height/2;

			/*
			tempX = blocks.at((tempNet.connectedblocks.at(j))).X;
			tempY = blocks.at((tempNet.connectedblocks.at(j))).Y;
			*/

			if (tempX < xLeft){
				xLeft = tempX;
			}
			if (tempX > xRight){
				xRight = tempX;
			}
			if (tempY < yDown){
				yDown = tempY;
			}
			if (tempY > yUp){
				yUp = tempY;
			}
		}
		hpwl = ((xRight - xLeft) + (yUp - yDown));
		//cout << "HPWL: " << hpwl << " X: "<<xLeft <<"," <<xRight <<" Y: "<<yDown <<","<<yUp << endl;
		hpwlSum += hpwl;
	}
	//cout <<"total HPWL: " << hpwlSum << endl;
	return hpwlSum;
}

int resetDegree(int blockNum){
	for (int i = 0; i<blockNum;++i){
		//cout << "SIZECHECK " << blocks.at(i).outputsH.size() << endl;

		blocks.at(i).outputsH.clear();
		blocks.at(i).outputsV.clear();
		//cout << "SIZECHECK " << blocks.at(i).outputsH.size() << endl;
		//blocks.at(i).outputsH.resize(blocks.at(i).outDegreeH);
		//blocks.at(i).outputsV.resize(blocks.at(i).outDegreeV);

		//cout << "SIZECHECK " << blocks.at(i).outputsH.size() <<" , "<<blocks.at(i).outDegreeH<< endl;

		blocks.at(i).inDegreeRemainH = 0;
		blocks.at(i).inDegreeRemainV = 0;
		blocks.at(i).inDegreeH = 0;
		blocks.at(i).inDegreeV = 0;
		blocks.at(i).isBeginningH = 1;
		blocks.at(i).isBeginningV = 1;
		blocks.at(i).outDegreeV = 0;
		blocks.at(i).outDegreeH = 0;
		blocks.at(i).outDegreeRemainH = 0;
		blocks.at(i).outDegreeRemainV = 0;
		
		blocks.at(i).X = 0;
		blocks.at(i).Y = 0;

		for (int j = 0; j<blockNum; ++j){
				(constraintGraphH.at(i)).at(j) = 0;
				(constraintGraphV.at(i)).at(j) = 0;
		}
	}	

	//Generate Constraint Graph
	for (int i = 0; i<blockNum; ++i){
		for(int j = 0; j<blockNum; ++j){
			if(i != j){

				(constraintGraphH.at(i)).at(j) = 0;
				(constraintGraphV.at(i)).at(j) = 0;

				if ((sequencePositivePos.at(i) > sequencePositivePos.at(j)) && (sequenceNegativePos.at(i) > sequenceNegativePos.at(j))){
					//i is after j in both P and N
					//i is right of b
					(constraintGraphH.at(i)).at(j) = blocks.at(i).width;
					blocks.at(j).isBeginningH = 0;
					blocks.at(j).inDegreeH++; blocks.at(j).inDegreeRemainH++;
					blocks.at(i).outDegreeH++; blocks.at(i).outDegreeRemainH++;
					blocks.at(i).outputsH.push_back(j);

				}else if((sequencePositivePos.at(i) < sequencePositivePos.at(j)) && (sequenceNegativePos.at(i) > sequenceNegativePos.at(j))){
					//i is before j in P and after j in N
					//i is above of b
					(constraintGraphV.at(i)).at(j) = blocks.at(i).height;
					blocks.at(j).isBeginningV = 0;
					blocks.at(j).inDegreeV++; blocks.at(j).inDegreeRemainV++;
					blocks.at(i).outDegreeV++; blocks.at(i).outDegreeRemainV++;
					blocks.at(i).outputsV.push_back(j);

				}
			}
		}
	}

		for (int i = 0; i<blockNum;++i){

			if (blocks.at(i).isBeginningV == 1){
				frontV.push(i);
			}
			if (blocks.at(i).isBeginningH == 1){
				frontH.push(i);
			}
	}	


	return 0;
}


int randomMove(int blockNum){
	//cout << "upbound "<<blockNum << endl;
	blocksBack  = blocks;
	sequencePositiveBack = sequencePositive;
	sequenceNegativeBack = sequenceNegative;
	sequencePositivePosBack = sequencePositivePos;
	sequenceNegativePosBack = sequenceNegativePos;



	int randMov = rand() % 3;
	//cout << "Random MOVE:" << randMov << endl;
	int index1 = rand() % blockNum;
	int index2 = rand() % blockNum;
	while(index2 == index1){
		index2 = rand() % blockNum;
	}

	if (randMov == 0 || randMov == 2){
		//cout << "Start Swap " << blocks.at(index1).X<<" , "<< blocks.at(index2).X<< endl;
		int tempInt = sequencePositive.at(index1);
		sequencePositive.at(index1) = sequencePositive.at(index2);
		sequencePositive.at(index2) = tempInt;
		//cout << "End Swap " << blocks.at(index1).X<<" , "<< blocks.at(index2).X<< endl;
	}
	if (randMov == 1 || randMov == 2){
		int tempInt = sequenceNegative.at(index1);
		sequenceNegative.at(index1) = sequenceNegative.at(index2);
		sequenceNegative.at(index2) = tempInt;
	}

	//reset position vertex
	for (int i = 0; i<blockNum; ++i){
		sequencePositivePos.at(sequencePositive.at(i)) = i;
		sequenceNegativePos.at(sequenceNegative.at(i)) = i;
	}
	return 0;
}



int forwardTraversalH(){
	int popGate = 0;
	int j = 0;
	int largestX = 0;
	while(!frontH.empty()){

        popGate = frontH.front();
        frontH.pop();
        //cout << "Traverse(H): "<<popGate <<endl;

        for(int i=0; i<blocks.at(popGate).outDegreeH; ++i){
        	j = (blocks.at(popGate).outputsH).at(i);
        	blocks.at(j).inDegreeRemainH--;
        	if(blocks.at(j).X < blocks.at(popGate).X + (constraintGraphH.at(popGate)).at(j)){
        		blocks.at(j).X = blocks.at(popGate).X + (constraintGraphH.at(popGate)).at(j);
        	}
        	if(blocks.at(j).X + blocks.at(j).width > largestX){
        		largestX = blocks.at(j).X + blocks.at(j).width;
        	}
        	if (blocks.at(j).inDegreeRemainH <= 0){
        		frontH.push(j);
        	}
        }




    }
return largestX;
}


int forwardTraversalV(){
	int popGate = 0;
	int j = 0;
	int largestY = 0;
	while(!frontV.empty()){

        popGate = frontV.front();
        frontV.pop();
        //cout << "Traverse(V): "<<popGate <<endl;

        for(int i=0; i<blocks.at(popGate).outDegreeV; ++i){
        	j = (blocks.at(popGate).outputsV).at(i);
        	blocks.at(j).inDegreeRemainV--;
        	if(blocks.at(j).Y < blocks.at(popGate).Y + (constraintGraphV.at(popGate)).at(j)){
        		blocks.at(j).Y = blocks.at(popGate).Y + (constraintGraphV.at(popGate)).at(j);
        	}
        	if(blocks.at(j).Y + blocks.at(j).height> largestY){
        		largestY = blocks.at(j).Y + blocks.at(j).height;
        	}
        	if (blocks.at(j).inDegreeRemainV <= 0){
        		frontV.push(j);
        	}
        }




    }
return largestY;
}
// Driver code 
int main(int argc, char *argv[])
{ 

	char* inputfile = argv[1];

    FILE *fp;
    char lineBuf[1024];

    if (argc < 3){
    	cout << "Usage: ./floorplan inputfile -option(-a/-w/-c)" << endl;
    }
    fp = fopen(argv[1], "r");

    char* option = argv[2];          

    if (fp == NULL) {
        //cout << "Error opening file " << fName << endl;
        return -1;
    }

int loadFlag = 0;
int blockNum = 0;
int netNum = 0;
int blockIndex = -1;
int netIndex = 0;
int degreeflag = 0;
int degreeIndex = 0;
int blockloadFlag = 0;


int area = 0;

while (!feof(fp)) {

fgets(lineBuf, 1023, fp); 
if (loadFlag == 0){
blockNum = stoi(lineBuf);
blocks.resize(blockNum+1);
loadFlag = 1;
}



istringstream iss(lineBuf);//assuminglineStrisastring//containingonelineoftextfromthefile.Seethe//examplepostedforPA0a.
iss.imbue(locale(cin.getloc(),new ParenCommaEq_is_space));
//cout << "..." << endl;
string firstword;
iss >> firstword;


if(loadFlag == 1){
	blockloadFlag = 0;
	while(iss){
		if (firstword == "Nets"){
			//cout << "NETS---------" << endl;
			loadFlag = 2;
		}

		else if(firstword.find_first_of("0123456789") != std::string::npos && loadFlag == 1){
			if (blockloadFlag == 1){
				blocks.at(blockIndex).width = stoi(firstword);
			}
			else if (blockloadFlag == 2){
				blocks.at(blockIndex).height = stoi(firstword);
			}
			blockloadFlag++;


		}

		iss >> firstword;
	}
	blockIndex++;
	//cout << blockIndex <<endl;
}

else{

	if(loadFlag == 2){
		netNum = stoi(firstword);
		hyperNets.resize(netNum+1);
		loadFlag = 3;
		//cout << firstword << endl;
		//cout << "OK" <<endl;
	}

	else if (loadFlag == 3){
	degreeflag = 1;
	while(iss){
		//cout << firstword << endl;
		if(firstword.find_first_of("0123456789") != std::string::npos){
			if (degreeflag == 1){
				(hyperNets.at(netIndex)).degree = stoi(firstword);
				hyperNets.at(netIndex).connectedblocks.resize(hyperNets.at(netIndex).degree+1);
				degreeflag = 0;
				degreeIndex = 0;
			}else{
				(hyperNets.at(netIndex)).connectedblocks.at(degreeIndex) = stoi(firstword);
				degreeIndex++;
			}
		}

		iss >> firstword;

	}
	netIndex++;

	}

}
//cout << endl;

}
cout << "Nodes:" << blockNum<< endl;



clock_t begin, end;


cout << "Begin floor planning..." << endl;

begin = clock();


//cout << (hyperNets.at(1)).connectedblocks.at(1) << endl;
//cout << (blocks.at(1).height) << endl;

sequenceNegative.resize(blockNum);
sequencePositive.resize(blockNum);
sequenceNegativePos.resize(blockNum);
sequencePositivePos.resize(blockNum);
constraintGraphH.resize(blockNum);
constraintGraphV.resize(blockNum);


for (int i = 0; i<blockNum; ++i){
	sequencePositive.at(i) = i;
	sequenceNegative.at(i) = i;
	constraintGraphH.at(i).resize(blockNum);
	constraintGraphV.at(i).resize(blockNum);
}

//Generate Random Floor Plan
auto t = time(nullptr);
std::srand(t);

random_shuffle (sequencePositive.begin(), sequencePositive.end(), randomFun);
random_shuffle (sequenceNegative.begin(), sequenceNegative.end(), randomFun);

for (int i = 0; i<blockNum; ++i){
	sequencePositivePos.at(sequencePositive.at(i)) = i;
	sequenceNegativePos.at(sequenceNegative.at(i)) = i;
}

//Generate Constraint Graph
for (int i = 0; i<blockNum; ++i){
	for(int j = 0; j<blockNum; ++j){
		if(i != j){
			(constraintGraphH.at(i)).at(j) = 0;
			(constraintGraphV.at(i)).at(j) = 0;

			if ((sequencePositivePos.at(i) > sequencePositivePos.at(j)) && (sequenceNegativePos.at(i) > sequenceNegativePos.at(j))){
				//i is after j in both P and N
				//i is right of b
				(constraintGraphH.at(i)).at(j) = blocks.at(i).width;
				blocks.at(j).isBeginningH = 0;
				blocks.at(j).inDegreeH++; blocks.at(j).inDegreeRemainH++;
				blocks.at(i).outDegreeH++; blocks.at(i).outDegreeRemainH++;
				blocks.at(i).outputsH.push_back(j);

			}else if((sequencePositivePos.at(i) < sequencePositivePos.at(j)) && (sequenceNegativePos.at(i) > sequenceNegativePos.at(j))){
				//i is before j in P and after j in N
				//i is above of b
				(constraintGraphV.at(i)).at(j) = blocks.at(i).height;
				blocks.at(j).isBeginningV = 0;
				blocks.at(j).inDegreeV++; blocks.at(j).inDegreeRemainV++;
				blocks.at(i).outDegreeV++; blocks.at(i).outDegreeRemainV++;
				blocks.at(i).outputsV.push_back(j);

			}
		}
	}
}

for (int i = 0; i<blockNum; ++i){
if (blocks.at(i).isBeginningV == 1){
	frontV.push(i);
}
if (blocks.at(i).isBeginningH == 1){
	frontH.push(i);
}
}

ofstream fpOut;
ofstream fpOutInter;

string outfileName = "default";
string intermediateName = "default";

if (strcmp(option,"-a") == 0){
	outfileName = "_Cui_Tianning.out2a";
	outfileName = argv[1] + outfileName;
	intermediateName = "_Cui_Tianning_a.csv";
	intermediateName = argv[1] + intermediateName;
}

if (strcmp(option,"-w") == 0){
	outfileName = "_Cui_Tianning.out2w";
	outfileName = argv[1] + outfileName;
	intermediateName = "_Cui_Tianning_w.csv";
	intermediateName = argv[1] + intermediateName;
}

if (strcmp(option,"-c") == 0){
	outfileName = "_Cui_Tianning.out2c";
	outfileName = argv[1] + outfileName;
	intermediateName = "_Cui_Tianning_c.csv";
	intermediateName = argv[1] + intermediateName;
}

if (strcmp(option,"-cw") == 0){
	outfileName = "_Cui_Tianning.out2cw";
	outfileName = argv[1] + outfileName;
	intermediateName = "_Cui_Tianning_cw.csv";
	intermediateName = argv[1] + intermediateName;
}


if (strcmp(option,"-ca") == 0){
	outfileName = "_Cui_Tianning.out2ca";
	outfileName = argv[1] + outfileName;
	intermediateName = "_Cui_Tianning_ca.csv";
	intermediateName = argv[1] + intermediateName;
}

fpOut.open(outfileName, ios::out | ios::app);
fpOutInter.open(intermediateName, ios::out | ios::app);


fpOutInter << "Iteration, Temperature, AcceptedMoves, RejectedMoves, Wirelength(HPWL), Area" << endl;

int circuitH = 0;
int circuitV = 0;
circuitH = forwardTraversalH();
circuitV = forwardTraversalV();

cout <<"----Initial floorplan: ----"<< endl;



cout <<"sequencePositive:" <<endl;
//fpOut <<"sequencePositive:" <<endl;
for (int i = 0; i<blockNum; ++i){
	cout << sequencePositive.at(i) << " ";
	//fpOut << sequencePositive.at(i) << " ";
}
cout << endl;
cout <<"sequenceNegative:" <<endl;
//fpOut << endl;
//fpOut <<"sequenceNegative:" <<endl;
for (int i = 0; i<blockNum; ++i){
	cout << sequenceNegative.at(i) << " ";
	//fpOut << sequenceNegative.at(i) << " ";

}
cout << endl;
cout<<"CIRCUIT-X, CIRCUIT Y, CIRCUIT-AREA:" << endl;
cout<<circuitH <<",   "<<circuitV<<",   "<< circuitH*circuitV <<endl;
//fpOut << endl;
//fpOut<<"CIRCUIT-X, CIRCUIT Y, CIRCUIT-AREA:" << endl;
//fpOut<<circuitH <<",   "<<circuitV<<",   "<< circuitH*circuitV <<endl;


cout<<"BLOCK-X:Y" << endl;
//fpOut<<"BLOCK-X:Y" << endl;

for (int i = 0; i<blockNum; ++i){
	cout << "block " << i<< " >>>>> "  << blocks.at(i).X <<" : "<<blocks.at(i).Y << endl; 
	//fpOut << "block " << i<< " >>>>> "  << blocks.at(i).X <<" : "<<blocks.at(i).Y << endl; 

}

float hpwl;
float hpwl_new;
area = circuitH*circuitV;
hpwl = getHPWL(netNum);

cout<<"INITIAL: CIRCUIT-X, CIRCUIT Y, CIRCUIT-AREA, HPWL:" << endl;
cout<<circuitH <<",   "<<circuitV<<",   "<< circuitH*circuitV<<",   "<<hpwl <<endl;



//graphChecking(blockNum);

float dCost = 0;
//graphChecking(blockNum,0);

double initial_costAvg = 0;

int i = 0;
while(i<blockNum){
		hpwl = getHPWL(netNum);

		randomMove(blockNum);
		resetDegree(blockNum);


		//cout << "circle "<<i<<endl;

		circuitH = forwardTraversalH();
		circuitV = forwardTraversalV();

		if (strcmp(option,"-a") == 0){
			dCost = circuitH * circuitV - area;
			//cout << "deltaCost(-a): "<<dCost << endl;
		}
		if (strcmp(option,"-w") == 0){
			hpwl_new = getHPWL(netNum);
			dCost = hpwl_new - hpwl;
			//cout << "deltaCost(-w): "<<dCost << endl;
		}
		if (strcmp(option,"-c") == 0){
			hpwl_new = getHPWL(netNum);
			dCost = ((hpwl_new - hpwl) + (circuitH * circuitV - area))/2;
			//cout << "deltaCost(-c): "<<dCost << endl;
		}
		if (strcmp(option,"-ca") == 0){
			hpwl_new = getHPWL(netNum);
			dCost = (hpwl_new - hpwl)*0.3 + (circuitH * circuitV - area)*0.7;
			//cout << "deltaCost(-ca): "<<dCost << endl;
		}
		if (strcmp(option,"-cw") == 0){
			hpwl_new = getHPWL(netNum);
			dCost = (hpwl_new - hpwl)*0.7 + (circuitH * circuitV - area)*0.3;
			//cout << "deltaCost(-cw): "<<dCost << endl;
		}


		sequenceNegative = sequenceNegativeBack;
		sequencePositive = sequencePositiveBack;
		sequencePositivePos = sequencePositivePosBack;
		sequenceNegativePos = sequenceNegativePosBack;
		blocks = blocksBack;
		if(dCost != 0){
			initial_costAvg += abs(dCost);
			++i;
		}
		
}

double valueK = 0; 
initial_costAvg = initial_costAvg / blockNum;
valueK = abs(initial_costAvg/(log(0.95)*temperature));
cout << "K= "<< valueK << endl;
//valueK = 3;
//valueK = 10;


double numMoves = 6;
int iterationCounter = 0;
int acceptCounter = 0;
int rejectCounter = 0;



while(temperature >= 0.01){
	numMoves *= 1.01;

	iterationCounter++;
	acceptCounter = 0;
	rejectCounter = 0;
	
	for(int i=0; i<numMoves; i++){

		hpwl = getHPWL(netNum);

		randomMove(blockNum);
		resetDegree(blockNum);


		circuitH = forwardTraversalH();
		circuitV = forwardTraversalV();

		if (strcmp(option,"-a") == 0){
			dCost = circuitH * circuitV - area;
			//cout << "deltaCost(-a): "<<dCost << endl;
		}
		if (strcmp(option,"-w") == 0){
			hpwl_new = getHPWL(netNum);
			dCost = hpwl_new - hpwl;
			//cout << "deltaCost(-w): "<<dCost << endl;
		}
		if (strcmp(option,"-c") == 0){
			hpwl_new = getHPWL(netNum);
			dCost = ((hpwl_new - hpwl) + (circuitH * circuitV - area))/2;
			//cout << "deltaCost(-c): "<<dCost << endl;
		}
		if (strcmp(option,"-ca") == 0){
			hpwl_new = getHPWL(netNum);
			dCost = (hpwl_new - hpwl)*0.3 + (circuitH * circuitV - area)*0.7;
			//cout << "deltaCost(-ca): "<<dCost << endl;
		}
		if (strcmp(option,"-cw") == 0){
			hpwl_new = getHPWL(netNum);
			dCost = (hpwl_new - hpwl)*0.7 + (circuitH * circuitV - area)*0.3;
			//cout << "deltaCost(-cw): "<<dCost << endl;
		}



		//cout << "deltaCost: "<<dCost << endl;
		if(acceptMove(temperature, dCost, valueK) == 0){
			sequenceNegative = sequenceNegativeBack;
			sequencePositive = sequencePositiveBack;
			sequencePositivePos = sequencePositivePosBack;
			sequenceNegativePos = sequenceNegativePosBack;
			blocks =  blocksBack;
			rejectCounter++;
			//cout << "NOT ACCEPT" << endl;
		}else{
			acceptCounter++;
			//cout << "ACCEPT" << endl;
		}

	}
		resetDegree(blockNum);

		circuitH = forwardTraversalH();
		circuitV = forwardTraversalV();
		hpwl = getHPWL(netNum);
		
	cout << "Iteration, Temperature, AcceptedMoves, RejectedMoves, Wirelength(HPWL), Area" << endl;
	cout <<  iterationCounter<<", "<< temperature<< ", " << acceptCounter << ", " << rejectCounter << ", " << hpwl << ", " << circuitH*circuitV << endl;

	//fpOutInter << "Iteration, Temperature, AcceptedMoves, RejectedMoves, Wirelength(HPWL), Area" << endl;
	fpOutInter <<  iterationCounter<<", "<< temperature<< ", " << acceptCounter << ", " << rejectCounter << ", " << hpwl << ", " << circuitH*circuitV << endl;

	temperature *= 0.95;
	area = circuitH*circuitV;

}




end = clock();

auto time_consume = ((double)end - (double)begin)/CLOCKS_PER_SEC;
//graphChecking(blockNum,0);
//graphChecking(blockNum,1);

cout << "---- Result floorplan: ----" << endl;
cout << "Time consume: " << time_consume << " (second)" << endl;

area = circuitH*circuitV;
hpwl = getHPWL(netNum);

cout<<"CIRCUIT-X, CIRCUIT Y: " << circuitH <<",   "<<circuitV<< endl;
cout<<"Circuit Area: "<< area <<endl;
cout<<"Circuit HPWL: "<< hpwl <<endl;


fpOut << "Time consume: " << time_consume << endl;
fpOut<<"CIRCUIT-X, CIRCUIT Y: " << circuitH <<",   "<<circuitV<< endl;
fpOut<<"Circuit Area: "<< area <<endl;
fpOut<<"Circuit HPWL: "<< hpwl <<endl;



cout <<"sequencePositive:" <<endl;
fpOut <<"sequencePositive:" <<endl;
for (int i = 0; i<blockNum; ++i){
	cout << sequencePositive.at(i) << " ";
	fpOut << sequencePositive.at(i) << " ";
}
cout << endl;
cout <<"sequenceNegative:" <<endl;
fpOut << endl;
fpOut <<"sequenceNegative:" <<endl;
for (int i = 0; i<blockNum; ++i){
	cout << sequenceNegative.at(i) << " ";
	fpOut << sequenceNegative.at(i) << " ";

}

cout << endl;
//cout<<"CIRCUIT-X, CIRCUIT Y, CIRCUIT-AREA:" << endl;
//cout<<circuitH <<",   "<<circuitV<<",   "<< circuitH*circuitV <<endl;
fpOut << endl;
//fpOut<<"CIRCUIT-X, CIRCUIT Y, CIRCUIT-AREA:" << endl;
//fpOut<<circuitH <<",   "<<circuitV<<",   "<< circuitH*circuitV <<endl;


cout<<"BLOCK-X:Y" << endl;
fpOut<<"BLOCK-X:Y" << endl;

for (int i = 0; i<blockNum; ++i){
	cout << "block " << i<< " >>>>> "  << blocks.at(i).X <<" : "<<blocks.at(i).Y << endl; 
	fpOut << "block " << i<< " >>>>> "  << blocks.at(i).X <<" : "<<blocks.at(i).Y << endl; 

}
fpOut.close();



return 0; 
} 

