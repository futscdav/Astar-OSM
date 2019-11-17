#include "RoutePlanner.h"

using namespace std;

PlannerResult RoutePlanner::Plan(Graph& RoadGraph, GraphNode& Origin, GraphNode& Destination) {
	//Create required structures
	Fringe<State, StateComparator> Fringe;
	unordered_set<long long> ClosedSet(RoadGraph.GetAllNodes().size());
	BestCostMap.clear();
	BestCostMap.reserve(RoadGraph.GetAllNodes().size());

	//initiate the costs map
	for (auto it = RoadGraph.GetAllNodes().begin(); it != RoadGraph.GetAllNodes().end(); ++it) {
		BestCostMap[(*it).first] = numeric_limits<double>::infinity();
	}
	BestCostMap[Origin.GetId()] = 0;
	CurrentGoal = &Destination;

	//find highest allowed speed for the heuristics
	CurrentMaxSpeed = FindHighestSpeed(RoadGraph.GetAllEdges());

	//go go A*
	//push the starting node
	State Start(this, &Origin);
	Start.SetCost(0);
	Fringe.Add(Start);

	while(!Fringe.Empty()) {

		//Pop best node
		GraphNode& CurrentNode = *(Fringe.Pop().Node);

		//Have we reached our goal?
		if(IsGoal(CurrentNode, Destination)) {
			cout << "A path has been found.." << endl;
			EdgeList Plan;
			ReconstructPath(Plan, RoadGraph, Origin, Destination);
			int NumberOfInserts = Fringe.GetCounter();
			return PlannerResult(true, NumberOfInserts, move(Plan));
		}

		//Add to closed list
		ClosedSet.insert(CurrentNode.GetId());

		//expand state
		vector<long long> ExpandedNodes;
		ExpandNode(CurrentNode, RoadGraph, ExpandedNodes);
		for (auto it = ExpandedNodes.begin(); it != ExpandedNodes.end(); ++it) {
			long long SuccessorNodeId = (*it);
			//check closed list
			if(ClosedSet.find(SuccessorNodeId)!=ClosedSet.end()) {
				continue;
			}

			GraphEdge& TempEdge = RoadGraph.GetEdge(CurrentNode.GetId(), SuccessorNodeId);
			double PotentialScore = BestCostMap[CurrentNode.GetId()] + TempEdge.GetLengthInMetres()/1000.0/TempEdge.GetAllowedMaxSpeedInKmph();
			//make state hold long long instead of node ptr?
			State NewState(this, &RoadGraph.GetNodeByNodeId(SuccessorNodeId));
			NewState.SetCost(PotentialScore);

			if(!Fringe.Contains(NewState)) {
				BestCostMap[SuccessorNodeId] = PotentialScore;
				Fringe.Add(NewState);
			}
			else if (PotentialScore < BestCostMap[SuccessorNodeId]) {
				BestCostMap[SuccessorNodeId] = PotentialScore;
				Fringe.Update(NewState);
			}
		}

	}

	int NumberOfInserts = Fringe.GetCounter();
	PlannerResult EmptyResult(false, NumberOfInserts, {});
	return EmptyResult;
}

void RoutePlanner::ExpandNode(GraphNode& Expandee, Graph& RoadGraph, vector<long long>& Container) {
	EdgeSet& OutcomingEdges = RoadGraph.GetNodeOutcomingEdges(Expandee.GetId());
	for (EdgeSet::iterator it = OutcomingEdges.begin(); it != OutcomingEdges.end(); ++it) {
		Container.push_back((*it)->GetToNodeId());
	}
}

void RoutePlanner::ReconstructPath(EdgeList& Plan, Graph& RoadGraph, GraphNode& Origin, GraphNode& Destination) {
	long long CurrentNode = Destination.GetId();

	while(!(CurrentNode == Origin.GetId())) {
		EdgeSet& Edges = RoadGraph.GetNodeIncomingEdges(CurrentNode);
		//IN CASE OF A ZERO LENGTH EDGE
		bool IsZero = false;
		GraphEdge* ZeroEdge = 0;
		//
		for (auto it = Edges.begin(); it != Edges.end(); ++it) {
			//fix edge length = 0
			if ((*it)->GetLengthInMetres() == 0) {
				IsZero = true;
				ZeroEdge = (*it);
				continue;
			}

			double DifferenceFromOptimal = BestCostMap[(*it)->GetFromNodeId()] 
				+ (*it)->GetLengthInMetres()/1000.0/(*it)->GetAllowedMaxSpeedInKmph() - BestCostMap[CurrentNode];
			if (DifferenceFromOptimal >= 0 && DifferenceFromOptimal < numeric_limits<double>::epsilon()) {
				IsZero = false;
				Plan.push_back((*it));
				CurrentNode = (*it)->GetFromNodeId();
				break;
			}
		}
		if (IsZero) {
			Plan.push_back(ZeroEdge);
			CurrentNode = ZeroEdge->GetFromNodeId();
		}
	}
	//reverse the backtracked list
	ReverseEdgeList(Plan);
}

void RoutePlanner::ReverseEdgeList(EdgeList& List) {
	reverse(List.begin(), List.end());
}

bool RoutePlanner::IsGoal(GraphNode& Testee, GraphNode& RealGoal) {
	return (Testee.GetId()==RealGoal.GetId());
}

double RoutePlanner::FindHighestSpeed(EdgeMap& Edges) {
	double Maximum = 0;
	for (auto it = Edges.begin(); it != Edges.end(); ++it) {
		if ((*it).second->GetAllowedMaxSpeedInKmph() > Maximum) {
			Maximum = (*it).second->GetAllowedMaxSpeedInKmph();
		}
	}
	return Maximum;
}

PlannerResult::PlannerResult(bool PathFound, int NumberOfOpenListInserts, EdgeList PlanIn) : 
			PathFound(PathFound), NumberOfOpenListInserts(NumberOfOpenListInserts), Plan(move(PlanIn)), PlanLength(0.), PlanTime(0.) {
	for (auto it = Plan.begin(); it != Plan.end(); ++it) {
		PlanLength += (*it)->GetLengthInMetres()/1000.0;
		PlanTime += (*it)->GetLengthInMetres()/1000.0/(*it)->GetAllowedMaxSpeedInKmph();
	}
}
