const canvas = document.getElementById('drawing-board');
const toolbar = document.getElementById('drawing-toolbar');
const ctx = canvas.getContext('2d');

var canvasOffsetX = canvas.offsetLeft;
var canvasOffsetY = canvas.offsetTop;

let isPainting = false;
let lineWidth = 5;
let pixelRes = 10; // Resolution (in pixels) between screen and LED board; ie how many screen pixels = one LED
let startX;
let startY;
let pixelDataString = "";

canvas.width = 32*pixelRes;
canvas.height = 32*pixelRes;

ctx.fillStyle = document.getElementById("pencolor").value;

toolbar.addEventListener('click', e => {
    if (e.target.id === 'fillscreen'){
        ctx.fillRect(0,0,canvas.width,canvas.height);
        updatePixelData();
    }
    else if (e.target.id === 'clear') {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
    }
});

toolbar.addEventListener('change', e => {
    if(e.target.id === 'pencolor') {
        ctx.fillStyle = e.target.value;
    }    
});

function disableScrolling(){
    var x=window.scrollX;
    var y=window.scrollY;
    window.onscroll=function(){window.scrollTo(x, y);};
    document.body.classList.add("stop-scrolling");
}

function enableScrolling(){
    window.onscroll=function(){};
    document.body.classList.remove("stop-scrolling");
}

const draw = (e) => {
    if(!isPainting) {
        return;
    }

    /* Determine pixel to draw at*/
    if (e.clientX) {
        var xCoord = e.clientX - canvasOffsetX;
        var yCoord = e.clientY - canvasOffsetY + window.scrollY;
    }
    else if (e.layerX){
        var xCoord = e.layerX;
        var yCoord = e.layerY;
    }

    pixelX = Math.floor(xCoord/pixelRes);
    pixelY = Math.floor(yCoord/pixelRes);
    /* Draw */
    ctx.fillRect(pixelX*pixelRes,pixelY*pixelRes,pixelRes,pixelRes);
    ctx.stroke();
}

function updatePixelData(){
    pixelDataString = ""
    imageData = ctx.getImageData(0,0,32*pixelRes,32*pixelRes);
    for (let i = 0; i < 32; i++){
        for (let j = 0; j < 32; j++){
            var pixelStartIndex = i*pixelRes*32*pixelRes*4 + j*pixelRes*4;
            var pixelRed = imageData.data[pixelStartIndex];
            var pixelGreen = imageData.data[pixelStartIndex+1];
            var pixelBlue = imageData.data[pixelStartIndex+2];

            /* Convert to 5/6/5 binary bit RGB */
            var pixelRedBinary = Math.floor(pixelRed/8).toString(2);
            var pixelGreenBinary = Math.floor(pixelGreen/4).toString(2);
            var pixelBlueBinary = Math.floor(pixelBlue/8).toString(2);
            
            /* Check for correct number of bits (and add if necessary)*/
            while (pixelRedBinary.length != 5) { 
                pixelRedBinary = "0" + pixelRedBinary;
            }
            while (pixelGreenBinary.length != 6) { 
                pixelGreenBinary = "0" + pixelGreenBinary;
            }
            while (pixelBlueBinary.length != 5) { 
                pixelBlueBinary = "0" + pixelBlueBinary;
            }

            /* Append binary together*/
            var pixelBinary = pixelRedBinary + pixelGreenBinary + pixelBlueBinary;

            /* Convert to hex (must do character by character to preserve leading zeroes)*/
            var hexChar1 = parseInt(pixelBinary.slice(0,4),2).toString(16);
            var hexChar2 = parseInt(pixelBinary.slice(4,8),2).toString(16);
            var hexChar3 = parseInt(pixelBinary.slice(8,12),2).toString(16);
            var hexChar4 = parseInt(pixelBinary.slice(12,16),2).toString(16);
            var hexPixel = "0x" + hexChar1 + hexChar2 + hexChar3 + hexChar4;

            pixelDataString = pixelDataString + hexPixel + " ";
        }
        pixelDataString = pixelDataString + '\n'
    }
document.getElementById("ImageData").value = pixelDataString;
}

/* Events for mouse users */
canvas.addEventListener('mousedown', (e) => {
    disableScrolling();
    isPainting = true;
    startX = e.layerX;
    startY = e.layerY;
});

canvas.addEventListener('mouseup', e => {
    enableScrolling();
    isPainting = false;
    ctx.stroke();
    ctx.beginPath();
    updatePixelData();
});

canvas.addEventListener('mousemove', draw);

/* Events for touchscreen users */
canvas.addEventListener('touchstart', (e) => {
    disableScrolling();
    isPainting = true;
    startX = e.layerX;
    startY = e.layerY;
});

canvas.addEventListener('touchend', e => {
    enableScrolling();
    isPainting = false;
    ctx.stroke();
    ctx.beginPath();
    updatePixelData();
});

canvas.addEventListener('touchmove', draw);

canvas.addEventListener('touchmove', (e) => {
    e.preventDefault();
});
