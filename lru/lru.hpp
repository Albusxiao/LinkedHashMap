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

            atom(atom *a1, atom *a2, T t) : pre(a1), next(a2), data(t) {
            };
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
        double_list() : begin_atom(nullptr), end_atom(nullptr), size(0) {
        }

        double_list(const double_list<T> &other) : begin_atom(nullptr), end_atom(nullptr), size(0) {
            atom *current = other.begin_atom;
            while (current != nullptr) {
                insert_tail(current->data);
                current = current->next;
            }
        }

        ~double_list() {
            while (size != 0)delete_head();
        }

        double_list &operator=(const double_list &other) {
            if (this == &other) return *this;
            clear();
            atom *current = other.begin_atom;
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
            atom *it, *end_atom;
            // --------------------------
            /**
             * the follows are constructors and destructors
             * you can also add some if needed.
             */
            iterator() : it(nullptr), end_atom(nullptr) {
            }

            iterator(atom *t, atom *end) : it(t), end_atom(end) {
            }

            iterator(const iterator &t) : it(t.it), end_atom(t.end_atom) {
            }

            ~iterator() = default;

            /**
             * iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                if (it == nullptr)throw "End++";
                if (it != nullptr && it->next == nullptr)return iterator();
                if (it != nullptr)it = it->next;
                return tmp;
            }

            /**
             * ++iter
             */
            iterator &operator++() {
                if (it == nullptr) {
                    try {
                        throw "End++";
                    } catch (const char *c) {
                        std::cout << c << std::endl;
                    }
                }
                if (it != nullptr && it->next == nullptr) {
                    this->it = nullptr;
                    return *this;
                }
                if (it != nullptr)it = it->next;
                return *this;
            }

            /**
             * iter--
             */
            iterator operator--(int) {
                iterator tmp = *this;
                if (it == nullptr)return iterator(end_atom, end_atom);
                if (it != nullptr && it->pre == nullptr)throw "Begin()--";
                if (it != nullptr)it = it->pre;
                return tmp;
            }

            /**
             * --iter
             */
            iterator &operator--() {
                if (it == nullptr) {
                    this->it = end_atom;
                    return *this;
                }
                if (it != nullptr && it->pre == nullptr)throw "Begin()--";
                if (it != nullptr)it = it->pre;
                return *this;
            }

            /**
             * if the iter didn't point to a value
             * throw " invalid"
             */
            T &operator*() const {
                if (it != nullptr)return it->data;
                throw "star invalid";
            }

            /**
             * other operation
             */
            T *operator->() const {
                if (it != nullptr)return &(it->data);
                throw "star invalid";
            }

            bool operator==(const iterator &rhs) const {
                return rhs.it == it;
            }

            bool operator!=(const iterator &rhs) const {
                return rhs.it != it;
            }
        };
        class const_iterator {
        public:
            atom *it, *end_atom;
            const_iterator() : it(nullptr), end_atom(nullptr) {}
            const_iterator(atom *t, atom *end) : it(t), end_atom(end) {}
            const_iterator(const const_iterator &t) : it(t.it), end_atom(t.end_atom) {}
            ~const_iterator() = default;
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                if (it == nullptr) throw "End++";
                if (it != nullptr && it->next == nullptr) return const_iterator();
                if (it != nullptr) it = it->next;
                return tmp;
            }
            const_iterator &operator++() {
                if (it == nullptr) throw "End++";
                if (it != nullptr && it->next == nullptr) {
                    this->it = nullptr;
                    return *this;
                }
                if (it != nullptr) it = it->next;
                return *this;
            }
            const_iterator operator--(int) {
                const_iterator tmp = *this;
                if (it == nullptr) return const_iterator(end_atom, end_atom);
                if (it != nullptr && it->pre == nullptr) throw "Begin()--";
                if (it != nullptr) it = it->pre;
                return tmp;
            }
            const_iterator &operator--() {
                if (it == nullptr) {
                    this->it = end_atom;
                    return *this;
                }
                if (it != nullptr && it->pre == nullptr) throw "Begin()--";
                if (it != nullptr) it = it->pre;
                return *this;
            }
            const T &operator*() const {
                if (it != nullptr) return it->data;
                throw "star invalid";
            }
            const T *operator->() const {
                if (it != nullptr) return &(it->data);
                throw "star invalid";
            }
            bool operator==(const const_iterator &rhs) const {
                return rhs.it == it;
            }
            bool operator!=(const const_iterator &rhs) const {
                return rhs.it != it;
            }
        };

        /**
         * return an iterator to the beginning
         */
        iterator begin() {
            return iterator(begin_atom, end_atom);
        }

        /**
         * return an iterator to the ending
         * in fact, it returns the iterator point to nothing,
         * just after the last element.
         */
        iterator end() {
            return iterator(nullptr, end_atom);
        }
        const_iterator begin() const {
            return const_iterator(begin_atom, end_atom);
        }
        const_iterator end() const {
            return const_iterator(nullptr, end_atom);
        }
        const_iterator cbegin() const {
            return begin();
        }
        const_iterator cend() const {
            return end();
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
            return iterator(tmp, end_atom);
        }

        /**
         * the following are operations of double list
         */
        void insert_head(const T &val) {
            atom *first = new atom(nullptr, begin_atom, val);
            if (begin_atom != nullptr) {
                begin_atom->pre = first;
            } else {
                end_atom = first;
            }
            begin_atom = first;
            ++size;
        }

        void insert_tail(const T &val) {
            atom *last = new atom(end_atom, nullptr, val);
            if (end_atom != nullptr) {
                end_atom->next = last;
            } else {
                begin_atom = last;
            }
            end_atom = last;
            ++size;
        }

        void delete_head() {
            erase(iterator(begin_atom, end_atom));
        }

        void delete_tail() {
            erase(iterator(end_atom, end_atom));
        }

        /**
         * if didn't contain anything, return true,
         * otherwise false.
         */
        bool empty() {
            return size == 0;
        }
        size_t length() const {
            return size;
        }
        void clear() {
            while (size != 0)delete_head();
        }
    };

    template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key> >
    class hashmap {
        const double load_factor = 0.75;

    public:
        using value_type = pair<const Key, T>;
        using bucket_type = list<value_type>;
        using hash_type = vector<bucket_type>;
        /**
         * elements
         * add whatever you want
         */
        hash_type hash_map;
        /**
         * size: the length of hashmap
         * current: the actual number of elements in hashmap
        */
        size_t size, current;
        // --------------------------

        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
         */
        hashmap() : size(131), current(0) {
            for (int i=1;i<=131;i++)hash_map.push_back(bucket_type());
        }

        hashmap(const hashmap &other) {
            size = other.size;
            current = other.current;
            hash_map.resize(size);
            for (size_t i = 0; i < other.hash_map.size(); ++i) {
                for (auto &j: other.hash_map[i]) {
                    hash_map[i].emplace_back(j.first, j.second);
                }
            }
        }

        ~hashmap() {
            for (auto &i: hash_map)i.clear();
            hash_map.clear();
        }

        hashmap &operator=(const hashmap &other) {
            if (this == &other) return *this;
            clear();
            size = other.size;
            current = other.current;
            hash_map.resize(size);
            for (size_t i = 0; i < other.hash_map.size(); ++i) {
                for (auto &j: other.hash_map[i]) {
                    hash_map[i].emplace_back(j.first, j.second);
                }
            }
            return *this;
        }

        class iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            const hash_type *bucket;
            size_t bucket_index;
            typename bucket_type::const_iterator bucket_iter;
            iterator() : bucket(nullptr), bucket_index(0), bucket_iter() {}
            iterator(const hash_type *a, size_t pos, typename bucket_type::const_iterator c) : bucket(a), bucket_index(pos), bucket_iter(c) {}
            iterator(const iterator &t) : bucket(t.bucket), bucket_index(t.bucket_index), bucket_iter(t.bucket_iter) {}
            ~iterator() = default;
            value_type &operator*() const {
                return const_cast<value_type&>(*bucket_iter);
            }
            value_type *operator->() const {
                return const_cast<value_type*>(&(*bucket_iter));
            }
            iterator &operator++() {
                if (!bucket || bucket_index >= bucket->size()) return *this;
                ++bucket_iter;
                while (bucket_index < bucket->size() && bucket_iter == (*bucket)[bucket_index].end()) {
                    ++bucket_index;
                    if (bucket_index < bucket->size())
                        bucket_iter = (*bucket)[bucket_index].begin();
                }
                return *this;
            }
            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }
            bool operator==(const iterator &rhs) const {
                if (bucket == nullptr && rhs.bucket == nullptr) return true;
                return bucket == rhs.bucket && bucket_index == rhs.bucket_index && bucket_iter == rhs.bucket_iter;
            }
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
        };
        class const_iterator {
        public:
            const hash_type *bucket;
            size_t bucket_index;
            typename bucket_type::const_iterator bucket_iter;
            const_iterator() : bucket(nullptr), bucket_index(0), bucket_iter() {}
            const_iterator(const hash_type *a, size_t pos, typename bucket_type::const_iterator c) : bucket(a), bucket_index(pos), bucket_iter(c) {}
            const_iterator(const const_iterator &t) : bucket(t.bucket), bucket_index(t.bucket_index), bucket_iter(t.bucket_iter) {}
            ~const_iterator() = default;
            const value_type &operator*() const {
                return *bucket_iter;
            }
            const value_type *operator->() const {
                return &(*bucket_iter);
            }
            const_iterator &operator++() {
                if (!bucket || bucket_index >= bucket->size()) return *this;
                ++bucket_iter;
                while (bucket_index < bucket->size() && bucket_iter == (*bucket)[bucket_index].end()) {
                    ++bucket_index;
                    if (bucket_index < bucket->size())
                        bucket_iter = (*bucket)[bucket_index].begin();
                }
                return *this;
            }
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                ++(*this);
                return tmp;
            }
            bool operator==(const const_iterator &rhs) const {
                if (bucket == nullptr && rhs.bucket == nullptr) return true;
                return bucket == rhs.bucket && bucket_index == rhs.bucket_index && bucket_iter == rhs.bucket_iter;
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };

        /**
         * you need to expand the hashmap dynamically
         */

        iterator begin() {
            for (size_t i = 0; i < hash_map.size(); ++i) {
                if (!hash_map[i].empty()) {
                    return iterator(&hash_map, i, hash_map[i].begin());
                }
            }
            return end();
        }
        iterator end() {
            return iterator(&hash_map, hash_map.size(), typename bucket_type::const_iterator());
        }
        const_iterator begin() const {
            for (size_t i = 0; i < hash_map.size(); ++i) {
                if (!hash_map[i].empty()) {
                    return const_iterator(&hash_map, i, hash_map[i].begin());
                }
            }
            return end();
        }
        const_iterator end() const {
            return const_iterator(&hash_map, hash_map.size(), typename bucket_type::const_iterator());
        }
        /**
         * find, return a pointer point to the value
         * not find, return the end (point to nothing)
         */
        iterator find(const Key &key) {
            Hash h;
            size_t pos = h(key) % size;
            Equal E;
            for (auto it = hash_map[pos].begin(); it != hash_map[pos].end(); ++it)
                if (E(it->first, key)) return iterator(&hash_map, pos, it);
            return end();
        }
        const_iterator find(const Key &key) const {
            Hash h;
            size_t pos = h(key) % size;
            Equal E;
            for (auto it = hash_map[pos].begin(); it != hash_map[pos].end(); ++it)
                if (E(it->first, key)) return const_iterator(&hash_map, pos, it);
            return end();
        }
        void clear() {
            for (auto &i: hash_map) i.clear();
            current = 0;
        }
        sjtu::pair<iterator, bool> insert(const value_type &value_pair) {
            iterator it = find(value_pair.first);
            bool hash_bool = true;
            if (it != end()) it->second = value_pair.second, hash_bool = false;
            else {
                ++current;
                if (size * load_factor < current)expand();
                Hash h;
                size_t pos = h(value_pair.first) % size;
                hash_map[pos].emplace_back(value_pair);
                it = iterator(&hash_map, pos, --hash_map[pos].end());
            }
            return pair<iterator, bool>{it, hash_bool};
        }

        /**
         * the value_pair exists, remove and return true
         * otherwise, return false
         */
        bool remove(const Key &key) {
            iterator it = find(key);
            if (it != end()) {
                hash_map[it.bucket_index].erase(it.bucket_iter);
                return true;
            } else return false;
        }
        void expand() {
            size <<= 1;
            hash_type tmp;
            tmp.resize(size);
            Hash h;
            for (auto &i: hash_map) {
                for (auto &j: i) {
                    tmp[h(j.first) % size].emplace_back(j.first, j.second);
                }
            }
            hash_map = std::move(tmp);
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
        using hmp = hashmap<Key, T, Hash, Equal>;
        double_list<value_type> insert_list;
        // --------------------------
        class const_iterator;

        class iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            typename double_list<value_type>::iterator it;
            iterator() {}
            iterator(const typename double_list<value_type>::iterator &_it) : it(_it) {}
            iterator(const iterator &other) : it(other.it) {}
            ~iterator() {}
            iterator &operator++() { ++it; return *this; }
            iterator operator++(int) { iterator tmp = *this; ++it; return tmp; }
            iterator &operator--() { --it; return *this; }
            iterator operator--(int) { iterator tmp = *this; --it; return tmp; }
            value_type &operator*() const { return *it; }
            value_type *operator->() const { return &(*it); }
            bool operator==(const iterator &rhs) const { return it == rhs.it; }
            bool operator!=(const iterator &rhs) const { return it != rhs.it; }
        };
        class const_iterator {
        public:
            typename double_list<value_type>::const_iterator it;
            const_iterator() {}
            const_iterator(const typename double_list<value_type>::const_iterator &_it) : it(_it) {}
            const_iterator(const const_iterator &other) : it(other.it) {}
            ~const_iterator() {}
            const_iterator &operator++() { ++it; return *this; }
            const_iterator operator++(int) { const_iterator tmp = *this; ++it; return tmp; }
            const_iterator &operator--() { --it; return *this; }
            const_iterator operator--(int) { const_iterator tmp = *this; --it; return tmp; }
            const value_type &operator*() const { return *it; }
            const value_type *operator->() const { return &(*it); }
            bool operator==(const const_iterator &rhs) const { return it == rhs.it; }
            bool operator!=(const const_iterator &rhs) const { return it != rhs.it; }
        };
        iterator begin() { return iterator(insert_list.begin()); }
        iterator end() { return iterator(insert_list.end()); }
        const_iterator begin() const { return const_iterator(insert_list.begin()); }
        const_iterator end() const { return const_iterator(insert_list.end()); }
        const_iterator cbegin() const { return begin(); }
        const_iterator cend() const { return end(); }
        size_t size() const { return insert_list.length(); }
        iterator find(const Key &key) {
            Equal eq;
            for (auto i = insert_list.begin(); i != insert_list.end(); ++i) {
                if (eq(i->first, key)) return iterator(i);
            }
            return end();
        }
        const_iterator find(const Key &key) const {
            Equal eq;
            for (auto i = insert_list.cbegin(); i != insert_list.cend(); ++i) {
                if (eq(i->first, key)) return const_iterator(i);
            }
            return end();
        }
        size_t count(const Key &key) const {
            return find(key) != end() ? 1 : 0;
        }
        void remove(iterator pos) {
            if (pos.it == insert_list.end()) throw "iterator invalid";
            Key key = pos.it->first;
            insert_list.erase(pos.it);
            hmp::remove(key);
        }
        void move_to_back(iterator pos) {
            if (pos == end()) return;
            value_type val = *pos;
            insert_list.erase(pos.it);
            insert_list.insert_tail(val);
        }

        /**
         * return the value connected with the Key(O(1))
         * if the key not found, throw
         */
        T &at(const Key &key) {
            auto it = hmp::find(key);
            if (it == hmp::end()) {
                throw "key not found";
            }
            return it->second;
        }

        const T &at(const Key &key) const {
            auto it = hmp::find(key);
            if (it == hmp::end()) {
                throw "key not found";
            }
            return it->second;
        }

        T &operator[](const Key &key) {
            return at(key);
        }

        const T &operator[](const Key &key) const {
            return at(key);
        }

        /**
         * if didn't contain anything, return true,
         * otherwise false.
         */
        bool empty() const {
            return insert_list.empty();
        }

        void clear() {
            hmp::clear();
            insert_list.clear();
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
            auto hash_result = hashmap<Key, T, Hash, Equal>::insert(value);
            if (hash_result.second) {
                insert_list.insert_tail(value);
                iterator it;
                it.it = --insert_list.end();
                return {it, true};
            } else {
                iterator it = find(value.first);
                return {it, false};
            }
        }
    };

    class lru {
        using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
        using value_type = sjtu::pair<const Integer, Matrix<int> >;

    public:
        mutable lmap cache;
        int capacity;

        lru(int size) : capacity(size) {
        }

        ~lru() {
            cache.clear();
        }

        /**
         * save the value_pair in the memory
         * delete something in the memory if necessary
         */
        void save(const value_type &v) const {
            const Integer &key = v.first;
            auto res = cache.insert(v);
            if (!res.second) {
                cache.move_to_back(res.first);
            } else {
                if ((int) cache.size() > capacity) {
                    auto first = cache.begin();
                    cache.remove(first);
                }
            }
        }

        /**
         * return a pointer contain the value
         */
        Matrix<int> *get(const Integer &v) const {
            auto it = cache.find(v);
            if (it != cache.end()) {
                cache.move_to_back(it);
                return &(it->second);
            }
            return nullptr;
        }

        /**
         * just print everything in the memory
         * to debug or test.
         * this operation follows the order, but don't
         * change the order.
         */
        void print (){
            sjtu :: linked_hashmap < Integer , Matrix < int > ,
            Hash , Equal >:: iterator it ;
            for (auto i= cache.begin(); i != cache.end(); i ++){
                std :: cout << i->first.val << " "<<i->second << std :: endl ;
            }
        }
    };
} // namespace sjtu

#endif
