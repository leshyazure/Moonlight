
$.get("/sensor/ldr", function(data){
document.getElementById("lightLevel").innerHTML = data.measureLight;
});

$.get("/read/color", function(readColor){
document.getElementById("redTargetView").innerHTML = readColor.rOn;
document.getElementById("redTarget").value = readColor.rOn;
document.getElementById("greenTargetView").innerHTML = readColor.gOn;
document.getElementById("greenTarget").value = readColor.gOn;
document.getElementById("blueTargetView").innerHTML = readColor.bOn;
document.getElementById("blueTarget").value = readColor.bOn;
document.getElementById("whiteTargetView").innerHTML = readColor.wOn;
document.getElementById("whiteTarget").value = readColor.wOn;
});

$.get("/read/timing", function(readTimings){
document.getElementById("duration").value = readTimings.duration;
document.getElementById("fadeIn").value = readTimings.fadeIn;
document.getElementById("fadeOut").value = readTimings.fadeOut;
});

$.get("/read/threshold", function(readThreshold){
document.getElementById("threshold").value = readThreshold.threshold;
});

$.get("/read/preview", function(readPreview){
document.getElementById("previewEnable").checked = readPreview.enable;
document.getElementById("previewTime").value = readPreview.time;
});

setupColors = function() {

var	redTarget = Number(document.getElementById("redTarget").value);
var	greenTarget = Number(document.getElementById("greenTarget").value);
var	blueTarget = Number(document.getElementById("blueTarget").value);
var	whiteTarget = Number(document.getElementById("whiteTarget").value);
var	redStandby = Number(document.getElementById("redStandby").value);
var	greenStandby = Number(document.getElementById("greenStandby").value);
var	blueStandby = Number(document.getElementById("blueStandby").value);
var	whiteStandby = Number(document.getElementById("whiteStandby").value);

    $.ajax({
        url: "/setup/color",
        type: "POST",
        data: JSON.stringify({ 
			redOn: redTarget, 
			redOff: redStandby,
			greenOn: greenTarget,
			greenOff: greenStandby,
			blueOn: blueTarget,
			blueOff: blueStandby,
			whiteOn: whiteTarget,
			whiteOff: whiteStandby}),
        dataType: "json",

    });
};

setupTiming = function() {

var DurationValue = Number(document.getElementById("duration").value);
var FadeInValue = Number(document.getElementById("fadeIn").value);
var FadeOutValue = Number(document.getElementById("fadeOut").value);

    $.ajax({
        url: "/setup/timing",
        type: "POST",
        data: JSON.stringify({ 
			duration: DurationValue,
			fadein: FadeInValue,
			fadeout: FadeOutValue}),
        dataType: "json",

    });
};

setupThreshold = function() {

var thresholdValue = Number(document.getElementById("threshold").value);

    $.ajax({
        url: "/setup/threshold",
        type: "POST",
        data: JSON.stringify({ 
			threshold: thresholdValue}),
        dataType: "json",

    });
};

setupPreview = function() {

var previewEnable = document.getElementById("previewEnable").checked;
var previewTime = Number(document.getElementById("previewTime").value);
    $.ajax({
        url: "/setup/preview",
        type: "POST",
        data: JSON.stringify({ 
			enable: previewEnable,
			time: previewTime}),
        dataType: "json",

    });
};


var redTargetView = document.getElementById("redTargetView");
var redStandbyView = document.getElementById("redStandbyView");
var greenTargetView = document.getElementById("greenTargetView");
var greenStandbyView = document.getElementById("greenStandbyView");
var blueTargetView = document.getElementById("blueTargetView");
var blueStandbyView = document.getElementById("blueStandbyView");
var whiteTargetView = document.getElementById("whiteTargetView");
var whiteStandbyView = document.getElementById("whiteStandbyView");
var threshold = document.getElementById("threshold");
var prevEnable = document.getElementById("previewEnable");
var prevTime = document.getElementById("previewTime");
var duration = document.getElementById("duration");
var fadeIn = document.getElementById("fadeIn");
var fadeOut = document.getElementById("fadeOut");

var lightLevel = document.getElementById("lightLevel");


redTargetView.innerHTML = redTarget.value;
greenTargetView.innerHTML = greenTarget.value;
blueTargetView.innerHTML = blueTarget.value;
whiteTargetView.innerHTML = whiteTarget.value;
redStandbyView.innerHTML = redStandby.value;
greenStandbyView.innerHTML = greenStandby.value;
blueStandbyView.innerHTML = blueStandby.value;
whiteStandbyView.innerHTML = whiteStandby.value;




redTarget.oninput = function() {
  redTargetView.innerHTML = this.value;
}
redTarget.onchange = function() {
  setupColors();
}
redStandby.oninput = function() {
  redStandbyView.innerHTML = this.value;
}
redStandby.onchange = function() {
  setupColors();
}
greenTarget.oninput = function() {
  greenTargetView.innerHTML = this.value;
}
greenTarget.onchange = function() {
  setupColors();
}
greenStandby.oninput = function() {
  greenStandbyView.innerHTML = this.value;
}
greenStandby.onchange = function() {
  setupColors();
}
blueTarget.oninput = function() {
  blueTargetView.innerHTML = this.value;
}
blueTarget.onchange = function() {
    setupColors();
}
blueStandby.oninput = function() {
  blueStandbyView.innerHTML = this.value;
}
blueStandby.onchange = function() {
    setupColors();
}
whiteTarget.oninput = function() {
  whiteTargetView.innerHTML = this.value;
}
whiteTarget.onchange = function() {
    setupColors();
}
whiteStandby.oninput = function() {
  whiteStandbyView.innerHTML = this.value;
}
whiteStandby.onchange = function() {
    setupColors();
}

threshold.onchange = function() {
    setupThreshold();
}

duration.onchange = function() {
    setupTiming();
}

fadeIn.onchange = function() {
    setupTiming();
}

fadeOut.onchange = function() {
    setupTiming();
}

prevEnable.onchange = function() {
	setupPreview();
}
prevTime.onchange = function() {
	setupPreview();
}