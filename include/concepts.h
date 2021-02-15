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
using std::to_string;

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
    virtual string get_concept_html_unmerged(const moment_col_store& moments, int64_t thread_num, int64_t per_thread_index) = 0;
    virtual string get_concept_html(const moment_col_store& moments, int64_t cur_time) = 0;
    void sample(const moment_col_store& moments, int64_t num_samples, bool sample_unmerged, string sample_file_path,
                const std::function<bool(const moment_col_store&, int64_t)> &filter);
    string label_as_html(string label_name, string label_value) {
        return "<span class=\"label\">" + label_name + " </span> " + label_value;
    }
};

class Possession : public Concept {
public:
    vector<int> possessor_indices_unmerged[MAX_THREADS];
    vector<int64_t> possessor_ids_unmerged[MAX_THREADS];
    vector<int64_t> possessor_team_unmerged[MAX_THREADS];
    int * possessor_indices;
    int64_t * possessor_ids;
    int64_t * possessor_team;
    double max_distance = 5.0;
    string get_concept_html_unmerged(const moment_col_store& moments, int64_t thread_num, int64_t per_thread_index) {
        return label_as_html("possessor_index", to_string(possessor_indices_unmerged[thread_num][per_thread_index])) + ";" +
            label_as_html("possessor_id", to_string(possessor_ids_unmerged[thread_num][per_thread_index])) + ";" +
            label_as_html("possessor_team", to_string(possessor_team_unmerged[thread_num][per_thread_index]));
    }
    string get_concept_html(const moment_col_store& moments, int64_t cur_window) {
        return label_as_html("possessor_index", to_string(possessor_ids[cur_window])) + ";" +
            label_as_html("possessor_id", to_string(possessor_ids[cur_window])) + ";" +
            label_as_html("possessor_team", to_string(possessor_team[cur_window]));
    }
    void compute(const moment_col_store &moments, const shot_col_store &shots);
    bool allow_all(const moment_col_store& moments, int64_t cur_time) const {
        return true;
    }
};

class Stoppage : public Concept {
public:
    bool * is_window_stoppage;
    double min_movement_per_tick = 0;
    double max_movement_per_tick = 1.0;
    void compute(const moment_col_store &moments, const shot_col_store &shots);
    string get_concept_html_unmerged(const moment_col_store& moments, int64_t thread_num, int64_t per_thread_index) {
        return "get_concept_html_unmerged not defined for stoppage";
    }
    string get_concept_html(const moment_col_store& moments, int64_t cur_window) {
        if (is_window_stoppage[cur_window]) {
            return label_as_html("is_window_stoppage", "true");
        }
        else {
            return label_as_html("is_window_stoppage", "false");
        }
    }
    bool get_stoppages(const moment_col_store& moments, int64_t cur_time) const {
        return is_window_stoppage[cur_time];
    }

    bool get_non_stoppages(const moment_col_store& moments, int64_t cur_time) const {
        return is_window_stoppage[cur_time];
    }
};


#endif //NBA_QUERIES_CONCEPTS_H
