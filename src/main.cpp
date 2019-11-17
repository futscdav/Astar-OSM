#include "Graph.h"
#include "Utils.h"
#include "RoutePlanner.h"
#include "DataLoader.h"
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <ctime>
#include <memory>

using namespace std;

void PrintSizes() {
	cout << "Node: " << sizeof(GraphNode) << endl;
	cout << "Edge: " << sizeof(GraphEdge) << endl;
	cout << "EdgeId: " << sizeof(EdgeId) << endl;
	cout << "EdgeSet: " << sizeof(EdgeSet) << endl;

	cout << "Estimated space complexity: " << 103000*sizeof(GraphNode) + 190000*sizeof(GraphEdge) + 300000*sizeof(EdgeId) << endl;
}

void PrintPlanProperties(double PlanLength, double PlanTime, int Nodes) {
	std::cout << "Plan length (km)  	: " << setprecision(11) << PlanLength << std::endl;
	std::cout << "Time to travel (hrs)	: " << setprecision(11) << PlanTime << std::endl;
	std::cout << "Plan length (nodes)	: " << Nodes << std::endl;
}

int main(int argc, char** argv) {

	//PrintSizes();
	DataLoader loader(argc < 2 ? "data" : argv[1]);

	//13823646L
	//188755778L
	//351122923L
	//7535730L

	// 13823646 188755778

	auto GraphPtr = loader.LoadFromFiles();
	if (!GraphPtr) {
		return 1;
	}
	Graph& RoadGraph = *GraphPtr;
	
	string line;
	ofstream DebugFile("results.csv");
	int CaseNr = 0;
	while(getline(cin, line)) {
		// solve invalid input
		istringstream sstream(line);
		long long OriginId, DestinationId;
		if (!(sstream >> OriginId >> DestinationId)) {
			std::cout << "Expecting 2 node IDs separated by a space." << std::endl;
			continue;
		}

		GraphNode *OriginPtr, *DestinationPtr;

		try {
			OriginPtr = &RoadGraph.GetNodeByNodeId(OriginId);
			DestinationPtr = &RoadGraph.GetNodeByNodeId(DestinationId);
		} catch (Graph::NodeNotFoundException const& ex) {
			cout << "Wrong node id: " << OriginId << " " << DestinationId << endl;
			continue;
		}

		GraphNode& Origin = *OriginPtr;
		GraphNode& Destination = *DestinationPtr;

		cout << "Planning out the best route between given points " << Origin.GetId() << " and " << Destination.GetId() << endl;

		RoutePlanner Planner;
		EdgeList ListOfEdges;
		int NumberOfInserts;

		DebugFile << CaseNr++ << ", ";

		//run A*
		clock_t begin = clock();
		PlannerResult Result = Planner.Plan(RoadGraph, Origin, Destination);
		if (Result.PathExists()) {
			PrintPlanProperties(Result.PathLength(), Result.PathTime(), Result.Path().size());
			DebugFile << setprecision(17) << Result.PathLength() << ", " << setprecision(17) << Result.PathTime();
			DebugFile << ", " << Origin.GetId();
			for (int i = 0; i < Result.Path().size(); i++) {
				DebugFile << ", " << Result.Path()[i]->GetToNodeId();
			}
		} else {
			cout << "No possible plan was found between given points." << endl;
			DebugFile << "-1.0" << ", " << "-1.0";
		}
		DebugFile << ", " << Result.NumberOfInserts() << "," << (int)(((double)(clock()-begin)/CLOCKS_PER_SEC)*1000) << endl;
		cout << "Plan was completed in\t: " << (double)(clock()-begin)/CLOCKS_PER_SEC << "s." << endl;
		cout << "(Added " << Result.NumberOfInserts() << " times to the open list.)" << endl;
		cout << "------------------------------" << endl;
	}
	
	return 0;
}
