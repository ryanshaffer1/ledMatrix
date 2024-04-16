const canvas_img = document.getElementById('image-canvas');
const ctx_img = canvas_img.getContext('2d');

var canvasOffsetX_img = canvas_img.offsetLeft;
var canvasOffsetY_img = canvas_img.offsetTop;


let isDrawing = false;
let startXcrop;
let startYcrop;
let img;
let rectWidth;

/* INITIAL IMAGE UPLOAD */
document.getElementById('inp').onchange = function(e) {
    img = new Image();
    img.onload = drawImg;
    img.onerror = failed;
    img.src = URL.createObjectURL(this.files[0]);

  };


  function drawImg() {
    /* Determine scale factor to draw at such that neither width nor height exceed their bounds */
    var scaleX = 0.6*window.innerWidth/this.width;
    var scaleY = 0.6*window.innerHeight/this.height;
    var scaleFactor = Math.min(scaleX,scaleY,1);

    canvas_img.width = this.width * scaleFactor;
    canvas_img.height = this.height * scaleFactor;

    ctx_img.drawImage(this, 0,0,this.width,this.height, 0,0,canvas_img.width,canvas_img.height);
    document.getElementById("canvasLabel").classList.remove("hide");

    canvasOffsetX_img = canvas_img.offsetLeft;
    canvasOffsetY_img = canvas_img.offsetTop;

  }
  function failed() {
    alert("The provided file couldn't be loaded as an Image media");
  }



  /* IMAGE CROPPING */
  function drawSquare(e) {
    if(!isDrawing) {
        return;
    }

    /* Determine current mouse coordinate*/
    if (e.clientX) {
        var xCoord = e.clientX - canvasOffsetX_img;
        var yCoord = e.clientY - canvasOffsetY_img + window.scrollY;
    }
    else if (e.layerX){
        var xCoord = e.layerX;
        var yCoord = e.layerY;
    }

    var rectWidth = xCoord-startXcrop;
    var rectHeight = yCoord-startYcrop;

    /* Constrain rectangle to a square that is a multiple of 32 pixels */
    if (rectWidth<rectHeight){
        rectWidth = Math.round(rectWidth/32)*32;
        rectHeight = rectWidth;
    }
    else{
        rectHeight = Math.round(rectHeight/32)*32;
        rectWidth = rectHeight;
    }

    /* Draw */
    ctx_img.clearRect(0,0,canvas_img.width,canvas_img.height);
    ctx_img.drawImage(img, 0,0,img.width,img.height, 0,0,canvas_img.width,canvas_img.height);
    ctx_img.strokeRect(startXcrop,startYcrop,rectWidth,rectHeight);
    return rectWidth;
  }

  /* IMAGE PROCESSING */
  function updatePixelDataCrop(){
    pixelDataString = ""
    var pixelRes = rectWidth/32;
    imageData = ctx_img.getImageData(startXcrop,startYcrop,rectWidth,rectWidth);
    // Compute the value of each pixel on the 32x32 display. Start by splitting the image into a 32x32 grid
    for (let i = 0; i < 32; i++){
        for (let j = 0; j < 32; j++){
            // Option 1: take the value of the center of each grid space as the value of that pixel in the display
            // var pixelStartIndex = (i+0.5)*pixelRes*32*pixelRes*4 + (j+0.5)*pixelRes*4;
            // var pixelRed = imageData.data[pixelStartIndex];
            // var pixelGreen = imageData.data[pixelStartIndex+1];
            // var pixelBlue = imageData.data[pixelStartIndex+2];
            // Option 2: average out the red/green/blue within each grid space and use as the value of that pixel in the display
            var pixelRed = 0;
            var pixelGreen = 0;
            var pixelBlue = 0;
            for (let subPixI = 0; subPixI < pixelRes; subPixI++){
                var PixelRowStartIndex = i*pixelRes*32*pixelRes*4 + subPixI*pixelRes*32*4;
                for (let subPixJ = 0; subPixJ < pixelRes; subPixJ++){
                     var subPixIndex = PixelRowStartIndex + j*pixelRes*4 + subPixJ*4;
                     var pixelRed = pixelRed + imageData.data[subPixIndex];
                     var pixelGreen = pixelGreen + imageData.data[subPixIndex+1];
                     var pixelBlue = pixelBlue + imageData.data[subPixIndex+2];
                }
            }
            pixelRed = pixelRed/(pixelRes*pixelRes);
            pixelGreen = pixelGreen/(pixelRes*pixelRes);
            pixelBlue = pixelBlue/(pixelRes*pixelRes);

            if(isNaN(pixelBlue)){
                console.log('Pixel i/j:');
                console.log(i);
                console.log(j);
            }

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
document.getElementById("ImageData-Upload").value = pixelDataString;
}

  /* Events for mouse users */
canvas_img.addEventListener('mousedown', (e) => {
    disableScrolling();
    isDrawing = true;
    startXcrop = e.clientX - canvasOffsetX_img;
    startYcrop = e.clientY - canvasOffsetY_img + window.scrollY;
});

canvas_img.addEventListener('mouseup', e => {
    enableScrolling();
    isDrawing = false;
    updatePixelDataCrop();
});

canvas_img.addEventListener('mousemove', (e) => {
    rectWidth = drawSquare(e);
    e.preventDefault();
});

/* Events for touchscreen users */
canvas_img.addEventListener('touchstart', (e) => {
    disableScrolling();
    isDrawing = true;
    startXcrop = e.layerX;
    startYcrop = e.layerY;
});

canvas_img.addEventListener('touchend', e => {
    enableScrolling();
    isDrawing = false;
    updatePixelDataCrop();
});

canvas_img.addEventListener('touchmove', (e) => {
    rectWidth = drawSquare(e);
    e.preventDefault();
});