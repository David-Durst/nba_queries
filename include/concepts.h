#ifndef NBA_QUERIES_CONCEPTS_H
#define NBA_QUERIES_CONCEPTS_H
#include "col_stores.h"
#include "clean_queries.h"
#include <vector>
#define MAX_THREADS 1000
using std::vector;

class Concept {
public:
    vector<int64_t> start_moment_index_unmerged[MAX_THREADS];
    vector<int64_t> start_moment_index;
    int64_t ticks_in_window;

    virtual void compute(const moment_col_store& moments, const shot_col_store& shots) = 0;
    void sample(vector<cleaned_moment> samples, const moment_col_store& moments, int64_t num_samples, bool sample_unmerged);
};

class Possession : Concept {
public:
    vector<int64_t> possessor_ids_unmerged[MAX_THREADS];
    vector<int64_t> possessor_team_unmerged[MAX_THREADS];
    vector<int64_t> possessor_ids;
    vector<int64_t> possessor_team;
    void compute(const moment_col_store &moments, const shot_col_store &shots);
};


#endif //NBA_QUERIES_CONCEPTS_H