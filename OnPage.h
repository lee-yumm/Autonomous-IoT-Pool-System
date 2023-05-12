const char OnPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
    <title>Automation Control</title>
        <link rel="shortcut icon" href="http://www.gifanimategratis.eu/img/animali/squalo/anim044.gif"/>
        <style>
            body {
                background-color: #222;
                position: initial;
                margin: auto;
                background-size: 100% 100%;
                height: 100vh;
                width: 100vw;
            }
            img{
                margin: 0 -261.475px 0;
                position: relative;
                height: 100%;
                left: 50%;
            }
            button{
              position: absolute;
              right: 10%;
              top: 30%;
              height: 40%;
              width: 30%;
              letter-spacing: 7px;
              opacity: 70%;
              font-size: ;
              color: darkslategray;
              font-family: sans;
            }
    p{
      color: azure;
      position: initial;
      letter-spacing: 3px;
    }
    div.header{
        background-color: rgb(22,22,22);
        height: 191px;
        width: 100%;
        margin: 0 0 -6px;
        position: absolute;
        }
    #container{
        background-image: url("https://i.pinimg.com/originals/45/40/f5/4540f52c9f00e10ff5c89d9db04c4b82.gif");
        position: absolute;
        top: 35%;
        margin-left: auto;
        margin-right: auto;
        left: 0;
        right: 0;
        height: 51%;
        width: 76%;
        background-size: cover;
        border-style: ridge;
        border-color: transparent;
        border-width: 7px;
        border-radius: 3px;
        opacity: 95%;
        }
    #container #temp-container{
        position: absolute;
        top: 50%;
        left: 25%;
        margin: -25px 0 0 -78.975px;
        padding: 0 10px;
        background-color: #222;
        border-radius: 5%;
        opacity: 70%;
        width:63%;
        height:55px;
        padding:19px;
        }
    #container #salinity-container{
        position: absolute;
        top: 80%;
        left: 25%;
        margin: -25px 0 0 -78.975px;
        padding: 0 10px;
        background-color: #222;
        border-radius: 5%;
        opacity: 70%;
        height:90px;
        text-align: center;
        width:66%;
        padding:5px;
        }
    #container #light-container{
        position: absolute;
        top: 20%;
        left: 25%;
        margin: -25px 0 0 -78.975px;
        padding: 0 10px;
        background-color: #222;
        border-radius: 5%;
        opacity: 70%;
        width: 63%;
        height: 55px;
        padding: 19px;
    }
    #container #temp{
        position: absolute;
        top: 48%;
        left: 75%;
        margin: -11px 0 0 -74.5px;
        opacity: 70%;
        transform: scaleX(1.3) scaleY(1.3);
        }
    #container #salinity{
        position: absolute;
        top: 66%;
        left: 75%;
        margin: -11px 0 0 -74.5px;
        opacity: 70%;
        transform: scaleX(1.3) scaleY(1.3);
        }
    #container #temp-container #temp_out{
        }
    #container #salinity-container #salt_out{
        display:inline-flex;
        margin: 0 20px 0 10px;
        position:absolute;
        left:2%;
        top:35%;
        }
    #container #salinity-container #class{
        display:inline-flex;
        height:100%
        margin: 30px 0px;
        position:absolute;
        left:58%;
        top:20%;     
        }
    @media only screen and (max-width: 800px){
        img{margin:0 auto;left:9%;height:76%;}
        #container{top:26%;height:68%;width:83%;}
        #light-container{top:3%;left:31%;height:39px;padding:24px;width:85%;}
        #light{text-align:center;}
        button{top:61%;right:23%;height:35%;width:56%;}
        #temp-container{top:30%;left:31%;height:104px;display:inline;}
        #temp{top:60%;left:50%;}
        #temp_out{margin:0;position:absolute;top:14%;}
        #salinity-container{top:76%;left:31%;width:94%;}
        #salt_out{top:49%;}
        #class{left:6%;top:-7%;margin:17px 0px;}
    }
        </style>
    </head>
  <body>
        <div class="header">
            <img src="https://i.imgur.com/MqtiIO0.png">
        </div>
        <div id="container">
            <div id="light-container">
              <p id="light">Light is ...</p>
              <button id="toggle" type="button ">Toggle</button>
            </div>
            <div id="temp-container">
                <p id="temp_out">Current Temperature: ???</p>
            <input id="temp" placeholder="Set Temperature" max="96" onchange="temperature(1)" type="number">
            </div>
            <div id="salinity-container">
                <p id="salt_out">Current Salinity: ???</p>
                <p id="class">Water is ...</p>
            </div>
        </div>
  </body>
    <script>
        var temp;
        var temp_input;
        var salt;
        var toggle=0;
        document.getElementById("toggle").addEventListener("click", light);
        function light(){
          if(toggle==0){
            toggle=1;
            console.log("toggle = ", toggle);
            socket.send(toggle);
            socket.send(parseInt(toggle).toString(10));
          }
          else{
            toggle=0;
            console.log("toggle = ", toggle);
            socket.send(parseInt(toggle).toString(10));
          }
        }
        function temperature(pls){
        temp=document.getElementById("temp").value;
    if(temp > 96){
    console.log("nononononono");
    }
    else{
        console.log("temp = ", temp);
        temp_input = "t" + parseFloat(document.getElementById('temp').value).toString(10);
        console.log(temp_input);
        socket.send(temp_input);
        }
        }
        function salinity(pls){
        salt=document.getElementById("salinity").value;
        console.log("salt = ", salt);
        }
        console.log("ws://" + location.host + ":81");
        socket = new WebSocket("ws://" + location.host + ":81", ['arduino']);
        socket.onmessage = function(acid){
          //console.log("[socket]" + acid.data);
          //console.log(acid.data[0]);
          //console.log(acid.data.length);
          //console.log(typeof acid.data);
          if(acid.data[0] == "t"){
          const tempa = acid.data.substring(1);
          document.getElementById("temp_out").innerHTML = "Current Temperature: " + tempa;
          }
          else if(acid.data[0] == "s"){
          const salty = acid.data.substring(1);
          document.getElementById("salt_out").innerHTML = "Current Salinity: " + salty;
          if(salty > 2000){
          document.getElementById("class").innerHTML = "Water is: Demineralized";
          }
          else if(salty > 1000){
          document.getElementById("class").innerHTML = "Water is: Fresh/Tap water";
          }
          else if(salty > 190){
          document.getElementById("class").innerHTML = "Water is: Brackish";
          }
          else if(salty < 190){
          document.getElementById("class").innerHTML = "Water is: Salty";
          }
          }
          else if(acid.data[1] == "n"){
          document.getElementById("light").innerHTML = "Light is On";
          toggle=1;
          }
          else if(acid.data[1] == "f"){
          document.getElementById("light").innerHTML = "Light is Off";
          toggle=0;
          }
          }
    </script>
</html>
)=====";
