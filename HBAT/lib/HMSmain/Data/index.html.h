#pragma once
#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char *frontEnd = R"====(
<title>HBAT - Powering the future</title>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<style>
    html {
        background-color: #2c3133;

        margin: 0px;
        border: 0px;
        padding: 0px;
        height: 99%;


        background-image: url(data:image/jpeg;base64,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Cvv4zsCyV/NS6bXvnYNkNzzsbAZTw7/2NNBW9gq1Bb8ayiw40H9pZlVRnpCKM9upYJRZ9jvm6fb5GwpPbXLiBZ2ldT7eMbi40/8AhDFhyxuWWlP2f7mtC6sXEP3b9lUTP/2UBP8ANxGtPxufLLfmJUsHEaijYv4bSqUU5y5V7BW6b7kWwB4bCzTUxvsKVur+wy7eIxBW9/KUfku79z76M32ByhMgxYM9wwpoQi77i/ye3GxbVOr2UOAtfgYUg2HtwaKadqAUWX9I/9oADAMBAAIAAwAAABCmmk2m222m2mkm202k2m2k2k2k0kkkmkm0200kkm22km20200kk222kmk20m00mm0k20k00m022mmkmmm20020kkkk0m00m0mk2m22220k0km20mk02km00mk0222k0kk02k2m0km2mk2kmm0kk00mkmm00mm0mk220020m00km2mkmm0m0m00m2k020kk0mk0m22m0202mkkkk2k2k002200k2202kmk0mk22mm2m20k00mkmm0kmmkk0mmk20k0k0kmm02m0020220m20kk0kmm0kmmmkmkk222mkm02kkm21kmkkk20m2k0mm022k0mkkm0mk0kmm22m0k00kk0k20mk0020k22k0m2m0km220mm02m222mm2mk0k2mk20kk2m22m02222200m00k0k0k00m0k2k2020km20mm2mkk22m0mk0kk220k00222mm0k2mmk22220022kmm22mk2m200m000mm02mm20k0m022k00mmm2k20mk0mmkkkk3/xAAZEQACAwEAAAAAAAAAAAAAAABwcQAwYYD/2gAIAQMBAT8Q5wQh0poFbQogT//EABQRAQAAAAAAAAAAAAAAAAAAAKD/2gAIAQIBAT8QAB//xAAlEAEAAgEEAQQDAQEAAAAAAAABESExAEFRYXGBkbHwocHR4fH/2gAIAQEAAT8QkXS3GVgod3RI3LUQbs+ft6ilSXFFcOzGfsrZ9EomzPekaqtQRiJd6/PuwKcBCUGPR/ZeliUSgwdxmt46nMa2GSgcLXnH3MEQs4ACfW5k9s6E2VKhBhOefPuAishuHEt/O1aTAOAjMu3POgxqKQFMeu3tvpeUpCCN4ez4k0CYAKoVOJcI76nZGm2UoXvA6aOsUpEjfyxEfvQIwsjBVX961CXKYiS5T3eoQoCE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);
        background-repeat: repeat;
    }

    table,tr,td{
      text-align:center;
      vertical-align:middle;
    }

    iframe {
        height: 100%;
        max-width: 360px;
        width: 100%;
        padding: 0px;
        border: 1px;
        margin: 0 auto;
        border-color: hotpink;
        border-style: solid;


    }

    div {
        text-align: center;
        width: 100%;
    }
</style>





<head>
    <meta charset="UTF-8">
    <title>HBAT - Powering the future</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style>
        html {
            font-family: 'Roboto', monospace, monospace;
            overflow-x: hidden;
        }

        br {
            width: 100%;
        }

        input {
            width: 100%;
            height:15vw;
        }


        button{
            width:100%;
            height: 15vw;
            padding: 0vw;
            background: #009cb8;
            font-size: 10vw;
            text-align: center;
            vertical-align: middle;
            border-radius: 3vw;
            border-color: rgb(234, 255, 255);

            border-style: solid;
            border-width: 1vw;
            box-shadow: 1vw 1vw 1vw #001729;
        }


        .centeredDiv {
            text-align: center;
            justify-content: space-around;
            display: flex;
            flex-wrap: wrap;
            width: 100%;
        }

        .flex-container {
            margin: 0;
            display: flex;
            flex-wrap: wrap;
            justify-content: space-around;
            width: 100%;
            padding: 0vw;
            text-align: center;
        }

        .flex-item {
            width: 13vw;
            height: 13vw;
            padding: 0vw;
            background: #009cb8;
            font-size: 10vw;
            text-align: center;
            vertical-align: middle;
            border-radius: 3vw;
            border-color: rgb(234, 255, 255);

            border-style: solid;
            border-width: 1vw;
            box-shadow: 1vw 1vw 1vw #001729;
        }

        .infoPane {
            display: none;
        }

        .infoPaneShow {
            display: inherit;
            color: ivory;
            text-emphasis: #1F51FF;
            text-align: center;
        }

        @keyframes growProgressBar {

            0%,
            33% {
                --pgPercentage: 0;
            }

            100% {
                --pgPercentage: var(--value);
            }
        }

        @property --pgPercentage {
            syntax: '<number>';
            inherits: false;
            initial-value: 0;
        }

        h1,
        h2 {
            text-align: center;
            width: 100%;
        }

        div[role="progressbar"] {
            --size: 15rem;
            --fg: #ffe91f;
            --bg: rgb(28, 79, 129);
            --pgPercentage: var(--value);
            animation: growProgressBar 1s 1 forwards;
            width: var(--size);
            height: var(--size);
            border-radius: 50%;
            display: grid;
            place-items: center;
            background: radial-gradient(closest-side, #161616 75%, transparent 0 99.9%, rgb(56, 5, 5) 0), conic-gradient(var(--fg) calc(var(--pgPercentage) * 1%), var(--bg) 0);
            font-family: monospace, monospace;
            font-size: calc(var(--size) / 5);
            color: var(--fg);
            box-shadow: 4vw 4vw 4vw #2f4446;
            outline-color: rgb(4, 62, 87);
            outline-style: solid;
            outline-width: 2vw;

            text-align: center;
            justify-content: space-around;
            display: flex;
            flex-wrap: wrap;

        }

        div[role="progressbar"]::before {
            counter-reset: percentage var(--value);
            content: counter(percentage) var(--typeOfValue);
        }
    </style>

</head>

<body style="width:100%;padding:0px;margin:0px;">

    <div style="height:100%; width:360px;">


        <svg style="max-height: 15%; width:80%; display: block;  margin-left: auto;margin-right: auto;" onclick="location.reload();" id="Layer_1" x="0px" y="0px" viewBox="0 0 1275 577">
            <path fill="white" d="M238.6,174.7v111.3H66V174.7H0v285.6h66.5V342.5H239v117.8h66.4V174.7H238.6z" />
            <path fill="white" d="M353,460.3h198.4c61.2,0,99.1-29.9,99.1-80.3c0.1-15.1-4.6-29.8-13.4-42c-8.8-12.2-21.3-21.3-35.6-26
      c12-4.9,22.3-13.2,29.6-24c7.2-10.8,11.1-23.5,11-36.5c0-49.6-35.3-76.7-97.7-76.7H353V460.3z M416.7,289.3v-60h120.9
      c26.6,0,41.8,9.9,41.8,29.8c0,19.8-15.2,30.2-41.8,30.2H416.7z M416.7,341h129.2c25.5,0,40.5,13.1,40.5,32.2
      c0,19.1-15.1,32.2-40.5,32.2H416.7V341z" />
            <path fill="white" d="M1274.7,231.6v-56.8H960.3v56.8h124.1v228.7h66.4V231.6H1274.7z" />
            <path fill="white" d="M881.6,174.7h-75.1L663.7,460.2h70.8l30.2-61.2l27.2-54.9h-0.1l19.4-39V305l32.3-65l40.7,82.8l10.5,21.3l56.9,116.1h71.9
      L881.6,174.7z" />
            <path fill="white" d="M844.2,576c16,0,28.9-13,28.9-28.9c0-16-13-28.9-28.9-28.9c-16,0-28.9,13-28.9,28.9C815.2,563.1,828.2,576,844.2,576z" />
            <path fill="white" d="M844.2,57.9c16,0,28.9-13,28.9-28.9S860.1,0,844.2,0c-16,0-28.9,13-28.9,28.9S828.2,57.9,844.2,57.9z" />
        </svg>


        <br>


        <div class="flex-container">
            <div class="flex-item" onclick="showDiv(this.id)" id="????">????</div>
            <div class="flex-item" onclick="showDiv(this.id)" id="????">????</div>
            <div class="flex-item" onclick="showDiv(this.id)" id="????">????</div>
            <div class="flex-item" onclick="showDiv(this.id)" id="???">???</div>
            <div class="flex-item" onclick="showDiv(this.id)" id="???">???</div>
        </div>
        <br>


        <div id="????widget" class="infoPane">
            <h2>Charge</h2><br>
            <div class="flex-container">
                <div id="batteryLevel" role="progressbar" aria-valuenow="65" aria-valuemin="0" aria-valuemax="100" style="--value:10;--typeOfValue:'%';"></div>
            </div>
        </div>

        <div id="????widget" class="infoPane">
            <div class="centeredDiv" style="width:100%;">
                <h2>Temp</h2><br>
                <div class="centeredDiv" style="width:100%;">
                    <div id="batteryTemp" role="progressbar" aria-valuemin="0" aria-valuemax="100" style="--value:10;--typeOfValue:'???';"></div>
                </div>

                <br>

                <h2>Humidity</h2><br>
                <div class="centeredDiv" style="width:100%;">
                    <div id="batteryHumidity" role="progressbar" aria-valuemin="0" aria-valuemax="100" style="--value:10;--typeOfValue:'%????';--fg: #0de1f0;"></div>
                </div>
            </div>
        </div>

        <div id="????widget" class="infoPane" style="text-align: left;">
            <h2>Advanced Stats</h2>
            <div id="graphList"></div>
        </div>

        <div id="???widget" class="infoPane" style="text-align: left;">
            <h2>Settings</h2>
            <label>AP Name</label>
            <input id="apName"><br>
            <label>AP Password</label>
            <input id="apPass"><br><hr>
            <button onclick="saveSettings()">Save</button>
            <br>
            <table style="color: ivory;">
              <tr>
                <td>Relay 0</td>
                <td id="relay0" onclick="toggleRelay(this);"></td>
              </tr>
              <tr>
                <td>Relay 1</td>
                <td id="relay1" onclick="toggleRelay(this);"></td>
              </tr>
              <tr>
                <td>Relay 2</td>
                <td id="relay2" onclick="toggleRelay(this);"></td>
              </tr>
              <tr>
                <td>Relay 3</td>
                <td id="relay3" onclick="toggleRelay(this);"></td>
              </tr>
              <tr>
                <td>Relay 4</td>
                <td id="relay4" onclick="toggleRelay(this);"></td>
              </tr>

            </table>            
        </div>



        <div id="???widget" class="infoPane">
            <div style="text-align: left;margin: 3vw;">
                ??? Help

                This is the monitoring app for your HBAT device. <br>
                From here you can see your: <br>
                1.) Stack Voltage<br>
                2.) Stack humidity and Temperature<br>
                3.) Per Cell Voltage<br>
                4.) Stack Current<br>
            </div>
        </div>
    </div>

</body>

<script>
var graphData = {};

const toggleOnString = `<svg version="1.1" x="0px" y="0px" viewBox="0 0 330 330" style="width:50px;height:auto;">
<path stroke="gray" stroke-width="1" fill="blue" d="M 240 0 H 90 c -49.626 0 -90 40.374 -90 90 s 40.374 90 90 90 h 150 c 49.626 0 90 -40.374 90 -90 S 289.626 0 240 0 z M 240 150 c -33.084 0 -60 -26.916 -60 -60 s 26.916 -60 60 -60 s 60 26.916 60 60 S 273.084 150 240 150 z"/>
</svg>`;


const toggleOffString = `<svg version="1.1" x="0px" y="0px" viewBox="0 0 330 330" style="width:50px;height:auto;transform: scale(-1,1)">
<path stroke="gray" stroke-width="1" fill="red" d="M 240 0 H 90 c -49.626 0 -90 40.374 -90 90 s 40.374 90 90 90 h 150 c 49.626 0 90 -40.374 90 -90 S 289.626 0 240 0 z M 240 150 c -33.084 0 -60 -26.916 -60 -60 s 26.916 -60 60 -60 s 60 26.916 60 60 S 273.084 150 240 150 z"/>
</svg>`;

    

    async function buildGraph(inputValues) {
        graphList = document.getElementById("graphList");
        graphList.innerHTML = "";
        for (let i = 0; i < inputValues.length; i++) {

            newGraphElement = document.createElement("div");
            newGraphElement.append(inputValues[i].label);
            newGraphElement.style.textAlign = "left";

            newMeeterElement = document.createElement("DIV");
            newMeeterElement.min = inputValues[i].minValue !== undefined ? inputValues[i].minValue : 0;
            newMeeterElement.max = inputValues[i].maxValue !== undefined ? inputValues[i].maxValue : 100;
            newMeeterElement.value = inputValues[i].value !== undefined ? inputValues[i].value : 0;
            newMeeterElement.innerHTML = inputValues[i].value;
            newMeeterElement.style.width = (100 * newMeeterElement.value) / newMeeterElement.max + "%";

            if (inputValues[i].type == "temp") newMeeterElement.style.background = "red";
            if (inputValues[i].type == "humidity") newMeeterElement.style.background = "blue";
            if (inputValues[i].type == "volt") newMeeterElement.style.background = "yellow";

            newGraphElement.appendChild(newMeeterElement);
            graphList.appendChild(newGraphElement);
            //graphList.innerHTML += "<br>";
        }
    }




    x = 1;
    async function grabData() {
        if (x >= 10) { x = 1 } else { x++ };
        graphData = await JSON.parse(await getHTML("./data.json"));
        setMeeterValue("batteryLevel", graphData.stack_voltage, "%", "Yellow");
        setMeeterValue("batteryTemp", graphData.stack_temp, "???", "red");
        setMeeterValue("batteryHumidity", graphData.stack_humidity, "%????", "cyan");

        updateToggls(graphData.relays);
     
        buildGraph(graphData.fakeGraphData);
    }

    window.setInterval(grabData, 5000);

    async function updateToggls(togglesToUpdate){
      for (let i = 0; i < togglesToUpdate.length; i++) {
        (await document.getElementById("relay"+i)).innerHTML = ((togglesToUpdate[i] == 1) ? toggleOnString:toggleOffString);
      }
    }


    async function saveSettings(){
      await getHTML("./wifiUpdate?apName="+document.getElementById("apName").value+ "&apPass="+ document.getElementById("apPass").value,false );
    }
    

    function setMeeterValue(itemId, itemValue, itemTypeString, colorForMeeter) {
        document.getElementById(itemId).style.cssText = `--value:` + itemValue + `;--typeOfValue:'` + itemTypeString + `';--fg: ` + colorForMeeter + `;`;
    }


  async function toggleRelay(relayToToggle){
    var relayNumberToToggle =relayToToggle.id.replace("relay","");
    await getHTML("./toggle?pin="+relayNumberToToggle);
    grabData();
  }

    //Navigation menu
    function showDiv(inputValue) {
        setCSSclass("????widget", "infoPane")
        setCSSclass("????widget", "infoPane")
        setCSSclass("????widget", "infoPane")
        setCSSclass("???widget", "infoPane")
        setCSSclass("???widget", "infoPane")

        setCSSclass(inputValue + "widget", "infoPaneShow")
    }

    function setCSSclass(inputID, newClass) {
        document.getElementById(inputID).setAttribute("class", newClass)
    }


    async function getHTML(url,sendTime = true) {
        try {
            let response = {};
            if (sendTime==true){
              response = await fetch(url + "?" + (new Date().getTime() / 1000));
            }else{
              response = await fetch(url);
            }
            
            return await response.text();

        } catch (err) {
            console.error(err);
            // Handle errors here
        }
    }

    grabData();
</script>









<script>
    if (window.location.search !== "?frame") {
        stringToReplaceContentsWith = `<div>
            <iframe id="myIframe" src="./?frame" frameBorder="0"></iframe>
        </div>`
        document.getElementsByTagName('body')[0].innerHTML = stringToReplaceContentsWith;
    }
</script>



)====";

#endif
