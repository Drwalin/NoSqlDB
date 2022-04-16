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

#pragma once

#ifndef GENERIC_RED_BLACK_TREE_IMPL_HPP
#define GENERIC_RED_BLACK_TREE_IMPL_HPP

#include "GenericRedBlackTree.hpp"

namespace Generic {
	namespace Prototypes {
		inline void* Tree::Root() {
			return root;
		}
		inline void Tree::Root(void* newRoot) {
			root = newRoot;
		}

		inline uint64_t Node::Color(Tree* tree, void* node) {
			return ((Node*)node)->color;
		}
		inline void Node::Color(Tree* tree, void* node, uint64_t newColor) {
			((Node*)node)->color = newColor;
		}
		inline void* Node::Left(Tree* tree, void* node) {
			return ((Node*)node)->left;
		}
		inline void Node::Left(Tree* tree, void* node, void* newLeft) {
			((Node*)node)->left = (Node*)newLeft;
		}
		inline void* Node::Right(Tree* tree, void* node) {
			return ((Node*)node)->right;
		}
		inline void Node::Right(Tree* tree, void* node, void* newRight) {
			((Node*)node)->right = (Node*)newRight;
		}
		inline void* Node::Parent(Tree* tree, void* node) {
			return ((Node*)node)->parent;
		}
		inline void Node::Parent(Tree* tree, void* node, void* newParent) {
			((Node*)node)->parent = (Node*)newParent;
		}
		inline uint64_t Node::Value(Tree* tree, void* node) {
			return ((Node*)node)->value;
		}
	}
	
	template<typename T, typename N>
	inline uint64_t NodeImpl<T,N>::Color(T* tree) {
		return N::Color(tree, this);
	}
	template<typename T, typename N>
	void NodeImpl<T,N>::Color(T* tree, uint64_t newColor) {
		N::Color(tree, this, newColor);
	}
	template<typename T, typename N>
	inline NodeImpl<T,N>* NodeImpl<T,N>::Left(T* tree) {
		return (NodeImpl<T,N>*)N::Left(tree, this);
	}
	template<typename T, typename N>
	void NodeImpl<T,N>::Left(T* tree, NodeImpl<T,N>* newLeft) {
		N::Left(tree, this, newLeft);
	}
	template<typename T, typename N>
	inline NodeImpl<T,N>* NodeImpl<T,N>::Right(T* tree) {
		return (NodeImpl<T,N>*)N::Right(tree, this);
	}
	template<typename T, typename N>
	void NodeImpl<T,N>::Right(T* tree, NodeImpl<T,N>* newRight) {
		N::Right(tree, this, newRight);
	}
	template<typename T, typename N>
	inline NodeImpl<T,N>* NodeImpl<T,N>::Parent(T* tree) {
		return (NodeImpl<T,N>*)N::Parent(tree, this);
	}
	template<typename T, typename N>
	void NodeImpl<T,N>::Parent(T* tree, NodeImpl<T,N>* newParent) {
		N::Parent(tree, this, newParent);
	}
	template<typename T, typename N>
	uint64_t NodeImpl<T,N>::Value(T* tree) {
		return N::Value(tree, this);
	}
	
	template<typename T, typename N>
	inline NodeImpl<T,N>* NodeImpl<T,N>::Prev(T* tree) {
		NodeImpl<T,N>* node = Left(tree);
		if(node) {
			NodeImpl<T,N>* next = NULL;
			while(true) {
				next = node->Right(tree);
				if(next == NULL)
					return node;
				node = next;
			}
		} else {
			// TODO: continue search
			node = this;
			while(true) {
				NodeImpl<T,N>* parent = node->Parent(tree);
				if(parent == NULL)
					return NULL;
				if(parent->Right(tree) == node) {
					return parent;
				} else {
					node = parent;
				}
				node = parent;
			}
		}
	}

	template<typename T, typename N>
	inline NodeImpl<T,N>* NodeImpl<T,N>::Next(T* tree) {
		NodeImpl<T,N>* node = Right(tree);
		if(node) {
			NodeImpl<T,N>* next = NULL;
			while(true) {
				next = node->Left(tree);
				if(next == NULL)
					return node;
				node = next;
			}
		} else {
			// TODO: continue search
			node = this;
			while(true) {
				NodeImpl<T,N>* parent = node->Parent(tree);
				if(parent == NULL)
					return NULL;
				if(parent->Left(tree) == node) {
					return parent;
				} else {
					node = parent;
				}
			}
		}
	}
	
#define OFF(PTR) (((PTR)!=0)?(((uint64_t)(PTR))-((uint64_t)(tree->origin)))/64:0)

	template<typename T, typename N>
	inline NodeImpl<T,N>* NodeImpl<T,N>::FindGreaterEqual(T* tree, uint64_t value) {
		uint64_t thisValue = Value(tree);
		//printf(" search: %lu ?? %lu\n", OFF(thisValue), OFF(value));
		if(thisValue == value) {
			//printf(" search: %lu == %lu\n", OFF(thisValue), OFF(value));
			return this;
		} else if(thisValue > value) {
			NodeImpl<T,N>* left = Left(tree);
			//printf(" search: %lu > %lu   -> left %lu\n", OFF(thisValue), OFF(value), OFF(left));
			if(left)
				return left->FindGreaterEqual(tree, value);
			//printf(" search ret this\n");
			return this;
		} else {
			NodeImpl<T,N>* right = Right(tree);
			//printf(" search: %lu < %lu   -> right %lu\n", OFF(thisValue), OFF(value), OFF(right));
			if(right)
				return right->FindGreaterEqual(tree, value);
			NodeImpl<T,N>* next = Next(tree);
			//printf(" search ret next: %lu\n", OFF(next));
			return next;//->FindGreaterEqual(tree, value);
		}
	}

	
	template<typename T, typename N>
	inline NodeImpl<T,N>* NodeImpl<T,N>::LeftMost(T* tree) {
		if(Left(tree))
			return Left(tree)->LeftMost(tree);
		return this;
	}
	template<typename T, typename N>
	inline NodeImpl<T,N>* NodeImpl<T,N>::RightMost(T* tree) {
		if(Right(tree))
			return Right(tree)->RightMost(tree);
		return this;
	}
	
	
	
	
	
	
	template<typename T, typename N>
	void RedBlackTree<T,N>::InsertImpl(Node* node) {
		BSTInsert(node);
// 		return;
		if(node == Root()) {
			node->Color(tree, BLACK);
		} else {
			node->Color(tree, RED);
			while(node != Root()) {
				if(node->Parent(tree)->Color(tree) != RED)
					break;
				if(node->Parent(tree) == node->Parent(tree)->Parent(tree)->Left(tree)) {
					// TODO: Check if RIGHT and LEFT are correct
					node = RBTInsertFixUpForRightChildUncle(node);
				} else {
					// TODO: Check if RIGHT and LEFT are correct
					node = RBTInsertFixUpForLeftChildUncle(node);
				}
				Root()->Color(tree, BLACK);
			}
			Root()->Color(tree, BLACK);
		}
	}
	
	template<typename T, typename N>
	void RedBlackTree<T, N>::BSTInsert(Node* node) {
		node->Left(tree, NULL);
		node->Right(tree, NULL);
		node->Parent(tree, NULL);
		//printf(" inserting: %p (%p) -> (L %p, R %p)\n", node->Parent(tree), node, node->Left(tree), node->Right(tree));
		if(Root() == NULL) {
			Root(node);
		} else {
			Node* next = Root()->FindGreaterEqual(tree, node->Value(tree));
			if(next == NULL) {
				//printf(" Not Found GE of %lu\n", ((size_t)node-(size_t)tree->origin)/64);
				Node* last = Last();
				last->Right(tree, node);
				node->Parent(tree, last);
				//printf(" inserted 1: %p (%p) -> (L %p, R %p) under %p (%p) -> (L %p, R %p)\n", node->Parent(tree), node, node->Left(tree), node->Right(tree), last->Parent(tree), last, last->Left(tree), last->Right(tree));
			} else {
				//printf(" Found GE of %lu: %lu\n", ((size_t)node-(size_t)tree->origin)/64, ((size_t)next-(size_t)tree->origin)/64);
				if(next->Left(tree) == NULL) {
					next->Left(tree, node);
					node->Parent(tree, next);
					//printf(" inserted 2: %p (%p) -> (L %p, R %p) under %p (%p) -> (L %p, R %p)\n", node->Parent(tree), node, node->Left(tree), node->Right(tree), next->Parent(tree), next, next->Left(tree), next->Right(tree));
				} else {
					// TODO does it works here?
					Node* prev = next->Prev(tree);
					//printf(" Found prev of GE of: %lu\n", ((size_t)prev-(size_t)tree->origin)/64);
					prev->Right(tree, node);
					node->Parent(tree, prev);
					//printf(" inserted 3: %p (%p) -> (L %p, R %p) under %p (%p) -> (L %p, R %p)\n", node->Parent(tree), node, node->Left(tree), node->Right(tree), prev->Parent(tree), prev, prev->Left(tree), prev->Right(tree));
				}
			}
		}
	}
	
	template<typename T, typename N>
	NodeImpl<T, N>* RedBlackTree<T, N>::RBTInsertFixUpForRightChildUncle(Node* node) {
		Node* uncle = node->Parent(tree)->Parent(tree)->Right(tree);
		if(Color(uncle) == RED) {
			node->Parent(tree)->Color(tree, BLACK);
			uncle->Color(tree, BLACK);
			node->Parent(tree)->Parent(tree)->Color(tree, RED);
			node = node->Parent(tree)->Parent(tree);
		} else {
			if(node == node->Parent(tree)->Right(tree)) {
				node = node->Parent(tree);
				RotateLeft(node);
			}
			node->Parent(tree)->Color(tree, BLACK);
			node->Parent(tree)->Parent(tree)->Color(tree, RED);
			RotateRight(node->Parent(tree)->Parent(tree));
		}
		return node;
	}

	template<typename T, typename N>
	NodeImpl<T, N>* RedBlackTree<T, N>::RBTInsertFixUpForLeftChildUncle(Node* node) {
		Node* uncle = node->Parent(tree)->Parent(tree)->Left(tree);
		if(Color(uncle) == RED) {
			node->Parent(tree)->Color(tree, BLACK);
			uncle->Color(tree, BLACK);
			node->Parent(tree)->Parent(tree)->Color(tree, RED);
			node = node->Parent(tree)->Parent(tree);
		} else {
			if(node == node->Parent(tree)->Left(tree)) {
				node = node->Parent(tree);
				RotateRight(node);
			}
			node->Parent(tree)->Color(tree, BLACK);
			node->Parent(tree)->Parent(tree)->Color(tree, RED);
			RotateLeft(node->Parent(tree)->Parent(tree));
		}
		return node;
	}
	
	template<typename T, typename N>
	void RedBlackTree<T, N>::RotateLeft(Node* x) {
		Node* y = x->Right(tree);
		if(y == NULL)
			return ;
		x->Right(tree, y->Left(tree));
		if(y->Left(tree) != NULL)
			y->Left(tree)->Parent(tree, x);
		y->Parent(tree, x->Parent(tree));
		if(x->Parent(tree) == NULL)
			Root(y);
		else if(x == x->Parent(tree)->Left(tree))
			x->Parent(tree)->Left(tree, y);
		else
			x->Parent(tree)->Right(tree, y);
		y->Left(tree, x);
		x->Parent(tree, y);
	}

	template<typename T, typename N>
	void RedBlackTree<T, N>::RotateRight(Node* x) {
		Node* y = x->Left(tree);
		if(y == NULL)
			return ;
		x->Left(tree, y->Right(tree));
		if(y->Right(tree) != NULL)
			y->Right(tree)->Parent(tree, x);
		y->Parent(tree, x->Parent(tree));
		if(x->Parent(tree) == NULL)
			Root(y);
		else if(x == x->Parent(tree)->Right(tree))
			x->Parent(tree)->Right(tree, y);
		else
			x->Parent(tree)->Left(tree, y);
		y->Right(tree, x);
		x->Parent(tree, y);
	}
	
	
}

#endif

