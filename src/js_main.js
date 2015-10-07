Pebble.addEventListener('ready',function(e) {
   console.log('JavaScript app for OEBBFahrplan ready and running!');
   Pebble.addEventListener('appmessage',function(e) {
      //console.log('Received message: ' + JSON.stringify(e.payload));
      //if(!("command" in e.payload)){
      console.log(e.payload[0]);   
      //Setup the pi request!    
      var req = new XMLHttpRequest();
      req.onload = function(e) {
         if(req.readyState == 4 && req.status == 200) {
            console.log('Recieved Anwer');
            //success, add acc for appmessage here
            var journeysObj;
            //console.log(req.responseText);
            eval(req.responseText);
            console.log(journeysObj.journey[0].ti);
            Pebble.sendAppMessage({ 0:journeysObj.journey[0].ti});
         } else {
            console.log('Error on sending Post request'); 
         }
      };
         
         switch (e.payload[1]) {
         case 0:
            console.log('sending');
            var requestString = "http://fahrplan.oebb.at/bin/stboard.exe/dn?L=vs_scotty.vs_liveticker&evaId=8100008&boardType=dep&productsFilter=1111111111111111&additionalTime=0&disableEquvis=yes&maxJourneys=50&outputMode=tickerDataOnly&start=yes&selectDate=today";
            req.open('GET', requestString, true);
				req.send();
            break;
         case 1:
            //req.open('POST', 'http://192.168.0.112:8000/macros/radioOn', true);
            //req.send();
            break;
         default:
            //hmm... somehow stop the request and send a appmessage back to the watch to tell that it failed
            break;
      }
         
      //}
      //else{
      //other command...   
     // }
   });	
});