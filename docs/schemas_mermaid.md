# 📊 Schémas Visuels Mermaid

## Vue d'ensemble du système

```mermaid
graph TD
    A["🔌 ESP32 DevKit"] -->|I2S Audio| B["🔊 Amplificateur PAM8403"]
    A -->|SPI| C["💾 Module Lecteur SD"]
    A -->|GPIO 34| D["🔘 Bouton Poussoir"]
    
    B --> E["🎵 Haut-parleur 8Ω"]
    C --> F["📀 Carte SD FAT32<br/>audio.mp3"]
    D --> G["📍 Résistance 10kΩ<br/>Pull-down"]
    
    H["⚡ Alimentation 5V 2A"] --> A
    H --> B
    
    style A fill:#4A90E2,color:#fff
    style B fill:#FF6B6B,color:#fff
    style C fill:#50C878,color:#fff
    style D fill:#FFD700,color:#000
    style E fill:#FF6B6B,color:#fff
    style F fill:#50C878,color:#fff
    style H fill:#FFA500,color:#000
```

---

## Schéma I2S - Audio

```mermaid
graph LR
    subgraph ESP32["ESP32 DevKit"]
        GPIO26["GPIO 26<br/>BCLK"]
        GPIO25["GPIO 25<br/>LRCK"]
        GPIO27["GPIO 27<br/>DIN"]
        GND1["GND"]
        VCC1["5V"]
    end
    
    subgraph AMPLI["Amplificateur PAM8403"]
        ACLK["CLK"]
        ALRCK["LRCK"]
        AIN["IN+"]
        AGND["GND/IN-"]
        AVCC["VCC"]
    end
    
    subgraph SPEAKER["Haut-parleur"]
        SP["8Ω 2W"]
    end
    
    GPIO26 -->|Jaune| ACLK
    GPIO25 -->|Orange| ALRCK
    GPIO27 -->|Rouge| AIN
    GND1 -->|Noir| AGND
    VCC1 -->|Rouge Épais| AVCC
    
    AMPLI --> SP
    
    style ESP32 fill:#4A90E2,color:#fff
    style AMPLI fill:#FF6B6B,color:#fff
    style SPEAKER fill:#FFD700,color:#000
```

---

## Schéma SPI - Carte SD

```mermaid
graph LR
    subgraph ESP32["ESP32 DevKit"]
        GPIO18["GPIO 18<br/>CLK"]
        GPIO23["GPIO 23<br/>MOSI"]
        GPIO19["GPIO 19<br/>MISO"]
        GPIO5["GPIO 5<br/>CS"]
        GND2["GND"]
        VCC2["3.3V"]
    end
    
    subgraph MODULE["Module SD Lecteur"]
        MCLK["CLK"]
        MMOSI["MOSI"]
        MMISO["MISO"]
        MCS["CS"]
        MGND["GND"]
        MVCC["VCC"]
    end
    
    subgraph SDCARD["Carte SD"]
        SD["FAT32<br/>audio.mp3"]
    end
    
    GPIO18 -->|Vert| MCLK
    GPIO23 -->|Bleu| MMOSI
    GPIO19 -->|Violet| MMISO
    GPIO5 -->|Gris| MCS
    GND2 -->|Noir| MGND
    VCC2 -->|Rouge| MVCC
    
    MODULE --> SD
    
    style ESP32 fill:#4A90E2,color:#fff
    style MODULE fill:#50C878,color:#fff
    style SDCARD fill:#FFD700,color:#000
```

---

## Schéma GPIO - Bouton

```mermaid
graph TD
    subgraph ESP32["ESP32 DevKit"]
        GPIO34["GPIO 34<br/>INPUT"]
        VCC3["3.3V"]
        GND3["GND"]
    end
    
    subgraph BUTTON["Bouton Poussoir"]
        BTN["6x6mm"]
    end
    
    subgraph RESISTOR["Résistance Pull-down"]
        RES["10kΩ"]
    end
    
    VCC3 -->|Signal| BTN
    BTN -->|Bleu| GPIO34
    GPIO34 -->|Violet| RES
    RES -->|Noir| GND3
    
    style ESP32 fill:#4A90E2,color:#fff
    style BUTTON fill:#FFD700,color:#000
    style RESISTOR fill:#A9A9A9,color:#fff
```

---

## Schéma Alimentation

```mermaid
graph TD
    ALI["⚡ Alimentation Externe<br/>5V 2A USB-C"]
    
    ALI --> ESP32["ESP32<br/>5V"]
    ALI --> AMPLI["Amplificateur<br/>5V"]
    
    ESP32 --> MODULE["Module SD<br/>3.3V via ESP32"]
    
    AMPLI --> SPEAKER["Haut-parleur<br/>8Ω"]
    
    ESP32 -.->|Ground Commun| AMPLI
    AMPLI -.->|Ground Commun| MODULE
    MODULE -.->|Ground Commun| SPEAKER
    
    style ALI fill:#FFA500,color:#000,stroke:#FF6B00,stroke-width:3px
    style ESP32 fill:#4A90E2,color:#fff
    style AMPLI fill:#FF6B6B,color:#fff
    style MODULE fill:#50C878,color:#fff
    style SPEAKER fill:#FFD700,color:#000
```

---

## Tableau Récapitulatif I2S

```mermaid
graph TB
    subgraph TABLE["Connexions I2S - Audio"]
        T1["GPIO 26 BCLK → CLK Ampli"]
        T2["GPIO 25 LRCK → LRCK Ampli"]
        T3["GPIO 27 DIN → IN+ Ampli"]
        T4["GND → GND Ampli"]
        T5["5V → VCC Ampli"]
    end
    
    style TABLE fill:#FF6B6B,color:#fff,stroke:#CC0000,stroke-width:2px
    style T1 fill:#FFB3B3,color:#000
    style T2 fill:#FFB3B3,color:#000
    style T3 fill:#FFB3B3,color:#000
    style T4 fill:#FFB3B3,color:#000
    style T5 fill:#FFB3B3,color:#000
```

---

## Tableau Récapitulatif SPI

```mermaid
graph TB
    subgraph TABLE["Connexions SPI - Carte SD"]
        T1["GPIO 18 CLK → CLK SD"]
        T2["GPIO 23 MOSI → MOSI SD"]
        T3["GPIO 19 MISO → MISO SD"]
        T4["GPIO 5 CS → CS SD"]
        T5["3.3V → VCC SD"]
        T6["GND → GND SD"]
    end
    
    style TABLE fill:#50C878,color:#fff,stroke:#228B22,stroke-width:2px
    style T1 fill:#A8E6C1,color:#000
    style T2 fill:#A8E6C1,color:#000
    style T3 fill:#A8E6C1,color:#000
    style T4 fill:#A8E6C1,color:#000
    style T5 fill:#A8E6C1,color:#000
    style T6 fill:#A8E6C1,color:#000
```

---

## Tableau Récapitulatif GPIO

```mermaid
graph TB
    subgraph TABLE["Connexions GPIO - Bouton"]
        T1["GPIO 34 → Signal Bouton"]
        T2["3.3V → VCC Bouton"]
        T3["GND → GND Bouton"]
        T4["10kΩ Résistance → Entre GPIO 34 et GND"]
    end
    
    style TABLE fill:#FFD700,color:#000,stroke:#FF8C00,stroke-width:2px
    style T1 fill:#FFED4E,color:#000
    style T2 fill:#FFED4E,color:#000
    style T3 fill:#FFED4E,color:#000
    style T4 fill:#FFED4E,color:#000
```

---

## Flux de Fonctionnement

```mermaid
graph LR
    A["🔘 Appui Bouton<br/>GPIO 34 = HIGH"] 
    --> B["✅ Détection<br/>Anti-rebond 200ms"]
    --> C["📂 Lecture Fichier<br/>/audio.mp3"]
    --> D["📤 I2S Audio<br/>44100 Hz 16-bit"]
    --> E["🔊 Amplificateur<br/>PAM8403"]
    --> F["🎵 Son<br/>Haut-parleur"]
    --> G["🔄 Bouton relâché<br/>Prêt pour suivant"]
    
    style A fill:#FFD700,color:#000
    style B fill:#90EE90,color:#000
    style C fill:#50C878,color:#fff
    style D fill:#4A90E2,color:#fff
    style E fill:#FF6B6B,color:#fff
    style F fill:#FFD700,color:#000
    style G fill:#90EE90,color:#000
```

---

**Schémas Mermaid - Escape Game Factory - 2026-09-10**
