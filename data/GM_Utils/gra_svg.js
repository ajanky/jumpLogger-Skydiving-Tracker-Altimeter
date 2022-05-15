// gra_svg
// Version vom 22. 5. 2021
// Jürgen Berkemeier
// www.j-berkemeier.de

"use strict";

console.info("gra_svg vom 22. 5. 2021");

var JB = window.JB || {};

// Das Grafikobjekt
JB.grafik = function(grafikelement) {
	this.method = "svg";
	// SVG in Größe des "grafikelement" anlegen
	if(typeof grafikelement == "string") grafikelement = document.getElementById(grafikelement);
	this.w = grafikelement.offsetWidth;
	this.h = grafikelement.offsetHeight;
	var linewidth = 1;
	var svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
	svg.setAttribute("width","100%");
	svg.setAttribute("height","100%");
	svg.setAttribute("viewBox","0 0 "+this.w+" "+this.h);
	svg.setAttribute("preserveAspectRatio","none");
	svg.style.position = "absolute";
	grafikelement.appendChild(svg);
	
	// Linienstärke setzen
	this.setwidth = function(width) {
		linewidth = width;
	} // setwidth
	
	// Punkt bei x,y, in Farbe c
	this.punkt = function(x,y,c) {
		var kreis = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
		kreis.setAttribute("cx",x);
		kreis.setAttribute("cy",y);
		kreis.setAttribute("r",linewidth);		
		kreis.setAttribute('fill',c);
		//kreis.setAttribute('stroke',c);
		svg.appendChild(kreis);
//		this.line(x,y,x+1,y,c);
	} // punkt

	// Linie von (xs,ys) nach (xe,ye) in Farbe color zeichnen
	this.line = function(xs,ys,xe,ye,color) {
		var linie = document.createElementNS('http://www.w3.org/2000/svg', 'line');
		linie.setAttribute("x1",xs);
		linie.setAttribute("y1",this.h-ys);
		linie.setAttribute("x2",xe);
		linie.setAttribute("y2",this.h-ye);
    linie.setAttribute('stroke', color);
    linie.setAttribute('stroke-width', linewidth);
		linie.setAttribute("vector-effect","non-scaling-stroke");
		svg.appendChild(linie);
	} // line

	// Polylinie mit den Werten in points in Farbe color zeichnen
	this.polyline = function(points,color) {
		var polyline = document.createElementNS('http://www.w3.org/2000/svg', 'polyline');
    polyline.setAttribute('stroke', color);
    polyline.setAttribute('stroke-width', linewidth);
    polyline.setAttribute('fill', "none");
		polyline.setAttribute("vector-effect","non-scaling-stroke");
		var pointstring = "";
		for(var i=0;i<points.length;i++) pointstring += points[i].x+","+(this.h-points[i].y)+" ";
		polyline.setAttribute('points', pointstring);
		svg.appendChild(polyline);
	} // polyline

	// Polylinie mit den Werten in points zeichnen
	// Die von der Polylinie umschlossene Fläche wird in Farbe color mit Alphawert alpha eingefärbt
	this.polyfill = function(points,color,alpha) { 
		var polygon = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');
    polygon.setAttribute('stroke', "none");
    polygon.setAttribute('fill', color);
    polygon.setAttribute('fill-opacity', alpha);
		var pointstring = "";
		for(var i=0;i<points.length;i++) pointstring += points[i].x+","+(this.h-points[i].y)+" ";
		polygon.setAttribute('points', pointstring);
		svg.appendChild(polygon);
	} // polyfill
	
	// Text an (x,y) ausgeben
	// size: Schriftgröße
	// text: Text
	// align: Bezug für (x,y), zwei Buchstaben, z.B. lu für links unten, s. case
	// diretion: Textrichtung: v für vertikal, sonst horizontal
	this.text = function(x,y,size,color,text,align,direction) {
		var stext = document.createElementNS('http://www.w3.org/2000/svg', 'text');
		stext.style.fontSize = size;
		stext.style.color = color;
		stext.style.fill = color;
		stext.textContent = text;
	
		var align_h = "m";
		var align_v = "m";
		if(align && align.length) {
			align_h = align.substr(0,1);
			if(align.length>1) align_v = align.substr(1,1);
		}
		switch(align_h) {
			case "l": stext.setAttribute("text-anchor","start"); break;
			case "m": stext.setAttribute("text-anchor","middle"); break;
			case "r": stext.setAttribute("text-anchor","end"); break;
			default:  stext.setAttribute("text-anchor","middle"); break;
		}
		switch(align_v) {
			case "o": stext.setAttribute("dy","1.1em"); break;
			case "m": stext.setAttribute("dy","0.3em"); break;
			case "u": stext.setAttribute("dy","-0.1em"); break;
			default:  stext.setAttribute("dy","-0.3em"); break;
		}
		
		stext.setAttribute("x",x);
		stext.setAttribute("y",this.h-y);
		if(direction && direction=="v") stext.setAttribute("transform","rotate(270 "+x+" "+(this.h-y)+")");
		
		svg.appendChild(stext);
		
	// Werte für unscale
		stext.setAttribute("class","noscale");
		stext.xorg = x;
		stext.yorg = this.h-y;
		if(direction && direction=="v") stext.phiorg = 270;
		else stext.phiorg = 0;
	} // text

	// Canvas löschen
	this.del = function() {
		grafikelement.innerHTML = "";
		grafikelement.appendChild(svg);
	} // del
	
	// Textbreite ermiteln
	this.getTextWidth = function(text,size) { 
		var stext = document.createElementNS('http://www.w3.org/2000/svg', 'text');
		stext.style.fontSize = size;
		stext.textContent = text;
		svg.appendChild(stext);
		var textLength = stext.getComputedTextLength();
		svg.removeChild(stext);
		return textLength;
	} // getTextWidth

	// Einige automatische Skalierungen rückgängig machen
	var unscale = function() {
		var etext = document.querySelectorAll(".noscale"),esvg,m;
		for(var i=0;i<etext.length;i++) {
			esvg = etext[i].ownerSVGElement;
			m = esvg.getScreenCTM();
			etext[i].setAttribute("transform","scale("+1/m.a+" "+1/m.d+")" + "rotate("+etext[i].phiorg+" "+etext[i].xorg*m.a+" "+etext[i].yorg*m.d+")");
			etext[i].setAttribute("x",etext[i].xorg*m.a);
			etext[i].setAttribute("y",etext[i].yorg*m.d);
		}
	}
	if(typeof(ResizeObserver) != "undefined") {
		let to,last=0,delta=100,now;
		const resizeObserver = new ResizeObserver(function(entries, observer) {
			now = Date.now();
			clearTimeout(to);
			to = setTimeout(function() { 
				unscale();
				last = Date.now();
			},delta);
			if( (now-last) > delta) {
				unscale();
				last = now;
			}
		});
		resizeObserver.observe(grafikelement); 
	}

} // grafik
