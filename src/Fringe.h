#ifndef _FRINGE_GUARD
#define _FRINGE_GUARD

#include <set>

// Simple implementation of a priority queue in terms of std::set functionality
// Performance could be made better by using a heap
template <class T, class Compare>
class Fringe
{
	public:
		Fringe() : Counter(0) {}

		void Add(T Item);
		T Pop();

		bool Contains(const T& Item) const;
		bool Empty() const;

		void Remove(const T& Item);
		void Update(const T& Item);
		int GetCounter() const { return Counter; }
		int Size() const { return Container.size(); }

		typename std::set<T, Compare>::iterator Find(const T& Item) const;

	private:
		int Counter;
		std::set<T, Compare> Container;
		std::set<T> ContainsContainer;
};

template <class T, class Compare>
void Fringe<T, Compare>::Add(T Item) {
	Container.insert(Item);
	ContainsContainer.insert(Item);
	Counter++;
}

template <class T, class Compare>
bool Fringe<T, Compare>::Empty() const {
	return Container.empty();
}

template <class T, class Compare>
T Fringe<T, Compare>::Pop() {
	typename set<T, Compare>::iterator First = Container.begin();
	T Temp = *First;
	Container.erase(First);
	ContainsContainer.erase(Temp);
	return Temp;
}

template <class T, class Compare>
bool Fringe<T, Compare>::Contains(const T& Item) const {
	return ContainsContainer.find(Item)!=ContainsContainer.end();
}

template <class T, class Compare>
typename std::set<T, Compare>::iterator Fringe<T, Compare>::Find(const T& Item) const {
	for (typename std::set<T, Compare>::iterator it = Container.begin(); it != Container.end(); ++it) {
		if ((*it) == Item) {
			return it;
		}
	}
	return Container.end();
}

template <class T, class Compare>
void Fringe<T, Compare>::Remove(const T& Item) {
	Container.erase(Find(Item));
	ContainsContainer.erase(Item);
}

template <class T, class Compare>
void Fringe<T, Compare>::Update(const T& Item) {
	T Copy = Item;
	Remove(Item);
	Container.insert(Copy);
	ContainsContainer.insert(Copy);
}

#endif
