


String hosturl="https://crwdgs.com/";  //server url where the static resources is hosted
const char home_header[] PROGMEM = R"=====(
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>GPX File Viewer</title>
    <base target="_parent" />
    
  <style>
    *{
    margin: 0;
    padding: 0;
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background-color: rgb(242, 242, 247);
    }
    .container {
      position: relative;
      column-count: 1;
      column-gap: 1em;
      column-rule: thin solid black;
    }
  </style>
</head>
<body>
    <div class='container'>
        <center>
            <h3>GPX Files</h3><br>
        </center>
        <form action=''>
            <table id='list'>
        <tr style='font-weight:bold;'> 
          <td>File Name</td>
          <td>Size(kb)</td>
          <td>View</td>
          <td>Download</td>
        </tr>
)=====";

const char home_footer[] PROGMEM = R"=====(
      </table>
        </form>
    </div>
</body>
</html>
)=====";
const char html_header1[] PROGMEM = R"=====(
  <html lang='de'> 
  <head> 
    <meta charset='utf-8'/> 
    <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
    <title>GPXViewer</title>
    <script src=')=====";

const char html_body1[] PROGMEM = R"=====('></script>
    <style>
      body { width:calc(100% - 50px);padding: 10px 10px 10px 30px }
      figure { margin:0;padding:0; }
      #gpxviewer { display: grid; gap: .3em; grid-template-columns: repeat(auto-fit, minmax(15em, 1fr )); }
      #map1 { height:70vh; }
      #map1_profiles { height:70vh; }
      #map1_hp { height:calc((100% + 64px)/3);margin-top:10px }
      #map1_vp { height:calc((100% + 64px)/3);margin-top:-37px }
      #map1_vpt { height:calc((100% + 64px)/3);margin-top:-37px }
    </style>
  </head>
  <body>
  <div id="gpxviewer">
    <figure id='map1' class='gpxview:)=====";

 const char html_body2[] PROGMEM = R"=====(:Karte'><noscript><p> </p></noscript></figure>

    <figure id='map1_profiles'>
        <figure id='map1_hp' class='no_x'><noscript><p></p></noscript></figure>
        <figure id='map1_vp' class='no_x'><noscript><p></p></noscript></figure>
        <figure id='map1_vpt'><noscript><p></p></noscript></figure>
    </figure>
  </div>
  </body>
  <iframe src='/list' width=100% height=700px>
</html>
)=====";


String listfiles(fs::FS &fs, const char * dirname, uint8_t levels){
  String filelist="";
  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return "Error occured.";
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return "Error occured.";
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      /*Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
        listfiles(fs, file.name(), levels -1);
      }*/
    } else {
      filelist+="<tr><td>"+(String)file.name()+"</td><td>"+(String)file.size()+"</td><td>";
      int8_t len = strlen(file.name());
      if(((String)file.name()).substring(len-3,len) == "gpx" ){
        filelist+="<a href='/index.html?filename="+(String)file.name()+"'>View</a>";
      }
      else
      {
        filelist+="View";
      }
      filelist+="</td><td><a href='/download?filename="+(String)file.name()+"'>Download</a></td></tr>";
    }
    file = root.openNextFile();
  }
  return filelist;
}


void setupurl()
{
    server.on("/list",HTTP_GET , [] (AsyncWebServerRequest *request){
      
      String response;
      response = listfiles(SD, "/", 0);
      request->send(200,"text/html",home_header+response+home_footer);
    });
    
    server.on("/index.html",HTTP_GET , [] (AsyncWebServerRequest *request){

      String html_scripts=hosturl+"GM_Utils/GPX2GM.js";
      String html_filename;
      
      //get parameter list
      int paramsNr = request->params();
      bool flg=true;
      Serial.println(paramsNr);
      //print all parameter list
      for(int i=0;i<paramsNr;i++){
          AsyncWebParameter* p = request->getParam(i);
          if(p->name()=="filename"){
            html_filename=p->value();
            request->send(200,"text/html",html_header1+html_scripts+html_body1+html_filename+html_body2);
            flg=false;
          }
      }
      if(flg){
        String response;
        response += listfiles(SD, "/", 0);
        request->send(200,"text/html",home_header+response+home_footer);
      }
      
    });
    server.on("/",HTTP_GET , [] (AsyncWebServerRequest *request){

      String html_scripts=hosturl+"GM_Utils/GPX2GM.js";
      String html_filename;
      
      //get parameter list
      int paramsNr = request->params();
      bool flg=true;
      Serial.println(paramsNr);
      //print all parameter list
      for(int i=0;i<paramsNr;i++){
          AsyncWebParameter* p = request->getParam(i);
          if(p->name()=="filename"){
            html_filename=p->value();
            request->send(200,"text/html",html_header1+html_scripts+html_body1+html_filename+html_body2);
            flg=false;
          }
      }
      if(flg){
        String response;
        response += listfiles(SD, "/", 0);
        request->send(200,"text/html",home_header+response+home_footer);
        // Serial.println(response);
      }
      
    });
    server.serveStatic("/",SD, "/");
    
    server.on("/download",HTTP_GET , [] (AsyncWebServerRequest *request){

      //get parameter list
      int paramsNr = request->params();
      bool flg=true;
      Serial.println(paramsNr);
      //print all parameter list
      for(int i=0;i<paramsNr;i++){
          AsyncWebParameter* p = request->getParam(i);
          if(p->name()=="filename"){
            request->send(SD,p->value(), "text/html",true);
            flg=false;
          }
      }
      if(flg){
       request->send(200, "text/html","File Name not found in request"); 
      }
    });
    server.begin();
}


void initwifi()
{
  wifiMulti.addAP("jam2", "T57707300$");
  wifiMulti.addAP("iPhone 11 Andreas", "bfpycrgbr39mv");
  wifiMulti.addAP("GoJump", "lovegojump");

  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  if(!MDNS.begin("jumplogger")) {
    Serial.println("Error starting mDNS");
    return;
  }
  Serial.println(WiFi.localIP());
}
