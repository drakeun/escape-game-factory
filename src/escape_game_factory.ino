/*
  ╔═══════════════════════════════════════════════════════════════╗
  ║         ESCAPE GAME FACTORY - AUDIO TRIGGER SYSTEM            ║
  ║                                                               ║
  ║  Fonction : Lire un fichier MP3 UNE SEULE FOIS               ║
  ║             au premier appui sur le bouton                    ║
  ║                                                               ║
  ║  Matériel :                                                  ║
  ║  - ESP32 DevKit                                              ║
  ║  - Module SD SPI (GPIO 18, 23, 19, 5)                       ║
  ║  - Bouton poussoir GPIO 34 + Résistance 10kΩ                ║
  ║  - Amplificateur I2S (GPIO 26, 25, 27)                      ║
  ║  - Haut-parleur 8Ω                                          ║
  ║                                                               ║
  ║  Comportement : 1 appui = 1 lecture complète                ║
  ║                 Bouton désactivé pendant lecture             ║
  ║                 Bouton réactivé après fin                    ║
  ╚═══════════════════════════════════════════════════════════════╝
*/

#include <Arduino.h>
#include <SD.h>
#include <FS.h>
#include "driver/i2s.h"

// ╔═══════════════════════════════════════════════════════════════╗
// ║                    PIN CONFIGURATION                          ║
// ╚═══════════════════════════════════════════════════════════════╝

// I2S Audio Pins (vers amplificateur)
#define I2S_BCK       26    // Bit Clock
#define I2S_LRCK      25    // Left/Right Clock
#define I2S_DOUT      27    // Data Out

// SPI SD Card Pins
#define SD_CLK        18    // Clock
#define SD_MOSI       23    // Master Out Slave In
#define SD_MISO       19    // Master In Slave Out
#define SD_CS         5     // Chip Select

// Bouton
#define BUTTON_PIN    34    // Input
#define DEBOUNCE_TIME 200   // Anti-rebond (ms)

// I2S Port
#define I2S_PORT      I2S_NUM_0

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   STATE VARIABLES                             ║
// ╚═══════════════════════════════════════════════════════════════╝

bool audioPlaying = false;           // Est-ce que l'audio est en cours de lecture ?
bool buttonPressed = false;          // État du bouton
unsigned long lastButtonPress = 0;   // Dernier appui (anti-rebond)
File audioFile;                      // Fichier audio

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   I2S CONFIGURATION                           ║
// ╚═══════════════════════════════════════════════════════════════╝

void configureI2S() {
  Serial.println("[I2S] Configuration I2S...");
  
  // Configuration I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };
  
  // Appliquer configuration
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  
  // Configuration des pins
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK,
    .ws_io_num = I2S_LRCK,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  i2s_set_pin(I2S_PORT, &pin_config);
  
  Serial.println("[I2S] ✅ I2S configuré");
}

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   SD CARD SETUP                               ║
// ╚═══════════════════════════════════════════════════════════════╝

void initSD() {
  Serial.println("[SD] Initialisation carte SD...");
  
  SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
  
  if (!SD.begin(SD_CS)) {
    Serial.println("[SD] ❌ ERREUR : Carte SD non détectée !");
    Serial.println("[SD] Vérifier :");
    Serial.println("  - Carte SD présente ?");
    Serial.println("  - Format FAT32 ?");
    Serial.println("  - Connexions SPI OK ?");
    while (1) delay(1000);
  }
  
  uint8_t cardType = SD.cardType();
  uint64_t cardSize = SD.cardSize();
  
  Serial.print("[SD] Type : ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  else Serial.println("INCONNU");
  
  Serial.print("[SD] Taille : ");
  Serial.print(cardSize / (1024 * 1024));
  Serial.println(" MB");
  Serial.println("[SD] ✅ Carte SD OK");
}

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   BUTTON SETUP                                ║
// ╚═══════════════════════════════════════════════════════════════╝

void initButton() {
  Serial.println("[BTN] Configuration bouton...");
  
  pinMode(BUTTON_PIN, INPUT);  // ← Pas de pullup interne
                                // La résistance 10kΩ fait le travail
  
  Serial.println("[BTN] ✅ Bouton configuré (GPIO 34)");
}

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   BOUTON DETECTION                            ║
// ╚═══════════════════════════════════════════════════════════════╝

bool checkButton() {
  int buttonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();
  
  // Si bouton appuyé ET pas de rebond ET pas en lecture
  if (buttonState == HIGH && 
      (now - lastButtonPress) > DEBOUNCE_TIME && 
      !audioPlaying) {
    
    lastButtonPress = now;
    Serial.println("\n🔘 [BTN] BOUTON DÉTECTÉ !");
    return true;  // Bouton appuyé valide
  }
  
  return false;  // Bouton non appuyé ou en rebond ou en lecture
}

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   MP3 PLAYBACK                                ║
// ╚═══════════════════════════════════════════════════════════════╝

void playMP3(const char* filename) {
  Serial.print("[AUDIO] Ouverture fichier : ");
  Serial.println(filename);
  
  // Vérifier que le fichier existe
  if (!SD.exists(filename)) {
    Serial.print("[AUDIO] ❌ ERREUR : Fichier ");
    Serial.print(filename);
    Serial.println(" non trouvé !");
    Serial.println("[AUDIO] Fichiers disponibles :");
    listFiles("/");
    return;
  }
  
  // Ouvrir le fichier
  audioFile = SD.open(filename, FILE_READ);
  if (!audioFile) {
    Serial.println("[AUDIO] ❌ ERREUR : Impossible d'ouvrir le fichier !");
    return;
  }
  
  audioPlaying = true;
  Serial.println("[AUDIO] ▶️  Lecture démarrée...");
  Serial.print("[AUDIO] Taille : ");
  Serial.print(audioFile.size());
  Serial.println(" bytes");
  
  // Buffer pour lire le fichier
  uint8_t buffer[512];
  size_t bytesRead = 0;
  size_t totalBytesWritten = 0;
  
  // Lire et jouer le fichier
  while (audioFile.available()) {
    bytesRead = audioFile.read(buffer, sizeof(buffer));
    
    if (bytesRead > 0) {
      // Envoyer au I2S (vers amplificateur)
      size_t bytesWritten = 0;
      i2s_write(I2S_PORT, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
      totalBytesWritten += bytesWritten;
    }
    
    // Petite pause pour éviter de bloquer
    delay(1);
  }
  
  // Fermer le fichier
  audioFile.close();
  audioPlaying = false;
  
  Serial.println("[AUDIO] ✅ Lecture terminée");
  Serial.print("[AUDIO] Bytes envoyés : ");
  Serial.println(totalBytesWritten);
}

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   LIST FILES (Debug)                          ║
// ╚═══════════════════════════════════════════════════════════════╝

void listFiles(const char* dirname) {
  Serial.print("[SD] Fichiers dans ");
  Serial.println(dirname);
  
  File root = SD.open(dirname);
  File file = root.openNextFile();
  
  while (file) {
    if (!file.isDirectory()) {
      Serial.print("  📄 ");
      Serial.print(file.name());
      Serial.print(" (");
      Serial.print(file.size());
      Serial.println(" bytes)");
    } else {
      Serial.print("  📁 ");
      Serial.println(file.name());
    }
    file = root.openNextFile();
  }
}

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   SETUP                                       ║
// ╚═══════════════════════════════════════════════════════════════╝

void setup() {
  // Démarrer la communication série
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔═══════════════════════════════════════════════════════════════╗");
  Serial.println("║         🎮 ESCAPE GAME FACTORY - AUDIO TRIGGER 🎮             ║");
  Serial.println("║                                                               ║");
  Serial.println("║  Appuyez UNE FOIS sur le bouton pour jouer l'audio            ║");
  Serial.println("║  Lecture complète puis fin automatique                        ║");
  Serial.println("╚═══════════════════════════════════════════════════════════════╝\n");
  
  // Initialiser composants
  initSD();           // Carte SD
  initButton();       // Bouton
  configureI2S();     // Audio I2S
  
  Serial.println("\n[SYSTEM] ✅ Prêt !\n");
  Serial.println("🔘 En attente d'appui bouton...\n");
}

// ╔═══════════════════════════════════════════════════════════════╗
// ║                   MAIN LOOP                                   ║
// ╚═══════════════════════════════════════════════════════════════╝

void loop() {
  // Vérifier si bouton appuyé
  if (checkButton()) {
    // Lancer la lecture UNE FOIS
    playMP3("/audio.mp3");
    
    Serial.println("\n🔘 Bouton déverrouillé (prêt pour prochain appui)\n");
  }
  
  // Petite pause pour ne pas surcharger le processeur
  delay(10);
}

/*
  ╔═══════════════════════════════════════════════════════════════╗
  ║                   NOTES IMPORTANTES                           ║
  ╚═══════════════════════════════════════════════════════════════╝

  1. FICHIER MP3 :
     ✅ Nom exacte : /audio.mp3 (à la racine SD)
     ✅ Format : MP3 44100 Hz 16-bit
     ✅ Bitrate : 128kbps recommandé
     ✅ Fichier bien formé (pas corrompu)

  2. COMPORTEMENT :
     ✅ 1 appui = 1 lecture COMPLÈTE
     ✅ Bouton désactivé pendant lecture
     ✅ Bouton réactivé après fin
     ✅ Pas de relecture si on reappuie pendant

  3. ANTI-REBOND :
     ✅ Délai 200ms entre 2 appuis
     ✅ Élimine les faux appuis

  4. DÉBOGAGE :
     ✅ Moniteur série 115200 baud
     ✅ Messages détaillés à chaque étape
     ✅ Affiche les fichiers SD présents

  5. PINS UTILISÉS :
     ✅ GPIO 34 = Bouton (Input)
     ✅ GPIO 26, 25, 27 = I2S Audio
     ✅ GPIO 18, 23, 19, 5 = SPI SD

  6. ALIMENTATION :
     ✅ ESP32 : 5V (via USB)
     ✅ Ampli : 5V
     ✅ GND commun

*/
