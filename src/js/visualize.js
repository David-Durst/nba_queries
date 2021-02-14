const top_left_x = 122;
const top_left_y = 61;
const background = new Image();
background.src = "court_background.jpg";
let canvas = null;
let ctx = null;

function init() {
    canvas = document.querySelector("#myCanvas");
    ctx = canvas.getContext('2d');
    ctx.drawImage(background,0,0);
}