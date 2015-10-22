Pebble.addEventListener('ready',function(e) {
   console.log('JavaScript app for OEBBFahrplan ready and running!');
   
   
   var updateTrains = function() {
 console.log('request data');
      var req = new XMLHttpRequest();
      req.onload = function(e) {
         if(req.readyState == 4 && req.status == 200) {
            console.log('Recieved response');
              var journeysObj;
              eval(req.responseText);
            for(var i = 0; i < 12; i++){
               var id1 = i*3;
               var id2 = i*3 + 1;
               var id3 = i*3 + 2;
               
          var dataDict = {};
          dataDict[id1] = journeysObj.journey[id2].ti;
          dataDict[id2] =   "false";
          dataDict[id3] = journeysObj.journey[id3].pr;
          //myArray.push(obj);
            //var dataDict = { id1:journeysObj.journey[id2].ti, id2:"false", id3:journeysObj.journey[id3].pr};
            Pebble.sendAppMessage(dataDict);
               console.log(JSON.stringify(dataDict));
            }
            //console.log(req.responseText);
            //console.log(journeysObj.journey[0].ti);
         } else {
            console.log('Error on sending Post request'); 
         }
      };
      var requestString = "http://fahrplan.oebb.at/bin/stboard.exe/dn?L=vs_scotty.vs_liveticker&evaId=8100008&boardType=dep&productsFilter=1111111111111111&additionalTime=0&disableEquivs=yes&maxJourneys=50&outputMode=tickerDataOnly&start=yes&selectDate=today";
      req.open('GET', requestString, true);
		req.send();
};
   
     
setInterval(updateTrains, 3000);
   
   
});