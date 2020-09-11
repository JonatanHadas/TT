#ifndef _UTILS_H
#define _UTILS_H

int rand_range(int min, int max);

class UnionFind{
	int* array;
	int* rank;
	int ancestor(int i);
public:
	UnionFind(int num);
	~UnionFind();
	bool unite(int i, int j);
	bool same(int i, int j);
};

class Data{
public:
	virtual ~Data(){}
};

#endif