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

function getData() {
    const url = document.querySelector("#data-url").value;
    console.log(url);
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