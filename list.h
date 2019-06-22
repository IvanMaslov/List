//
// Created by maslov on 22.06.19.
//

#ifndef MYLIST_LIST_H
#define MYLIST_LIST_H

#include <algorithm>
#include <iterator>

template<class T>
class list {
private:
    struct base_node;
    struct node;
public:

    template<typename D>
    struct list_iterator : std::iterator<std::bidirectional_iterator_tag, D> {

        list_iterator() = default;

        list_iterator(list_iterator<T> const &other) : ptr(other.ptr) {}

        list_iterator &operator=(list_iterator const &) = default;

        D &operator*() const {
            return static_cast<node *>(ptr)->value;
        }

        D *operator->() const {
            return &static_cast<node *>(ptr)->value;
        }

        list_iterator operator++() &{
            ptr = ptr->next;
            return *this;
        }

        const list_iterator operator++(int) &{
            list_iterator copy = *this;
            ++*this;
            return copy;
        }

        list_iterator operator--() &{
            ptr = ptr->prev;
            return *this;
        }

        list_iterator operator--(int) &{
            list_iterator copy = *this;
            --*this;
            return copy;
        }

        friend bool operator==(list_iterator const &a, list_iterator const &b) {
            return a.ptr == b.ptr;
        }

        friend bool operator!=(list_iterator const &a, list_iterator const &b) {
            return a.ptr != b.ptr;
        }

    private:
        base_node *ptr;

        explicit list_iterator(base_node *p) : ptr(p) {};

        friend struct list;
    };

public: // public methods declaration

    using iterator = list_iterator<T>;
    using const_iterator = list_iterator<T const>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list() noexcept;

    list(const list &);

    ~list() noexcept;

    list &operator=(const list &);

    void push_back(const T &);

    void push_front(const T &);

    void pop_back();

    void pop_front();

    T &front();

    const T &front() const noexcept;

    T &back();

    const T &back() const noexcept;

    bool empty() const noexcept;

    void clear() noexcept;

    void swap(list &) noexcept;

    void splice(const_iterator, list &, const_iterator, const_iterator);

private: // private struct declaration

    struct base_node {
        base_node *l;
        base_node *r;

        base_node() : l(nullptr), r(nullptr) {}

        base_node *next() { return r; }

        base_node *prev() { return l; }
    };

    struct node : public base_node {
        T value;

        explicit node(const T &arg) : value(arg) {}
    };

private: //private members declaration

    base_node ROOT;

private: //private methods declaration

    void clone(base_node *);

    void lnk(base_node *, base_node *);

public: //iterators methods


    iterator begin() noexcept { return iterator(&ROOT.l); }
    const_iterator begin() const noexcept { return iterator(&ROOT.l); }

    iterator end() noexcept { return iterator(&ROOT); }
    const_iterator end() const noexcept { return iterator(&ROOT); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

    iterator insert(list::const_iterator pos, const T &val) {
        node *v = new node(val);
        base_node *right = pos.base;
        base_node *left = right->l;
        lnk(left, v);
        lnk(v, right);
        return iterator(v);
    }

    iterator erase(list::const_iterator pos) {
        const_iterator pos_minus_one = pos++;
        return erase(pos_minus_one, pos);
    }

    iterator erase(list::const_iterator left, list::const_iterator right) {
        base_node *r = right.base;
        base_node *l = left.base->l;
        lnk(l, r);
        list<T> deleter;
        lnk(left.base, &deleter.ROOT);
        lnk(&deleter.ROOT, right.base->l);
        return iterator(r);
    }

};

template<class T>
bool list<T>::empty() const noexcept {
    return ROOT.l == &ROOT && ROOT.r == &ROOT;
}

template<class T>
void list<T>::clear() noexcept {
    base_node *t = ROOT.r;
    while (t != &ROOT) {
        t = t->l;
        delete t->r;
    }
    ROOT.l = ROOT.r = &ROOT;
}


template<class T>
void list<T>::lnk(list::base_node *lhs, list::base_node *rhs) {
    lhs->r = rhs;
    rhs->l = lhs;
}

template<class T>
void list<T>::swap(list &arg) noexcept {
    if (empty()) {
        if (arg.empty())
            return;
        lnk(&ROOT, arg.ROOT.r);
        lnk(arg.ROOT.l, &ROOT);
        arg.ROOT.l = arg.ROOT.r = &arg.ROOT;
        return;
    }

    if (arg.empty()) {
        lnk(&arg.ROOT, ROOT.r);
        lnk(ROOT.l, &arg.ROOT);
        ROOT.l = ROOT.r = &ROOT;
        return;
    }
    base_node *L1 = ROOT.l;
    base_node *R1 = ROOT.r;

    base_node *L2 = arg.ROOT.l;
    base_node *R2 = arg.ROOT.r;

    lnk(&ROOT, R2);
    lnk(L2, &ROOT);

    lnk(&arg.ROOT, R1);
    lnk(L1, &arg.ROOT);
}

template<class T>
list<T>::list() noexcept {
    ROOT.l = ROOT.r = &ROOT;
}


template<class T>
void list<T>::clone(list::base_node *res) {
    base_node *t = ROOT.r;
    base_node *start_res = res;
    while (t != &ROOT) {
        base_node *e = new node(t);
        lnk(e, res);
        lnk(start_res, e);
        res = e;
        t = t->l;
    }
}

template<class T>
list<T>::list(const list &arg) {
    arg.clone(&ROOT);
}

template<class T>
list<T>::~list() noexcept {
    clear();
}

template<class T>
list<T> &list<T>::operator=(const list &arg) {
    clear();
    arg.clone(&ROOT);
    return *this;
}

template<class T>
void list<T>::push_back(const T &val) { insert(end(), val); }

template<class T>
void list<T>::push_front(const T &val) { insert(begin(), val); }

template<class T>
void list<T>::pop_back() { erase(--end()); }

template<class T>
void list<T>::pop_front() { erase(begin()); }

template<class T>
T &list<T>::front() {
    return *begin();
}

template<class T>
const T &list<T>::front() const noexcept {
    return *begin();
}

template<class T>
T &list<T>::back() {
    return *--end();
}

template<class T>
const T &list<T>::back() const noexcept {
    return *--end();
}

template<class T>
void list<T>::splice(list::const_iterator pos, list & src, list::const_iterator from, list::const_iterator to) {
    base_node* src_left = from.ptr->r;
    base_node* src_right = to.ptr;
    base_node* from_base = from.ptr;
    base_node* to_base = to.ptr->l;
    lnk(src_left, src_right);
    base_node* insert_from = pos.ptr->l;
    base_node* insert_to = pos.ptr;
    lnk(insert_from, from_base);
    lnk(to_base, insert_to);
}


#endif //MYLIST_LIST_H
