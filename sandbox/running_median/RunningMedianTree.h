/*
 * RunningMedianTree.h
 *
 *  Created on: Nov 5, 2008
 *      Author: david
 */

#ifndef RUNNINGMEDIANTREE_H_
#define RUNNINGMEDIANTREE_H_

template<class T>
// the class T must have the > operator defined
class RunningMedianTree {
protected:
	enum COLOR {
		RED, BLACK
	};
	class Node {
	public:
		// this is a red-black tree so we need colors
		COLOR color;
		// these are the pointers for the tree structure
		Node* parent;
		Node* left;
		Node* right;
		// these are the pointers for the insertion list structure
		Node* next;
		Node* last;
		T data;
		Node() {
			color = RED;
			parent = 0;
			left = 0;
			right = 0;
			next = 0;
			last = 0;
		}
		Node(T data) {
			color = RED;
			parent = 0;
			left = 0;
			right = 0;
			next = 0;
			last = 0;
			this->data = data;
		}
		~Node() {
			if (left)
				delete left;
			if (right)
				delete right;
		}
		Node* sibling() {
			/*
			 * Get this node's parent's other child.
			 */
			if (parent) {
				if (this == parent->left)
					return parent->right;
				else
					return parent->left;
			}
			return 0;
		}
		Node* grandpa() {
			/*
			 * Get this node's parent's parent
			 */
			if (parent)
				return parent->parent;
			return 0;
		}
		Node* uncle() {
			/*
			 * Get this node's parent's sibling.
			 */
			if (parent)
				return parent->sibling();
			return 0;
		}
		void insert(Node* node) {
			/*
			 * Insert a node below this one.
			 */
			if (node->data > data) {
				if (right) {
					right->insert(node);
				} else {
					right = node;
					right->parent = this;
				}
			} else {
				if (left) {
					left->insert(node);
				} else {
					left = node;
					left->parent = this;
				}
			}
		}
	};
	Node* root;
	Node* front;
	Node* back;
	unsigned int size;
	unsigned int max_size;
	void rotateLeft(Node* node) {
		/*
		 * Rotate a subtree to the left around node.
		 */
		Node* parent = node->parent;
		Node* pivot = node->right;
		node->right = pivot->left;
		if (node->right)
			node->right->parent = node;
		pivot->left = node;
		node->parent = pivot;
		if (node == root) {
			root = pivot;
			pivot->parent = 0;
		} else {
			if (node == parent->left)
				parent->left = pivot;
			else
				parent->right = pivot;
			pivot->parent = parent;
		}
	}
	void rotateRight(Node* node) {
		/*
		 * Rotate a subtree to the right around node.
		 */
		Node* parent = node->parent;
		Node* pivot = node->left;
		node->left = pivot->right;
		if (node->left)
			node->left->parent = node;
		pivot->right = node;
		node->parent = pivot;
		if (node == root) {
			root = pivot;
			pivot->parent = 0;
		} else {
			if (node == parent->left)
				parent->left = pivot;
			else
				parent->right = pivot;
			pivot->parent = parent;
		}
	}
	void replaceNode(Node* n1, Node* n2) {
		if (n2 == n2->parent->left)
			n2->parent->left = 0;
		else
			n2->parent->right = 0;
		if (n1 == root) {
			root = n2;
		} else {
			if (n1 == n1->parent->left)
				n1->parent->left = n2;
			else
				n1->parent->right = n2;
		}
		n2->color = n1->color;
		n2->parent = n1->parent;
		n2->left = n1->left;
		n2->right = n1->right;
		n1->left = 0;
		n1->right = 0;
		//n1->parent = 0;
	}
	void balance(Node* node) {
		/*
		 * Use the red-black properties to re-balance the tree after inserting a node.
		 */
		// case 1
		if (!node->parent) {
			node->color = BLACK;
			return;
		}
		// case 2
		if (node->parent->color == BLACK) {
			return;
		}
		// case 3
		Node* u = node->uncle();
		Node* g = node->grandpa();
		if (u && u->color == RED) {
			node->parent->color = BLACK;
			u->color = BLACK;
			g->color = RED;
			balance(g);
		} else {
			// case 4
			if (node == node->parent->right && node->parent == g->left) {
				rotateLeft(node->parent);
				node = node->left;
			} else if (node == node->parent->left && node->parent == g->right) {
				rotateRight(node->parent);
				node = node->right;
			}
			// case 5
			g = node->grandpa();
			node->parent->color = BLACK;
			g->color = RED;
			if (node == node->parent->left && node->parent == g->left) {
				rotateRight(g);
			} else {
				rotateLeft(g);
			}
		}
	}
	void removeLoop(Node* node) {
		/*
		 * This does all the work for removing a node.
		 * This is a separate function to facilitate the
		 * recursion in case 3.
		 */
		// case 1
		if (node->parent) {
			// case 2
			Node* s = node->sibling();
			if (s && s->color == RED) {
				node->parent->color = RED;
				s->color = BLACK;
				if (node == node->parent->left)
					rotateLeft(node->parent);
				else
					rotateRight(node->parent);
			}
			// case 3
			s = node->sibling();
			if (node->parent->color == BLACK && s && s->color == BLACK
					&& s->left->color == BLACK && s->right->color == BLACK) {
				s->color = RED;
				removeLoop(node->parent);
			} else {
				// case 4
				if (node->parent->color == RED && s && s->color == BLACK
						&& s->left->color == BLACK && s->right->color == BLACK) {
					s->color = RED;
					node->parent->color = BLACK;
				} else {
					// case 5
					if (s && s->color == BLACK) {
						if (node == node->parent->left && s->right->color
								== BLACK && s->left->color == RED) {
							s->color = RED;
							s->left->color = BLACK;
							rotateRight(s);
						} else if (node == node->parent->right
								&& s->left->color == BLACK && s->right->color
								== RED) {
							s->color = RED;
							s->right->color = BLACK;
							rotateLeft(s);
						}
					}
					// case 6
					s = node->sibling();
					if (s) {
						s->color = node->parent->color;
						node->parent->color = BLACK;
						if (node == node->parent->left) {
							s->right->color = BLACK;
							rotateLeft(node->parent);
						} else {
							s->left->color = BLACK;
							rotateRight(node->parent);
						}
					}
				}
			}
		}
	}
	void remove(Node* node) {
		/*
		 * Remove a node from the tree.
		 */
		Node* child = node->right ? node->right : node->left;
		replaceNode(node, child);
		if (node->color == BLACK) {
			if (child->color == RED)
				child->color = BLACK;
			else
				removeLoop(child);
		}
		// zero the child pointers so that deleting this node
		// later won't destroy the remaining tree
		node->right = 0;
		node->left = 0;
		delete node;
	}
public:
	RunningMedianTree() {
		root = 0;
		front = 0;
		back = 0;
		size = 0;
		max_size = 0;
	}
	RunningMedianTree(unsigned int max_size) {
		root = 0;
		front = 0;
		back = 0;
		size = 0;
		this->max_size = max_size;
	}
	~RunningMedianTree() {
		delete root;
	}
	void pop() {
		/*
		 * Remove the least recently inserted node
		 */
		// remove the node form the list
		Node* node = front;
		node->last->next = 0;
		front = node->last;
		// remove the node from the tree
		remove(node);
		--size;
	}
	void push(T data) {
		/*
		 * Add a node to the tree and the insertion order list.
		 */
		// add to the tree
		Node* node = new Node(data);
		if (root) {
			root->insert(node);
		} else {
			root = node;
			front = node;
			back = node;
		}
		// re-balance the tree
		balance(node);
		// add to the list
		back->last = node;
		node->next = back;
		back = node;
		// update the size and remove a node if need be
		++size;
		if (size > max_size) {
			pop();
		}
	}
	unsigned int getSize() {
		return size;
	}
	unsigned int maxSize() {
		return max_size;
	}
	void maxSize(unsigned int max_size) {
		/*
		 * Change the maximum number of samples in the set.
		 * If necessary remove expired samples.
		 */
		this->max_size = max_size;
		while (size > max_size) {
			pop();
		}
	}
	T& median() {
		return root->data;
	}
};

#endif /* RUNNINGMEDIANTREE_H_ */
