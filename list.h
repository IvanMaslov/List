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

        list_iterator(nullptr_t) = delete;

        list_iterator(list_iterator<T> const &other) : base(other.base) {}

        list_iterator &operator=(list_iterator const &) = default;

        D &operator*() const {
            return static_cast<node *>(base)->value;
        }

        D *operator->() const {
            return &static_cast<node *>(base)->value;
        }

        list_iterator operator++() &{
            base = base->r;
            return *this;
        }

        const list_iterator operator++(int) &{
            list_iterator copy = *this;
            ++*this;
            return copy;
        }

        list_iterator operator--() &{
            base = base->l;
            return *this;
        }

        list_iterator operator--(int) &{
            list_iterator copy = *this;
            --*this;
            return copy;
        }

        friend bool operator==(list_iterator const &a, list_iterator const &b) {
            return a.base == b.base;
        }

        friend bool operator!=(list_iterator const &a, list_iterator const &b) {
            return a.base != b.base;
        }

    private:
        base_node *base;

        explicit list_iterator(const base_node *p) : base(p) {};

        explicit list_iterator(base_node *p) : base(p) {};

        friend class list;
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

    template<class E>
    friend void swap(list<E> &a, list<E> &b) { a.swap(b); }

    void splice(const_iterator, list &, const_iterator, const_iterator);

private: // private struct declaration

    struct base_node {
        base_node *l;
        base_node *r;

        base_node() : l(this), r(this) {}
        virtual ~base_node() = default;
    };

    struct node : public base_node {
        T value;

        explicit node(const T &arg) : value(arg) {}
    };

private: //private members declaration

    base_node ROOT;

private: //private methods declaration

    void clone(base_node *) const;

    static void lnk(base_node *, base_node *);

public: //iterators methods


    iterator begin() noexcept { return iterator(ROOT.r); }

    const_iterator begin() const noexcept { return const_iterator(ROOT.r); }

    iterator end() noexcept { return iterator(&ROOT); }

    const_iterator end() const noexcept { return const_iterator(const_cast<base_node *>(&ROOT)); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    iterator insert(list::const_iterator pos, const T &val) {
        node *v = nullptr;
        try{
            v = new node(val);
        } catch(...){
            throw;
        }
        base_node *right = pos.base;
        base_node *left = right->l;
        lnk(left, v);
        lnk(v, right);
        return iterator(v);
    }

    iterator erase(list::const_iterator pos) {
        if(pos == begin()){
            base_node* begin_next = pos.base->r;
            delete pos.base;
            lnk(&ROOT, begin_next);
            return iterator(begin_next);
        }
        const_iterator pos_minus_one = pos++;
        return erase(pos_minus_one, pos);
    }

    iterator erase(list::const_iterator left, list::const_iterator right) {
        base_node *r = right.base;
        base_node *l = left.base->l;
        base_node* L = left.base;
        base_node* R = right.base->l;
        lnk(l, r);
        list<T> deleter;
        lnk(&deleter.ROOT, L);
        lnk(R, &deleter.ROOT);
        return iterator(r);
    }

};

template<class T>
bool list<T>::empty() const noexcept {
    return ROOT.l == &ROOT && ROOT.r == &ROOT;
}

template<class T>
void list<T>::clear() noexcept {
    base_node *t = ROOT.l;
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
void list<T>::clone(list::base_node *res) const {
    base_node *t = ROOT.l;
    base_node *start_res = res;
    while (t != &ROOT) {
        base_node *e = nullptr;
        try{
            e = new node(static_cast<node *>(t)->value);
        } catch (...){
            while(res != start_res){
                res = res->r;
                delete res->l;
            }
            throw;
        }
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
    if(&arg == this) return *this;
    list<T> cop(arg);
    swap(cop);
    return *this;
#ifndef __LIST_OPERATOR_EQUAL_NO_SAFE
    clear();
    arg.clone(&ROOT);
    return *this;
#endif
}

template<class T>
void list<T>::push_back(const T &val) { insert(end(), val); }

template<class T>
void list<T>::push_front(const T &val) { insert(begin(), val); }

template<class T>
void list<T>::pop_back() { auto j = end(); --j; erase(j); }

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
    auto k = end();
    --k;
    return *k;
}

template<class T>
const T &list<T>::back() const noexcept {
    auto k = end();
    --k;
    return *k;
}

template<class T>
void list<T>::splice(list::const_iterator pos, list &src, list::const_iterator from, list::const_iterator to) {
    if(from == to)return;
    base_node *src_left = from.base->l;
    base_node *src_right = to.base;
    base_node *from_base = from.base;
    base_node *to_base = to.base->l;
    lnk(src_left, src_right);
    base_node *insert_from = pos.base->l;
    base_node *insert_to = pos.base;
    lnk(insert_from, from_base);
    lnk(to_base, insert_to);
}


#endif //MYLIST_LIST_H



