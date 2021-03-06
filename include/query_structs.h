#ifndef QUERY_STRUCTS_H
#define QUERY_STRUCTS_H
#include <string>
#include <vector>
#include <cmath>
#include <functional>

using std::string;
using std::vector;

struct moment {
    long int team_id;
    int player_id;
    double x_loc;
    double y_loc;
    double radius;
    double game_clock;
    double shot_clock;
    short int quarter;
    long int game_id;
    long int event_id;
    int moment_in_event;
    int64_t internal_id;
} ;

bool operator==(moment const & lhs, moment const & rhs);
std::ostream& operator<<(std::ostream& os, moment const& value);
void print_moment_csv(std::ostream& os, const moment& value);

struct player_data {
    long int team_id;
    int player_id;
    double x_loc;
    double y_loc;
    double radius;
};

bool operator==(player_data const & lhs, player_data const & rhs);
std::ostream& operator<<(std::ostream& os, player_data const& value);
void print_player_data_csv(std::ostream& os, const player_data& value);

struct event_moment_data {
    long int event_id;
    int moment_in_event;
};

bool operator==(event_moment_data const & lhs, event_moment_data const & rhs);
std::ostream& operator<<(std::ostream& os, event_moment_data const& value);
void print_event_moment_data_csv(std::ostream& os, const event_moment_data& value);

struct extra_game_data {
    long int game_id;
    int game_num;
    int num_ot_periods;
};

bool operator==(extra_game_data const & lhs, extra_game_data const & rhs);
std::ostream& operator<<(std::ostream& os, extra_game_data const& value);
void print_extra_game_data_csv(std::ostream& os, const extra_game_data& value);

class clock_fixed_point {
public:
    long int seconds;
    int twenty_fifths_of_second;

    clock_fixed_point () { }

    clock_fixed_point (double f) {
        seconds = std::floor(f);
        twenty_fifths_of_second = std::round((f - seconds) * 25);
        if (twenty_fifths_of_second == 25) {
            seconds++;
            twenty_fifths_of_second = 0;
        }
    }

    inline double to_double() const {
        return seconds + (twenty_fifths_of_second / 25.0);
    }

    clock_fixed_point abs_diff(const clock_fixed_point& other) const {
        return clock_fixed_point(std::abs(this->to_double() - other.to_double()));
    }

    bool gt(double f) const {
        return this->to_double() > f;
    }

    inline bool gt(clock_fixed_point c) const {
        return this->seconds > c.seconds || (this->seconds == c.seconds && this->twenty_fifths_of_second > c.twenty_fifths_of_second);
    }

    inline int64_t time_to_index(vector<extra_game_data>& extra_data, int game_num, int quarter) {
        int ot_quarters = 0;
        for (int i = 0; i < game_num; i++) {
            ot_quarters += extra_data.at(i).num_ot_periods;
        }
        int non_ot_quarters_finished_this_game = std::min(4, quarter - 1);
        int ot_quarters_finished_this_game = std::max(0, quarter - 5);
        int seconds_elapsed_this_quarter = quarter >= 5 ? 300 - seconds : 720 - seconds;
        // 720 seconds in a quarter
        return
            // time for multiple games
            25 * 720 * 4 * game_num +
            // time for Ot
            25 * 300 * ot_quarters +
            // time in game
            (25 * (720 * non_ot_quarters_finished_this_game + 300 * ot_quarters_finished_this_game + seconds_elapsed_this_quarter) - twenty_fifths_of_second);
    }
};

bool operator==(clock_fixed_point const & lhs, clock_fixed_point const & rhs);
bool operator!=(clock_fixed_point const& lhs, clock_fixed_point const& rhs);

struct cleaned_moment {
    player_data ball;
    player_data players[10];
    clock_fixed_point game_clock;
    double shot_clock;
    short int quarter;
    long int game_id; // game_id assigned by nba
    int game_num; // game num stores game's number in vector of cleaned_moments
    vector<event_moment_data> events;
} ;

bool operator==(cleaned_moment const & lhs, cleaned_moment const & rhs);
std::ostream& operator<<(std::ostream& os, cleaned_moment const& value);
void print_cleaned_moment_csv(std::ostream& os, const cleaned_moment& value);
void print_cleaned_moment_csv_header(std::ostream& os);
void get_all_player_data(vector<std::reference_wrapper<player_data>>& data, cleaned_moment& c);

struct event {
    long int game_id;
    long int event_num;
    int event_msg_type;
    int event_msg_action_type;
    short int period;
    string wc_timestring;
    string pc_timestring;
    string home_description;
    string neutral_description;
    string visitor_description;
    string score;
    string score_margin;
    int person1_type;
    int player1_id;
    string player1_name;
    float player1_team_id;
    string player1_team_city;
    string player1_team_nickname;
    string player1_team_abbreviation;
    int person2_type;
    int player2_id;
    string player2_name;
    float player2_team_id;
    string player2_team_city;
    string player2_team_nickname;
    string player2_team_abbreviation;
    int person3_type;
    int player3_id;
    string player3_name;
    float player3_team_id;
    string player3_team_city;
    string player3_team_nickname;
    string player3_team_abbreviation;
} ;

struct shot {
    string action_type;
    int event_time;
    string event_type;
    string game_date;
    long int game_event_id;
    long int game_id;
    string grid_type;
    string htm;
    float loc_x;
    float loc_y;
    int minutes_remaining;
    int period;
    int player_id;
    string player_name;
    float quarter;
    int seconds_remaining;
    bool shot_attempted_flag;
    int shot_distance;
    bool shot_made_flag;
    clock_fixed_point shot_time;
    string shot_type;
    string shot_zone_area;
    string shot_zone_basic;
    string shot_zone_range;
    long int team_id;
    string team_name;
    string team_vtm;
} ;

bool operator==(shot const & lhs, shot const & rhs);
std::ostream& operator<<(std::ostream& os, shot const& value);
void print_shot_csv(std::ostream& os, const shot& value);

bool shot_before_moment(const shot & s, const moment & m);
bool moment_before_shot(const moment & m, const shot & s);

struct cleaned_shot {
    string action_type;
    int event_time;
    string event_type;
    string game_date;
    long int game_event_id;
    long int game_id;
    int game_num;
    string grid_type;
    string htm;
    float loc_x;
    float loc_y;
    int minutes_remaining;
    int period;
    int player_id;
    string player_name;
    int quarter;
    int seconds_remaining;
    bool shot_attempted_flag;
    int shot_distance;
    bool shot_made_flag;
    clock_fixed_point shot_time;
    string shot_type;
    string shot_zone_area;
    string shot_zone_basic;
    string shot_zone_range;
    long int team_id;
    string team_name;
    string team_vtm;
} ;

bool operator==(cleaned_shot const & lhs, shot const & rhs);
std::ostream& operator<<(std::ostream& os, cleaned_shot const& value);
void print_cleaned_shot_csv(std::ostream& os, const cleaned_shot& value);

template <typename T>
struct list_node {
    T data;
    list_node<T>* next;
};

template <typename T>
class list {
    list_node<T> *head, *tail;
    size_t size;
public:
    list() : head(NULL), tail(NULL), size(0) { }
    ~list() { clear(); }
    void append_node(T elem);
    T * get(size_t i);
    size_t get_size();
    void clear();
    void to_vector(vector<T> & vec);
};

template <typename T>
inline void list<T>::append_node(T elem) {
    if (head == NULL) {
        head = new list_node<T>();
        tail = head;
        head->data = elem;
    }
    else {
        list_node<T> * next_tail = new list_node<T>();
        next_tail->data = elem;
        tail->next = next_tail;
        tail = tail->next;
    }
    size++;
}

template <typename T>
T * list<T>::get(size_t i) {
    if (i >= size) {
        throw std::invalid_argument("size " + std::to_string(i) + "greater than list size " + std::to_string(size));
    }
    list_node<T> * result_node = head;
    for (int j = 0; j < i; j++) {
        result_node = result_node->next;
    }
    return result_node->data;
}

template <typename T>
size_t list<T>::get_size() {
    return size;
}

template <typename T>
void list<T>::clear() {
    list_node<T> * cur_node = head;
    tail = NULL;
    for (int i = 0; i < size; i++) {
        list_node<T> * prior_node = cur_node;
        cur_node = cur_node->next;
        free(prior_node);
    }
    head = NULL;
    size = 0;
}

template <typename T>
void list<T>::to_vector(vector<T> &vec) {
    vec.clear();
    list_node<T> * cur_node = head;
    for (int i = 0; i < size; i++) {
        vec.push_back(cur_node->data);
        cur_node = cur_node->next;
    }
}
#endif
