"use strict";
const mapeles = document.querySelectorAll("div[class*='gpxview:'],figure[class*='gpxview:']");
for(let i=0;i<mapeles.length;i++) { mapeles[i].addEventListener("click_Marker_Bild",function(event) { 
	const text = event.detail.events.eventparameter.text;
	const coord = event.detail.events.eventparameter.coord;
	const src = event.detail.events.eventparameter.src;
	
	event.preventDefault();

	const imdiv = document.createElement("div");
	const fig = document.createElement("figure");
	const im = document.createElement("img");
	const figcap = document.createElement("figcaption");
	const but = document.createElement("button");
	const cross = '<svg role="img" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1 1"><line x1="0.1" y1="0.1" x2="0.9" y2="0.9" stroke-width="0.15" stroke="black"/><line x1="0.1" y1="0.9" x2="0.9" y2="0.1" stroke-width="0.15" stroke="black"/></svg>'; 
	but.innerHTML = cross;
	im.src = src;
	imdiv.className = "JB_Photo";
	figcap.innerHTML = text;
	fig.appendChild(but);
	fig.appendChild(im);
	fig.appendChild(figcap);
	imdiv.appendChild(fig);
	document.body.appendChild(imdiv);
	window.setTimeout(function(){
		try { imdiv.style.backgroundColor = "rgba(0,0,0,0.3)"; } catch(e) {};
		imdiv.style.fontSize = "1em";
		im.className="gross";
	},100);
	but.onclick = function() { 
		but.onclick = null;
		try { imdiv.style.backgroundColor = "rgba(0,0,0,0.0)"; } catch(e) {};
		imdiv.style.fontSize = "0.1em";
		im.className="";
		window.setTimeout(function(){document.body.removeChild(imdiv)},1000) ;
	} 
	if (coord.link && coord.link.length) {
		im.style.cursor = "pointer";
		im.onclick = function() {
			location.href = coord.link;
		}
	}
})}	
( function() {
	var style = function() {};
	style.prototype.create = function() {
		this.style = document.createElement("style");
		document.getElementsByTagName("head")[0].appendChild(this.style);
		this.style = document.styleSheets[document.styleSheets.length-1];
	}
	style.prototype.add = function(selector,rule) {
		if( this.style.cssRules )
			this.style.insertRule(selector+" {"+rule+"}", 0);
		else if ( this.style.rules ) 
			this.style.addRule(selector, rule);
	}
	var show_style = new style();
	show_style.create();
	show_style.add(".JB_Photo","position:fixed; display: flex; align-items: center; top:0; left:0; width:100%; height:100%; height:100vh; margin:0; background-color:rgba(0,0,0,0.0); font-size:0.1em; z-index:10000000; transition: all 0.5s; -webkit-transition: all 0.5s ");
	show_style.add(".JB_Photo>figure","margin:auto; display:inline-block; padding:10px; border:1px solid black; border-radius:10px; background-color:white; position:relative ");
	show_style.add(".JB_Photo>figure>img","max-width:50px; max-height:50px; transition: all 1s; -webkit-transition: all 1s ");
	show_style.add(".JB_Photo>figure>img.gross","max-width:90%; max-height:90%; max-width:calc(100vw - 50px); max-height:calc(100vh - 50px - 3em) ");
	show_style.add(".JB_Photo>figure>button","position:absolute; display:block; top:-0.5em; right:-0.5em; background-color:white; border-radius:1em; cursor:pointer; margin:0; padding:0.2em;");	
	show_style.add(".JB_Photo>figure>figcaption","text-align:center");
//	show_style.add(".JB_Photo>figure>button>img","width:1em; height:1em; margin:0; display:block");
//	show_style.add(".JB_Photo>figure>button:hover","color:red");
//	show_style.add(".JB_Photo>figure>button img svg:hover","stroke:red");
	show_style.add(".JB_Photo>figure>button>svg","width:1em; height:1em; margin:0; display:block");
	show_style.add(".JB_Photo>figure>button:hover>svg line","stroke:red");
})();
