// HEADER FOR EACH PAGE


// OPTIONS MENU
function menuFunction(x) {
  x.classList.toggle("change");
  
  document.getElementById('myDropdown').style.display = document.getElementById('myDropdown').style.display =='block' ?   'none' : 'block';
}

// HELPER FUNCTIONS/INFO
// Color conversions for data transfer
function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16)
    } : null;
  }

// Metro Station Database
// const metroStations = [['A01','Metro Center'],['A02','Farragut North'],['A03','Dupont Circle'],['A04','Woodley Park-Zoo/Adams Morgan']];
const metroStations = [["G03","Addison Road-Seat Pleasant"],["F06","Anacostia"],["F02","Archives-Navy Memorial-Penn Quarter"],["C06","Arlington Cemetery"],["N12","Ashburn"],["K04","Ballston-MU"],["G01","Benning Road"],["A09","Bethesda"],["C12","Braddock Road"],["F11","Branch Ave"],["B05","Brookland-CUA"],["G02","Capitol Heights"],["D05","Capitol South"],["D11","Cheverly"],["K02","Clarendon"],["A05","Cleveland Park"],["E09","College Park-U of Md"],["E04","Columbia Heights"],["F07","Congress Heights"],["K01","Court House"],["C09","Crystal City"],["D10","Deanwood"],["G05","Downtown Largo"],["K07","Dunn Loring-Merrifield"],["A03","Dupont Circle"],["K05","East Falls Church"],["D06","Eastern Market"],["C14","Eisenhower Avenue"],["A02","Farragut North"],["C03","Farragut West"],["D04","Federal Center SW"],["D01","Federal Triangle"],["C04","Foggy Bottom-GWU"],["B09","Forest Glen"],["E06","Fort Totten"],["J03","Franconia-Springfield"],["A08","Friendship Heights"],["F01","Gallery Pl-Chinatown"],["E05","Georgia Ave-Petworth"],["B11","Glenmont"],["E10","Greenbelt"],["N03","Greensboro"],["A11","Grosvenor-Strathmore"],["N08","Herndon"],["C15","Huntington"],["E08","Hyattsville Crossing"],["N09","Innovation Center"],["B02","Judiciary Square"],["C13","King St-Old Town"],["F03","L'Enfant Plaza"],["D12","Landover"],["N11","Loudoun Gateway"],["N01","McLean"],["C02","McPherson Square"],["A10","Medical Center"],["C01","Metro Center"],["D09","Minnesota Ave"],["G04","Morgan Boulevard"],["E01","Mt Vernon Sq 7th St-Convention Center"],["F05","Navy Yard-Ballpark"],["F09","Naylor Road"],["D13","New Carrollton"],["B35","NoMa-Gallaudet U"],["A12","North Bethesda"],["C07","Pentagon"],["C08","Pentagon City"],["D07","Potomac Ave"],["C11","Potomac Yard"],["N07","Reston Town Center"],["B04","Rhode Island Ave-Brentwood"],["A14","Rockville"],["C10","Ronald Reagan Washington National Airport"],["C05","Rosslyn"],["A15","Shady Grove"],["E02","Shaw-Howard U"],["B08","Silver Spring"],["D02","Smithsonian"],["F08","Southern Avenue"],["N04","Spring Hill"],["D08","Stadium-Armory"],["F10","Suitland"],["B07","Takoma"],["A07","Tenleytown-AU"],["A13","Twinbrook"],["N02","Tysons"],["E03","U Street/African-Amer Civil War Memorial/Cardozo"],["B03","Union Station"],["J02","Van Dorn Street"],["A06","Van Ness-UDC"],["K08","Vienna/Fairfax-GMU"],["K03","Virginia Square-GMU"],["N10","Washington Dulles International Airport"],["F04","Waterfront"],["K06","West Falls Church"],["E07","West Hyattsville"],["B10","Wheaton"],["N06","Wiehle-Reston East"],["A04","Woodley Park-Zoo/Adams Morgan"]];

// METROTRACKER Functions
if(document.body.id=='metrotracker'){
  // Add all metro stations to dropdown list
  dropdown = document.getElementById("MetroSelector")

  for (let i = 0; i < metroStations.length; i++){
  var opt = document.createElement('option');
    opt.value = metroStations[i][0];
    opt.innerHTML = metroStations[i][1];
    dropdown.appendChild(opt);
  }

  dropdown.value = "C04";

}

// TEXTDISPLAY Functions
if(document.body.id=='textdisplay'){
  // Input text to display: sanitize and re-insert into field for submission
  document.getElementById("textSubmit").addEventListener('click',(e)=>{
      var textToSubmit = document.getElementById("FreeText").value;
      textToSubmit = textToSubmit.replace("'","/'");
      textToSubmit = textToSubmit.replace('"',"&quot");
      document.getElementById("FreeText").value = textToSubmit;    
  });
}

// FIREPLACE Functions
if(document.body.id=='fireplace'){
  // Toggle between automatic and manual fire control
  document.getElementById("fireplacetoggle").addEventListener('change',(e)=>{
    if(document.getElementById("fireplacetoggle").checked){
      document.getElementById("fire_mode").value="1";
    }
    else{
      document.getElementById("fire_mode").value="0";
    }
    document.getElementById("fire_mode").click();
  });

  // Pull current mode (0 for automatic, 1 for manual) from url parameters
  var urlParams = new URLSearchParams(window.location.search);
  var curr_mode = parseInt(urlParams.get("fire_mode"));
  var stokedFire = urlParams.get("StokeFire");
  if(stokedFire!=null){
    curr_mode = 1;
    // If StokeFire is in the URL, then fire was stoked, so set current mode to manual
  }
  document.getElementById("fireplacetoggle").checked=curr_mode;
  if(document.getElementById("fireplacetoggle").checked){
    document.getElementById("StokeFire").removeAttribute("disabled","");
  }
  else{
    document.getElementById("StokeFire").setAttribute("disabled","");
  }
  // If StokeFire is in the URL, then fire was stoked, so set current mode to manual
  // var stokedFire = urlParams.get("StokeFire");
  // if(stokedFire!=null){
  //   document.getElementById("fireplacetoggle").checked=1;
  //   if(document.getElementById("fireplacetoggle").checked){
  //     document.getElementById("StokeFire").removeAttribute("disabled","");
  //   }
  //   else{
  //     document.getElementById("StokeFire").setAttribute("disabled","");
  //   }
  // }
}

// IMAGEDISPLAY Functions
if(document.body.id=='imagedisplay'){
  // OBE: color conversions and data handling for fullscreen color
  // document.getElementById("Fullscreen Color").addEventListener('change', (e) => {
  //     var color = e.target.value;
  //     var red = hexToRgb(color).r;
  //     var green = hexToRgb(color).g;
  //     var blue = hexToRgb(color).b;
  //     document.getElementById("ColorDisp Red").value = red;
  //     document.getElementById("ColorDisp Green").value = green;
  //     document.getElementById("ColorDisp Blue").value = blue;
  // });

  // Change between drawing images and uploading images
  document.getElementById("imagetoggle").addEventListener('change',(e)=>{
    if(document.getElementById("imagetoggle").checked){
      document.getElementById("drawing-elements").classList.add("hide");
      document.getElementById("imageupload-elements").classList.remove("hide");
    }
    else{
      document.getElementById("drawing-elements").classList.remove("hide");
      document.getElementById("imageupload-elements").classList.add("hide");
    }
  });
}