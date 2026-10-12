#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SpotifyEsp32.h>
#include <Preferences.h>
#include "FreeSansBold9pt7b.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//These are only temp. placeholders, it will be outsourced to a file mentiond in .gitignore lateron...
//@Some random Programmer who will read / edit this: If you edit this code: DO NOT COMMIT WIFI PASSWORDS AND SPOTIFY CLIENT SECRETS!!!

const char* SSID = "your_ssid";
const char* PASSWORD = "your_password";
const char* CLIENT_ID = "your_client_id";
const char* CLIENT_SECRET = "your_client_secret";

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create an instance of the Spotify class (optional: specify retry count)
Spotify *sp = nullptr;

#define DISABLE_LIBRARY         //Saved items & followed artists
#define DISABLE_PLAYLISTS       //Playlist management
#define DISABLE_METADATA        //Albums, artists, search, shows, tracks
#define DISABLE_PLAYER          //Playback control
#define DISABLE_USER            //Profile & top items
#define DISABLE_SIMPLIFIED      //Helper convenience functions

Preferences prefs;

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// Declarations...
void connect_to_wifi();
void save_sp_refresh_token();
String load_sp_refresh_token();
bool hasRefreshToken();
void drawPlayback(String title, int songProgress, int playstate);

void setup() {
    Serial.begin(115200);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    } else {
        // display.clearDisplay();
        // display.setCursor(0, 20);
        // display.setTextColor(WHITE);
        // display.print("Hello World");
        // display.display();
    }

    drawPlayback("FCK AFD", 50, 1);

    // connect_to_wifi(); // Skipping till this shit is finished

    if (hasRefreshToken()) {
        String token = load_sp_refresh_token();
        sp = new Spotify(CLIENT_ID, CLIENT_SECRET, token.c_str());
    } else {
        sp = new Spotify(CLIENT_ID, CLIENT_SECRET);
        sp->begin();
        while (!sp->is_auth()) {
            sp->handle_client();
        }
    }


    sp->begin();
    while (!sp->is_auth()) {
        sp->handle_client(); // Required for receiving the authorization code
    }

    save_sp_refresh_token();
    Serial.printf("Authenticated! Refresh token: %s\n", sp->get_user_tokens().refresh_token);
}


void loop() {
    // drawPlayback("FCK AFD", 50, 1);
}

void connect_to_wifi() {
    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
}

void save_sp_refresh_token() {
    prefs.begin("spotify", false);
    prefs.putString("refresh_token", sp->get_user_tokens().refresh_token);
    prefs.end();
}

String load_sp_refresh_token() {
    prefs.begin("spotify", true);
    String token = prefs.getString("refresh_token", "");
    prefs.end();
    return token;
}

bool hasRefreshToken() {
    return load_sp_refresh_token().length() > 0;
}

// Going to build all the graphic shit here...

void drawPlayback(String title, int songProgress, int playstate) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(7, 14);
    display.print(title);
    // display.startscrolldiagright(6, 121);
    display.fillCircle(63, 48, 8, 1);
    if (playstate == 1) {
        display.fillRect(110, 18, 10, 10, BLACK);
    } else {
        display.fillTriangle(110, 18, 110, 28, 120, 23, BLACK);
    }

    display.drawRoundRect(13, 59, 100, 5, 5, WHITE);
    display.fillRoundRect(13, 59, songProgress, 5, 5, WHITE);

    display.display();

}