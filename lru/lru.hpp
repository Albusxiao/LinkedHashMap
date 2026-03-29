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
            T *data;
            atom *pre, *next;

            atom(atom *a1, atom *a2, T *t) : pre(a1), next(a2), data(t) {
            };
        };

        atom *begin_atom, *end_atom;
        size_t size;

        static bool is_end_atom(atom *ato) {
            return ato->next == nullptr && ato->data == nullptr;
        }

    public:
        /**
         * elements
         * add whatever you want
         */
        class iterator;
        class const_iterator;
        friend class iterator;
        // --------------------------
        /**
         * the follows are constructors and destructors
         * you can also add some if needed.
         */
        double_list() : size(0) {
            begin_atom = end_atom = new atom(nullptr, nullptr, nullptr);
        }

        double_list(const double_list<T> &other) : size(0) {
            begin_atom = end_atom = new atom(nullptr, nullptr, nullptr);
            atom *current = other.begin_atom;
            while (!is_end_atom(current)) {
                insert_tail(*(current->data));
                current = current->next;
            }
        }

        ~double_list() {
            while (size != 0)delete_head();
            delete end_atom;
        }

        double_list &operator=(const double_list &other) {
            if (this == &other) return *this;
            clear();
            atom *current = other.begin_atom;
            while (!is_end_atom(current)) {
                insert_tail(*(current->data));
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
            atom *it;
            // --------------------------
            /**
             * the follows are constructors and destructors
             * you can also add some if needed.
             */
            iterator() : it(nullptr) {
            }

            iterator(atom *t) : it(t) {
            }

            iterator(const iterator &t) : it(t.it) {
            }

            ~iterator() = default;

            /**
             * iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                if (is_end_atom(it)) throw sjtu::invalid_iterator();
                if (it != nullptr) {
                    it = it->next;
                    return tmp;
                }
                throw sjtu::invalid_iterator();
            }

            /**
             * ++iter
             */
            iterator &operator++() {
                if (is_end_atom(it)) throw sjtu::invalid_iterator();
                if (it != nullptr) {
                    it = it->next;
                    return *this;
                }
                throw sjtu::invalid_iterator();
            }

            /**
             * iter--
             */
            iterator operator--(int) {
                iterator tmp = *this;
                if (it != nullptr && it->pre == nullptr) throw sjtu::invalid_iterator();
                if (it != nullptr) {
                    it = it->pre;
                    return tmp;
                }
                throw sjtu::invalid_iterator();
            }

            /**
             * --iter
             */
            iterator &operator--() {
                if (it != nullptr && it->pre == nullptr) throw sjtu::invalid_iterator();
                if (it != nullptr) {
                    it = it->pre;
                    return *this;
                }
                throw sjtu::invalid_iterator();
            }

            /**
             * if the iter didn't point to a value
             * throw " invalid"
             */
            T &operator*() const {
                if (it != nullptr && !is_end_atom(it)) return *(it->data);
                throw sjtu::invalid_iterator();
            }

            /**
             * other operation
             */
            T *operator->() const {
                if (it != nullptr && !is_end_atom(it)) return it->data;
                throw sjtu::invalid_iterator();
            }

            bool operator==(const iterator &rhs) const {
                return rhs.it == it;
            }

            bool operator!=(const iterator &rhs) const {
                return rhs.it != it;
            }

            bool operator==(const const_iterator &rhs) const { return it == rhs.it; }
            bool operator!=(const const_iterator &rhs) const { return it != rhs.it; }
        };

        class const_iterator {
        public:
            atom *it;

            const_iterator() : it(nullptr) {
            }

            const_iterator(atom *t) : it(t) {
            }
            const_iterator(const iterator &t): it(t.it) {
            }
            const_iterator(const const_iterator &t) : it(t.it) {
            }

            ~const_iterator() = default;

            const_iterator operator++(int) {
                const_iterator tmp = *this;
                if (is_end_atom(it)) throw sjtu::invalid_iterator();
                if (it != nullptr) {
                    it = it->next;
                    return tmp;
                }
                throw sjtu::invalid_iterator();
            }

            const_iterator &operator++() {
                if (is_end_atom(it)) throw sjtu::invalid_iterator();
                if (it != nullptr) {
                    it = it->next;
                    return *this;
                }
                throw sjtu::invalid_iterator();
            }

            const_iterator operator--(int) {
                const_iterator tmp = *this;
                if (it != nullptr && it->pre == nullptr) throw sjtu::invalid_iterator();
                if (it != nullptr) {
                    it = it->pre;
                    return tmp;
                }
                throw sjtu::invalid_iterator();
            }

            const_iterator &operator--() {
                if (it != nullptr && it->pre == nullptr) throw sjtu::invalid_iterator();
                if (it != nullptr) {
                    it = it->pre;
                    return *this;
                }
                throw sjtu::invalid_iterator();
            }

            const T &operator*() const {
                if (it != nullptr && !is_end_atom(it)) return *(it->data);
                throw sjtu::invalid_iterator();
            }

            const T *operator->() const {
                if (it != nullptr && !is_end_atom(it)) return it->data;
                throw sjtu::invalid_iterator();
            }

            bool operator==(const const_iterator &rhs) const {
                return rhs.it == it;
            }

            bool operator!=(const const_iterator &rhs) const {
                return rhs.it != it;
            }

            bool operator==(const iterator &rhs) const { return it == rhs.it; }
            bool operator!=(const iterator &rhs) const { return it != rhs.it; }
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
            return iterator(end_atom);
        }

        const_iterator begin() const {
            return const_iterator(begin_atom);
        }

        const_iterator end() const {
            return const_iterator(end_atom);
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
            if (pos.it == nullptr || is_end_atom(pos.it)) return end();
            atom *tmp = pos.it->next;
            if (pos.it->pre != nullptr) pos.it->pre->next = pos.it->next;
            else begin_atom = tmp;
            tmp->pre = pos.it->pre;
            delete pos.it->data;
            delete pos.it;
            --size;
            if (!is_end_atom(tmp)) return iterator(tmp);
            else return end();
        }

        /**
         * the following are operations of double list
         */
        void insert_head(const T &val) {
            atom *first = new atom(nullptr, begin_atom, new T(val));
            begin_atom->pre = first;
            if (begin_atom == end_atom)end_atom->pre = first;
            begin_atom = first;
            ++size;
        }

        void insert_tail(const T &val) {
            atom *last = new atom(end_atom->pre, end_atom, new T(val));
            if (last->pre != nullptr)last->pre->next = last;
            else begin_atom = last;
            end_atom->pre = last;
            ++size;
        }

        void delete_head() {
            erase(iterator(begin_atom));
        }

        void delete_tail() {
            erase(iterator(end_atom->pre));
        }

        /**
         * if didn't contain anything, return true,
         * otherwise false.
         */
        bool empty() const {
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
        using value_type = pair<Key, T>;
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
        hashmap() : size(131), current(0), hash_map(131, bucket_type()) {
        }

        hashmap(const hashmap &other) {
            size = other.size;
            current = other.current;
            hash_map.assign(other.hash_map.begin(), other.hash_map.end());
        }

        ~hashmap() {
            clear();
            hash_map.clear();
        }

        hashmap &operator=(const hashmap &other) {
            if (this == &other) return *this;
            clear();
            size = other.size;
            current = other.current;
            hash_map.assign(other.hash_map.begin(), other.hash_map.end());
            return *this;
        }

        class iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            hash_type *bucket;
            size_t bucket_index;
            typename bucket_type::iterator bucket_iter;

            iterator() : bucket(nullptr), bucket_index(0), bucket_iter() {
            }

            iterator(hash_type *a, size_t pos, typename bucket_type::iterator c) : bucket(a),
                bucket_index(pos), bucket_iter(c) {
            }

            iterator(const iterator &t) : bucket(t.bucket), bucket_index(t.bucket_index), bucket_iter(t.bucket_iter) {
            }

            ~iterator() = default;

            value_type &operator*() const {
                return *bucket_iter;
            }

            value_type *operator->() const {
                return &(*bucket_iter);
            }

            iterator &operator++() {
                if (!bucket || bucket_index >= bucket->size()) throw "End++";
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
                if (!bucket || bucket_index >= bucket->size())throw "Undefined Act";
                ++bucket_iter;
                while (bucket_index < bucket->size() && bucket_iter == (*bucket)[bucket_index].end()) {
                    ++bucket_index;
                    if (bucket_index < bucket->size())
                        bucket_iter = (*bucket)[bucket_index].begin();
                }
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

            const_iterator() : bucket(nullptr), bucket_index(0), bucket_iter() {
            }
            const_iterator(const iterator &t) :bucket(t.bucket), bucket_index(t.bucket_index), bucket_iter(t.bucket_iter) {
            }
            const_iterator(const hash_type *a, size_t pos,
                           typename bucket_type::const_iterator c) : bucket(a), bucket_index(pos), bucket_iter(c) {
            }

            const_iterator(const const_iterator &t) : bucket(t.bucket), bucket_index(t.bucket_index),
                                                      bucket_iter(t.bucket_iter) {
            }

            ~const_iterator() = default;

            const value_type &operator*() const {
                return *bucket_iter;
            }

            const value_type *operator->() const {
                return &(*bucket_iter);
            }

            const_iterator &operator++() {
                if (!bucket || bucket_index >= bucket->size())throw "End++";
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
                if (!bucket || bucket_index >= bucket->size())throw "Undefined Act";
                ++bucket_iter;
                while (bucket_index < bucket->size() && bucket_iter == (*bucket)[bucket_index].end()) {
                    ++bucket_index;
                    if (bucket_index < bucket->size())
                        bucket_iter = (*bucket)[bucket_index].begin();
                }
                return tmp;
            }

            bool operator==(const const_iterator &rhs) const {
                if (bucket == nullptr && rhs.bucket == nullptr) return true;
                return bucket == rhs.bucket && bucket_index == rhs.bucket_index && bucket_iter == rhs.bucket_iter;
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator==(const iterator &rhs) const {
                if (bucket == nullptr && rhs.bucket == nullptr) return true;
                return bucket == rhs.bucket && bucket_index == rhs.bucket_index && bucket_iter == rhs.bucket_iter;
            }

            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
        };

        /**
         * you need to expand the hashmap dynamically
         */

        iterator begin() {
            for (size_t i = 0; i < hash_map.size(); ++i) {
                if (!hash_map[i].empty())return iterator(&hash_map, i, hash_map[i].begin());
            }
            return end();
        }
        const_iterator begin() const{
            for (size_t i = 0; i < hash_map.size(); ++i) {
                if (!hash_map[i].empty())return const_iterator(&hash_map, i, hash_map[i].cbegin());
            }
            return end();
        }

        iterator end() {
            return iterator(&hash_map, hash_map.size(), typename bucket_type::iterator());
        }
        const_iterator end() const {
            return const_iterator(&hash_map, hash_map.size(), typename bucket_type::const_iterator());
        }
        const_iterator cbegin() const {
            for (size_t i = 0; i < hash_map.size(); ++i) {
                if (!hash_map[i].empty())return const_iterator(&hash_map, i, hash_map[i].cbegin());

            }
            return end();
        }

        const_iterator cend() const {
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
                --current;
                return true;
            } else return false;
        }

        void expand() {
            size <<= 1;
            hash_type tmp;
            tmp.resize(size);
            Hash h;
            for (auto &i: hash_map) {
                for (auto &j: i)tmp[h(j.first) % size].emplace_back(j.first, j.second);
            }
            hash_map = std::move(tmp);
        }
    };

    template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key> >
    class linked_hashmap : public hashmap<Key, T, Hash, Equal> {
        public:
            typedef pair<Key, T> value_type;
        using hmp = hashmap<Key, T, Hash, Equal>;
        double_list<value_type> insert_list;
        using list_iter= typename double_list<value_type>::iterator;
        using con_list_iter= typename double_list<value_type>::const_iterator;
        hashmap<Key,list_iter,Hash,Equal> key_to_iter;
        // --------------------------
        class const_iterator;

        class iterator {
        public:
            /**
             * elements
             * add whatever you want
             */
            typename double_list<value_type>::iterator it;

            iterator() {
            }

            iterator(const typename double_list<value_type>::iterator &_it) : it(_it) {
            }

            iterator(const iterator &other) : it(other.it) {
            }

            ~iterator() {
            }

            iterator &operator++() {
                ++it;
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                ++it;
                return tmp;
            }

            iterator &operator--() {
                --it;
                return *this;
            }

            iterator operator--(int) {
                iterator tmp = *this;
                --it;
                return tmp;
            }

            value_type &operator*() const { return *it; }
            value_type *operator->() const { return &(*it); }
            bool operator==(const iterator &rhs) const { return it == rhs.it; }
            bool operator!=(const iterator &rhs) const { return it != rhs.it; }
            bool operator==(const const_iterator &rhs) const { return it == rhs.it; }
            bool operator!=(const const_iterator &rhs) const { return it != rhs.it; }
            operator const_iterator() const {
                return const_iterator(it);
            };
        };

        class const_iterator {
        public:
            typename double_list<value_type>::const_iterator it;

            const_iterator() {
            }

            const_iterator(const typename double_list<value_type>::const_iterator &_it) : it(_it) {
            }
            const_iterator(const typename double_list<value_type>::iterator &_it): it(_it) {
            }

            const_iterator(const const_iterator &other) : it(other.it) {
            }
            const_iterator(const iterator &other) : it(other.it) {}
            ~const_iterator() {
            }

            const_iterator &operator++() {
                ++it;
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator tmp = *this;
                ++it;
                return tmp;
            }

            const_iterator &operator--() {
                --it;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp = *this;
                --it;
                return tmp;
            }

            const value_type &operator*() const { return *it; }
            const value_type *operator->() const { return &(*it); }
            bool operator==(const const_iterator &rhs) const { return it == rhs.it; }
            bool operator!=(const const_iterator &rhs) const { return it != rhs.it; }
        };
        iterator begin() { return iterator(insert_list.begin()); }
        iterator end() { return iterator(insert_list.end()); }
        const_iterator begin()const { return const_iterator(insert_list.begin()); }
        const_iterator end()const { return const_iterator(insert_list.end()); }
        const_iterator cbegin() const { return const_iterator(insert_list.begin()); }
        const_iterator cend() const { return const_iterator(insert_list.end()); }
        size_t size() const { return insert_list.length(); }

        iterator find(const Key &key) {
            Equal eq;
            auto tmp=key_to_iter.find(key);
            if (tmp==key_to_iter.end())return end();
            return iterator(tmp->second);
        }
        const_iterator find(const Key &key)const {
            Equal eq;
            auto tmp=key_to_iter.find(key);
            if (tmp==key_to_iter.end())return end();
            return const_iterator(tmp->second);
        }
        size_t count(const Key &key) const {
            return key_to_iter.find(key)!=key_to_iter.end()?1:0;
        }
        bool remove(const Key &key) {
            auto iter_map_it = key_to_iter.find(key);
            if (iter_map_it == key_to_iter.end())return false;
            list_iter list_it = iter_map_it->second;
            insert_list.erase(list_it);
            key_to_iter.remove(key);
            hmp::remove(key);
            return true;
        }
        void remove(iterator pos) {
            if (pos == end()) throw "iterator invalid";
            Key key = pos.it->first;
            remove(key);
        }

        iterator move_to_back(iterator pos) {
            if (pos == end()) return end();
            value_type val = *pos;
            Key key=pos->first;
            insert_list.erase(pos.it);
            insert_list.insert_tail(val);
            auto new_it=--insert_list.end();
            auto iter_it=key_to_iter.find(key);
            if (iter_it!=key_to_iter.end()) {
                iter_it->second=new_it;
            }
            return iterator(new_it);
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
            key_to_iter.clear();
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
                auto list_it = --insert_list.end();
                key_to_iter.insert({value.first, list_it});
                return {iterator(list_it), true};
            } else {
                // key 已存在，先更新链表节点的 value
                auto iter_map_it = key_to_iter.find(value.first);
                if (iter_map_it != key_to_iter.end()) {
                    *(iter_map_it->second) = value;
                }
                iterator it = find(value.first);
                auto new_it = move_to_back(it);
                return {new_it, false};
            }
        }
    };

    class lru {
        using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
        using value_type = sjtu::pair<Integer, Matrix<int> >;

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
        void save(const value_type &v)const{
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
        Matrix<int> *get(const Integer &v)const{
            auto it = cache.find(v);
            if (it != cache.end()) {
                auto new_it=cache.move_to_back(it);
                return &(new_it->second);
            }
            return nullptr;
        }

        /**
         * just print everything in the memory
         * to debug or test.
         * this operation follows the order, but don't
         * change the order.
         */
        void print(){
            sjtu::linked_hashmap<Integer, Matrix<int>,
                Hash, Equal>::iterator it;
            for (auto i = cache.begin(); i != cache.end(); i++) {
                std::cout << i->first.val << " " << i->second << std::endl;
            }
        }
    };
} // namespace sjtu

#endif



