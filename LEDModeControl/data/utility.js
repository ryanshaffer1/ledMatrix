function RGBtoHexColor(Red,Green,Blue){
    /* Convert to 5/6/5 binary bit RGB */
    var RedBinary = Math.floor(Red/8).toString(2);
    var GreenBinary = Math.floor(Green/4).toString(2);
    var BlueBinary = Math.floor(Blue/8).toString(2);
    
    /* Check for correct number of bits (and add if necessary)*/
    while (RedBinary.length != 5) { 
        RedBinary = "0" + RedBinary;
    }
    while (GreenBinary.length != 6) { 
        GreenBinary = "0" + GreenBinary;
    }
    while (BlueBinary.length != 5) { 
        BlueBinary = "0" + BlueBinary;
    }

    /* Append binary together*/
    var Binary = RedBinary + GreenBinary + BlueBinary;

    /* Convert to hex (must do character by character to preserve leading zeroes)*/
    var hexChar1 = parseInt(Binary.slice(0,4),2).toString(16);
    var hexChar2 = parseInt(Binary.slice(4,8),2).toString(16);
    var hexChar3 = parseInt(Binary.slice(8,12),2).toString(16);
    var hexChar4 = parseInt(Binary.slice(12,16),2).toString(16);
    var hex = "0x" + hexChar1 + hexChar2 + hexChar3 + hexChar4;

    return hex;

}