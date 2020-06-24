//CS323-34
//Joseph Bdaro
//Project 9 - Scheduling

#include <iostream>
#include <fstream>

using namespace std;

class scheduling {
public:

	class node {
	public:
		int jobId;
		int jobTime;
		int dependentCount;
		node* next;

		node(int jobId, int jobTime, int dependentCount) {
			this->jobId = jobId;
			this->jobTime = jobTime;
			this->dependentCount = dependentCount;
			next = NULL;
		}
	};

	class linkedList {
	public:
		int length;
		node* head;

		linkedList() {
			head = new node(0, 0, 0);
			length = 1;
		}

		//this method inserts nodes
		void listInsert(node* newNode) {
			node* spot = findSpot(head, newNode);
			newNode->next = spot->next;
			spot->next = newNode;
			length++;
		}
		//finds the spot in the LL to insert the node
		node* findSpot(node* head, node* newNode) {
			node* spot = head;
			while (spot->next != NULL && spot->next->jobTime > newNode->jobTime) {
				spot = spot->next;
			}
			return spot;
		}

		//prints the entire LL
		void printList(node* listHead, ofstream& outFile2) {
			node* iterate = listHead;
			outFile2 << "listHead ->(" << iterate->jobId << ", " << iterate->jobTime << ") ";
			iterate = iterate->next;
			while (iterate->next != NULL) {
				outFile2 << "->(" << iterate->jobId << ", " << iterate->jobTime << ")";
				iterate = iterate->next;
			}
			outFile2 << endl;
		}
	};

	class JOBS {
	public:
		int jobTime;
		int onWhichProc = 0;
		int onOpen = 0;
		int parentCount;
		int dependentCount;

		JOBS() {
			this->jobTime = 0;
			this->onWhichProc = 0;
			this->onOpen = 0;
			this->parentCount = 0;
			this->dependentCount = 0;
		}

		JOBS(int jobTime, int onWhichProc, int onOpen, int parentCount, int dependentCount) {
			this->jobTime = jobTime;
			this->onWhichProc = onWhichProc;
			this->onOpen = onOpen;
			this->parentCount = parentCount;
			this->dependentCount = dependentCount;
		}
	};

	class Proc {
	public:
		int doWhichJob;
		int timeRemain;

		Proc() {
			this->doWhichJob = 0;
			this->timeRemain = 0;
		}

		Proc(int doWhichJob, int timeRemain) {
			this->doWhichJob = doWhichJob;
			this->timeRemain = timeRemain;
		}
	};

	int numNodes;
	int numProcs;
	int procUsed = 0;
	int totalJobTimes;
	int currentTime;
	linkedList* OPEN;

	//dynamically allocated arrays;
	JOBS* jobAry;
	Proc* procAry;
	int** adjMatrix;
	int* parentCountAry;
	int* dependCountAry;
	int* onGraphAry;
	int** scheduleTable;

	scheduling(ifstream& inFile1, ifstream& inFile2, int numNodes, int numProcs) {
		this->procUsed = 0;
		this->numProcs = numProcs;
		this->currentTime = 0;
		OPEN = new linkedList();
		this->numNodes = numNodes;

		//dynamically allocates and initialises the 2d adjAry
		adjMatrix = new int* [numNodes + 1];
		for (int i = 0; i < numNodes + 1; i++) {
			adjMatrix[i] = new int[numNodes + 1];
		}
		for (int i = 0; i < numNodes + 1; i++) {
			for (int j = 0; j < numNodes + 1; j++) {
				adjMatrix[i][j] = 0;
			}
		}

		//dynamically alocates and initializes jobary
		jobAry = new JOBS[numNodes + 1];
		for (int i = 0; i < numNodes + 1; i++) {
			JOBS* initializer = new JOBS();
			jobAry[i] = *initializer;
		}

		//dynamically alocates and initializes procAry
		procAry = new Proc[numProcs + 1];
		for (int i = 0; i < numProcs + 1; i++) {
			Proc* initializer = new Proc();
			procAry[i] = *initializer;
		}

		//dynamically alocates and initializes parentCountAry
		parentCountAry = new int[numNodes + 1];
		for (int i = 0; i < numNodes + 1; i++) {
			parentCountAry[i] = 0;
		}

		//dynamically alocates and initializes dependCountAry
		dependCountAry = new int[numNodes + 1];
		for (int i = 0; i < numNodes + 1; i++) {
			dependCountAry[i] = 0;
		}

		loadMatrix(inFile1);
		computeParentCount();
		computeDependentCount();
		totalJobTimes = constructJobAry(inFile2);

		//dynamically allocates and initialises the 2d ScheduleTable
		scheduleTable = new int* [numProcs + 1];
		for (int i = 0; i < numProcs + 1; i++) {
			scheduleTable[i] = new int[totalJobTimes + 1];
		}

		for (int i = 0; i < numProcs + 1; i++) {
			for (int j = 0; j < totalJobTimes + 1; j++) {
				scheduleTable[i][j] = 0;
			}
		}

		//dynamically allocated and initialises the 1d onGraphAry
		onGraphAry = new int[numNodes + 1];
		for (int i = 0; i < numNodes + 1; i++) {
			onGraphAry[i] = 1;
		}
	}

	void loadMatrix(ifstream& inFile1) {
		int x, y;
		while (inFile1 >> x && inFile1 >> y) {
			adjMatrix[x][y] = 1;
		}
	}

	//finds how many jobs a specific job depends on or the parents for each job
	void computeParentCount() {
		for (int i = 1; i < numNodes + 1; i++) {
			for (int j = 1; j < numNodes + 1; j++) {
				if (adjMatrix[j][i] != 0) {
					parentCountAry[i] += adjMatrix[j][i];
				}
			}
			jobAry[i].parentCount = parentCountAry[i];
		}
	}

	//finds how many dependents each job has
	void computeDependentCount() {
		for (int i = 1; i < numNodes + 1; i++) {
			for (int j = 1; j < numNodes + 1; j++) {
				if (adjMatrix[i][j] != 0) {
					dependCountAry[i] += adjMatrix[i][j];
				}
			}
			jobAry[i].dependentCount = dependCountAry[i];
		}
	}

	int constructJobAry(ifstream& inFile2) {
		int totalTime = 0;
		int jobTime;
		int nodeId;
		int numNodes;
		inFile2 >> numNodes;

		while (inFile2 >> nodeId && inFile2 >> jobTime) {
			totalTime += jobTime;
			jobAry[nodeId].jobTime = jobTime;
			jobAry[nodeId].onWhichProc = -1;
			jobAry[nodeId].onOpen = 0;
		}

		return totalTime;
	}

	void loadOpen() {
		int orphanNode = 0;
		while (orphanNode != -1) {
			orphanNode = findOrphan();
			if (orphanNode > 0) {
				int jobId = orphanNode;
				int jobTime = jobAry[jobId].jobTime;
				int dependents = jobAry[jobId].dependentCount;
				node* newNode = new node(jobId, jobTime, dependents);
				OPEN->listInsert(newNode);
				jobAry[jobId].onOpen = 1;
			}
		}
	}

	int findOrphan() {
		for (int i = 1; i < numNodes + 1; i++) {
			if (jobAry[i].parentCount <= 0 && jobAry[i].onOpen == 0 && jobAry[i].onWhichProc <= 0) {
				return i;
			}
		}
		return -1;
	}

	void loadProcAry() {
		int availProc = findProcessor();

		while (availProc > 0 && OPEN->head->next != NULL && procUsed < numProcs) {
			availProc = findProcessor();
			if (availProc > 0) {
				procUsed++;
				node* newJob = OPEN->head->next;
				OPEN->head->next = OPEN->head->next->next;

				int jobId = newJob->jobId;
				int jobTime = newJob->jobTime;
				procAry[availProc].doWhichJob = jobId;
				procAry[availProc].timeRemain = jobTime;
				putJobOnTable(availProc, currentTime, jobId, jobTime);
				availProc = findProcessor();
			}
		}
	}

	int findProcessor() {
		for (int i = 1; i < numProcs + 1; i++) {
			if (procAry[i].timeRemain <= 0) {
				return i;
			}
		}
		return -1;
	}

	void putJobOnTable(int availProc, int currentTime, int jobId, int jobTime) {
		int time = currentTime;
		int endTime = time + jobTime;

		while (time < endTime) {
			scheduleTable[availProc][time] = jobId;
			time++;
		}
	}

	bool checkCycle() {
		if ((OPEN->head->next == NULL) && (graphIsEmpty() == false) && (allProcessorsFinished() == true)) {
			return true;
		}
		else {
			false;
		}
	}

	bool graphIsEmpty() {
		for (int i = 1; i < numNodes + 1; i++) {
			if (onGraphAry[i] == 1) {
				return false;
			}
		}
		return true;
	}

	bool allProcessorsFinished() {
		for (int i = 1; i < numProcs + 1; i++) {
			if (procAry[i].timeRemain > 0) {
				return false;
			}
		}
		return true;
	}

	void printScheduleTable(ofstream& outFile1) {
		outFile1 << "     ";
		for (int i = 0; i < totalJobTimes + 1; i++) {
			outFile1 << "---" << i;
		}
		outFile1 << endl;
		for (int i = 1; i < numProcs + 1; i++) {
			outFile1 << "P(" << i << ") |";
			for (int j = 0; j < totalJobTimes + 1; j++) {
				if (scheduleTable[i][j] == 0) {
					outFile1 << " " << " " << " |";
				}
				else {
					outFile1 << " " << scheduleTable[i][j] << " |";
				}
			}
			outFile1 << endl;
		}
		outFile1 << endl << endl;
	}
	void updateProcTime() {
		for (int i = 1; i < numProcs + 1; i++) {
			if (procAry[i].timeRemain > 0) {
				procAry[i].timeRemain--;
			}
		}
	}

	void deleteFinishedNodes() {
		int jobId = findDoneProc();
		while (jobId > 0) {
			if (jobId > 0) {
				onGraphAry[jobId] = 0;
				deleteEdge(jobId);
			}
			jobId = findDoneProc();
		}
	}

	int findDoneProc() {
		for (int i = 1; i < numProcs + 1; i++) {
			if (procAry[i].doWhichJob > 0 && procAry[i].timeRemain <= 0) {
				int jobId = procAry[i].doWhichJob;
				procAry[i].doWhichJob = -1;
				return jobId;
			}
		}
		return 0;
	}

	void deleteEdge(int jobId) {
		for (int j = 1; j < numNodes + 1; j++) {
			if (adjMatrix[jobId][j] > 0) {
				adjMatrix[jobId][j] = 0;
				parentCountAry[j]--;
				jobAry[j].parentCount--;
				jobAry[jobId].dependentCount = 0;
				jobAry[jobId].jobTime = 0;
				jobAry[jobId].onOpen = 0;
				jobAry[jobId].onWhichProc = 0;
				jobAry[jobId].parentCount = 0;
				dependCountAry[jobId] = 0;
			}
		}
	}
};

int main(int argc, char* argv[])
{
	ifstream inFile1, inFile2;
	ofstream outFile1, outFile2;

	inFile1.open(argv[1]);
	inFile2.open(argv[2]);
	int numProcs = strtol(argv[3], NULL, 10);
	outFile1.open(argv[4]);
	outFile2.open(argv[5]);
	int numNodes;
	inFile1 >> numNodes;

	if (numProcs > numNodes) {
		numProcs = numNodes;
	}

	scheduling* newSched = new scheduling(inFile1, inFile2, numNodes, numProcs);
	//this is so the project does not produce errors
	int i = 0;
		while (newSched->graphIsEmpty() != true && i < 11) {
			newSched->loadOpen();
			newSched->OPEN->printList(newSched->OPEN->head, outFile2);
			newSched->loadProcAry();
			bool hasCycle = newSched->checkCycle();
			if (hasCycle == true) {
				cout << "ERROR! There's a cycle in the graph";
				return 99;
			}

			newSched->printScheduleTable(outFile1);
			newSched->currentTime++;
			newSched->updateProcTime();
			newSched->deleteFinishedNodes();
			i++;
		}

	newSched->printScheduleTable(outFile1);
	outFile1.close();
	outFile2.close();
	inFile1.close();
	inFile1.close();
}