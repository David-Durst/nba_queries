#ifndef NBA_QUERIES_CONCEPTS_H
#define NBA_QUERIES_CONCEPTS_H
#include "col_stores.h"
#include "clean_queries.h"
#include <vector>
#include <string>
#define MAX_THREADS 1000
#define START_SAMPLE_WINDOW_TOKEN "SW_START_TOKEN"
using std::vector;
using std::string;

struct SampleWindow {
    // a sample contains data before and after the window for visualization
    int64_t sample_length;
    int64_t window_length;
    int64_t window_start;
    string window_html;
    vector<cleaned_moment> moments_in_sample;
};

class Concept {
public:
    // unmerged may or may not be used by concepts depending on if order matters
    vector<int64_t> start_moment_index_unmerged[MAX_THREADS];
    int64_t * start_moment_index;
    int64_t num_windows;
    int64_t ticks_in_window = 25;
    int64_t buffer_ticks_for_sample = 25;

    virtual void compute(const moment_col_store& moments, const shot_col_store& shots) = 0;
    void sample(const moment_col_store& moments, int64_t num_samples, bool sample_unmerged, string sample_file_path);
};

class Possession : public Concept {
public:
    vector<int64_t> possessor_ids_unmerged[MAX_THREADS];
    vector<int64_t> possessor_team_unmerged[MAX_THREADS];
    int64_t * possessor_ids;
    int64_t * possessor_team;
    void compute(const moment_col_store &moments, const shot_col_store &shots);
};

class Stoppage : public Concept {
public:
    bool * is_window_stoppage;
    double min_movement_per_tick = 0;
    double max_movement_per_tick = 1.0;
    void compute(const moment_col_store &moments, const shot_col_store &shots);
};


#endif //NBA_QUERIES_CONCEPTS_H
