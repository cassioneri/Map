/*******************************************************************************

 Copyright (C) 2014 Cassio Neri.

 This work is licensed under a Creative Commons Attribution-ShareAlike 4.0
 International License. See
 http://creativecommons.org/licenses/by-sa/4.0/

*******************************************************************************/

#ifndef MAP_H_
#define MAP_H_

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <random>
#include <type_traits>
#include <utility>

template <typename K, typename T>
class map {

public:

  using key_type        = const K;
  using mapped_type     = T;
  using value_type      = std::pair<key_type, mapped_type>;
  using reference       = value_type&;
  using const_reference = const value_type&;
  using pointer         = value_type*;
  using const_pointer   = const value_type*;

private:

  enum find_type_ { exact_, lower_bound_, upper_bound_ };

  struct lnode_; // list node
  struct tnode_; // tree node

  using owning_ptr_   = std::unique_ptr<tnode_>;
  using sentinel_ptr_ = std::unique_ptr<lnode_>;
  using tnode_ptr_    = tnode_*;
  using lnode_ptr_    = lnode_*;
  using priority_t    = std::default_random_engine::result_type;

  // list node
  struct lnode_ {
    lnode_ptr_ prev;
    lnode_ptr_ next;
  };

  // tree node
  struct tnode_ : lnode_ {

    tnode_(lnode_ lnode, const value_type& value, tnode_ptr_ parent,
      priority_t priority) :
      lnode_(lnode),
      value(value),
      priority(priority),
      parent(parent) {
    }

    tnode_(lnode_ lnode, const value_type& value, tnode_ptr_ parent,
      priority_t priority, owning_ptr_&& left, owning_ptr_&& right) :
      tnode_(lnode, value, parent, priority) {
      this->left  = std::move(left);
      this->right = std::move(right);
    }

    value_type  value;
    priority_t  priority;
    tnode_ptr_  parent;
    owning_ptr_ left  = nullptr;
    owning_ptr_ right = nullptr;
  };

  template <bool is_const>
  class iterator_ :

  public std::iterator<
    std::bidirectional_iterator_tag,
    typename std::conditional<is_const, const value_type, value_type>::type
  > {

  public:

    iterator_() noexcept = default;

    // This is the copy constructor of iterator_<false> (a.k.a. iterator) and
    // converting constructor of iterator_<true> (a.k.a. const_iterator) from
    // iterator_<false> (a.k.a. iterator).
    iterator_(const iterator_<false>& other) noexcept :
      observer_(other.observer_) {
    }

    iterator_&
    operator =(const iterator_<false>& other) noexcept {
      observer_ = other.observer_;
      return *this;
    }

    bool
    operator ==(const iterator_<true>& other) const noexcept {
      return observer_ == other.observer_;
    }

    bool
    operator !=(const iterator_<true>& other) const noexcept {
      return !(*this == other);
    }

    reference
    operator *() const noexcept {
      return static_cast<tnode_ptr_>(observer_)->value;
    }

    pointer
    operator ->() const noexcept {
      return std::addressof(**this);
    }

    iterator_&
    operator ++() noexcept {
      observer_ = observer_->next;
      return *this;
    }

    iterator_
    operator ++(int) noexcept {
      iterator_ result{*this};
      ++*this;
      return result;
    }

    iterator_&
    operator --() noexcept {
      observer_ = observer_->prev;
      return *this;
    }

    iterator_
    operator --(int) noexcept {
      iterator_ result{*this};
      --*this;
      return result;
    }

  private:

    friend class map;

    iterator_(lnode_ptr_ observer) noexcept :
      observer_(observer) {
    }

    lnode_ptr_ observer_ = nullptr;

  }; // class iterator<is_const>

public:

  using iterator               = iterator_<false>;
  using const_iterator         = iterator_<true>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using difference_type = typename iterator::difference_type;
  using size_type       = typename std::make_unsigned<difference_type>::type;

  map() :
    engine_(std::random_device()()) {
  }

  map(const map& other) {
    copy_(other);
  }

  map(map&& other) noexcept {
    move_(other);
  }

  template <typename Iterator>
  map(Iterator first, Iterator last) {
    insert(first, last);
  }

  map(std::initializer_list<value_type> il) : map(il.begin(), il.end()) {
  }

  map&
  operator =(const map& other) {
    copy_(other);
    return *this;
  }

  map&
  operator =(map&& other) noexcept {
    move_(other);
    return *this;
  }

  bool
  empty() const noexcept {
    return !size_;
  }

  size_type
  size() const noexcept {
    return size_;
  }

  iterator
  begin() noexcept {
    return first_;
  }

  const_iterator
  begin() const noexcept {
    return first_;
  }

  const_iterator
  cbegin() const noexcept {
    return begin();
  }

  iterator
  end() noexcept {
    return sentinel_.get();
  }

  const_iterator
  end() const noexcept {
    return sentinel_.get();
  }

  const_iterator
  cend() const noexcept {
    return end();
  }

  reverse_iterator
  rbegin() noexcept {
    return reverse_iterator(end());
  }

  const_reverse_iterator
  rbegin() const noexcept {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator
  crbegin() const noexcept {
    return rbegin();
  }

  reverse_iterator
  rend() noexcept {
    return reverse_iterator(begin());
  }

  const_reverse_iterator
  rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator
  crend() const noexcept {
    return rend();
  }

  std::pair<iterator, bool>
  insert(const key_type& key, const mapped_type& value) {

    tnode_ptr_ position = root_.get();
    tnode_ptr_ parent   = nullptr;
    tnode_ptr_ previous = nullptr;
    tnode_ptr_ next     = nullptr;

    while (position) {

      if (key == position->value.first)
        return {position, false};

      parent = position;

      if (key < position->value.first) {
        next     = position;
        position = position->left.get();
      }

      else {
        previous = position;
        position = position->right.get();
      }
    }

    owning_ptr_ node(new tnode_{{previous, next}, {key, value}, parent,
      engine_()});

    if (!sentinel_)
      sentinel_.reset(new lnode_);

    // No throwing code.

    if (node->prev)
      node->prev->next = node.get();

    else
      first_ = node.get();

    if (node->next)
      node->next->prev = node.get();

    else {
      node->next      = sentinel_.get();
      sentinel_->prev = node.get();
    }

    position = node.get();

    if (empty())
      root_ = std::move(node);

    else if (next == parent)
      parent->left = std::move(node);

    else
      parent->right = std::move(node);

    position = max_heapify_(position);

    ++size_;

    #ifndef NDEBUG
    check_invariants_();
    #endif

    return std::pair<iterator, bool>(position, true);
  }

  std::pair<iterator, bool>
  insert(const value_type& value) {
    return insert(value.first, value.second);
  }

  template <typename Iterator>
  void
  insert(Iterator first, Iterator last) {
    std::for_each(first, last, [=](const value_type& kv){ insert(kv); });
  }

  iterator
  erase(const_iterator position) noexcept {

    tnode_ptr_ node = static_cast<tnode_ptr_>(position.observer_);
    tnode_ptr_ next = static_cast<tnode_ptr_>(node->next);
    tnode_ptr_ prev = static_cast<tnode_ptr_>(node->prev);

    // Remove node from linked list.

    if (prev)
      prev->next = next;
    else
      first_ = next;

    if (next)
      next->prev = prev;
    else
      sentinel_->prev = prev;

    // Remove node from tree.

    if (node->left && node->right) {

      owning_ptr_ prev_owner = std::move(get_owner_(prev));

      if (prev->parent != node) { // <=> prev is not node's left child
        if (prev->left) // prev has a left child
          replace_by_child_<&tnode_::left>(prev);
        prev->left = std::move(node->left);
        prev->left->parent = prev;
      }

      prev->right = std::move(node->right);
      if (prev->right)
        prev->right->parent = prev;

      prev->priority   = node->priority;
      prev->parent     = node->parent;
      get_owner_(node) = std::move(prev_owner);
    }

    else if (node->left)
      replace_by_child_<&tnode_::left>(node);

    else if (node->right)
      replace_by_child_<&tnode_::right>(node);

    else
      get_owner_(node) = nullptr;

    --size_;

    #ifndef NDEBUG
    check_invariants_();
    #endif

    return next;
  }

  size_type
  erase(const key_type& key) {
    iterator position = find(key);
    if (position != end()) {
      erase(position);
      return 1;
    }
    return 0;
  }

  iterator
  erase(const_iterator first, const_iterator last) {
    iterator it = last.observer_;
    std::for_each(first, last, [=](const value_type& kv){
      erase(find(kv.first)); });
    return it;
  }

  mapped_type&
  operator [](const key_type& key) {
    std::pair<iterator, bool> r = insert(key, mapped_type{});
    return r.first->second;
  }

  iterator
  find(const key_type& key) noexcept {
    return find_<exact_>(key);
  }

  const_iterator
  find(const key_type& key) const noexcept {
    return find_<exact_>(key);
  }

  iterator
  lower_bound(const key_type& key) noexcept {
    return find_<lower_bound_>(key);
  }

  const_iterator
  lower_bound(const key_type& key) const noexcept {
    return find_<lower_bound_>(key);
  }

  iterator
  upper_bound(const key_type key) noexcept {
    return find_<upper_bound_>(key);
  }

  const_iterator
  upper_bound(const key_type key) const noexcept {
    return find_<upper_bound_>(key);
  }

  void
  clear() noexcept {
    root_.reset();
    sentinel_.reset();
    first_ = nullptr;
    size_  = 0;
  }

  void
  swap(map& other) noexcept {
    root_.swap(other.root_);
    sentinel_.swap(other.sentinel_);
    std::swap(first_, other.first_);
    std::swap(size_, other.size_);
  }

private:

  owning_ptr_
  clone_(tnode_ptr_ node, lnode_ptr_& first, lnode_ptr_& last) const {

    lnode_ptr_ prev = nullptr;
    lnode_ptr_ next = nullptr;

    owning_ptr_ left = node->left ? clone_(node->left.get(), first, prev) :
      nullptr;

    owning_ptr_ right = node->right ? clone_(node->right.get(), next, last) :
      nullptr;

    owning_ptr_ root(new tnode_{{prev, next}, node->value, nullptr,
      node->priority, std::move(left), std::move(right)});

    if (root->left)
      root->left->parent = root.get();
    else
      first = root.get();

    if (root->right)
      root->right->parent = root.get();
    else
      last = root.get();

    if (root->prev)
      root->prev->next = root.get();

    if (root->next)
      root->next->prev = root.get();

    return root;
  }

  void
  copy_(const map& other) {

    if (other.empty()) {
      clear();
      return;
    }

    lnode_ptr_    last = nullptr;
    owning_ptr_   root = clone_(other.root_.get(), first_, last);
    sentinel_ptr_ sentinel(new lnode_{last, nullptr});

    // No throwing code.

    root_      = std::move(root);
    sentinel_  = std::move(sentinel);
    last->next = sentinel_.get();
    size_      = other.size_;
  }

  void
  move_(map& other) noexcept {
    root_     = std::move(other.root_);
    sentinel_ = std::move(other.sentinel_);
    first_    = other.first_;
    size_     = other.size_;
    other.clear();
  }

  owning_ptr_&
  get_owner_(tnode_ptr_ node) noexcept {

      if (!node->parent)
        return root_;

      else if (node->parent->left.get() == node)
        return node->parent->left;

      return node->parent->right;
  }

  template <owning_ptr_ tnode_::*right>
  void
  replace_by_child_(tnode_ptr_ node) noexcept {
    owning_ptr_ child = std::move(node->*right);
    child->priority   = node->priority;
    child->parent     = node->parent;
    get_owner_(node)  = std::move(child);
  }

  template <find_type_ find_type>
  lnode_ptr_
  find_(const key_type& k) const noexcept {

    tnode_ptr_ x = root_.get();
    lnode_ptr_ b = sentinel_.get();

    while (x) {

      if (find_type != upper_bound_ && k == x->value.first)
        return x;

      else if (k < x->value.first) {
        b = x;
        x = x->left.get();
      }

      else
        x = x->right.get();
    }

    return find_type == exact_ ? sentinel_.get() : b;
  }

  tnode_ptr_
  max_heapify_(tnode_ptr_ node) noexcept {

    tnode_ptr_ parent = node->parent;

    if (!parent || node->priority < parent->priority)
      return node;

    if (parent->left.get() == node)
      node = rotate_<&tnode_::right, &tnode_::left>(parent);

    else
      node = rotate_<&tnode_::left, &tnode_::right>(parent);

    return max_heapify_(node);
  }

  template <owning_ptr_ tnode_::* right, owning_ptr_ tnode_::* left>
  tnode_ptr_
  rotate_(tnode_ptr_ a) noexcept {
    /*
            a        b
           / \      / \
          b   * => *   a
         / \          / \
        *   x        x   *
    */

    owning_ptr_ b        = std::move(a->*left);        // b        = a.left

    a->*left             = std::move(b.get()->*right); // a.left   = x
    if (a->*left)
      (a->*left)->parent = a;                          // x.parent = a

    owning_ptr_& pa      = get_owner_(a);
    b->parent            = a->parent;                  // b.parent = a.parent
    a->parent            = b.get();                    // a.parent = b

    b.get()->*right      = std::move(pa);              // b.right  = a
    pa                   = std::move(b);

    return pa.get();
  }

  #ifndef NDEBUG

  void
  check_invariants_() const {
    check_invariants_(root_.get());
  }

  void
  check_invariants_(tnode_ptr_ node) const {

    if (!node)
      return;

    tnode_ptr_ prev = static_cast<tnode_ptr_>(node->prev);
    tnode_ptr_ next = static_cast<tnode_ptr_>(node->next);

    assert(!prev || prev->next == node);
    assert(!next || next->prev == node);

    assert(!prev || prev->value.first < node->value.first);

    assert(!next || next == sentinel_.get() ||
      node->value.first < next->value.first);

    tnode_ptr_ left  = node->left.get();
    tnode_ptr_ right = node->right.get();

    assert(!left  || left ->parent == node);
    assert(!right || right->parent == node);

    assert(!left  || (prev && left->value.first <= prev->value.first));
    assert(!right || (next && next->value.first <= right->value.first));

    assert(!left  || left ->priority <= node->priority);
    assert(!right || right->priority <= node->priority);

    check_invariants_(node->left.get());
    check_invariants_(node->right.get());
  }

  #endif

  std::default_random_engine engine_;
  owning_ptr_                root_;
  sentinel_ptr_              sentinel_;
  lnode_ptr_                 first_ = nullptr;
  size_type                  size_  = 0;

}; // class map

#endif // MAP_H_
