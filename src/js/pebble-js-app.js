"use strict";

// Travel destinations
var destinations =
	[{"name": "Work", "latitude": "50.8729926", "longitude": "4.2892823"},
		{"name": "Home", "latitude": "50.8698831266266", "longitude": "4.678352298219388"}];

// API settings
var touringEndpoint = 'http://touring.api.staging.be-mobile.be';
var baseMessage = '{"LANG":"nl","M":["car"],"RC":"os","TO":{"COORD":{"LA":"{toLat}","LO":"{toLong}"}},"EC":"rs","FR":{"COORD":{"LA":"{fromLat}","LO":"{fromLong}"}}}';

// Cache current location
var myLocation;

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    navigator.geolocation.getCurrentPosition(function(pos) {
    	myLocation = pos;
    	sendTravelTimesToPebble();
    }, function(err) {console.log("Location error: " + err)}, {
		enableHighAccuracy: true, 
		maximumAge: 0, 
		timeout: 5000
	});
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    sendTravelTimesToPebble();
  }                     
);

function sendTravelTimesToPebble() {
	getTravelTimeForAllLocations(function(travelTimes) {
		Pebble.sendAppMessage({
			'KEY_TRAVELTIMES_DATA': encodeTravelTimesData(travelTimes)
		},
		function(e) {
			console.log('Successfully sent');
		},
		function(e) {
			console.log('Error sending');
		});
	});
}

function getTravelTimeForAllLocations(callback) {
	var travelTimes = [];
	destinations.forEach(function(destination, index) {
		getTravelTime(destination, function(travelTime) {
			travelTimes.push({"name": destination.name, "travelTime": travelTime.travelTime, "delay": travelTime.delay});
			console.log(travelTimes.length + " " + destination.name + ": " + travelTime.travelTime + " " + travelTime.delay + " delay");
			if (index === destinations.length - 1) {
				callback(travelTimes);
			}
		});
	});
}

function getTravelTime(loc, callback) {
	var req = new XMLHttpRequest();
	req.setRequestHeader('Content-Type', 'application/json');
	req.setRequestHeader('Content-Length', baseMessage.length);
	// Mask the fact that Pebble time is sending request
	req.setRequestHeader('User-Agent', 'Mobilis/2.0.4 (iPhone; iOS 9.0; Scale/2.00)');
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			var response = JSON.parse(req.responseText);
			var delay = Math.round(response.TD / 60);
			var travelTime = Math.round(response.TA / 60);
			callback({"travelTime": travelTime, "delay": delay});
		} else {
			console.log("Got status code " + req.status + " from API");
		}
	};
	req.open('POST', touringEndpoint + '/service/TravelTimes/Personal3');
	var message = 
		baseMessage.supplant({"fromLat": myLocation.coords.latitude, "fromLong": myLocation.coords.longitude, "toLat": loc.latitude, "toLong": loc.longitude});
	req.send(message);
}


// Util
String.prototype.supplant = function (o) {
    return this.replace(/{([^{}]*)}/g,
        function (a, b) {
            var r = o[b];
            return typeof r === 'string' || typeof r === 'number' ? r : a;
        }
    );
};

function encodeTravelTimesData(travelTimes) {
	var encoded = '';
	travelTimes.forEach(function(travelTime, index) {
		encoded += travelTime.name + ',' + travelTime.travelTime + ',' + travelTime.delay + ';';
	});
}