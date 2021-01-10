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

shot_col_store::shot_col_store(vector<shot> &shots) {
    size = shots.size();
    action_type = new string[shots.size()];
    event_time = new int[shots.size()];
    event_type = new string[shots.size()];
    game_date = new string[shots.size()];
    game_event_id = new long int[shots.size()];
    game_id = new long int[shots.size()];
    grid_type = new string[shots.size()];
    htm = new string[shots.size()];
    loc_x = new float[shots.size()];
    loc_y = new float[shots.size()];
    minutes_remaining = new int[shots.size()];
    period = new int[shots.size()];
    player_id = new int[shots.size()];
    player_name = new string[shots.size()];
    quarter = new float[shots.size()];
    seconds_remaining = new int[shots.size()];
    shot_attempted_flag = new bool[shots.size()];
    shot_distance = new int[shots.size()];
    shot_made_flag = new bool[shots.size()];
    shot_time = new float[shots.size()];
    shot_type = new string[shots.size()];
    shot_zone_area = new string[shots.size()];
    shot_zone_basic = new string[shots.size()];
    shot_zone_range = new string[shots.size()];
    team_id = new float[shots.size()];
    team_name = new string[shots.size()];
    team_vtm = new string[shots.size()];

    for (int i = 0; i < shots.size(); i++) {
        shot& s = shots.at(i);
        action_type[i] = s.action_type;
        event_time[i] = s.event_time;
        event_type[i] = s.event_type;
        game_date[i] = s.game_date;
        game_event_id[i] = s.game_event_id;
        game_id[i] = s.game_id;
        grid_type[i] = s.grid_type;
        htm[i] = s.htm;
        loc_x[i] = s.loc_x;
        loc_y[i] = s.loc_y;
        minutes_remaining[i] = s.minutes_remaining;
        period[i] = s.period;
        player_id[i] = s.player_id;
        player_name[i] = s.player_name;
        quarter[i] = s.quarter;
        seconds_remaining[i] = s.seconds_remaining;
        shot_attempted_flag[i] = s.shot_attempted_flag;
        shot_distance[i] = s.shot_distance;
        shot_made_flag[i] = s.shot_made_flag;
        shot_time[i] = s.shot_time;
        shot_type[i] = s.shot_type;
        shot_zone_area[i] = s.shot_zone_area;
        shot_zone_basic[i] = s.shot_zone_basic;
        shot_zone_range[i] = s.shot_zone_range;
        team_id[i] = s.team_id;
        team_name[i] = s.team_name;
        team_vtm[i] = s.team_vtm;
    }

}