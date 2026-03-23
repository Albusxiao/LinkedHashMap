#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include <list>

#include "class-integer.hpp"
#include "class-matrix.hpp"
#include "exceptions.hpp"
#include "utility.hpp"

using std::vector;
using std::list;
class Hash {
public:
    unsigned int operator()(Integer lhs) const {
        int val = lhs.val;
        return std::hash<int>()(val);
    }
};

class Equal {
public:
    bool operator()(const Integer &lhs, const Integer &rhs) const { return lhs.val == rhs.val; }
};

namespace sjtu {
    template<class T>
    class double_list {
        struct atom {
            T data;
            atom *pre, *next;
            atom(atom *a1, atom *a2, T t) : pre(a1), next(a2), data(t) {};
        };
        atom *begin_atom, *end_atom;
        size_t size;

    public:
        /**
         * elements
         * add whatever you want
         */

        // --------------------------
        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
         */
        double_list() : begin_atom(nullptr),end_atom(nullptr),size(0){}

        double_list(const double_list<T> &other) : begin_atom(nullptr), end_atom(nullptr), size(0) {
            atom* current = other.begin_atom;
            while (current != nullptr) {
                insert_tail(current->data);
                current = current->next;
            }
        }

        ~double_list() {
            while (size != 0)delete_head();
        }
        double_list &operator=(const double_list & other) {
            atom* current = other.begin_atom;
            while (current != nullptr) {
                insert_tail(current->data);
                current = current->next;
            }
            return *this;
        }
        class iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            atom *it,*end_atom;
            // --------------------------
            /**
             * the follows are constructors and destructors
             * you can also add some if needed.
             */
            iterator() : it(nullptr),end_atom(nullptr) {
            }
            iterator(const atom* &t,const atom* end) : it(t),end_atom(end) {
            }
            iterator(const iterator &t) : it(t.it),end_atom(t.end_atom) {
            }

            ~iterator() = default;

            /**
             * iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                if (it==nullptr)throw "End++";
                if (it!=nullptr&&it->next==nullptr)return iterator();
                if (it != nullptr)it = it->next;
                return tmp;
            }

            /**
             * ++iter
             */
            iterator &operator++() {
                if (it==nullptr)throw "End++";
                if (it!=nullptr&&it->next==nullptr)return iterator();
                if (it != nullptr)it = it->next;
                return *this;
            }

            /**
             * iter--
             */
            iterator operator--(int) {
                iterator tmp = *this;
                if (it==nullptr)return iterator(end_atom,end_atom);
                if (it!=nullptr&&it->pre == nullptr)throw "Begin()--";
                if (it!=nullptr)it = it->pre;
                return tmp;
            }

            /**
             * --iter
             */
            iterator &operator--() {
                if (it==nullptr)return iterator(end_atom,end_atom);
                if (it!=nullptr&&it->pre == nullptr)throw "Begin()--";
                if (it!=nullptr)it = it->pre;
                return *this;
            }

            /**
             * if the iter didn't point to a value
             * throw " invalid"
             */
            T &operator*() const {
                if (it != nullptr)return it->data;
                throw "nullptr";
            }

            /**
             * other operation
             */
            T *operator->() const noexcept {
                if (it != nullptr)return &(it->data);
                throw "nullptr";
            }

            bool operator==(const iterator &rhs) const {
                return rhs.it == it;
            }

            bool operator!=(const iterator &rhs) const {
                return rhs.it != it;
            }
        };

        /**
         * return an iterator to the beginning
         */
        iterator begin() {
            return iterator(begin_atom);
        }

        /**
         * return an iterator to the ending
         * in fact, it returns the iterator point to nothing,
         * just after the last element.
         */
        iterator end() {
            return iterator(nullptr);
        }

        /**
         * if the iter didn't point to anything, do nothing,
         * otherwise, delete the element pointed by the iter
         * and return the iterator point at the same "index"
         * e.g.
         * 	if the origin iterator point at the 2nd element
         * 	the returned iterator also point at the
         *  2nd element of the list after the operation
         *  or nothing if the list after the operation
         *  don't contain 2nd elememt.
         */
        iterator erase(iterator pos) {
            atom *tmp = pos.it->next;
            if (pos.it == nullptr)return iterator();
            if (pos.it->next != nullptr)pos.it->next->pre = pos.it->pre;
            if (pos.it->pre != nullptr)pos.it->pre->next = pos.it->next;
            delete pos.it;
            --size;
            return iterator(tmp);
        }

        /**
         * the following are operations of double list
         */
        void insert_head(const T &val) {
            atom *first = new atom(nullptr, begin_atom, val);
            begin_atom->pre = first;
            begin_atom = first;
            ++size;
        }

        void insert_tail(const T &val) {
            atom *last = new atom(end_atom, nullptr, val);
            end_atom->next = last;
            end_atom = last;
            ++size;
        }

        void delete_head() { erase(iterator(begin_atom)); --size;}
        void delete_tail() { erase(iterator(end_atom)); --size;}
        /**
         * if didn't contain anything, return true,
         * otherwise false.
         */
        bool empty() {
            return size == 0;
        }
        void clear() {
            while (size != 0)delete_head();
        }
    };

    template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key> >
    class hashmap {
        const double load_factor=0.75;
    public:
        using value_type = pair<const Key, T>;
        using bucket_type = list<value_type>;
        using hash_type=vector<bucket_type>;
        /**
         * elements
         * add whatever you want
         */
        hash_type hash_map;
        /**
         * size: the length of hashmap
         * current: the actual number of elements in hashmap
        */
        size_t size,current;
        // --------------------------

        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
         */
        hashmap():size(131),current(0) {hash_map.assign(131,list<value_type>());}
        hashmap(const hashmap &other):size(other.size),hash_map(other.hash_map),current(other.current){}

        ~hashmap() {
            for (auto i:hash_map)i.clear();
            hash_map.clear();
        }

        hashmap &operator=(const hashmap &other) {
            size=other.size;
            hash_map=other.hash_map;
            current=other.current;
            return *this;
        }

        class iterator {

        public:
            /**
             * elements
             * add whatever you want
             */
            hash_type* bucket;
            size_t bucket_index;
            bucket_type::iterator bucket_iter;
            // --------------------------
            /**
             * the follows are constructors and destructors
             * you can also add some if needed.
             */
            iterator():bucket(nullptr),bucket_index(0),bucket_iter(){}
            iterator(const iterator &t):bucket_index(t.bucket_index),bucket_iter(t.bucket){
                bucket_iter=t.bucket_iter;
            }

            ~iterator() =default;

            /**
             * if point to nothing
             * throw
             */
            value_type &operator*() const {
                if (bucket_iter==nullptr)throw "*nullptr";
                return *bucket_iter;
            }

            /**
             * other operation
             */
            value_type *operator->() const noexcept {
                if (bucket_iter==nullptr)throw "*nullptr";
                return *bucket_iter;
            }

            bool operator==(const iterator &rhs) const {
                if (bucket==nullptr&&rhs.bucket==nullptr)return true;
                else if (bucket==rhs.bucket)return bucket_iter==rhs.bucket_iter;
                return false;
            }

            bool operator!=(const iterator &rhs) const {
                return !(*this== rhs);
            }
        };

        void clear() {
            for (auto i:hash_map)i.clear();
            current=0;
        }

        /**
         * you need to expand the hashmap dynamically
         */
        void expand() {
            size<<=1;
            hash_type tmp;
            tmp.assign(size,bucket_type());
            Hash h;
            for (auto i:hash_map) {
                for (auto j:i) {
                    tmp[h(j.first)%size].emplace_back(j);
                }
            }
            hash_map=move(tmp);
        }

        /**
         * the iterator point at nothing
         */
        iterator end() const {
            return iterator(hash_map,hash_map.size()-1,hash_map.end());
        }

        /**
         * find, return a pointer point to the value
         * not find, return the end (point to nothing)
         */
        iterator find(const Key &key) const {
            Hash h;
            size_t pos=h(key)%size;
            for (auto it=hash_map[pos].begin();it!=hash_map[pos].end();++it)
                if (it->firs==key)return iterator(hash_map,pos,it);
            return end();
        }

        /**
         * already have a value_pair with the same key
         * -> just update the value, return false
         * not find a value_pair with the same key
         * -> insert the value_pair, return true
         */
        sjtu::pair<iterator, bool> insert(const value_type &value_pair) {
            iterator it=find(value_pair.first);
            bool hash_bool=true;
            if (it!=hash_map.end())it->second=value_pair.second,hash_bool=false;
            else {
                ++current;
                if (size*load_factor<current)expand();
                Hash h;
                size_t pos=h(value_pair.first)%size;
                it=(hash_map,pos,hash_map[pos].emplace_back(value_pair));
            }
            return pair<iterator,bool>{it,hash_bool};
        }

        /**
         * the value_pair exists, remove and return true
         * otherwise, return false
         */
        bool remove(const Key &key) {
            iterator it=find(key);
            if (it!=hash_map.end()) {
                hash_map[it.bucket_index].erase(it.bucket_iter);
                return true;
            }
            else return false;
        }
    };

    template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key> >
    class linked_hashmap : public hashmap<Key, T, Hash, Equal> {
    public:
        typedef pair<const Key, T> value_type;
        /**
         * elements
         * add whatever you want
         */
        // --------------------------
        class const_iterator;

        class iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            // --------------------------
            iterator() {
            }

            iterator(const iterator &other) {
            }

            ~iterator() {
            }

            /**
             * iter++
             */
            iterator operator++(int) {
            }

            /**
             * ++iter
             */
            iterator &operator++() {
            }

            /**
             * iter--
             */
            iterator operator--(int) {
            }

            /**
             * --iter
             */
            iterator &operator--() {
            }

            /**
             * if the iter didn't point to a value
             * throw "star invalid"
             */
            value_type &operator*() const {
            }

            value_type *operator->() const noexcept {
            }

            /**
             * operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
            }

            bool operator!=(const iterator &rhs) const {
            }

            bool operator==(const const_iterator &rhs) const {
            }

            bool operator!=(const const_iterator &rhs) const {
            }
        };

        class const_iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            // --------------------------
            const_iterator() {
            }

            const_iterator(const iterator &other) {
            }

            /**
             * iter++
             */
            const_iterator operator++(int) {
            }

            /**
             * ++iter
             */
            const_iterator &operator++() {
            }

            /**
             * iter--
             */
            const_iterator operator--(int) {
            }

            /**
             * --iter
             */
            const_iterator &operator--() {
            }

            /**
             * if the iter didn't point to a value
             * throw
             */
            const value_type &operator*() const {
            }

            const value_type *operator->() const noexcept {
            }

            /**
             * operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
            }

            bool operator!=(const iterator &rhs) const {
            }

            bool operator==(const const_iterator &rhs) const {
            }

            bool operator!=(const const_iterator &rhs) const {
            }
        };

        linked_hashmap() {
        }

        linked_hashmap(const linked_hashmap &other) {
        }

        ~linked_hashmap() {
        }

        linked_hashmap &operator=(const linked_hashmap &other) {
        }

        /**
         * return the value connected with the Key(O(1))
         * if the key not found, throw
         */
        T &at(const Key &key) {
        }

        const T &at(const Key &key) const {
        }

        T &operator[](const Key &key) {
        }

        const T &operator[](const Key &key) const {
        }

        /**
         * return an iterator point to the first
         * inserted and existed element
         */
        iterator begin() {
        }

        const_iterator cbegin() const {
        }

        /**
         * return an iterator after the last inserted element
         */
        iterator end() {
        }

        const_iterator cend() const {
        }

        /**
         * if didn't contain anything, return true,
         * otherwise false.
         */
        bool empty() const {
        }

        void clear() {
        }

        size_t size() const {
        }

        /**
         * insert the value_piar
         * if the key of the value_pair exists in the map
         * update the value instead of adding a new element，
         * then the order of the element moved from inner of the
         * list to the head of the list
         * and return false
         * if the key of the value_pair doesn't exist in the map
         * add a new element and return true
         */
        pair<iterator, bool> insert(const value_type &value) {
        }

        /**
         * erase the value_pair pointed by the iterator
         * if the iterator points to nothing
         * throw
         */
        void remove(iterator pos) {
        }

        /**
         * return how many value_pairs consist of key
         * this should only return 0 or 1
         */
        size_t count(const Key &key) const {
        }

        /**
         * find the iterator points at the value_pair
         * which consist of key
         * if not find, return the iterator
         * point at nothing
         */
        iterator find(const Key &key) {
        }
    };

    class lru {
        using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
        using value_type = sjtu::pair<const Integer, Matrix<int> >;

    public:
        lru(int size) {
        }

        ~lru() {
        }

        /**
         * save the value_pair in the memory
         * delete something in the memory if necessary
         */
        void save(const value_type &v) const {
        }

        /**
         * return a pointer contain the value
         */
        Matrix<int> *get(const Integer &v) const {
        }

        /**
         * just print everything in the memory
         * to debug or test.
         * this operation follows the order, but don't
         * change the order.
         */
        void print() {
        }
    };
} // namespace sjtu

#endif
