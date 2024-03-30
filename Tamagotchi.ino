#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET -1  // Share Reset pin with Arduino
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

#define BUTTON_A 9
#define BUTTON_B 8
#define BUTTON_C 7
#define SOUND_PIN 6

int tamagotchiPosX = 64;        // Start at the middle horizontally
int tamagotchiPosY = 32;        // Start below the menu line vertically
int moveDirectionX = 1;         // Movement direction: 1 for right, -1 for left
int moveDirectionY = 1;         // Movement direction: 1 for down, -1 for up
const int moveSpeed = 1;        // Adjust for faster or slower movement
const int menuLineHeight = 16;  // Height of the menu line area

// Tamagotchi stats
int hunger = 50;
int happy = 50;
int health = 100;
int sleep = 100;
int discipline = 100;
int poops = 0;

bool menuOpened = false;
bool displayingStats = false;
int menuSel = 0;

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
  const int animationDelay = 500; // Time in milliseconds to show each frame
  
  // Frame 1: Looking at food
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( o.o ) >1");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 2: Eating food
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  >[ ]<  ");
  display.display();
  delay(animationDelay);
  
  // Frame 3: Happy after eating
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( ^_^ ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
}

void playPlayingAnimation() {
  const int animationDelay = 500; // Time in milliseconds to show each frame

  // Frame 1: Looking at the ball
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  >o");
  display.display();
  delay(animationDelay);
  
  // Frame 2: Hitting the ball
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( >.< ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ < o ");
  display.display();
  delay(animationDelay);
  
  // Frame 3: Happy with the ball on the other side
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( ^_^ ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  o");
  display.display();
  delay(animationDelay);
}

void playHealingAnimation() {
  const int animationDelay = 500; // Time in milliseconds to show each frame

  // Frame 1: Sad or sick expression
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( >.< ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 2: With a bandage and hopeful
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" (-.-) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 3: Happy and healed
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( ^_^ ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
}

void playSleepingAnimation() {
  const int animationDelay = 500; // Time in milliseconds to show each frame

  // Frame 1: Getting ready to sleep
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( -.- ) zZ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 2: Sleeping soundly
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( -.- ) zZz");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(2000); // Hold this frame a bit longer to simulate the Tamagotchi sleeping
  
  // Frame 3: Waking up refreshed
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( ^_^ ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
}

void playDisciplineAnimation() {
  const int animationDelay = 500; // Time in milliseconds to show each frame

  // Frame 1: Neutral
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 2: Sad but attentive
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( o_o ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
  
  // Frame 3: Sad
  display.clearDisplay();
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( >_< ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
  display.display();
  delay(animationDelay);
}

void flushPoops() {
    int menuLineHeight = 16; // Height of the menu line at the top of the screen
    int waterHeight = display.height() - menuLineHeight; // Adjust water flow height to start below the menu bar
    int waterSpeed = 10; // Speed of the water flow animation
    
    // Starting from the left side of the screen, draw water rectangles that increase in width
    for (int waterWidth = 0; waterWidth <= display.width(); waterWidth += waterSpeed) {
        // Clear the area below the menu bar on each frame to redraw the water
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
    poops = 0;
}

void displayPoops() {
    // Assuming each poop takes up about 10 pixels horizontally and 16 pixels vertically
    int startingPoopPosX = 0; // Starting X position for the first poop
    int poopPosY = display.height() - 16; // Position Y for poops at the bottom of the screen
    int poopSpacing = 12; // Space between poops
    
    for (int i = 0; i < poops; i++) {
        int poopPosX = startingPoopPosX + (i * poopSpacing); // Calculate X position for each poop

        // Ensure we don't draw outside the display width
        if (poopPosX + 10 > display.width()) {
            break; // Stop drawing more poops if we run out of space
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
  tamagotchiPosX += moveSpeed * moveDirectionX;
  // Reverse direction if hitting the horizontal display boundaries
  if (tamagotchiPosX <= 0 || tamagotchiPosX >= display.width() - 64) {
    moveDirectionX *= -1;
  }

  // Update vertical position
  tamagotchiPosY += moveSpeed * moveDirectionY;
  // Reverse direction if hitting the vertical display boundaries, taking into account the menu line height
  if (tamagotchiPosY <= menuLineHeight || tamagotchiPosY >= display.height() - 32) {
    moveDirectionY *= -1;
  }

  // Ensure the position stays within the display boundaries, accounting for the menu line height
  tamagotchiPosX = max(0, min(tamagotchiPosX, display.width() - 64));
  tamagotchiPosY = max(menuLineHeight, min(tamagotchiPosY, display.height() - 32));

  // Set cursor based on current position and draw the Tamagotchi
  display.setCursor(tamagotchiPosX, tamagotchiPosY);
  display.println("  /\\_/\\  ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 10);
  display.println(" ( o.o ) ");
  display.setCursor(tamagotchiPosX, tamagotchiPosY + 20);
  display.println("  > ^ <  ");
}

void displayStats() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Stats:");
  display.println("");
  display.print("Hunger: ");
  display.println(hunger);
  display.print("Happy: ");
  display.println(happy);
  display.print("Health: ");
  display.println(health);
  display.print("Sleep: ");
  display.println(sleep);
  display.print("Discipline: ");
  display.println(discipline);
  display.print("Poops: ");
  display.println(poops);
}

void updateStats() {

  static unsigned long lastUpdateTime = 0; // Last update time
  unsigned long currentTime = millis(); // Current time in milliseconds
  
  // Update stats every 10 seconds instead of every loop. Adjust the timing as needed for your gameplay balance.
  if (currentTime - lastUpdateTime > 10000) { // 10,000 milliseconds = 10 seconds
    lastUpdateTime = currentTime; // Reset the last update time
    
    // Now apply the slower stat decreases
    capValue(hunger, -random(0, 2)); // Decrease hunger by 0 or 1 point
    if (hunger < 30 || health < 30) {
      capValue(happy, -1); // Decrease happiness by 1 point if hunger or health is low
    }

    if (hunger < 30 || sleep < 30) {
      capValue(health, -random(0, 2)); // Decrease health by 0 or 1 point if hunger or sleep is low
    }

    capValue(sleep, -1); // Decrease sleep by 1 point
    capValue(discipline, -random(0, 1)); // Decrease discipline by 0 or 1 point
    
    if (random(0, 10) < 2 && hunger > 70) { // Lower chance of "poops" accumulation
      poops++;
    }
  }
}

void handleMenuSelection(int menuSel) {
  // Modify stats based on menu selection
  switch (menuSel) {
    case 0: capValue(hunger, 20); playEatingAnimation(); break;   // Feed
    case 1: capValue(happy, 10);  playPlayingAnimation(); break;    // Play
    case 2: capValue(health, 10); playHealingAnimation(); break;   // Heal
    case 3: capValue(sleep, 20);  playSleepingAnimation(); break;    // Sleep
    case 4: capValue(discipline, 10); playDisciplineAnimation(); break; // Discipline
    case 5: flushPoops(); break; // Flush poops
    case 6: displayingStats = !displayingStats; break; // Show stats
  }
}

void setup() {
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(SOUND_PIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize with the I2C addr 0x3C
  display.display();
  delay(2000); // Show initial display buffer

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
  display.display();  // Draw everything on the screen
  delay(100);         // Small delay to reduce flickering
}
