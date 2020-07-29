var c;
var ctx;
var potSse = false;
var touchSse = false;

function loaded() {
    $("#payload").hide();
    dragElement(document.getElementById("overlay"));
    $("#overlay").css("left", ($(document).width()-$("#overlay").width())/2 + "px");
    c = document.getElementById("canvas");
    ctx = c.getContext("2d");
    canvas_fillScreen(0);
}

/* Click */
function click_btnDrawLine() {
    const x0 = Math.floor($('#drawLineX0').val());
    const y0 = Math.floor($('#drawLineY0').val());
    const x1 = Math.floor($('#drawLineX1').val());
    const y1 = Math.floor($('#drawLineY1').val());
    const color = $('#drawLineColor').val();
    drawLine(x0, y0, x1, y1, color);
    canvas_drawLine(x0, y0, x1, y1, color);
}

function click_btnFillScreen() {
    const color = $('#fillScreenColor').val();
    canvas_fillScreen(color);
    
    const r = parseInt(color.substring(1, 3), 16);
    const g = parseInt(color.substring(3, 5), 16);
    const b = parseInt(color.substring(5, 7), 16);
    const col = (( r + g + b) / 3) <= 60 ? "#eeeeee" : "#111111";
    $("h1").css("color", col);
    $("p").css("color", col);
    $("#bar").css("color", col);
    $("#payload").css("background", col);
    $("#bar").css("background", color);
    fillScreen(color);
}

function click_btnPot() {    
    if(potSse) {
        closePotSSE();
    } else {
        if(touchSse) {
            closeTouchSSE();
        }
        listenPotSSE();
    }
}

function click_btnTouch() {    
    if(touchSse) {
        closeTouchSSE();
    } else {
        if(potSse) {
            closePotSSE();
        }
        listenTouchSSE();
    }    
}

/* Canvas */
function canvas_drawLine(x0, y0, x1, y1, color) {
    ctx.beginPath();
    ctx.strokeStyle = color;    
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

/* SSE */
var eventSourcePot;
var eventSourceTouch;

function listenPotSSE() {
    eventSourcePot = new EventSource("/pot");
    potSse = true;

    eventSourcePot.onmessage = (data) => {
        $('#bar').text(data.data);
        $('#bar').css('width', data.data/10.23 + '%');
    };
    
    eventSourcePot.onError = (error) => {
        potSse = false;
        $.get('/close');
        eventSourcePot.close();
        console.log(error);
    };
}

function closePotSSE() {
    $('#bar').text("");
    $('#bar').css('width', '0px');
    potSse = false;
    $.get('/close');
    eventSourcePot.close();    
}

function listenTouchSSE() {
    eventSourceTouch = new EventSource("/touch");
    touchSse = true;
    $("#payload").show();

    eventSourceTouch.onmessage = (data) => {
        let coord = JSON.parse(data.data);
        $("#payload").css({left:coord.x/3.2 + "%", top:coord.y/2.4 + "%"});
    };
    
    eventSourceTouch.onError = (error) => {
        touchSse = false;
        $.get('/close');
        eventSourceTouch.close();
        $("#payload").hide();
        console.log(error);
    };
}

function closeTouchSSE() {
    $("#payload").hide();
    touchSse = false;
    $.get('/close');
    eventSourceTouch.close();
}