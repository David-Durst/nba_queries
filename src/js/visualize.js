const top_left_x = 122;
const top_left_y = 61;
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

function csvJSON(csv){
    const lines=csv.split("\n");
    data = [];
    const headers=lines[0].split(",");
    for(let i=1; i < lines.length; i++){
        let obj = {};
        let currentline = lines[i].split(",");
        for(let j=0; j < headers.length; j++){
            obj[headers[j]] = currentline[j];
        }
        data.push(obj);
    }
    console.log("data length: " + data.length);
    console.log("lines length: " + lines.length);
}