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
#pragma omp parallel for
    for (int i = 0; i < 11; i++) {
        team_id[i] = (long int *) malloc(moments.size() * sizeof(long int));
        player_id[i] = (int *) malloc(moments.size() * sizeof(int));
        x_loc[i] = (double *) malloc(moments.size() * sizeof(double));
        y_loc[i] = (double *) malloc(moments.size() * sizeof(double));
        radius[i] = (double *) malloc(moments.size() * sizeof(double));
    }
    game_clock = (clock_fixed_point *) malloc(moments.size() * sizeof(clock_fixed_point));
    shot_clock = (double *) malloc(moments.size() * sizeof(double));
    quarter = (short int *) malloc(moments.size() * sizeof(short int));
    game_id = (long int *) malloc(moments.size() * sizeof(long int));
    game_num = (int *) malloc (moments.size() * sizeof(int));
    events = (vector<event_moment_data> *) malloc(moments.size() * sizeof(vector<event_moment_data>));
#pragma omp parallel for
    for (int64_t i = 0; i < moments.size(); i++) {
        cleaned_moment& m = moments[i];

        team_id[0][i] = m.ball.team_id;
        player_id[0][i] = m.ball.player_id;
        x_loc[0][i] = m.ball.x_loc;
        y_loc[0][i] = m.ball.y_loc;
        radius[0][i] = m.ball.radius;

        for (int j = 1; j < NUM_PLAYERS_AND_BALL; j++) {
            team_id[j][i] = m.players[j-1].team_id;
            player_id[j][i] = m.players[j-1].player_id;
            x_loc[j][i] = m.players[j-1].x_loc;
            y_loc[j][i] = m.players[j-1].y_loc;
            radius[j][i] = m.players[j-1].radius;
        }
        game_clock[i] = m.game_clock;
        shot_clock[i] = m.shot_clock;
        quarter[i] = m.quarter;
        game_id[i] = m.game_id;
        game_num[i] = m.game_num;
        events[i] = m.events;
    }
}

shot_col_store::shot_col_store(vector<cleaned_shot> &shots) {
    size = shots.size();
    action_type = new string[shots.size()];
    event_time = new int[shots.size()];
    event_type = new string[shots.size()];
    game_date = new string[shots.size()];
    game_event_id = new long int[shots.size()];
    game_id = new long int[shots.size()];
    game_num = new int[shots.size()];
    grid_type = new string[shots.size()];
    htm = new string[shots.size()];
    loc_x = new float[shots.size()];
    loc_y = new float[shots.size()];
    minutes_remaining = new int[shots.size()];
    period = new int[shots.size()];
    player_id = new int[shots.size()];
    player_name = new string[shots.size()];
    quarter = new int[shots.size()];
    seconds_remaining = new int[shots.size()];
    shot_attempted_flag = new bool[shots.size()];
    shot_distance = new int[shots.size()];
    shot_made_flag = new bool[shots.size()];
    shot_time = new clock_fixed_point[shots.size()];
    shot_type = new string[shots.size()];
    shot_zone_area = new string[shots.size()];
    shot_zone_basic = new string[shots.size()];
    shot_zone_range = new string[shots.size()];
    team_id = new long int[shots.size()];
    team_name = new string[shots.size()];
    team_vtm = new string[shots.size()];

    for (int i = 0; i < shots.size(); i++) {
        cleaned_shot& s = shots.at(i);
        action_type[i] = s.action_type;
        event_time[i] = s.event_time;
        event_type[i] = s.event_type;
        game_date[i] = s.game_date;
        game_event_id[i] = s.game_event_id;
        game_id[i] = s.game_id;
        game_num[i] = s.game_num;
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
        shot_time[i] = clock_fixed_point(s.shot_time);
        shot_type[i] = s.shot_type;
        shot_zone_area[i] = s.shot_zone_area;
        shot_zone_basic[i] = s.shot_zone_basic;
        shot_zone_range[i] = s.shot_zone_range;
        team_id[i] = std::lround(s.team_id);
        team_name[i] = s.team_name;
        team_vtm[i] = s.team_vtm;
    }

}