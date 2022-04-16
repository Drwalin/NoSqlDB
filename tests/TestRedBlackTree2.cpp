/*
 *  This file is part of NoSqlDB.
 *  Copyright (C) 2022 Marek Zalewski aka Drwalin
 *
 *  ICon3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <RedBlackTreeAllocator.cpp>

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <random>
#include <chrono>
#include <set>
#include <map>
#include <unordered_set>

#include "Debug.hpp"

std::mt19937_64 rd;
std::uniform_int_distribution<uint64_t> dist(0llu, ~0llu);

constexpr auto FIELD = RedBlackTreeAllocator::POINTER_FIELD;
constexpr auto FIELD2 = RedBlackTreeAllocator::SIZE_FIELD;
constexpr uint64_t elements = 1024;
constexpr uint64_t cc = 16;
using Node = RedBlackTreeAllocator::Node;
RedBlackTreeAllocator::Node* nodes = (decltype(nodes))malloc((elements+100)*64);
Node** all = new Node*[elements];
uint64_t used = 0;

// std::unordered_set<Node*> notUsedNodes;

Node* PopRandomNode() {
	Node* node = all[used];
	used++;
	return node;
	/*
	for(int i=0; i<10; ++i) {
		uint64_t ll = dist(rd);
		uint64_t id = ll%elements;
		id++;
		Node* node = nodes+id+1;
		if(notUsedNodes.count(node)) {
			notUsedNodes.erase(node);
			return node;
		}
	}
	Node* node = *notUsedNodes.begin();
	notUsedNodes.erase(node);
	return node;
	*/
}

template<uint64_t capacity>
class HashSet {
public:
	
	struct N {
		uint64_t padding[6];
		struct N* next, *prev;
	};
	
	N** hashtable;
	
	HashSet() {
		hashtable = new N*[capacity];
		for(uint64_t i=0; i<capacity; ++i) {
			hashtable[i] = NULL;
		}
	}
	~HashSet() {
		delete hashtable;
	}
	
	inline void Insert(Node* node_) {
		N* node = (N*)node_;
		node->prev = NULL;
		node->next = NULL;
		uint64_t key = (uint64_t)node;
		uint64_t hash = (key^14695981039346656037llu)*1099511628211llu;
		
		uint64_t id = hash % capacity;
		
		if(hashtable[id]) {
			N* n = hashtable[id];
			while(n->next) {
				n = n->next;
			}
			n->next = node;
			node->prev = n;
		} else {
			hashtable[id] = node;
		}
		
		/*
		node->next = hashtable[id];
		hashtable[id] = node;
		if(node->next) {
			node->next->prev = node;
		}
		*/
	}
	
};

#include <GenericRedBlackTree.hpp>

struct TreeAccessor {
	TreeAccessor() {
		origin = NULL;
		root = NULL;
	}
	void* origin;
	Node* root;
	
	Node* Root() {
		return root;
	}
	void Root(void* newRoot) {
		root = (Node*)newRoot;
	}
};

struct NodeAccessor {
	static uint64_t Color(TreeAccessor* tree, void* node) {
		return ((Node*)node)->Color<FIELD2>();
	}
	static void Color(TreeAccessor* tree, void* node, uint64_t newColor) {
		((Node*)node)->Color<FIELD2>(newColor);
	}
	static void* Left(TreeAccessor* tree, void* node) {
		return ((Node*)node)->Left<FIELD2>(tree->origin);
	}
	static void Left(TreeAccessor* tree, void* node, void* newLeft) {
		((Node*)node)->Left<FIELD2>(tree->origin, (Node*)newLeft);
	}
	static void* Right(TreeAccessor* tree, void* node) {
		return ((Node*)node)->Right<FIELD2>(tree->origin);
	}
	static void Right(TreeAccessor* tree, void* node, void* newRight) {
		((Node*)node)->Right<FIELD2>(tree->origin, (Node*)newRight);
	}
	static void* Parent(TreeAccessor* tree, void* node) {
		return ((Node*)node)->Parent<FIELD2>(tree->origin);
	}
	static void Parent(TreeAccessor* tree, void* node, void* newParent) {
		((Node*)node)->Parent<FIELD2>(tree->origin, (Node*)newParent);
	}
	static uint64_t Value(TreeAccessor* tree, void* node) {
		return ((Node*)node)->GetValue<FIELD2>();
	}
};

int main() {
	Start();
	SEARCH = false;
	RedBlackTreeAllocator rbtree, *rbtrees = new RedBlackTreeAllocator[cc];
	Generic::RedBlackTree<TreeAccessor, NodeAccessor> grbt;
	TreeAccessor ttt;
	grbt.tree = &ttt;
	ttt.origin = nodes;
	
	
	rbtree.SetOrigin(nodes);
	for(int i=0; i<cc; ++i) {
		rbtrees[i].SetOrigin(nodes);
	}
	HashSet</*134217757llu*/268435459llu> test_hs;
	End();
	printf(" Init 1 took: %.2f s\n", DeltaTime());
	Start();
	
	/*
	std::unordered_multiset<Node*> test_us;
	test_us.reserve(elements);
	std::multiset<Node*> test_s;
	std::multimap<Node*, Node*> test_m;
	*/
	for(int i=0; i<elements; ++i) {
		all[i] = nodes+i+1;
		all[i]->size = 0;
	}
	End();
	printf(" Init 2 took: %.2f s\n", DeltaTime());
	Start();
	std::shuffle(all, all+elements, rd);
	End();
	printf(" Init 3 took: %.2f s\n", DeltaTime());
	/*
	notUsedNodes.reserve(elements+100);
	for(int i=0; i<elements; ++i) {
		notUsedNodes.insert(nodes+i+1);
	}
	*/
	
	
	
	for(int i=0; i<10; ++i)
		grbt.Insert(all[i]);
	
	SEARCH = true;
	ttt.root->VerifyParenting<FIELD2>(true, nodes);
	
	if(false)
	{
		float times[32];
		uint64_t counts[32];
		Start();
		uint64_t I=0;
		for(int J=0; J<32; ++J) {
			for(uint64_t i=0; i<cc; ++i) {
				for(uint64_t j=0; j<1; ++j, ++I) {
					rbtrees[i].Insert(all[I]);
				}
			}
			End();
			times[J] = DeltaTime();
			counts[J] = I;
		}
		for(int J=0; J<32; ++J) {
			float time = times[J];
			I = counts[J];
			printf(" rbtree small trees (%i) inserting: %lu nodes in %.2fs -> %.3f kops\n", J+1, I, time, I/(time*1000.0f));
		}
	}
	
	uint64_t invalid_parenting;
	/*
	for(int i=0; i<16; ++i) {
		rbtree.RBTInsert<FIELD>(PopRandomNode());
	}
	invalid_parenting = rbtree.Root<FIELD>()->VerifyParenting<FIELD>(true, nodes);
	printf(" invalid parenting: %lu\n", invalid_parenting);
	rbtree.Root<FIELD>()->Print<FIELD>(nodes);
	rbtree.RotateLeft<FIELD>(rbtree.Root<FIELD>());
	rbtree.Root<FIELD>()->Print<FIELD>(nodes);
	rbtree.RotateRight<FIELD>(rbtree.Root<FIELD>());
	rbtree.Root<FIELD>()->Print<FIELD>(nodes);
	*/
	
// 	return 0;
	
	float time;
	for(int j=0; j<5/*elements/cc*/; ++j) {
		printf(" ----\n");
		SEARCH = false;
		
		Start();
		for(uint64_t i=0; i<cc; ++i) {
			grbt.Insert(PopRandomNode());
		}
		End();
		time = DeltaTime();
		printf(" grbt inserted %lu nodes in %.2fs -> %.3f kops\n", cc, time, cc/(time*1000.0f));
		
		Start();
		for(uint64_t i=0; i<cc; ++i) {
			rbtree.RBTInsert<FIELD>(PopRandomNode());
		}
		End();
		time = DeltaTime();
		printf(" rbtree inserted %lu nodes in %.2fs -> %.3f kops\n", cc, time, cc/(time*1000.0f));
		
		/*
		Start();
		for(uint64_t i=0; i<cc; ++i) {
			test_us.insert(all[used-cc+i]);
		}
		End();
		time = DeltaTime();
		printf(" std::unordered_set inserted %lu nodes in %.2fs -> %.3f kops\n", cc, time, cc/(time*1000.0f));
		
		Start();
		for(uint64_t i=0; i<cc; ++i) {
			test_s.insert(all[used-cc+i]);
		}
		End();
		time = DeltaTime();
		printf(" std::set inserted %lu nodes in %.2fs -> %.3f kops\n", cc, time, cc/(time*1000.0f));
		
		Start();
		for(uint64_t i=0; i<cc; ++i) {
			test_m.insert({all[used-cc+i], NULL});
		}
		End();
		time = DeltaTime();
		printf(" std::map inserted %lu nodes in %.2fs -> %.3f kops\n", cc, time, cc/(time*1000.0f));
		*/
		
		Start();
		for(uint64_t i=0; i<cc; ++i) {
			test_hs.Insert(all[used-cc+i]);
		}
		End();
		time = DeltaTime();
		printf(" HashTable inserted %lu nodes in %.2fs -> %.3f kops\n", cc, time, cc/(time*1000.0f));
		
		
		
		uint64_t invalid_parenting = rbtree.Root<FIELD>()->VerifyParenting<FIELD>(false, nodes);
		printf(" rbta invalid parenting: %lu\n", invalid_parenting);
		
		SEARCH = false;
		auto it = rbtree.FindGreaterEqual<FIELD>(0);
		uint64_t prev = 0;
		uint64_t invalid_order = 0;
		for(; it; it = it->Next<FIELD>(nodes)) {
			if(prev > it->GetValue<FIELD>()) {
				++invalid_order;
			}
			prev = it->GetValue<FIELD>();
		}
		printf("\n rbta invalid ordering: %lu\n", invalid_order);
		
		
// 		rbtree.Root<FIELD>()->Print<FIELD>(nodes);
		ttt.root->VerifyParenting<FIELD2>(false, nodes);
		printf(" grbt invalid parenting: %lu\n", invalid_parenting);
		
		prev = 0;
		invalid_order = 0;
		for(auto it=grbt.begin(); it!=grbt.end(); ++it) {
			if(prev > it.node->Value(&ttt)) {
				++invalid_order;
			}
			prev = it.node->Value(&ttt);
		}
		printf("\n grbt invalid ordering: %lu\n", invalid_order);
		
		/*
		printf("\n");
		Node* it = rbtree.FindGreaterEqual<FIELD>(0);
		for(; it; it = it->Next<FIELD>(nodes)) {
			printf("  %lu", ((size_t)it-(size_t)nodes)/64);
		}
		printf("\n");
		it = rbtree.FindGreaterEqual<FIELD>(0);
		uint64_t prev = 0;
		for(; it; it = it->Next<FIELD>(nodes)) {
			if(prev > it->GetValue<FIELD>()) {
				printf(" %lu > %lu ", prev, it->GetValue<FIELD>());
			}
			prev = it->GetValue<FIELD>();
		}
		printf("\n");
		*/
		
		
		uint64_t height = rbtree.Root<FIELD>()->Height<FIELD>(nodes);
		uint64_t minHeight = rbtree.Root<FIELD>()->MinHeight<FIELD>(nodes);
		uint64_t size = used;//elements - notUsedNodes.size();
		printf(" elments: %.2fM -> height: %lu, minHeight: %lu\n", size/1000000.0f, height, minHeight);
	}
	
	invalid_parenting = rbtree.Root<FIELD>()->VerifyParenting<FIELD>(false, nodes);
		auto it = rbtree.FindGreaterEqual<FIELD>(0);
		uint64_t prev = 0;
		uint64_t invalid_order = 0;
		for(; it; it = it->Next<FIELD>(nodes)) {
			if(prev > it->GetValue<FIELD>()) {
				++invalid_order;
			}
			prev = it->GetValue<FIELD>();
		}
		printf("\n invalid ordering: %lu\n", invalid_order);
	printf(" invalid parenting: %lu\n", invalid_parenting);
	
	return 0;
}

