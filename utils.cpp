#include "utils.h"
#include <stdlib.h>

int rand_range(int min, int max){
	return rand()%(max-min) + min;
}

UnionFind::UnionFind(int num){
	array = new int[num];
	rank = new int[num];
	for(int i = 0; i<num; i++){
		array[i] = i;
		rank[i] = 0;
	}
}
UnionFind::~UnionFind(){
	delete[] array;
	delete[] rank;
}
bool UnionFind::unite(int i, int j){
	i = ancestor(i);
	j = ancestor(j);
	if(i==j) return false;
	
	if(rank[i] < rank[j]) array[i] = j;
	else{
		array[j] = i;
		if(rank[i]==rank[j]) rank[i]++;
	}
	return true;
}
int UnionFind::ancestor(int i){
	int anc = i;
	while(anc != array[anc]) anc = array[anc];
	while(i!=anc){
		int n = array[i];
		array[i] = anc;
		i = n;
	}
	return anc;
}
bool UnionFind::same(int i, int j){
	return ancestor(i)==ancestor(j);
}
