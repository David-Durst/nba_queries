#ifndef NBA_QUERIES_STORAGE_STRUCTS_H
#define NBA_QUERIES_STORAGE_STRUCTS_H
#include <vector>
using std::vector;
using std::reference_wrapper;

template<typename T>
struct noncontiguous_vector {
    int64_t start_size = 10000;
    vector<reference_wrapper<T>> sub_vectors;
    vector<int64_t> sub_vector_sizes;
    int64_t index = 0;

    noncontiguous_vector() {
        sub_vectors.push_back(new T[start_size]);
        sub_vector_sizes.push_back(start_size);
    }

    ~noncontiguous_vector() {
        for (auto & t_ref : sub_vectors) {
            delete [] t_ref;
        }
    }
};

template<typename T>
static inline __attribute__((always_inline)) T* ensure_enough_space(const noncontiguous_vector<T>& ncv, int64_t min_size) {
    int64_t old_size = ncv.sub_vectors_sizes[ncv.sub_vectors.size() - 1];
    if (old_size - ncv.index < min_size) {
        int64_t new_size = std::min(min_size + 1, 2 * old_size);
        ncv.sub_vectors.push_back(new T[new_size]);
        ncv.sub_vector_sizes.push_back(new_size);
    }
    return ncv.sub_vectors.at(ncv.size() - 1);
}

#endif //NBA_QUERIES_STORAGE_STRUCTS_H
