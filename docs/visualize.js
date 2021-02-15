const top_left_x = 122;
const top_left_y = 61;
const background = new Image();
background.src = "court_background.jpg";
let canvas = null;
let ctx = null;
let data = null;
const dark_blue = "#04bec4"
const light_blue = "#c2fff3"
const dark_red = "#d10000"
const light_red = "#ff8f8f"

function init() {
    canvas = document.querySelector("#myCanvas");
    ctx = canvas.getContext('2d');
    ctx.drawImage(background,0,0);
}

function gameClockAsText(game_clock_total_seconds) {
    const mins = game_clock_total_seconds / 60;
    const seconds = game_clock_total_seconds % 60;
    return mins.toFixed(0) + ":" + seconds.toFixed(2)
}

function drawTimeStep(sample, t, draw_entire_series) {
    const t_data = sample.points[t];
    const in_window = t >= sample.window_start && t < sample.window_start + sample.window_length;
    let player_text = "x";
    ctx.fillStyle = "black";
    ctx.fillText("b", top_left_x + t_data.ball.x_loc * 10,
        top_left_y + t_data.ball.y_loc * 10);
    for (let j = 0; j < 10; j++) {
        if (t_data.players[j].team_id === sample.team0) {
            if (in_window) {
                ctx.fillStyle = dark_red;
            }
            else {
                ctx.fillStyle = light_red;
            }
            player_text = "x";
        } else {
            if (in_window) {
                ctx.fillStyle = dark_blue;
            }
            else {
                ctx.fillStyle = light_blue;
            }
            player_text = "o";
        }
        if (!draw_entire_series) {
            player_text = j;
        }
        ctx.fillText(player_text, top_left_x + t_data.players[j].x_loc * 10,
            top_left_y + t_data.players[j].y_loc * 10);
    }
}

function redrawCanvas(draw_entire_series) {
    const sample = data[document.querySelector("#sample-selector").value];
    document.querySelector("#cur-sample").innerHTML = document.querySelector("#sample-selector").value;
    ctx.drawImage(background,0,0);
    document.querySelector("#gameid").innerHTML = sample.points[0].game_id;
    document.querySelector("#quarter").innerHTML = sample.points[0].quarter;
    document.querySelector("#start-time").innerHTML = gameClockAsText(sample.points[sample.window_start].game_clock);
    document.querySelector("#end-time").innerHTML = gameClockAsText(sample.points[sample.window_start + sample.window_length - 1].game_clock);
    document.querySelector("#start-shot-clock").innerHTML = sample.points[sample.window_start].shot_clock;
    document.querySelector("#end-shot-clock").innerHTML = sample.points[sample.window_start + sample.window_length - 1].shot_clock;
    document.querySelector("#red-team").innerHTML = sample.team0;
    document.querySelector("#blue-team").innerHTML = sample.team1;
    document.querySelector("#concept-specific").innerHTML = sample.window_html;
    ctx.font = "30px Arial"
    if (draw_entire_series) {
        for (let i = 0; i < sample.sample_length; i++) {
            drawTimeStep(sample, i, draw_entire_series);
        }
        document.querySelector("#time-selector").max = sample.sample_length - 1;
        document.querySelector("#cur-time-step").innerHTML = "all"
    }
    else {
        const t = document.querySelector("#time-selector").value;
        if (t < sample.window_start) {
            document.querySelector("#cur-time-step").innerHTML = "window - " + (sample.window_start - t);
        }
        else if (t >= sample.window_start + sample.window_length) {
            document.querySelector("#cur-time-step").innerHTML = "window + " + (t - sample.window_start - sample.window_length + 1);
        }
        else {
            document.querySelector("#cur-time-step").innerHTML = "" + (t - sample.window_start);
        }
        drawTimeStep(sample, t, draw_entire_series);
    }
}

function getData() {
    const url = document.querySelector("#data-url").value;
    fetch(url)
        .then(response =>
            response.text().then(text =>
                csvJSON(text)))
}

function makePlayer(row, index) {
   return {
       team_id: parseInt(row[index]),
       player_id: parseInt(row[index+1]),
       x_loc: parseFloat(row[index+2]),
       y_loc: parseFloat(row[index+3]),
       radius: parseFloat(row[index+4])
   };
}

function csvJSON(csv){
    document.querySelector("#sample-selector").value = 0;
    document.querySelector("#cur-sample").innerHTML = "" + 0;
    const lines=csv.split("\n");
    data = [];
    let cur_sample_size = 0;
    for(let s = 1; s < lines.length; s += cur_sample_size){
        // skip empty lines
        if (lines[s].trim() === "") {
            continue;
        }
        let sample = {team0: 0, team1: 0, sample_length: 0, window_length: 0,
            window_start: 0, window_html: 0, points: []};
        let current_line = lines[s].split(",");
        // parse start of window
        sample.sample_length = parseInt(current_line[1]);
        cur_sample_size = sample.sample_length + 1;
        sample.window_length = parseInt(current_line[2]);
        sample.window_start = parseInt(current_line[3]);
        sample.window_html = current_line[4];
        for (let i = s+1; i < lines.length && i < s+1 + sample.sample_length; i++) {
            let obj = {};
            current_line = lines[i].split(",");
            obj.ball = makePlayer(current_line, 0);
            obj.players = [];
            for(let j = 0; j < 10; j++){
                obj.players[j] = makePlayer(current_line, (j+1)*5);
            }
            obj.game_clock = parseFloat(current_line[55]);
            obj.shot_clock = parseFloat(current_line[56]);
            obj.quarter = parseInt(current_line[57]);
            obj.game_id = parseInt(current_line[58]);
            obj.game_num = parseInt(current_line[59]);
            sample.points.push(obj);
        }
        // figure out two teams ids
        sample.team0 = sample.points[0].players[0].team_id;
        for (let i = 1; i < 10; i++) {
            if (sample.points[0].players[i].team_id !== sample.team0) {
                sample.team1 = sample.points[0].players[i].team_id;
                break;
            }
        }
        data.push(sample);
    }
    document.querySelector("#sample-selector").max = data.length - 1;
}