#ifndef HASHTBL_H
#define HASHTBL_H

#include <iostream>
#include <forward_list> // std::forward_list


namespace ac{
    template <class KeyType, class DataType>
    class HashEntry{
        //=== Puclic interface.
        public:
            /// Constructor
            HashEntry(KeyType k_, DataType d_)
            : m_key(k_), m_data(d_){/* Empty */}

            KeyType m_key; //!< Stores the key for an entry .
            DataType m_data; //!< Stores the data for an entry .
    };

    bool isPrime(int number){
        int i;
        for(i=2; i < number; i++){
            if(number % i == 0)
                return false;
        }
        return true; //will return true otherwise
    }
    
    template <typename KeyType,
              typename DataType,
              typename KeyHash = std::hash<KeyType>,
              typename KeyEqual = std::equal_to<KeyType> >
    class HashTbl{
        public:
            using Entry = HashEntry < KeyType , DataType >; //!< Alias
        //=== Private data.
        private:
            unsigned int m_size ; //!< Hash table size.
            unsigned int m_count ; //!< Number of elements currently stored in the table.
            std::forward_list < Entry > *m_data_table; //!< The table : array of pointers to collision list.
            static const short DEFAULT_SIZE = 11; //!< Hash table’s default size: 11 table entries.
            /*!
             * Changes Hash table size if load factor λ > 1.0
             */
            void rehash(){
                    KeyHash hashFunc; // Instantiate the "functor" for primary hash.

                    unsigned int m_aux_size;
                    for(auto i(m_size*2); i > 0; i++){
                        if(isPrime(i)){
                            m_aux_size = i;
                            break;
                        }
                    }
                    std::forward_list < Entry > *m_data_table_aux = new std::forward_list< Entry >[m_aux_size];
                    
                    for(auto i(0u); i < m_size; i++){
                        auto it(m_data_table[i].begin());
                        while(it != m_data_table[i].end()){
                            auto end(hashFunc(it->m_key) % m_aux_size); // Apply double hashing method, one functor and the other with modulo function.
                            Entry new_entry(it->m_key, it->m_data);
                            m_data_table_aux[end].push_front(new_entry);
                            it++;
                        }
                    }
                    
                    m_size = m_aux_size;
                    delete [] m_data_table;
                    m_data_table = m_data_table_aux;
            }
        //=== Puclic interface.
        public:
            /// Default constructor that creates an empty hash table.
            HashTbl(size_t tbl_size_ = DEFAULT_SIZE){
                m_size = tbl_size_;
                m_count = 0;
                for(int i = m_size; i > 0; i++){
                    if(isPrime(i)){
                        m_size = i;
                        break;
                    }
                }
                m_data_table = new std::forward_list< Entry >[m_size];
            }

            /// Destructs the hash table.
            virtual ~HashTbl(){
                delete [] m_data_table;
            }

            /// Copy constructor. Constructs the hash table with the deep copy of the contents of other.
            HashTbl(const HashTbl &other){
                m_data_table = new std::forward_list< Entry >[other.m_size];
                m_size = other.m_size;
                for(auto i(0u); i < m_size; i++){
                    if(other.m_data_table[i].empty() == false){
                        for(auto it(other.m_data_table[i].begin()); it != other.m_data_table[i].end(); it++){
                            Entry new_entry(it->m_key, it->m_data);
                            m_data_table[i].push_front(new_entry);
                        }
                    }
                }
                m_count = other.m_count;
            }

            /// Constructs the hash table with the contents of the initializer list ilist.
            HashTbl(std::initializer_list < Entry > ilist){
                m_data_table = new std::forward_list< Entry >[ilist.size()];
                KeyHash hashFunc;
                m_size = ilist.size();

                for(auto i(0u); i < ilist.size(); i++){
                    auto end(hashFunc((ilist.begin()+i)->m_key) % m_size);
                    m_data_table[end].push_front(*(ilist.begin()+i));
                }
                m_count = ilist.size();
            }

            /// Copy assignment operator. Replaces the contents with a copy of the contents of other.
            HashTbl & operator=(const HashTbl &other){
                // delete [] m_data_table;
                // m_data_table = new std::forward_list<Entry>[other.m_size];
                clear();
                m_size = other.m_size;

                for(auto i(0u); i < m_size; i++){
                    if(other.m_data_table[i].empty() == false){
                        for(auto it(other.m_data_table[i].begin()); it != other.m_data_table[i].end(); it++){
                            Entry new_entry(it->m_key, it->m_data);
                            m_data_table[i].push_front(new_entry);
                        }
                    }
                }
                m_count = other.m_count;
                return *this;
            }

            /// Replaces the contents with those identified by initializer list ilist.
            HashTbl & operator=(std::initializer_list < Entry > ilist){
                clear();
                KeyHash hashFunc;
                m_size = DEFAULT_SIZE;

                for(auto i(0u); i < ilist.size(); i++){
                    auto end(hashFunc((ilist.begin()+i)->m_key) % m_size);
                    m_data_table[end].push_front(*(ilist.begin()+i));
                }
                m_count = ilist.size();
                return *this;
            }

            /*!
             * Inserts in the table the information contained in d_ and associated with a key k_
             @return True if the insertion was a success, anf False if the key already exists in te table.
             */
            bool insert(const KeyType & k_, const DataType & d_){
                KeyHash hashFunc; // Instantiate the "functor" for primary hash.
                KeyEqual equalFunc; // Instantiate the "functor" for the equal to test.
                Entry new_entry(k_, d_);
                auto end(hashFunc(k_) % m_size); // Apply double hashing method, one functor and the other with modulo function.
                for(auto it(m_data_table[end].begin()); it != m_data_table[end].end(); it++){
                    if(true == equalFunc(it->m_key, new_entry.m_key)){
                        it->m_data = d_;
                        m_count += 1;
                        return false;
                    }
                }
                m_count += 1;
                if((size()/m_size) > 1){
                    rehash();
                }
                m_data_table[end].push_front(new_entry);
                return true;
            }

            /*!
             * Removes a table item identified by its key k_.
             @return True if the key was found, or False otherwise.
             */
            bool erase(const KeyType & k_){
                KeyHash hashFunc;
                KeyEqual equalFunc;
                auto end(hashFunc(k_) % m_size);
                if(m_data_table[end].empty()) return false;
                auto aux(m_data_table[end].before_begin());
                for(auto it(m_data_table[end].before_begin()); it != m_data_table[end].end(); it++){
                    if(true == equalFunc(it->m_key, k_)){
                        m_count -= 1;
                        m_data_table[end].erase_after(aux);
                        return true;
                    }
                }
                return false;
            }

            /*!
             * Retrieves in d_ the information associated with the key k_ passed
             * as argument to the method.
             @return True if the key was found, or False otherwise.
             */
            bool retrieve(const KeyType & k_, DataType & d_) const{
                KeyHash hashFunc;
                KeyEqual equalFunc;
                auto end(hashFunc(k_) % m_size);
                for(auto it(m_data_table[end].begin()); it != m_data_table[end].end(); it++){
                    if(true == equalFunc(it->m_key, k_)){
                        d_ = it->m_data;
                        return true;
                    }
                }
                return false;
            }

            /*!
             * Clears (erases) all memory associated with collision lists from the table,
             * removing all its elements.
             */
            void clear(void){
                for(auto i(0u); i < m_size; i++){
                    m_data_table[i].clear();
                }
                m_count = 0;
                m_size = 0;
            }

            /*!
             @return True if the hash table is empty, or False otherwise 
             */
            bool empty (void) const{
                return m_count == 0;
            }

            /*!
             @return The amount of element currently stored in the table.
             */
            size_t size(void) const{
                size_t size = 0;
                for(auto i(0u); i < m_size; i++){
                    for(auto it(m_data_table[i].begin()); it != m_data_table[i].end(); it++){
                        size++;
                    }
                }
                return size;
            }

            /*!
             * If the key is not in the table, the method performs the insert and returns the 
             * reference to the inserted data in the table.
             @return Reference to the data associated with the key k_ provided, if it exists.
             */ 
            DataType & operator[](const KeyType & k_){
                KeyHash hashFunc;
                KeyEqual equalFunc;
                auto end(hashFunc(k_) % m_size);
                for(auto it(m_data_table[end].begin()); it != m_data_table[end].end(); it++){
                    if(true == equalFunc(it->m_key, k_)){
                        return it->m_data;
                    }
                }
                DataType x = DataType();
                Entry new_entry(k_, x);
                m_data_table[end].push_front(new_entry);
                return m_data_table[end].front().m_data;
            }

            /*!
             @return A reference to the data associated with the key k_ provided.
             */
            DataType& at(const KeyType& k_){
                KeyHash hashFunc;
                KeyEqual equalFunc;
                auto end(hashFunc(k_) % m_size);
                for(auto it(m_data_table[end].begin()); it != m_data_table[end].end(); it++){
                    if(true == equalFunc(it->m_key, k_)){
                        return it->m_data;
                    }
                }
                 throw std::out_of_range("[HashTbl::at()] Invalid Action!");
            }

            /*!
             @return The amount of table elements that are in the collision list associated with key k_.
             */
            size_t count(const KeyType & k_) const{
                KeyHash hashFunc;
                auto end(hashFunc(k_) % m_size);
                size_t count = 0;
                for(auto it(m_data_table[end].begin()); it != m_data_table[end].end(); it++){
                    count++;
                }
                return count;
            }

            friend std::ostream & operator << (std::ostream & os, const HashTbl <KeyType, DataType, KeyHash, KeyEqual> & hashtbl){
                for(auto i(0u); i < hashtbl.m_size; i++){
                    os << "[" << i << "]";
                    for(auto it = hashtbl.m_data_table[i].begin(); it != hashtbl.m_data_table[i].end(); it++){
                        os << " -> " << it->m_data;
                    }
                    os << std::endl;
                }
                return os;
            }
   };

}

#endif