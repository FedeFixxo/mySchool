var c;
var ctx;
var sse = false;

function loaded() {
    dragElement(document.getElementById("overlay"));
    $("#overlay").css("left", ($(document).width()-$("#overlay").width())/2 + "px");
    c = document.getElementById("canvas");
    ctx = c.getContext("2d");
    canvas_fillScreen(0);
}

/* Click */
function click_drawLine() {
    const x0 = Math.floor($('#drawLineX0').val());
    const y0 = Math.floor($('#drawLineY0').val());
    const x1 = Math.floor($('#drawLineX1').val());
    const y1 = Math.floor($('#drawLineY1').val());
    const color = $('#drawLineColor').val();
    drawLine(x0, y0, x1, y1, color);
    canvas_drawLine(x0, y0, x1, y1, color);
}

function click_fillScreen() {
    const color = $('#fillScreenColor').val();
    fillScreen(color);
    canvas_fillScreen(color);

    const r = parseInt(color.substring(1, 3), 16);
    const g = parseInt(color.substring(3, 5), 16);
    const b = parseInt(color.substring(5, 7), 16);
    const col = (( r + g + b) / 3) <= 60 ? "#eeeeee" : "#111111";    
    $("h1").css("color", col);
    $("p").css("color", col);
    $("#bar").css("color", col);
    $("#bar").css("background", color);
}

function click_btnPot() {    
    if(sse) {
        closeSSE();
        sse = false;
        return;
    }
    listenSSE();
    sse = true;
}

async function click_drawImage() {
    const myFile = document.getElementById("fileImage").files[0];    
    const reader = new FileReader();
    reader.addEventListener('load', (e) => {
        let base_image = new Image();
        base_image.src = e.target.result;
        //console.log(e.target.result);
        base_image.onload = async function(){
            ctx.drawImage(base_image, 0, 0);
            for(let y = 0; y < 120; y++) {
                line = [];
                for(let x = 0; x < 320; x++) {
                    const cols = ctx.getImageData(x, y, 1, 1).data;
                    line.push(RGBTo565(cols[0], cols[1], cols[2]).toString(10).padStart(5, "0"));
                    if((x + 1) % 160 == 0) {
                        drawSegment(y, x - 159, 160, line);                        
                        line = [];
                        await wait(20);
                    }
                }                
            }
        }
    });
    reader.readAsDataURL(myFile);
}

/* Canvas */
function canvas_drawLine(x0, y0, x1, y1, color) {
    ctx.beginPath();
    ctx.strokeStyle = color;
    //console.log(x0, y0, x1, y1);
    ctx.moveTo(x0,y0);
    ctx.lineTo(x1,y1);
    ctx.stroke();
}

function canvas_fillScreen(color) {
    ctx.beginPath();
    ctx.fillStyle = color;
    ctx.rect(0, 0, c.width, c.height);
    ctx.fill();
}

/* APIs */
function drawLine(x0, y0, x1, y1, color) {
    $.ajax({
        url : '/drawLine', 
        method : 'POST',
        data : '<params><x0>' + x0 + '</x0><y0>' + y0 + '</y0><x1>' + x1 + '</x1><y1>' + y1 + '</y1><color>' + hexTo565(color) + '</color></params>'
    });
    window.createNotification({showDuration: 5000})({ title: "Drawline", message: "From (" + x0 + ", " + y0 + ") to (" + x1 + ", " + y1 + ")"});
}

function drawSegment(rowIndex, colIndex, length, line) {
    $.ajax({
        url : '/drawRow',
        method : 'POST',
        data : '<params><rowIndex>' + rowIndex + '</rowIndex><colIndex>' + colIndex + '</colIndex><length>' + length + '</length><line>' + line + '</line></params>'
    });    
}

function fillScreen(color) {
    $.ajax({
        url : '/fillScreen',
        method : 'POST',
        data : '<params><color>' + hexTo565(color) + '</color></params>'
    });
    window.createNotification({showDuration: 5000})({ title: "Filled", message: color });
}

/* Extra */
function hexTo565(hexColor) {    
    const hexR = parseInt(hexColor.substring(1, 3), 16);
    const hexG = parseInt(hexColor.substring(3, 5), 16);
    const hexB = parseInt(hexColor.substring(5, 7), 16);    
    return (hexR >> 3) << 11 | (hexG >> 2) << 5 | (hexB >> 3);
}

function RGBTo565(r, g, b) {
    return (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3);
}

function dragElement(elmnt) {
    let pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
    if (document.getElementById(elmnt.id + "header")) { document.getElementById(elmnt.id + "header").onmousedown = dragMouseDown; } else { elmnt.onmousedown = dragMouseDown; }
    function dragMouseDown(e) { e = e || window.event; e.preventDefault(); pos3 = e.clientX; pos4 = e.clientY; document.onmouseup = closeDragElement; document.onmousemove = elementDrag; }
    function elementDrag(e) { e = e || window.event; e.preventDefault(); pos1 = pos3 - e.clientX; pos2 = pos4 - e.clientY; pos3 = e.clientX; pos4 = e.clientY; elmnt.style.top = (elmnt.offsetTop - pos2) + "px"; elmnt.style.left = (elmnt.offsetLeft - pos1) + "px"; }
    function closeDragElement() { document.onmouseup = null; document.onmousemove = null; }
}

function map(x, in_min, in_max, out_min, out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

function wait(time) {
    return new Promise(resolve => {
        setTimeout(() => {
            resolve();
        }, time);
    });
}
/* SSE */
var eventSourcePot;
var eventSourceTouch;

function listenPotSSE() {
    eventSourcePot = new EventSource("/pot");

    eventSourcePot.onmessage = (data) => {
        if(data.data == "CLOSE") {
            $('#bar').text("");
            $('#bar').css('width', '0px');            
            eventSourcePot.close();
            return;
        }
        $('#bar').text(data.data);
        $('#bar').css('width', map(data.data, 0, 1023, 0, 100) + '%');
    };
    
    eventSourcePot.onError = (error) => {
        eventSourcePot.close();
        console.log(error);
    };
}

function closePotSSE() {
    $('#bar').text("");
    $('#bar').css('width', '0px');
    eventSourcePot.close();
    $.get( "/endPot");
}

function listenTouchSSE() {
    eventSourceTouch = new EventSource("/touch");
    $("#payload").css("background-color", "rgba(255, 255, 255, 255)");
    eventSourceTouch.onmessage = (data) => {
        if(data.data == "CLOSE") {
            eventSourceTouch.close();
            return;
        }
        let coord = JSON.parse(data.data);
        $("#payload").css({left:coord.x/3.2 + "%", top:coord.y/2.4 + "%"});
    };
    
    eventSourceTouch.onError = (error) => {
        eventSourceTouch.close();
        $("#payload").css("background-color", "rgba(255, 255, 255, 0)");
        console.log(error);
    };
}

function closeTouchSSE() {
    eventSourceTouch.close();
    $("#payload").css("background-color", "rgba(255, 255, 255, 0)");
    $.get( "/endTouch");
}