#ifndef _ROUTEPLANNER_GUARD
#define _ROUTEPLANNER_GUARD

#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <algorithm>
#include <fstream>
#include "Graph.h"
#include "Fringe.h"
#include "Utils.h"

// type returned by the planner encapsulating all of the computed information
// PlannerResult is only valid as long as the Graph it was computed on is live
class PlannerResult {

	public:
	PlannerResult(bool PathFound, int NumberOfOpenListInserts, EdgeList Plan);
	
	bool PathExists() const { return PathFound; }
	int NumberOfInserts() const { return NumberOfOpenListInserts; }
	const EdgeList& Path() const { return Plan; }
	double PathLength() const { return PlanLength; }
	double PathTime() const { return PlanTime; }

	private:
	bool PathFound;
	int NumberOfOpenListInserts;
	EdgeList Plan;
	double PlanLength;
	double PlanTime;
};

class RoutePlanner {

	public:
		RoutePlanner() { };
		~RoutePlanner() { };
		
		PlannerResult Plan(Graph& RoadGraph, GraphNode& Origin, GraphNode& Destination);

	private:
		double FindHighestSpeed(EdgeMap& Edges);
		void ReverseEdgeList(EdgeList& List);
		void ReconstructPath(EdgeList& Plan, Graph& RoadGraph, GraphNode& Origin, GraphNode& Destination);
		void ExpandNode(GraphNode& Expandee, Graph& RoadGraph, std::vector<long long>& Container);
		bool IsGoal(GraphNode& Testee, GraphNode& RealGoal);

		std::unordered_map<long long, double> BestCostMap;
		GraphNode* CurrentGoal;
		double CurrentMaxSpeed;

		friend class State;
		friend class StateComparator;

	//State class
	class State {
		private:
			double Cost;
		public:
			State(RoutePlanner* Parent, GraphNode* Node): Cost(0), Parent(Parent), Node(Node), PrecalculatedF(std::numeric_limits<double>::infinity()) {}
			State(const State& Other) { Parent = Other.Parent; Node = Other.Node; Cost = Other.Cost; PrecalculatedF = Other.PrecalculatedF; }
			void SetCost(double Cost) { this->Cost = Cost; PrecalculatedF = Cost + Estimate(*(Parent->CurrentGoal), Parent->CurrentMaxSpeed); }
			double CalculateF(GraphNode& Goal, double MaxSpeed) const { return PrecalculatedF; }
			double Estimate(GraphNode& Goal, double MaxSpeed) const { return EuclideanDistanceInKm(*Node, Goal)/(MaxSpeed); }
			bool operator==(State const& other) const { return (other.Node)==Node; }
			bool operator<(State const& other) const { return Node<other.Node; }

			RoutePlanner* Parent;
			GraphNode* Node;
			double PrecalculatedF;
			
			friend class StateComparator;
	};

	class StateComparator {
		public:
			bool operator()(State const& lhv, State const& rhv) const {
				//false for reflexevity in std::set
				if (lhv.Node->GetId() == rhv.Node->GetId()) return false;
				return lhv.CalculateF(*(lhv.Parent->CurrentGoal), lhv.Parent->CurrentMaxSpeed) <
					   rhv.CalculateF(*(rhv.Parent->CurrentGoal), rhv.Parent->CurrentMaxSpeed);
			}
	};
};

#endif
