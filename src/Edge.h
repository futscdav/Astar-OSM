#ifndef _EDGE_GUARD
#define _EDGE_GUARD

#include <unordered_map>

class EdgeId {
	public:
		EdgeId(const long long FromNodeId, const long long ToNodeId) : FromNodeId(FromNodeId), ToNodeId(ToNodeId) { }
		inline long long operator()() const { return (FromNodeId+ToNodeId); } // ad hoc hashing function
		friend bool operator<(const EdgeId& lhs, const EdgeId& rhs) { return lhs.FromNodeId+lhs.ToNodeId<rhs.FromNodeId+rhs.ToNodeId; }
		friend bool operator==(const EdgeId& lhs, const EdgeId& rhs) { return lhs.FromNodeId==rhs.FromNodeId&&rhs.ToNodeId==lhs.ToNodeId; }
		friend class EdgeIdComparator;
	private:
		const long long FromNodeId;
		const long long ToNodeId;
};

class EdgeIdComparator
{
	public:
		inline bool operator()(const EdgeId* obj1, const EdgeId* obj2) const {
			return *obj1 == *obj2;
		}
};

namespace std {
	template<>
	struct hash<EdgeId*> {
		size_t operator() (const EdgeId* obj) const { return (size_t)(*obj)(); }
	};
}

class Edge {
	public:
		Edge(const long long FromNodeId, const long long ToNodeId, const double LengthInMetres, const double AllowedMaxSpeedInKmph) 
			: FromNodeId(FromNodeId), ToNodeId(ToNodeId), LengthInMetres(LengthInMetres), AllowedMaxSpeedInKmph(AllowedMaxSpeedInKmph)
			{ }
		~Edge() { };

		long long		GetFromNodeId() const				{ return FromNodeId; }
		long long		GetToNodeId() const					{ return ToNodeId; }
		double			GetLengthInMetres() const			{ return LengthInMetres; }
		double 			GetAllowedMaxSpeedInKmph() const	{ return AllowedMaxSpeedInKmph; }
		const EdgeId	GetEdgeId() const					{ return EdgeId(FromNodeId, ToNodeId); }

	protected:
		const long long FromNodeId;
		const long long ToNodeId;
		const double	LengthInMetres;
		const double 	AllowedMaxSpeedInKmph;
};

#endif
