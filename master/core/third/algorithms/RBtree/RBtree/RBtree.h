/*******************************************************************************
*
* FileName : RBtree.h
* Comment  : red black tree
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-12 23:03
*
*******************************************************************************/

#ifndef _RBTREE_H_INCLUDED
#define _RBTREE_H_INCLUDED

enum RBtree_color_type { RBtree_red, RBtree_black };

template<class T>
struct RBtree_node {
	typedef RBtree_color_type Color_type;
	typedef RBtree_node* pRBN;

	T element;
	pRBN lchild;
	pRBN rchild;
	pRBN parent;
	Color_type color;

	explicit RBtree_node(T elem = T()) : element(elem), 
		lchild(0), rchild(0), parent(0), color(RBtree_red) {}
	static pRBN minmum(pRBN x)
	{
		while (x->lchild != 0)
			x = x->lchild;
		return x;
	}
	static pRBN maxmum(pRBN x)
	{
		while (x->rchild != 0)
			x = x->rchild;
		return x;
	}
};

//-----------------------iterator of RBtree-----------------------
//the base iterator struct
template<class T>
struct RBtree_iterator_base {
	typedef RBtree_node<T>* pRBN;
	pRBN node;

	void increment();
	void decrement();
};

template<class T>
void RBtree_iterator_base<T>::increment()
{
	if (node->rchild != 0) {
		node = node->rchild;
		while (node->lchild != 0)
			node = node->lchild;
	}
	else {
		pRBN y = node->parent;
		while (node == y->rchild) {
			node = y;
			y = y->parent;
		}
		//find the next node of root while the root don't has right child
		if (node->rchild != y)
			node = y;
	}
}

template<class T>
void RBtree_iterator_base<T>::decrement()
{
	//when node is the header( end() )
	if (node->color == RBtree_red && node->parent->parent == node)
		node = node->rchild;
	else if (node->lchild != 0) {
		node = node->lchild;
		while (node->rchild != 0)
			node = node->rchild;
	}
	else {
		pRBN y = node->parent;
		while (node == y->lchild) {
			node = y;
			y = y->parent;
		}
		node = y;
	}
}

template<class T>
inline bool operator==(const RBtree_iterator_base<T>& lhs,
					   const RBtree_iterator_base<T>& rhs)
{
	return lhs.node->element == rhs.node->element;
}

template<class T>
inline bool operator!=(const RBtree_iterator_base<T>& lhs,
					   const RBtree_iterator_base<T>& rhs)
{
	return lhs.node->element != rhs.node->element;
}

//now the iterator struct
template<class T, class Ref, class Ptr>
struct RBtree_iterator : public RBtree_iterator_base<T> {
	typedef Ref reference;
	typedef Ptr pointer;
	typedef RBtree_iterator<T,T&,T*> iterator;
	typedef RBtree_iterator<T,Ref,Ptr> self;

	RBtree_iterator() {}
	RBtree_iterator(RBtree_node<T>* x) { node = x; }
	RBtree_iterator(const iterator& other) { node = other.node; }

	reference operator*() const;
	pointer operator->() const;

	self& operator++();
	self operator++(int);
	self& operator--();
	self operator--(int);
};

template<class T, class Ref, class Ptr>
Ref RBtree_iterator<T,Ref,Ptr>::operator*() const
{
	return node->element;
}

template<class T, class Ref, class Ptr>
Ptr RBtree_iterator<T,Ref,Ptr>::operator->() const
{
	return node;
}

template<class T, class Ref, class Ptr>
typename RBtree_iterator<T,Ref,Ptr>::self&
RBtree_iterator<T,Ref,Ptr>::operator++()
{
	increment();
	return *this;
}

template<class T, class Ref, class Ptr>
typename RBtree_iterator<T,Ref,Ptr>::self
RBtree_iterator<T,Ref,Ptr>::operator++(int)
{
	self tmp = *this;
	increment();
	return tmp;
}

template<class T, class Ref, class Ptr>
typename RBtree_iterator<T,Ref,Ptr>::self&
RBtree_iterator<T,Ref,Ptr>::operator--()
{
	decrement();
	return *this;
}

template<class T, class Ref, class Ptr>
typename RBtree_iterator<T,Ref,Ptr>::self
RBtree_iterator<T,Ref,Ptr>::operator--(int)
{
	self tmp = *this;
	decrement();
	return *this;
}

//-----------------------red-black class-----------------------
template<class T>
class RBtree {
protected:
	typedef RBtree_color_type Color_type;
	typedef RBtree_node<T>* pRBN;

public:
	typedef RBtree_iterator<T,T&,T*> iterator;
	typedef RBtree_iterator<T,const T&, const T*> const_iterator;

protected:
	pRBN clone_node(pRBN x); //copy the value and color of x
	void destory_node(pRBN x);

	pRBN& root() const { return header->parent; }
	pRBN& leftmost() const { return header->lchild; }
	pRBN& rightmost() const { return header->rchild; }

	static pRBN& left(pRBN x) { return x->lchild; }
	static pRBN& right(pRBN x) { return x->rchild; }
	static pRBN& parent(pRBN x) {return x->parent; }
	static T& value(pRBN x) { return x->element; }
	static Color_type& color(pRBN x) { return x->color; }

	static pRBN minmum(pRBN x) { return RBtree_node<T>::minmum(x); }
	static pRBN maxmum(pRBN x) { return RBtree_node<T>::maxmum(x); }

public:
	RBtree();
	~RBtree();

	RBtree(const RBtree& other);
	RBtree& operator=(const RBtree& other);

	bool empty() { return 0 == hb_size; }
	int size() { return hb_size; }
	iterator begin() { return leftmost(); } //implicit convertion
	iterator end() { return header; }	//implicit convertion

	iterator find(const T& elem);
	std::pair<iterator, bool> insert(const T& elem);
	void erase(iterator position);

protected:
	pRBN header;
	int hb_size;	//node number

private:
	void init();
	pRBN copy(pRBN now, pRBN pa);
	void destory(pRBN now);
	void clear();
	iterator insert(pRBN x, pRBN y, const T& elem);
};

template<class T>
typename RBtree<T>::pRBN 
RBtree<T>::clone_node(typename RBtree<T>::pRBN x)
{
	pRBN tmp = new RBtree_node<T>(x->element);
	tmp->color = x->color;
	return tmp;
}

template<class T>
void RBtree<T>::destory_node(typename RBtree<T>::pRBN x)
{
	delete x;
}

template<class T>
RBtree<T>::RBtree() : hb_size(0)
{
	init();
}

template<class T>
RBtree<T>::~RBtree()
{
	clear();
	delete header;
}

template<class T>
RBtree<T>::RBtree(const RBtree<T>& other) : hb_size(0)
{
	init();
	if (other.root() != 0) {
		color(header) = RBtree_red;
		root() = copy(other.root(), header);
		leftmost() = minmum( root() );
		rightmost() = maxmum( root() );
		hb_size = other.size;
	}
}

template<class T>
RBtree<T>& RBtree<T>::operator=(const RBtree<T>& other)
{
	clear();
	color(header) = RBtree_red;
	root() = copy(other.root(), header);
	leftmost() = minmum( root() );
	rightmost() = maxmum( root() );
	hb_size = other.size;
}

template<class T>
void RBtree<T>::init()
{
	header = new RBtree_node<T>;
	color(header) = RBtree_red;
	root() = 0;
	leftmost() = header;
	rightmost() = header;
}

template<class T>
typename RBtree<T>::pRBN	//now, pa must not be null
RBtree<T>::copy(typename RBtree<T>::pRBN now, 
				typename RBtree<T>::pRBN pa)
{
	pRBN top = clone_node(now);
	parent(top) = pa;

	if (right(now) != 0)
		right(top) = copy(right(now), top);
	if (left(now) != 0)
		left(top) = copy(left(now), top);

	return top;
}

template<class T>
void RBtree<T>::destory(typename RBtree<T>::pRBN now)
{
	if (0 == now)
		return;
	destory(left(now));
	destory(right(now));
	delete now;
	--hb_size;
}

template<class T>
void RBtree<T>::clear()
{
	destory(root());
	root() = 0;
	leftmost() = 0;
	rightmost() = 0;
}

template<class T>
typename RBtree<T>::iterator
RBtree<T>::insert(typename RBtree<T>::pRBN x, 
					typename RBtree<T>::pRBN y, const T& elem)
{
	pRBN z;
	if (y == header || x != 0 || elem < y->element) {
		z = new RBtree_node<T>(elem);
		left(y) = z;
		if (y == header) {
			root() = z;
			rightmost() = z;
		}
		else if (y == leftmost()) {
			leftmost() = z;
		}
	}//if
	else {
		z = new RBtree_node<T>(elem);
		right(y) = z;
		if (y == rightmost()) {
			rightmost() = z;
		}
	}//else

	parent(z) = y;
	left(z) = right(z) = 0;
	RBtree_rebalance(z,header->parent);
	++hb_size;
	return iterator(z);
}

template<class T>
std::pair<typename RBtree<T>::iterator, bool>
RBtree<T>::insert(const T& elem)
{
	pRBN y = header;
	pRBN x = root();
	bool comp = true;
	while (x != 0) {
		if (elem == x->element) {
			iterator j = iterator(x);
			return std::pair<iterator, bool>(x, false);
		}
		y = x;
		comp = elem < value(x);
		x = comp ? left(x) : right(x);
	}
	return std::pair<iterator, bool>(insert(x, y, elem), true);
}

template<class T>
inline void RBtree<T>::erase(typename RBtree<T>::iterator position)
{
  pRBN y = Rb_tree_rebalance_for_erase(position.node,
                                              header->parent,
                                              header->left,
                                              header->right);
  destroy_node(y);
  --hb_size;
}

template <class T>
typename RBtree<T>::iterator 
RBtree<T>::find(const T& elem)
{
  pRBN y = header;      // Last node which is not less than __k. 
  pRBN x = root();      // Current node. 

  while (x != 0) {
	  if (elem == x->element)
		  return iterator(x);
	  if (elem < x->element)
		  x = left(x);
	  else
		  x = right(x);
  } //while
  if (0 == x)
	  return end();
}


//---------------------global function for rotation---------------------
template<class T>
inline void 
RBtree_rotate_to_left(RBtree_node<T>* x, RBtree_node<T>*& root)
{
  RBtree_node<T>* y = x->rchild;
  x->rchild = y->lchild;
  if (y->lchild !=0)
    y->lchild->parent = x;
  y->parent = x->parent;

  if (x == root)
    root = y;
  else if (x == x->parent->lchild)
    x->parent->lchild = y;
  else
    x->parent->rchild = y;
  y->lchild = x;
  x->parent = y;
}

template<class T>
inline void 
RBtree_rotate_to_right(RBtree_node<T>* x, RBtree_node<T>*& root)
{
  RBtree_node<T>* y = x->lchild;
  x->lchild = y->rchild;
  if (y->rchild != 0)
    y->rchild->parent = x;
  y->parent = x->parent;

  if (x == root)
    root = y;
  else if (x == x->parent->rchild)
    x->parent->rchild = y;
  else
    x->parent->lchild = y;
  y->rchild = x;
  x->parent = y;
}

template<class T>	//down to up
inline void 
RBtree_rebalance(RBtree_node<T>* x, RBtree_node<T>*& root)
{
  x->color = RBtree_red;
  while (x != root && x->parent->color == RBtree_red) {
    if (x->parent == x->parent->parent->lchild) {
      RBtree_node<T>* y = x->parent->parent->rchild;
      if (y != 0 && y->color == RBtree_red) {
        x->parent->color = RBtree_black;
        y->color = RBtree_black;
        x->parent->parent->color = RBtree_red;
        x = x->parent->parent;
      }
      else { //y == 0 || y->color == RBtree_black
        if (x == x->parent->rchild) {
          x = x->parent;
          RBtree_rotate_to_left(x, root);
        }
        x->parent->color = RBtree_black;
        x->parent->parent->color = RBtree_red;
        RBtree_rotate_to_right(x->parent->parent, root);
      }
    } //if
    else { //x->parent != x->parent->parent->lchild
      RBtree_node<T>* y = x->parent->parent->lchild;
      if (y != 0 && y->color == RBtree_red) {
        x->parent->color = RBtree_black;
        y->color = RBtree_black;
        x->parent->parent->color = RBtree_red;
        x = x->parent->parent;
      }
      else { //y == 0 || y->color == RBtree_black
        if (x == x->parent->lchild) {
          x = x->parent;
          RBtree_rotate_to_right(x, root);
        }
        x->parent->color = RBtree_black;
        x->parent->parent->color = RBtree_red;
        RBtree_rotate_to_left(x->parent->parent, root);
      }
    }//else
  }//while
  root->color = RBtree_black;
}

template<class T>
inline RBtree_node<T>*
RBtree_rebalance_for_erase(RBtree_node<T>* z,
                             RBtree_node<T>*& root,
                             RBtree_node<T>*& leftmost,
                             RBtree_node<T>*& rightmost)
{
  RBtree_node<T>* y = z;
  RBtree_node<T>* x = 0;
  RBtree_node<T>* x_parent = 0;
  if (y->lchild == 0)     // z has at most one non-null child. y == z.
    x = y->rchild;     // x might be null.
  else
    if (y->rchild == 0)  // z has exactly one non-null child. y == z.
      x = y->lchild;    // x is not null.
    else {                   // z has two non-null children.  Set y to
      y = y->rchild;   //   z's successor.  x might be null.
      while (y->lchild != 0)
        y = y->lchild;
      x = y->rchild;
    }
  if (y != z) {          // relink y in place of z.  y is z's successor
    z->lchild->parent = y; 
    y->lchild = z->lchild;
    if (y != z->rchild) {
      x_parent = y->parent;
      if (x) x->parent = y->parent;
      y->parent->lchild = x;      // y must be a child of left
      y->rchild = z->rchild;
      z->rchild->parent = y;
    }
    else
      x_parent = y;  
    if (root == z)
      root = y;
    else if (z->parent->lchild == z)
      z->parent->lchild = y;
    else 
      z->parent->rchild = y;
    y->parent = z->parent;
    STD::swap(y->color, z->color);
    y = z;
    // y now points to node to be actually deleted
  }
  else {                        // y == z
    x_parent = y->parent;
    if (x) x->parent = y->parent;   
    if (root == z)
      root = x;
    else 
      if (z->parent->lchild == z)
        z->parent->lchild = x;
      else
        z->parent->rchild = x;
    if (leftmost == z) 
      if (z->rchild == 0)        // z->lchild must be null also
        leftmost = z->parent;
    // makes leftmost == header if z == root
      else
        leftmost = RBtree_node<T>::_S_minimum(x);
    if (rightmost == z)  
      if (z->lchild == 0)         // z->rchild must be null also
        rightmost = z->parent;  
    // makes rightmost == header if z == root
      else                      // x == z->lchild
        rightmost = RBtree_node<T>::_S_maximum(x);
  }
  if (y->color != RBtree_red) { 
    while (x != root && (x == 0 || x->color == RBtree_black))
      if (x == x_parent->lchild) {
        RBtree_node<T>* w = x_parent->rchild;
        if (w->color == RBtree_red) {
          w->color = RBtree_black;
          x_parent->color = RBtree_red;
          RBtree_rotate_to_left(x_parent, root);
          w = x_parent->rchild;
        }
        if ((w->lchild == 0 || 
             w->lchild->color == RBtree_black) &&
            (w->rchild == 0 || 
             w->rchild->color == RBtree_black)) {
          w->color = RBtree_red;
          x = x_parent;
          x_parent = x_parent->parent;
        } else {
          if (w->rchild == 0 || 
              w->rchild->color == RBtree_black) {
            if (w->lchild) w->lchild->color = RBtree_black;
            w->color = RBtree_red;
            RBtree_rotate_to_right(w, root);
            w = x_parent->rchild;
          }
          w->color = x_parent->color;
          x_parent->color = RBtree_black;
          if (w->rchild) w->rchild->color = RBtree_black;
          RBtree_rotate_to_left(x_parent, root);
          break;
        }
      } else {                  // same as above, with right <-> left.
        RBtree_node<T>* w = x_parent->lchild;
        if (w->color == RBtree_red) {
          w->color = RBtree_black;
          x_parent->color = RBtree_red;
          RBtree_rotate_to_right(x_parent, root);
          w = x_parent->lchild;
        }
        if ((w->rchild == 0 || 
             w->rchild->color == RBtree_black) &&
            (w->lchild == 0 || 
             w->lchild->color == RBtree_black)) {
          w->color = RBtree_red;
          x = x_parent;
          x_parent = x_parent->parent;
        } else {
          if (w->lchild == 0 || 
              w->lchild->color == RBtree_black) {
            if (w->rchild) w->rchild->color = RBtree_black;
            w->color = RBtree_red;
            RBtree_rotate_to_left(w, root);
            w = x_parent->lchild;
          }
          w->color = x_parent->color;
          x_parent->color = RBtree_black;
          if (w->lchild) w->lchild->color = RBtree_black;
          RBtree_rotate_to_right(x_parent, root);
          break;
        }
      }
    if (x) x->color = RBtree_black;
  }
  return y;
}

#endif