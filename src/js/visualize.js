const top_left_x = 122;
const top_left_y = 61;
const window_size = 25;
const background = new Image();
background.src = "court_background.jpg";
let canvas = null;
let ctx = null;
let data = null;

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

function drawTimeStep(win, t_data, draw_entire_series) {
    let player_text = "x";
    ctx.fillStyle = "black";
    ctx.fillText("b", top_left_x + t_data.ball.x_loc * 10,
        top_left_y + t_data.ball.y_loc * 10);
    for (let j = 0; j < 10; j++) {
        if (t_data.players[j].team_id == win.team0) {
            ctx.fillStyle = "red";
            player_text = "x";
        } else {
            ctx.fillStyle = "blue";
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
    const win = data[document.querySelector("#win-selector").value];
    ctx.drawImage(background,0,0);
    document.querySelector("#gameid").innerHTML = win.points[0].game_id;
    document.querySelector("#quarter").innerHTML = win.points[0].quarter;
    document.querySelector("#start-time").innerHTML = gameClockAsText(win.points[0].game_clock);
    document.querySelector("#end-time").innerHTML = gameClockAsText(win.points[window_size - 1].game_clock);
    document.querySelector("#start-shot-clock").innerHTML = win.points[0].shot_clock;
    document.querySelector("#end-shot-clock").innerHTML = win.points[window_size - 1].shot_clock;
    document.querySelector("#red-team").innerHTML = win.team0;
    document.querySelector("#blue-team").innerHTML = win.team1;
    ctx.font = "30px Arial"
    if (draw_entire_series) {
        for (let i = 0; i < window_size; i++) {
            drawTimeStep(win, win.points[i], draw_entire_series);
        }
        document.querySelector("#cur-time-step").innerHTML = "all"
    }
    else {
        const t = document.querySelector("#time-selector").value;
        document.querySelector("#cur-time-step").innerHTML = t;
        drawTimeStep(win, win.points[t], draw_entire_series);
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
    document.querySelector("#win-selector").value = 0;
    const lines=csv.split("\n");
    data = [];
    for(let w = 1; w < lines.length; w += window_size){
        let win = {team0: 0, team1: 0, points: []};
        for (let i = w; i < lines.length && i < w + window_size; i++) {
            let obj = {};
            let current_line = lines[i].split(",");
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
            win.points.push(obj);
        }
        // figure out two teams ids
        win.team0 = win.points[0].players[0].team_id;
        for (let i = 1; i < 10; i++) {
            if (win.points[0].players[i].team_id != win.team0) {
                win.team1 = win.points[0].players[i].team_id;
                break;
            }
        }
        data.push(win);
    }
}