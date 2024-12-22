#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

TFT_eSPI tft = TFT_eSPI();

#define XPT2046_IRQ 34   
#define XPT2046_MOSI 32  
#define XPT2046_MISO 35  
#define XPT2046_CLK 25   
#define XPT2046_CS 33    

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

const char* ssid = "Vanilla";
const char* password = "gilbertstrang";

AsyncWebServer server(80);

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 2

int button1_x = 10, button1_y = 10, button1_w = 300, button1_h = 66;
int button2_x = 10, button2_y = 86, button2_w = 300, button2_h = 66;
int button3_x = 10, button3_y = 162, button3_w = 300, button3_h = 66;

String button1_label = "Button 1";
String button2_label = "Button 2";
String button3_label = "Button 3";

String selectedAnswer = "";

unsigned long previousMillis = 0;
const long interval = 50000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(1);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);

  drawButton(button1_x, button1_y, button1_w, button1_h, button1_label, TFT_BLUE);
  drawButton(button2_x, button2_y, button2_w, button2_h, button2_label, TFT_GREEN);
  drawButton(button3_x, button3_y, button3_w, button3_h, button3_label, TFT_YELLOW);

  server.on("/update_labels", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  server.on("/update_labels", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("animal1", true) && request->hasParam("animal2", true) && request->hasParam("animal3", true)) {
        String animal1 = request->getParam("animal1", true)->value();
        String animal2 = request->getParam("animal2", true)->value();
        String animal3 = request->getParam("animal3", true)->value();

        button1_label = animal1;
        button2_label = animal2;
        button3_label = animal3;

        tft.fillScreen(TFT_WHITE);  
        drawButton(button1_x, button1_y, button1_w, button1_h, button1_label, TFT_GREEN);
        drawButton(button2_x, button2_y, button2_w, button2_h, button2_label, TFT_YELLOW);
        drawButton(button3_x, button3_y, button3_w, button3_h, button3_label, TFT_BLUE);
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Labels updated successfully");
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type");
        request->send(response);
    } else {
        AsyncWebServerResponse *response = request->beginResponse(400, "text/plain", "Missing parameters");
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type");
        request->send(response);
    }
  });

  server.on("/get_answer", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (selectedAnswer != "") {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", selectedAnswer);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    } else {
        request->send(200, "text/plain", "no");
    }
  });

  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    tft.fillScreen(TFT_WHITE);
    drawButton(button1_x, button1_y, button1_w, button1_h, button1_label, TFT_BLUE);
    drawButton(button2_x, button2_y, button2_w, button2_h, button2_label, TFT_GREEN);
    drawButton(button3_x, button3_y, button3_w, button3_h, button3_label, TFT_YELLOW);
  }

  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    int touchX = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    int touchY = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    
    Serial.print("Touch X = ");
    Serial.print(touchX);
    Serial.print(" | Touch Y = ");
    Serial.println(touchY);

    if (touchX > button1_x && touchX < button1_x + button1_w && touchY > button1_y && touchY < button1_y + button1_h) {
      Serial.println("Button 1 Pressed!");
      selectedAnswer = button1_label;  
      displayMessage("Thank You for pressing " + button1_label + "!");
    } 
    else if (touchX > button2_x && touchX < button2_x + button2_w && touchY > button2_y && touchY < button2_y + button2_h) {
      Serial.println("Button 2 Pressed!");
      selectedAnswer = button2_label;  
      displayMessage("Thank You for pressing " + button2_label + "!");
    } 
    else if (touchX > button3_x && touchX < button3_x + button3_w && touchY > button3_y && touchY < button3_y + button3_h) {
      Serial.println("Button 3 Pressed!");
      selectedAnswer = button3_label;  
      displayMessage("Thank You for pressing " + button3_label + "!");
    }
  }
}

void drawButton(int x, int y, int w, int h, String label, uint32_t color) {
  tft.fillRect(x, y, w, h, color);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(FONT_SIZE);

  int textWidth = label.length() * 6 * FONT_SIZE;
  int textX = x + (w - textWidth) / 2;
  int textY = y + (h - 16) / 2;
  
  tft.setCursor(textX, textY);
  tft.print(label);
}

void displayMessage(String message) {
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1); 
  tft.setCursor(20, SCREEN_HEIGHT / 2 - 10); 
  
  tft.print(message); 
  
  delay(2000); 
  tft.fillScreen(TFT_WHITE); 
  drawButton(button1_x, button1_y, button1_w, button1_h, button1_label, TFT_BLUE);
  drawButton(button2_x, button2_y, button2_w, button2_h, button2_label, TFT_GREEN);
  drawButton(button3_x, button3_y, button3_w, button3_h, button3_label, TFT_YELLOW);
}
