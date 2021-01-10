#include "query_structs.h"
#include "col_stores.h"

player_col_store::player_col_store(vector<cleaned_moment> &moments, int player_id) {
    team_id = new long int[moments.size()];
    this->player_id = new int[moments.size()];
    x_loc = new double[moments.size()];
    y_loc = new double[moments.size()];
    radius = new double[moments.size()];

    vector<std::reference_wrapper<player_data>> data;
    for (int i = 0; i < moments.size(); i++) {
        cleaned_moment& m = moments.at(i);
        get_all_player_data(data, m);
        team_id[i] = data.at(player_id).get().team_id;
        this->player_id[i] = data.at(player_id).get().player_id;
        x_loc[i] = data.at(player_id).get().x_loc;
        y_loc[i] = data.at(player_id).get().y_loc;
        radius[i] = data.at(player_id).get().radius;
    }
}

moment_col_store::moment_col_store(vector<cleaned_moment> &moments) {
    size = moments.size();
    for (int i = 0; i < 11; i++) {
        team_id[i] = new long int[moments.size()];
        player_id[i] = new int[moments.size()];
        x_loc[i] = new double[moments.size()];
        y_loc[i] = new double[moments.size()];
        radius[i] = new double[moments.size()];
    }
    game_clock = new clock_fixed_point[moments.size()];
    shot_clock = new double[moments.size()];
    quarter = new short int[moments.size()];
    game_id = new long int[moments.size()];
    events = new vector<event_moment_data>[moments.size()];
    vector<std::reference_wrapper<player_data>> data;
    for (int i = 0; i < moments.size(); i++) {
        cleaned_moment& m = moments.at(i);
        get_all_player_data(data, m);
        for (int j = 0; j < 11; j++) {
            team_id[j][i] = data.at(j).get().team_id;
            player_id[j][i] = data.at(j).get().player_id;
            x_loc[j][i] = data.at(j).get().x_loc;
            y_loc[j][i] = data.at(j).get().y_loc;
            radius[j][i] = data.at(j).get().radius;
        }
        game_clock[i] = m.game_clock;
        shot_clock[i] = m.shot_clock;
        quarter[i] = m.quarter;
        game_id[i] = m.game_id;
        events[i] = m.events;
    }
}
