# 🔌 Schéma de Branchement Détaillé - ESP32

## Vue d'ensemble complète

```
┌─────────────────────────────────────────────────────────────────┐
│                        ESCAPE GAME FACTORY                       │
│                      Audio Trigger System                        │
└─────────────────────────────────────────────────────────────────┘


                          ┌──────────────┐
                          │    ESP32     │
                          │  DevKit      │
                          └──────────────┘
                                 │
                  ┌──────────────┼──────────────┐
                  │              │              │
             [I2S Audio]     [SPI SD]      [GPIO Button]
                  │              │              │
                  │              │              │
        ┌─────────▼──────┐  ┌────▼─────┐   ┌──▼─────┐
        │ Amplificateur  │  │ Module SD │   │ Bouton │
        │   PAM8403      │  │           │   │ 6x6mm  │
        └────────┬───────┘  └─────┬─────┘   └──┬─────┘
                 │                │             │
        ┌────────▼────────┐       │         ┌───▼────┐
        │  Haut-parleur   │   [Carte SD]    │10kΩ Res│
        │    8Ω 2W        │   (FAT32)       └────────┘
        └─────────────────┘


═══════════════════════════════════════════════════════════════════
```

---

## 🔴 Schéma I2S - Audio (ESP32 → Amplificateur)

```
         ESP32              PAM8403 Amplificateur
         ┌─────┐            ┌──────────┐
         │ 26  │ BCLK ────→ │ CLK      │
         │     │            │          │
         │ 25  │ LRCK ────→ │ LRCK     │
         │     │            │          │
         │ 27  │ DIN ─────→ │ IN+      │
         │     │            │          │
         │GND  │ GND ─────→ │ GND      │
         │     │            │ IN-      │
         │ 5V  │ VCC ─────→ │ VCC      │
         └─────┘            └────┬─────┘
                                 │
                           ┌─────▼──────┐
                           │ Haut-parleur│
                           │  8Ω 2W      │
                           └─────────────┘
```

---

## 🔵 Schéma SPI - Carte SD (ESP32 → Module SD)

```
         ESP32            Module SD Lecteur
         ┌──────┐         ┌──────────┐
         │ 18   │ CLK ──→ │ CLK      │
         │      │         │          │
         │ 23   │ MOSI ─→ │ MOSI     │
         │      │         │          │
         │ 19   │ MISO ←─ │ MISO     │
         │      │         │          │
         │ 5    │ CS ───→ │ CS       │
         │      │         │          │
         │GND   │ GND ──→ │ GND      │
         │      │         │          │
         │3.3V  │ VCC ──→ │ VCC      │
         └──────┘         └────┬─────┘
                               │
                          [Carte SD
                           FAT32]
```

---

## 🟡 Schéma GPIO - Bouton (ESP32 → Bouton)

```
         ESP32              Bouton Poussoir
         ┌──────┐           ┌──────┐
         │ 34   │◄──┬──────►│Signal│
         │      │   │       └──────┘
         │3.3V  │   │       
         │      │   │       Résistance 10kΩ
         │GND   │───┤       (Pull-down)
         │      │   │
         └──────┘   │
                    └─┬─────┬─┘
                      │ 10k │
                      └─────┘
                        │
                       GND
```

---

## 📋 Tableau de Branchement Complet

### I2S Audio

| ESP32 | Signal | Ampli | Couleur câble |
|-------|--------|-------|---------------|
| 26 | BCLK | CLK | Jaune |
| 25 | LRCK | LRCK | Orange |
| 27 | DIN | IN+ | Rouge |
| GND | GND | GND | Noir |
| 5V | VCC | VCC | Rouge (épais) |

### SPI Carte SD

| ESP32 | Signal | Module SD | Couleur câble |
|-------|--------|-----------|---------------|
| 18 | CLK | CLK | Vert |
| 23 | MOSI | MOSI | Bleu |
| 19 | MISO | MISO | Violet |
| 5 | CS | CS | Gris |
| 3.3V | VCC | VCC | Rouge |
| GND | GND | GND | Noir |

### GPIO Bouton

| ESP32 | Signal | Bouton | Notes |
|-------|--------|--------|-------|
| 34 | Input | Signal | Lecture uniquement |
| 3.3V | VCC | + | Alimentation bouton |
| GND | GND | - | Ground commun |
| - | 10kΩ | Entre 34 et GND | Résistance pull-down |

---

## ⚡ Alimentation

```
Alimentation externe 5V 2A
        │
        ├─────→ ESP32 (5V)
        │
        └─────→ Amplificateur PAM8403 (5V)
                │
                └─────→ Haut-parleur
```

**Important** : Tous les GND doivent être connectés ensemble (commun)

---

## ✅ Checklist Branchement

- [ ] I2S BCLK (GPIO 26) → Ampli CLK
- [ ] I2S LRCK (GPIO 25) → Ampli LRCK
- [ ] I2S DIN (GPIO 27) → Ampli IN+
- [ ] GND → Ampli GND et Ampli IN-
- [ ] 5V → Ampli VCC
- [ ] SPI CLK (GPIO 18) → Module SD CLK
- [ ] SPI MOSI (GPIO 23) → Module SD MOSI
- [ ] SPI MISO (GPIO 19) → Module SD MISO
- [ ] SPI CS (GPIO 5) → Module SD CS
- [ ] 3.3V → Module SD VCC
- [ ] GND → Module SD GND
- [ ] GPIO 34 → Bouton Signal
- [ ] 3.3V → Bouton +
- [ ] 10kΩ résistance entre GPIO 34 et GND
- [ ] Haut-parleur connecté à Ampli

---

## ⚠️ Précautions

1. **Tension mixte** : ESP32 (3.3V logique) + Ampli (5V)
2. **Alimentation suffisante** : 2A minimum requis
3. **Ground commun** : Tous les circuits doivent partager GND
4. **Résistance 10kΩ** : OBLIGATOIRE pour stabilité GPIO 34
5. **Câbles courts** : I2S < 50cm pour éviter interférences
6. **Polarité** : Vérifier +/- du haut-parleur

---

## 📸 Photos Utiles

```
ESP32 DevKit Pinout:
  
    USB
     │
  ┌──┴──┐
  │     │
EN▌     ▌GND
36▌     ▌23 (MOSI)
39▌ ESP32▌22
34▌ (BTN)▌TX/1
35▌     ▌RX/3
32▌     ▌21
33▌     ▌20
25▌(I2S)▌19 (MISO)
26▌(I2S)▌18 (CLK)
27▌(I2S)▌5  (CS)
14▌     ▌17
12▌     ▌16
GND▌    ▌4
13▌     ▌0
  └─────┘
  3.3V/5V
```

---

**Schéma finalisé - ESP32 - 2026-09-10**
