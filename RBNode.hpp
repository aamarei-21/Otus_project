#ifndef RBNODE_HPP
#define RBNODE_HPP

#include "pair.hpp"
#include <iostream>
#include <memory>

enum {BLACK, RED};

template<class T, class Allocator = std::allocator<T> >
class RBNode {
private:
public:
	typedef T				value_type;
	typedef Allocator		allocator_type;
	typedef typename allocator_type::template rebind<RBNode>::other		allocator_node;

	value_type 		_Val;
	RBNode*			_parent;
	RBNode*			_left;
	RBNode*			_right;
	bool 			_color; // 0 - black, 1 - red
	allocator_node alloc;

	RBNode* grandfather(){ //поиск дедушки
		if(!_parent || !_parent->_parent)
			return NULL; //нет дедушки
		return _parent->_parent;
	}

	RBNode* uncle(){//поиск дяди
		RBNode* gr = grandfather();
		if(!gr)
			return NULL; //нет дедушки, а значит нет и дяди
		if(gr->_left == _parent)
			return gr->_right;
		return gr->_left;
	}

	RBNode* brother(){
		RBNode* br = NULL;
		if (_parent)
			br = (this->_parent->_left == this) ? _parent->_right : _parent->_left;
		return br;
	}

	RBNode* search_min(RBNode* node){
		RBNode* temp = node;
		while (temp->_left->_left)
			temp = temp->_left;
		return temp;
	}

	RBNode* search_max(RBNode* node){
		RBNode* temp = node;
		while (temp->_right->_right)
			temp = temp->_right;
		return temp;
	}

	RBNode* searchNode(const value_type val) {
		RBNode* temp = this;
		while (temp) {
			if (val < temp->_Val)
				temp = temp->_left;
			else if (val > temp->_Val)
				temp = temp->_right;
			else
				return temp;
		}
		return NULL;
	}

	template<class U>
			void swap(U& left, U& right){
		U temp = left;
		left = right;
		right = temp;
	}

public:
	RBNode() : _Val(value_type()), _parent(NULL), _left(NULL), _right(NULL), _color(1), alloc() {}
	RBNode(value_type const& arg) : _Val(arg), _parent(NULL), _left(NULL), _right(NULL), _color(1), alloc() {}

	RBNode(RBNode const& other) : _Val(other._Val), _parent(NULL), _left(NULL), _right(NULL), _color(other._color), alloc() {
		if (other._left){
			this->_left = alloc.allocate(1);
			alloc.construct(this->_left, *other._left);
			if (other._left->_parent)
				this->_left->_parent = this;
		}
		if (other._right){
			this->_right = alloc.allocate(1);
			alloc.construct(this->_right, *other._right);
			if (other._right->_parent)
				this->_right->_parent = this;
		}
	}

	~RBNode(){
		if (this->_left){
			alloc.destroy(this->_left);
			alloc.deallocate(this->_left, 1);
		}
		if (this->_right){
			alloc.destroy(this->_right);
			alloc.deallocate(this->_right, 1);
		}
	}

	RBNode* min_node(){
		RBNode* temp = this;
		while(temp->_left)
			temp = temp->_left;
		return temp;
	}

	RBNode* max_node(){
		RBNode* temp = this;
		while(temp->_right)
			temp = temp->_right;
		return temp;
	}

	RBNode* add_node(value_type const& arg){ //, RBNode* sheet, RBNode* first, RBNode* last){
		//RBNode* temp;
		if(arg < _Val){ // идем в левое поддерево
			if(_left == NULL) { //first || _left == sheet){
				//temp = _left;
				_left = new RBNode(arg);
				_left->_parent = this;
				_left->_left = NULL;
				//temp->_parent = _left;
				_left->_right = NULL;
				_left->_color = 1;
				return _left;
			}
			else
				return _left->add_node(arg);
		}
		else{ //идем в правое поддерево
			if(_right == NULL) { //last || _right == sheet){
				//temp = _right;
				_right = new RBNode(arg);
				_right->_parent = this;
				_right->_right = NULL;
				//temp->_parent = _right;
				_right->_left = NULL;
				_right->_color = 1;
				return _right;
			}
			else
				return _right->add_node(arg);
		}
	}

	RBNode* decrem(){
		RBNode* temp = this;
		if(temp->_left and temp->_left->_parent){
			temp = temp->_left;
			while(temp->_right and temp->_right->_left)
				temp = temp->_right;
			return temp;
		}
		else if(temp->_parent && temp->_parent->_right == temp){
			temp = temp->_parent;
			return temp;
		}
		while(temp->_parent && temp->_parent->_left == temp)
			temp = temp->_parent;
		temp = temp->_parent;
		return temp;
	}

	RBNode* increm(){
		RBNode* temp = this;
		if(temp->_right) {// and temp->_right->_parent){
			temp = temp->_right;
			while(temp->_left) // and temp->_left->_left)
				temp = temp->_left;
			return temp;
		}
		else if(temp->_parent && temp->_parent->_left == temp) {
			temp = temp->_parent;
			return temp;
		}
		while(temp->_parent && temp->_parent->_right == temp)
			temp = temp->_parent;
		return temp->_parent;
	}

	RBNode *delete_node(RBNode *node){
		RBNode* parent = node->_parent;
		if (parent){ // Удаляемый узел - не корень
			if(parent->_left == node) { //Удаляемый узел слева
				if(node->_right) {
					RBNode* temp = node->_right;
					temp = search_min(temp);
					if (temp->_parent->_right == temp) {
						temp->_parent = parent;
						parent->_left = temp;
						temp->_left = node->_left;
					}
					else {
						node->_Val = temp->_Val;
						node = temp;
					}
				} 
				else{
					parent->_left = node->_left;
					if (node->_left)
						node->_left->_parent = parent;
				}
			} 
			else { // parent->_right == node  - Удаляемый узел справа
				if (node->_left) {
					RBNode* temp = node->_left;
					temp = search_max(temp);
					if (temp->_parent->_left == temp) {
						temp->_parent = parent;
						parent->_right = temp;
						temp->_right = node->_right;
					}
					else {
						node->_Val = temp->_Val;
						node = temp;
					}
				}
				else{
					parent->_right = node->_right;
					if (node->_right)
						node->_right->_parent = parent;
				}
			}
			node->_parent = NULL;
			node->_left = NULL;
			node->_right = NULL;
			alloc.destroy(node);
			alloc.deallocate(node, 1);
		}
		else{ // Удаляемый узел - корень 
			parent = node;
			if (node->_right) {
				RBNode* temp = node->_right;
				if (temp->_left) {
					while (temp && temp->_left)
						temp = temp->_left;
					swap(_Val, temp->_Val);
					_color = 0;
					// temp->_left = node->_left;
					// node->_right->_parent = NULL;
					// parent = node->_right;
					temp->_parent->_left = temp->_right;
					if (temp->_right)
						temp->_right->_parent = temp->_parent;
					
				}
				else {
					swap(_Val, temp->_Val);
					node->_right = temp->_right;
					if (temp->_right) 
						temp->_right->_parent = node;
				}
				temp->_left = NULL;
				temp->_right = NULL;
				temp->_parent = NULL;
				alloc.destroy(temp);
				alloc.deallocate(temp, 1);
			}
			else {
				if (node->_left) {
					RBNode* temp = node->_left;
					swap(_Val, temp->_Val);
					node->_right = temp->_right;
					if (temp->_right)
						temp->_right->_parent = node;
					node->_left = temp->_left;
					if (temp->_left)
						temp->_left->_parent = node;
					temp->_parent = NULL;
					temp->_left = NULL;
					temp->_right = NULL;
					alloc.destroy(temp);
					alloc.deallocate(temp, 1);
				}
			}
			
		}
		balancing(parent);
		return parent;
	}
	RBNode* delete_node(value_type val) {
		RBNode* temp = searchNode(val);
		if (temp)
			return delete_node(temp);
		return this;
	}

	RBNode* erase(){
		RBNode* node = this;


	}

	/* ******************* Вспомогательные функции ****************** */

	int	Height(RBNode *arg ){
		if(!arg)
			return 0;
		int h_left = Height(arg->_left);
		int	h_right = Height(arg->_right);
		if(h_left > h_right)
			return h_left + 1;
		else
			return  h_right + 1;
	}

	void print_n(RBNode const* root, int n, int level){
		if(root){
			if(n == level) {
				std::cout << root->_Val;
				std::cout << (root->_color == RED ? 'r' : 'b') << ' ';
			}
			else{
				print_n(root->_left, n, level + 1);
				print_n(root->_right, n, level + 1);
			}
		}
	}

	void print_Root(){
		std::cout << _Val;
		std::cout << (_color == RED ? 'r' : 'b') << ' ' << std::endl;
	}


	/***************** балансировка ***********************/
		void balancing(RBNode* ptr) {
			if (!ptr->_parent) {
				ptr->_color = BLACK;
				return;
			}
			RBNode* P = ptr->_parent;
			if (P->_color == BLACK)
				return;
			RBNode* G = ptr->grandfather();
			RBNode* U = ptr->uncle();
			std::cout << "P = " << P->_color << std::endl;
			std::cout << "G = " << G << std::endl;
			std::cout << "U = " << U << std::endl;
			if (P->_color == RED and U and U->_color == RED) { //случай 3
				P->_color = BLACK;
				U->_color = BLACK;
				if (G)
				G->_color = RED;
				balancing(G);
				return;
			} 
			else if (P->_color == RED and (!U or U->_color == BLACK)) { //случай 4
				if (ptr == P->_right and G and P == G->_left) {
					LeftTurn(P);
					ptr = ptr->_left;
				} 
				else if (ptr == P->_left and G and P == G->_right) {
					RightTurn(P);
					ptr = ptr->_right;
				}
			}
			P = ptr->_parent;
			G = ptr->grandfather();
			U = ptr->uncle();
			P->_color = BLACK;
			if (G)
				G->_color = RED;
			if (ptr == P->_left and G and P == G->_left) 
				RightTurn(G);
			else 
				LeftTurn(G);
		}

		void LeftTurn(RBNode* root) {
			RBNode* RightSubTree = root->_right;
			RBNode* RightSubTreeLeft = RightSubTree->_left;
			if (root->_parent) {
				if (root->_parent->_left == root)
					root->_parent->_left = RightSubTree;
				else
					root->_parent->_right = RightSubTree;
			}
			RightSubTree->_parent = root->_parent;
			root->_parent = RightSubTree;
			RightSubTree->_left = root;
			root->_right = RightSubTreeLeft;
			if (RightSubTreeLeft and RightSubTreeLeft->_parent)
				RightSubTreeLeft->_parent = root;
		}

		void RightTurn(RBNode* root) {
			RBNode* LeftSubTree = root->_left;
			RBNode* LeftSubTreeRight = LeftSubTree->_right;
			if (root->_parent) {
				if (root->_parent->_left == root)
					root->_parent->_left = LeftSubTree;
				else
					root->_parent->_right = LeftSubTree;
			}
			LeftSubTree->_parent = root->_parent;
			root->_parent = LeftSubTree;
			LeftSubTree->_right = root;
			root->_left = LeftSubTreeRight;
			if (LeftSubTreeRight and LeftSubTreeRight->_parent)
				LeftSubTreeRight->_parent = root;
		}
};


#endif //RBNODE_HPP