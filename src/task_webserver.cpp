/** @file task_webserver.h
 * task for running a web server to demonstrate IoT functionality
 * 
 * @author Jackson Cordova
 * @date December 2025
 */

#include <Arduino.h>
#include "task_webserver.h"
#include <WebServer.h>
#include "PrintStream.h"
#include <ArduinoJson.h>
#include <task_read_camera.h>
#include "shares.h"


const int THERM_W = 32;
const int THERM_H =24;


const char* ssid = "EMBER";
const char* password = "_EMBER_3374";

IPAddress local_ip (192, 168, 5, 1); // Address of ESP32 on its own network
IPAddress gateway (192, 168, 5, 1);  // The ESP32 acts as its own gateway
IPAddress subnet (255, 255, 255, 0); // Network mask; just leave this as is

/** @brief   The web server object for this project.
 *  @details This server is responsible for responding to HTTP requests from
 *           other computers, replying with useful information
*/
WebServer server(80);

/** @brief Function which sets up the WiFi access point
 * 
*/
void setup_wifi(void) {
    Serial << "Setting up WiFi access point...";
    WiFi.mode (WIFI_AP);
    WiFi.softAPConfig (local_ip, gateway, subnet);
    WiFi.softAP (ssid, password);
    Serial << "done." << endl;
}




/** @brief   Put a web page header into an HTML string. 
 *  @details This header may be modified if the developer wants some actual
 *           @a style for her or his web page. It is intended to be a common
 *           header (and stylle) for each of the pages served by this server.
 *  @param   a_string A reference to a string to which the header is added; the
 *           string must have been created in each function that calls this one
 *  @param   page_title The title of the page
*/
void HTML_header (String& a_string, const char* page_title)
{
    a_string += "<!DOCTYPE html> <html>\n";
    a_string += "<head><meta name=\"viewport\" content=\"width=device-width,";
    a_string += " initial-scale=1.0, user-scalable=no\">\n<title> ";
    a_string += page_title;
    a_string += "</title>\n";
    a_string += "<style>html { font-family: Helvetica; display: inline-block;";
    a_string += " margin: 0px auto; text-align: center;}\n";
    a_string += "body{margin-top: 50px;} h1 {color: #db7100ff;margin: 50px auto 30px;}\n";
    a_string += "p {font-size: 24px;color: #222222;margin-bottom: 10px;}\n";
    a_string += "</style>\n</head>\n";
}

/** @brief   Callback function that responds to HTTP requests without a subpage
 *           name.
 *  @details When another computer contacts this ESP32 through TCP/IP port 80
 *           (the insecure Web port) with a request for the main web page, this
 *           callback function is run. It sends the main web page's text to the
 *           requesting machine.
 */
void handle_DocumentRoot (void)
{
    Serial << "HTTP request from client #" << server.client () << endl;
        server.send(200, "text/html", R"rawliteral(
            <!DOCTYPE html>
            <html>
            <head>
            <meta charset="utf-8">
            <title>EMBER Thermal Viewer</title>
            <style>
                body { font-family: sans-serif; background: #111; color: #eee; text-align:center; padding: 20px; }
                #container { display: flex; justify-content: center; align-items: center; gap: 30px; margin-top: 30px; }
                #leftPanel { display: flex; flex-direction: column; align-items: center; }
                canvas { image-rendering: pixelated; border: 2px solid #555; }
                #rightPanel { display: flex; gap: 10px; align-items: center; }
                #scaleBar { 
                    width: 40px; 
                    height: 320px; 
                    border: 2px solid #aaa; 
                    background: linear-gradient(to top, rgb(0,0,255), rgb(0,255,0), rgb(255,255,0), rgb(255,128,0), rgb(255,0,0));
                }
                #scaleLabels { 
                    display: flex; 
                    flex-direction: column; 
                    justify-content: space-between; 
                    height: 320px; 
                    text-align: left;
                    font-size: 14px;
                    min-width: 70px;
                }
                .scaleLabel { font-weight: bold; }
                #tempDisplay { font-size: 18px; margin-top: 20px; font-weight: bold; }
                
                /* Fire alert styles */
                @keyframes flashOrange {
                    0% { background: #111; }
                    50% { background: #ff8800; }
                    100% { background: #111; }
                }
                body.fire-alert {
                    animation: flashOrange 0.6s infinite;
                }
                #fireWarning {
                    display: none;
                    position: fixed;
                    top: 50%;
                    left: 50%;
                    transform: translate(-50%, -50%);
                    font-size: 48px;
                    font-weight: bold;
                    color: #ff0000;
                    text-shadow: 3px 3px 6px rgba(0,0,0,0.8);
                    background: rgba(255, 136, 0, 0.9);
                    padding: 30px 60px;
                    border-radius: 10px;
                    z-index: 1000;
                    animation: fireFlash 0.6s infinite;
                }
                @keyframes fireFlash {
                    0% { opacity: 1; }
                    50% { opacity: 0.6; }
                    100% { opacity: 1; }
                }
                body.fire-alert #fireWarning {
                    display: block;
                }
            </style>
            </head>
            <body>
            <div id="fireWarning">WARNING FIRE DETECTED</div>
            <h1>Thermal Camera</h1>
            <div id="container">
                <div id="leftPanel">
                    <canvas id="thermCanvas" width="240" height="320"></canvas>
                </div>
                <div id="rightPanel">
                    <div id="scaleBar"></div>
                    <div id="scaleLabels">
                        <span class="scaleLabel" id="maxLabel">Max: --°C</span>
                        <span class="scaleLabel" id="midLabel">Mid: --°C</span>
                        <span class="scaleLabel" id="minLabel">Min: --°C</span>
                    </div>
                </div>
            </div>
            <div id="tempDisplay">Temperature Range: -- °C to -- °C</div>
            <script>
                const canvas = document.getElementById('thermCanvas');
                const ctx = canvas.getContext('2d');
                const maxLabel = document.getElementById('maxLabel');
                const midLabel = document.getElementById('midLabel');
                const minLabel = document.getElementById('minLabel');
                const tempDisplay = document.getElementById('tempDisplay');

                let srcW = 0;
                let srcH = 0;

                async function fetchFrame() {
                try {
                    const res = await fetch('/thermal');
                    const json = await res.json();
                    const w = json.w;
                    const h = json.h;
                    const data = json.data;
                    srcW = w;
                    srcH = h;

                    // Compute min/max (for auto scaling)
                    let min = Infinity, max = -Infinity;
                    for (let i = 0; i < data.length; i++) {
                    if (data[i] < min) min = data[i];
                    if (data[i] > max) max = data[i];
                    }
                    // Avoid division by zero
                    if (max === min) { max = min + 0.0001; }

                    // Update scale labels
                    const mid = (min + max) / 2;
                    maxLabel.textContent = 'Max: ' + max.toFixed(1) + '°C';
                    midLabel.textContent = 'Mid: ' + mid.toFixed(1) + '°C';
                    minLabel.textContent = 'Min: ' + min.toFixed(1) + '°C';
                    tempDisplay.textContent = `Temperature Range: ${min.toFixed(1)} °C to ${max.toFixed(1)} °C`;

                    const img = ctx.createImageData(w, h);
                    for (let i = 0; i < data.length; i++) {
                    const t = data[i];
                    const norm = (t - min) / (max - min); // 0..1
                    const idx = i * 4;

                    // Simple blue→red gradient
                    const r = Math.floor(255 * norm);
                    const g = Math.floor(255 * Math.max(0, 1 - 2 * Math.abs(norm - 0.5)));
                    const b = Math.floor(255 * (1 - norm));

                    img.data[idx + 0] = r;
                    img.data[idx + 1] = g;
                    img.data[idx + 2] = b;
                    img.data[idx + 3] = 255; // alpha
                    }

                    // Draw low-res image to an offscreen canvas and scale up
                    const off = document.createElement('canvas');
                    off.width = w;
                    off.height = h;
                    const offCtx = off.getContext('2d');
                    offCtx.putImageData(img, 0, 0);

                    // Draw marker for hot pixel (if provided)
                    const hot = json.hot;
                    if (typeof hot !== 'undefined' && hot !== null && hot >= 0) {
                        const hx = hot % w;
                        const hy = Math.floor(hot / w);
                        // Draw a small crosshair in white on the offscreen (low-res) canvas
                        offCtx.strokeStyle = 'white';
                        offCtx.lineWidth = 1;
                        offCtx.beginPath();
                        offCtx.moveTo(hx - 1 + 0.5, hy + 0.5);
                        offCtx.lineTo(hx + 2 + 0.5, hy + 0.5);
                        offCtx.moveTo(hx + 0.5, hy - 1 + 0.5);
                        offCtx.lineTo(hx + 0.5, hy + 2 + 0.5);
                        offCtx.stroke();
                    }

                    // Clear and draw scaled with 90° CCW rotation so the long axis is vertical
                    ctx.clearRect(0, 0, canvas.width, canvas.height);
                    ctx.imageSmoothingEnabled = false;
                    ctx.save();
                    // Move origin to bottom-left and rotate -90deg (CCW)
                    ctx.translate(0, canvas.height);
                    ctx.rotate(-Math.PI / 2);
                    // Draw offscreen image scaled; width/height swapped due to rotation
                    ctx.drawImage(off, 0, 0, canvas.height, canvas.width);
                    ctx.restore();

                } catch (e) {
                    console.error(e);
                } finally {
                    // Get next frame
                    setTimeout(fetchFrame, 100); // 10 FPS
                }
                }

                fetchFrame();
            </script>
            </body>
            </html>
            )rawliteral");
        
}


/** @brief   Respond to a request for an HTTP page that doesn't exist.
 *  @details This function produces the Error 404, Page Not Found error. 
 */
void handle_NotFound (void)
{
    server.send (404, "text/plain", "Not found");
}

/** @brief Function to serve thermal frame as JSON for web UI
 */
void handle_Thermal(void) {
  // Build JSON
  DynamicJsonDocument doc(16384); // size depends on resolution; adjust as needed
  doc["w"] = THERM_W;
  doc["h"] = THERM_H;

  JsonArray arr = doc.createNestedArray("data");
  for (int i = 0; i < THERM_W * THERM_H; i++) {
    arr.add(Frame[i]);
  }

    // Include the current hot pixel index so web UI can highlight it
    doc["hot"] = hotIndex.get();

  String out;
  serializeJson(doc, out);

  server.send(200, "application/json", out);
}

// Serve fire state as JSON
/** @brief Function to serve fire state as JSON for web UI
 * 
 */
void handle_Fire() {
  DynamicJsonDocument doc(128);
  doc["fire"] = fire.get();

  String out;
  serializeJson(doc, out);

  server.send(200, "application/json", out);
}


/** @brief task which sets up and runs a web server
 *  @details function @c handleClient() must be called periodically to 
 *           process incoming HTTP requests, though the priority is low
 * 
 * @param p_params pointer to paramters (not used, should be set to void)
 */
void task_webserver (void* p_params)
{
    // The server has been created statically when the program was started and
    // is accessed as a global object because not only this function but also
    // the page handling functions referenced below need access to the server
    server.on ("/", handle_DocumentRoot);
    // server.on ("/toggle", handle_Toggle_LED);
    server.on ("/thermal", handle_Thermal);
    server.on ("/fire", handle_Fire);
    server.onNotFound (handle_NotFound);

    // Get the web server running
    server.begin ();
    Serial.println ("HTTP server started");
    server.sendHeader("refresh", "10");
    for (;;)
    {
        // The web server must be periodically run to watch for page requests
        server.handleClient ();
        vTaskDelay (100);
    }
}