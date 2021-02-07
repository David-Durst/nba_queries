#ifndef NBA_QUERIES_STORAGE_STRUCTS_H
#define NBA_QUERIES_STORAGE_STRUCTS_H
#include <vector>
using std::vector;
using std::reference_wrapper;

template<typename T>
struct noncontiguous_vector {
    int64_t start_size = 10000;
    vector<vector<T *>> sub_vectors;
    vector<vector<int64_t>> sub_vector_sizes;
    int64_t * indices;
    int num_threads;

    noncontiguous_vector(int num_threads) {
        this->num_threads = num_threads;
        sub_vectors.resize(num_threads);
        sub_vector_sizes.resize(num_threads);
        for (int i = 0; i < num_threads; i++) {
            sub_vectors[i].push_back(new T[start_size]);
            sub_vector_sizes[i].push_back(start_size);
        }
        indices = new int64_t[num_threads];
    }

    ~noncontiguous_vector() {
        for (int i = 0; i < num_threads; i++) {
            for (auto & t_ref : sub_vectors[i]) {
                delete [] t_ref;
            }
        }
    }
};

template<typename T>
static inline __attribute__((always_inline)) T* ensure_enough_space(noncontiguous_vector<T>& ncv, int64_t min_size, int thread_num) {
    int64_t old_size = ncv.sub_vector_sizes[thread_num][ncv.sub_vectors.size() - 1];
    if (old_size - ncv.indices[thread_num] < min_size) {
        int64_t new_size = std::min(min_size + 1, 2 * old_size);
        ncv.sub_vectors[thread_num].push_back(new T[new_size]);
        ncv.sub_vector_sizes[thread_num].push_back(new_size);
    }
    return ncv.sub_vectors[thread_num][ncv.sub_vectors[thread_num].size() - 1];
}

#endif //NBA_QUERIES_STORAGE_STRUCTS_H
