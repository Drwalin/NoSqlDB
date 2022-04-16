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

#include "RedBlackTreeAllocator.hpp"

bool SEARCH = false;

RedBlackTreeAllocator::RedBlackTreeAllocator() {
	pointerRootId = 0;
	sizeRootId = 0;
	origin = 0;
	pointerRoot = 0;
	sizeRoot = 0;
}

RedBlackTreeAllocator::~RedBlackTreeAllocator() {
}


uint64_t RedBlackTreeAllocator::Allocate(uint64_t size, uint64_t* allocatedSize) {
	Node* node = FindGreaterEqual<SIZE_FIELD>(size);
	if(!node) {
		// TODO: implement MergeBlocks()
		return 0;
// 		MergeBlocks();
		node = FindGreaterEqual<SIZE_FIELD>(size);
		if(node == NULL)
			return 0;
	}
	if(allocatedSize)
		*allocatedSize = node->size;
	Erase(node);
	return GetNodeOffset(origin, node);
}

void RedBlackTreeAllocator::Free(uint64_t ptr, uint64_t size) {
	if(ptr) {
		Node* node = GetNodeFromPtr(origin, ptr);
		node->size = RoundSize(size);
		Insert(node);
		Merge(node);
	}
}


void RedBlackTreeAllocator::Merge(Node* node) {
	Node* next = node->Next<POINTER_FIELD>(origin);
	if(next) {
		if(GetNodeOffset(node, next) == node->size) {
			uint64_t newSize = node->size + next->size;
			Erase(next);
			node->size = newSize;
		}
	}
	Node* prev = node->Prev<POINTER_FIELD>(origin);
	if(prev) {
		if(GetNodeOffset(prev, node) == prev->size) {
			uint64_t newSize = node->size + prev->size;
			Erase(node);
			prev->size = newSize;
		}
	}
}

void RedBlackTreeAllocator::Insert(Node* node) {
	RBTInsert<POINTER_FIELD>(node);
	RBTInsert<SIZE_FIELD>(node);
}

void RedBlackTreeAllocator::Erase(Node* node) {
	RBTErase<POINTER_FIELD>(node);
	RBTErase<SIZE_FIELD>(node);
}



template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
void RedBlackTreeAllocator::RBTInsert(Node* node) {
	BSTInsert<field>(node);
	if(node == Root<field>()) {
		node->Color<field>(BLACK);
	} else {
		node->Color<field>(RED);
		while(node != Root<field>()) {
			if(node->Parent<field>(origin)->template Color<field>() != RED)
				break;
			if(node->Parent<field>(origin) == node->Parent<field>(origin)->template Parent<field>(origin)->template Left<field>(origin)) {
				// TODO: Check if RIGHT and LEFT are correct
				node = RBTInsertFixUpForRightChildUncle<field>(node);
			} else {
				// TODO: Check if RIGHT and LEFT are correct
				node = RBTInsertFixUpForLeftChildUncle<field>(node);
			}
			Root<field>()->template Color<field>(BLACK);
		}
		Root<field>()->template Color<field>(BLACK);
	}
}

template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
void RedBlackTreeAllocator::BSTInsert(Node* node) {
	node->Left<field>(origin, NULL);
	node->Right<field>(origin, NULL);
	if(Root<field>() == NULL) {
		Root<field>(node);
		node->Parent<field>(origin, NULL);
	} else {
		Node* next = FindGreaterEqual<field>(node->GetValue<field>());
		if(next == NULL) {
			if(SEARCH)
				printf(" Not Found GE of %lu\n", ((size_t)node-(size_t)origin)/64);
			Node* last = Last<field>();
			last->Right<field>(origin, node);
			node->Parent<field>(origin, last);
		} else {
			if(SEARCH)
				printf(" Found GE of %lu: %lu\n", ((size_t)node-(size_t)origin)/64, ((size_t)next-(size_t)origin)/64);
			if(next->Left<field>(origin) == NULL) {
				next->Left<field>(origin, node);
				node->Parent<field>(origin, next);
			} else {
				// TODO dos it works here?
				Node* prev = next->Prev<field>(origin);
				if(SEARCH)
					printf(" Found prev of GE of: %lu\n", ((size_t)prev-(size_t)origin)/64);
				prev->Right<field>(origin, node);
				node->Parent<field>(origin, prev);
			}
		}
	}
}

template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
RedBlackTreeAllocator::Node* RedBlackTreeAllocator::RBTInsertFixUpForRightChildUncle(Node* node) {
	Node* uncle = node->Parent<field>(origin)->template Parent<field>(origin)->template Right<field>(origin);
	if(Color<field>(uncle) == RED) {
		node->Parent<field>(origin)->template Color<field>(BLACK);
		uncle->Color<field>(BLACK);
		node->Parent<field>(origin)->template Parent<field>(origin)->template Color<field>(RED);
		node = node->Parent<field>(origin)->template Parent<field>(origin);
	} else {
		if(node == node->Parent<field>(origin)->template Right<field>(origin)) {
			node = node->Parent<field>(origin);
			RotateLeft<field>(node);
		}
		node->Parent<field>(origin)->template Color<field>(BLACK);
		node->Parent<field>(origin)->template Parent<field>(origin)->template Color<field>(RED);
		RotateRight<field>(node->Parent<field>(origin)->template Parent<field>(origin));
	}
	return node;
}

template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
RedBlackTreeAllocator::Node* RedBlackTreeAllocator::RBTInsertFixUpForLeftChildUncle(Node* node) {
	Node* uncle = node->Parent<field>(origin)->template Parent<field>(origin)->template Left<field>(origin);
	if(Color<field>(uncle) == RED) {
		node->Parent<field>(origin)->template Color<field>(BLACK);
		uncle->Color<field>(BLACK);
		node->Parent<field>(origin)->template Parent<field>(origin)->template Color<field>(RED);
		node = node->Parent<field>(origin)->template Parent<field>(origin);
	} else {
		if(node == node->Parent<field>(origin)->template Left<field>(origin)) {
			node = node->Parent<field>(origin);
			RotateRight<field>(node);
		}
		node->Parent<field>(origin)->template Color<field>(BLACK);
		node->Parent<field>(origin)->template Parent<field>(origin)->template Color<field>(RED);
		RotateLeft<field>(node->Parent<field>(origin)->template Parent<field>(origin));
	}
	return node;
}





template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
void RedBlackTreeAllocator::RBTErase(Node* node) {
	if(node->Left<field>(origin) == NULL && node->Right<field>(origin) == NULL) {
		if(node->Parent<field>(origin) == NULL) {
			Root<field>(NULL);
		} else {
			Node* parent = node->Parent<field>(origin);
			if(parent->Left<field>(origin) == node)
				parent->Left<field>(origin, NULL);
			else
				parent->Right<field>(origin, NULL);

			if(node->Color<field>() == BLACK) {
				// TODO
			} else {
				return;
			}
		}
	} else if(node->Left<field>(origin) == NULL || node->Right<field>(origin) == NULL) {
		Node* child = node->Left<field>(origin) ? node->Left<field>(origin) : node->Right<field>(origin);
		if(node->Parent<field>(origin) == NULL) {
			Root<field>(child);
			child->Parent<field>(origin, NULL);
		} else {
			Node* parent = node->Parent<field>(origin);
			if(parent->Left<field>(origin) == node)
				parent->Left<field>(origin, child);
			else
				parent->Right<field>(origin, child);
			child->Parent<field>(origin, parent);
		}
		
		if(child->Color<field>() == node->Color<field>()) {
			// TODO
			
			
		} else {
			child->Color<field>(BLACK);
			return;
		}
	} else {
		Node* next = node->Next<field>(origin);
		RBTErase<field>(next);
		next->Left<field>(origin, node->Left<field>(origin));
		next->Right<field>(origin, node->Right<field>(origin));
		next->Color<field>(node->Color<field>());
		next->Parent<field>(origin, node->Parent<field>(origin));
	}
}

template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
RedBlackTreeAllocator::Node* RedBlackTreeAllocator::FindGreaterEqual(uint64_t value) {
	Node* const root = Root<field>();
	if(root)
		return root->FindGreaterEqual<field>(origin, value);
	else
		return NULL;
}

template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
void RedBlackTreeAllocator::RotateLeft(Node* x) {
	Node* y = x->Right<field>(origin);
	if(y == NULL)
		return ;
	x->Right<field>(origin, y->Left<field>(origin));
	if(y->Left<field>(origin) != NULL)
		y->Left<field>(origin)->template Parent<field>(origin, x);
	y->Parent<field>(origin, x->Parent<field>(origin));
	if(x->Parent<field>(origin) == NULL)
		Root<field>(y);
	else if(x == x->Parent<field>(origin)->template Left<field>(origin))
		x->Parent<field>(origin)->template Left<field>(origin, y);
	else
		x->Parent<field>(origin)->template Right<field>(origin, y);
	y->Left<field>(origin, x);
	x->Parent<field>(origin, y);
}

template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
void RedBlackTreeAllocator::RotateRight(Node* x) {
	Node* y = x->Left<field>(origin);
	if(y == NULL)
		return ;
	x->Left<field>(origin, y->Right<field>(origin));
	if(y->Right<field>(origin) != NULL)
		y->Right<field>(origin)->template Parent<field>(origin, x);
	y->Parent<field>(origin, x->Parent<field>(origin));
	if(x->Parent<field>(origin) == NULL)
		Root<field>(y);
	else if(x == x->Parent<field>(origin)->template Right<field>(origin))
		x->Parent<field>(origin)->template Right<field>(origin, y);
	else
		x->Parent<field>(origin)->template Left<field>(origin, y);
	y->Right<field>(origin, x);
	x->Parent<field>(origin, y);
}









template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
RedBlackTreeAllocator::Node* RedBlackTreeAllocator::Node::Prev(void* origin) {
	Node* node = Left<field>(origin);
	if(node) {
		Node* next = NULL;
		while(true) {
			next = node->Right<field>(origin);
			if(next == NULL)
				return node;
			node = next;
		}
	} else {
		// TODO: continue search
		node = this;
		while(true) {
			Node* parent = node->Parent<field>(origin);
			if(parent == NULL)
				return NULL;
			if(parent->Right<field>(origin) == node) {
				return parent;
			} else {
				node = parent;
			}
			node = parent;
		}
	}
	
}

template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
RedBlackTreeAllocator::Node* RedBlackTreeAllocator::Node::Next(void* origin) {
	Node* node = Right<field>(origin);
	if(node) {
		Node* next = NULL;
		while(true) {
			next = node->Left<field>(origin);
			if(next == NULL)
				return node;
			node = next;
		}
	} else {
		// TODO: continue search
		node = this;
		while(true) {
			Node* parent = node->Parent<field>(origin);
			if(parent == NULL)
				return NULL;
			if(parent->Left<field>(origin) == node) {
				return parent;
			} else {
				node = parent;
			}
		}
	}
}

template<RedBlackTreeAllocator::RedBlackTreeNode RedBlackTreeAllocator::Node::*field>
RedBlackTreeAllocator::Node* RedBlackTreeAllocator::Node::FindGreaterEqual(void* origin, const uint64_t value) {
	uint64_t thisValue = GetValue<field>();
	if(thisValue == value) {
		if(SEARCH)
			printf(" search: %lu == %lu\n", (thisValue-(uint64_t)origin)/64, GetNodeOffset(origin, (void*)value)/64);
		return this;
	} else if(thisValue > value) {
		if(SEARCH)
			printf(" search: %lu > %lu\n", (thisValue-(uint64_t)origin)/64, GetNodeOffset(origin, (void*)value)/64);
		Node* left = Left<field>(origin);
		if(left)
			return left->FindGreaterEqual<field>(origin, value);
		if(SEARCH)
			printf(" search ret this\n");
		return this;
	} else {
		if(SEARCH)
			printf(" search: %lu < %lu\n", (thisValue-(uint64_t)origin)/64, GetNodeOffset(origin, (void*)value)/64);
		Node* right = Right<field>(origin);
		if(right)
			return right->FindGreaterEqual<field>(origin, value);
		Node* next = Next<field>(origin);
		if(SEARCH)
			printf(" search ret next: %lu\n", GetNodeOffset(origin, next)/64);
		return next;
	}
}

