#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>


#define OLED_RESET -1  // Share Reset pin with Arduino

#define BUTTON_A 9
#define BUTTON_B 8
#define BUTTON_C 7
#define SOUND_PIN 6

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

struct GameState {
  int version; // Add a version number to the game state
 int tamagotchiPosX;
  int tamagotchiPosY;
  int hunger;
  int happy;
  int health;
  int sleep;
  int discipline;
  int poops;
  // Constructor to initialize default values
  GameState() : version(1), tamagotchiPosX(64), tamagotchiPosY(32),
    hunger(50), happy(50), sleep(50),health(100),discipline(50), poops(0) {}
};

void saveGameState(const GameState& state) {
  EEPROM.put(0, state); // Write the state to EEPROM starting at address 0
}

void loadGameState(GameState& state) {
  EEPROM.get(0, state); // Read the state from EEPROM starting at address 0
  if (state.version != 1) { 
    state = GameState(); // Reset to default values
  }
}

int moveDirectionX = 1;         // Movement direction: 1 for right, -1 for left
int moveDirectionY = 1;         // Movement direction: 1 for down, -1 for up
const int moveSpeed = 1;        // Adjust for faster or slower movement
const int menuLineHeight = 16;  // Height of the menu line area

GameState state;
bool menuOpened = false;
bool displayingStats = false;
int menuSel = 0;
unsigned long lastSaveTime = 0; // Tracks the last save time


void playButtonSound() {
  tone(SOUND_PIN, 1000, 100);
  delay(150);
  noTone(SOUND_PIN);
}

void capValue(int &value, int increment, int min = 0, int max = 100) {
  value += increment;
  if (value > max) {
    value = max;
  } else if (value < min) {
    value = min;
  }
}

void playEatingAnimation() {
  const int animationDelay = 500;
  
  // Frame 1: Looking at food
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( o.o ) >1");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 2: Eating food
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  >[ ]<  ");
  display.display();
  delay(animationDelay);
  
  // Frame 3: Happy after eating
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( ^_^ ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
    // Serial.println("Eating animation played.");

}

void playPlayingAnimation() {
  const int animationDelay = 500;

  // Frame 1: Looking at the ball
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  >o");
  display.display();
  delay(animationDelay);
  
  // Frame 2: Hitting the ball
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( >.< ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ < o ");
  display.display();
  delay(animationDelay);
  
  // Frame 3: Happy
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( ^_^ ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  o");
  display.display();
  delay(animationDelay);
    // Serial.println("Play animation played.");

}

void playHealingAnimation() {
  const int animationDelay = 500; 

  // Frame 1: Sad or sick expression
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( >.< ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 2:
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" (-.-) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 3: Happy and healed
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( ^_^ ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
    // Serial.println("Healing animation played.");

}

void playSleepingAnimation() {
  const int animationDelay = 500;

  // Frame 1: Getting ready to state.sleep
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( -.- ) zZ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 2: Sleeping soundly
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( -.- ) zZz");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(2000); // Hold this frame a bit longer
  
  // Frame 3: Waking up refreshed
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( ^_^ ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
    // Serial.println("Sleep animation played.");

}

void playDisciplineAnimation() {
  const int animationDelay = 500; 

  // Frame 1: Neutral
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 2: Sad but attentive
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( o_o ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 3: Sad
  display.clearDisplay();
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( >_< ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);

  // Serial.println("Discipline animation played.");
}

void flushPoops() {
    int menuLineHeight = 16; // Height of the menu line at the top of the screen
    int waterHeight = display.height() - menuLineHeight; // Adjust water flow height to start below the menu bar
    int waterSpeed = 10; // Speed of the water flow animation
    
    // Starting from the left side of the screen, draw water rectangles that increase in width
    for (int waterWidth = 0; waterWidth <= display.width(); waterWidth += waterSpeed) {
        // This keeps the menu bar intact
        display.fillRect(0, menuLineHeight, display.width(), waterHeight, SSD1306_BLACK);

        // Draw the water rectangle below the menu bar
        display.fillRect(0, menuLineHeight, waterWidth, waterHeight, SSD1306_WHITE);

        // Display the updated frame
        display.display();
        delay(100); // Delay to control the speed of the animation
    }

    // After the water has covered the area below the menu bar, clear any remaining visuals in that area
    display.fillRect(0, menuLineHeight, display.width(), waterHeight, SSD1306_BLACK);
    display.display();

    // Reset the poop count after flushing
    state.poops = 0;
    // Serial.println("Poops Cleaned. Poops = " + state.poops);
}

void displayPoops() {
    // Assuming each poop takes up about 10 pixels horizontally and 16 pixels vertically
    int startingPoopPosX = 0; // Starting X position for the first poop
    int poopPosY = display.height() - 16; // Position Y for state.poops at the bottom of the screen
    int poopSpacing = 12; // Space between state.poops
    
    for (int i = 0; i < state.poops; i++) {
        int poopPosX = startingPoopPosX + (i * poopSpacing); // Calculate X position for each poop

        // Ensure we don't draw outside the display width
        if (poopPosX + 10 > display.width()) {
            break; // Stop drawing more state.poops if we run out of space
        }

        // Draw poop at calculated position
        display.setCursor(poopPosX, poopPosY);
        display.println("  ~-~");
        display.setCursor(poopPosX, poopPosY + 8);
        display.println(" (   )");
    }
   
}



void displayMenuLine(int menuSel) {
  display.setCursor(0, 0);
  display.clearDisplay();
  // Highlight the selected menu item
  for (int i = 0; i < 7; i++) {
    if (i == menuSel) {
      display.print("|");
    }
    switch (i) {
      case 0: display.print("F"); break;  // Feed
      case 1: display.print("P"); break;  // Play
      case 2: display.print("H"); break;  // Heal
      case 3: display.print("S"); break;  // Sleep
      case 4: display.print("D"); break;  // Discipline
      case 5: display.print("C"); break;  // Clean
      case 6: display.print("Z"); break;  // Stats
    }
    if (i == menuSel) {
      display.print("| ");
    } else {
      display.print("  ");
    }
  }
  display.println();

}

void displayTamagotchi() {
  // Update horizontal position
  state.tamagotchiPosX += moveSpeed * moveDirectionX;
  // Reverse direction if hitting the horizontal display boundaries
  if (state.tamagotchiPosX <= 0 || state.tamagotchiPosX >= display.width() - 64) {
    moveDirectionX *= -1;
  }

  // Update vertical position
  state.tamagotchiPosY += moveSpeed * moveDirectionY;
  // Reverse direction if hitting the vertical display boundaries, taking into account the menu line height
  if (state.tamagotchiPosY <= menuLineHeight || state.tamagotchiPosY >= display.height() - 32) {
    moveDirectionY *= -1;
  }

  // Ensure the position stays within the display boundaries, accounting for the menu line height
  state.tamagotchiPosX = max(0, min(state.tamagotchiPosX, display.width() - 64));
  state.tamagotchiPosY = max(menuLineHeight, min(state.tamagotchiPosY, display.height() - 32));

  // Set cursor based on current position and draw the Tamagotchi
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(state.tamagotchiPosX, state.tamagotchiPosY + 20);
  display.println("  > ^ <  ");
}

void displayStats() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Stats:");
  display.println("");
  display.print("Hunger: ");
  display.println(state.hunger);
  display.print("Happy: ");
  display.println(state.happy);
  display.print("Health: ");
  display.println(state.health);
  display.print("Sleep: ");
  display.println(state.sleep);
  display.print("Discipline: ");
  display.println(state.discipline);
  display.print("Poops: ");
  display.println(state.poops);
}

void updateStats() {

  static unsigned long lastUpdateTime = 0; // Last update time
  unsigned long currentTime = millis(); // Current time in milliseconds
  
  // Update stats every 10 seconds instead of every loop.
  if (currentTime - lastUpdateTime > 10000) { // 10,000 milliseconds = 10 seconds
    lastUpdateTime = currentTime; // Reset the last update time
    
    // Now apply the slower stat decreases
    capValue(state.hunger, -random(0, 2)); // Decrease state.hunger by 0 or 1 point
    if (state.hunger < 30 || state.health < 30) {
      capValue(state.happy, -1); // Decrease happiness by 1 point if state.hunger or state.health is low
    }

    if (state.hunger < 30 || state.sleep < 30) {
      capValue(state.health, -random(0, 2)); // Decrease state.health by 0 or 1 point if state.hunger or state.sleep is low
    }

    capValue(state.sleep, -1); // Decrease state.sleep by 1 point
    capValue(state.discipline, -random(0, 1)); // Decrease state.discipline by 0 or 1 point
    
    if (random(0, 10) < 2 && state.hunger > 70) { // Lower chance of "state.poops" accumulation
      state.poops++;
    }
  }
}

void handleMenuSelection(int menuSel) {  
  // Modify stats based on menu selection
  switch (menuSel) {
    case 0: 
      capValue(state.hunger, 20);
      playEatingAnimation();
      // Serial.print("Hunger decreased to: ");
      // Serial.println(state.hunger);
      break;   // Feed
    case 1: 
      capValue(state.happy, 10);
      playPlayingAnimation();
      // Serial.print("Happiness increased to: ");
      // Serial.println(state.happy);
      break;    // Play
    case 2: 
      capValue(state.health, 10);
      playHealingAnimation();
      // Serial.print("Health increased to: ");
      // Serial.println(state.health);
      break;   // Heal
    case 3: 
      capValue(state.sleep, 20);
      playSleepingAnimation();
      // Serial.print("Sleep increased to: ");
      // Serial.println(state.sleep);
      break;    // Sleep
    case 4: 
      capValue(state.discipline, 10);
      playDisciplineAnimation();
      // Serial.print("Discipline increased to: ");
      // Serial.println(state.discipline);
      break; // Discipline
    case 5: 
      flushPoops();
      Serial.println("Poops flushed.");
      break; // Flush state.poops
    case 6: 
      displayingStats = !displayingStats;
      // Serial.print("Displaying stats: ");
      // Serial.println(displayingStats ? "ON" : "OFF");
      break; // Show stats
  }
}
void setup() {
  Serial.begin(1200);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(SOUND_PIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize with the I2C addr 0x3C
  display.display();
  delay(2000); // Show initial display buffer

   loadGameState(state); // Load any saved game state.

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  display.clearDisplay();

  // Button A cycles menu options
  if (!digitalRead(BUTTON_A)) {
    playButtonSound();
    menuSel = (menuSel + 1) % 7;  // Cycle through menu options
    while (!digitalRead(BUTTON_A));  // Wait for button release
  }

  // Button B selects menu option
  if (!digitalRead(BUTTON_B)) {
    playButtonSound();
    handleMenuSelection(menuSel);
    saveGameState(state);
    while (!digitalRead(BUTTON_B));  // Wait for button release
  }

  // Button C opens/closes the menu or hides stats
  if (!digitalRead(BUTTON_C)) {
    playButtonSound();
    menuOpened = !menuOpened;
    displayingStats = false;
    while (!digitalRead(BUTTON_C));  // Wait for button release
  }

  if (!displayingStats) {
    if (menuOpened) {
      displayMenuLine(menuSel);  // Show the menu line
    }
    displayTamagotchi();
  } else {
    displayStats();
  }

  updateStats();
  displayPoops();

  unsigned long currentTime = millis();
  if (currentTime - lastSaveTime >= 300000) { // Check if 5 minutes have passed
    saveGameState(state); // Save the game state
    lastSaveTime = currentTime; // Update the last save time
  }

  display.display();  // Draw everything on the screen
  delay(100);         // Small delay to reduce flickering
}
