#ifndef NBA_QUERIES_QUERY_STRUCTS_H
#define NBA_QUERIES_QUERY_STRUCTS_H

class car_col_store {
public:
    int num_ticks;
    int64_t ** t;
    double ** x;
    double ** y;
    double ** z;

    // player_id doesn't matter if ball is set
    player_col_store(vector<cleaned_moment>& moments, int player_id);
};

#endif //NBA_QUERIES_QUERY_STRUCTS_H
