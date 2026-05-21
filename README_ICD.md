# Interface Control Document (ICD) - KinetiX HDC Core
**Statut :** CONFIDENTIEL INDUSTRIEL / BOÎTE NOIRE OEM  
**Version :** 1.0 (Architecture V12)  
**Normes cibles :** DO-178C (DAL-A), MISRA-C  

Ce document spécifie le contrat d'interface formel du disjoncteur géométrique KinetiX. Le non-respect strict de ces spécifications par l'intégrateur (Proxy) entraînera un comportement indéfini.

---

## 1. Spécifications Opérationnelles (SWaP)

### Empreinte Mémoire (RAM)
- **Zero Allocation Dynamique** : Le noyau KinetiX garantit l'absence totale d'appels à la mémoire du tas (`malloc`, `free`, `calloc`).
- **SRAM Statique Exigée** : Le système hôte DOIT pré-allouer une zone mémoire contiguë et la transmettre lors de l'initialisation. La taille requise est d'environ **55 Mo** (pour le buffer de contexte et les opérations de projection hyperdimensionnelles).

### Temps d'Exécution et Complexité
- **Déterminisme O(1)** : La fonction d'évaluation (`Kinetix_Step`) s'exécute en temps strictement constant.
- **Profil Matériel** : L'évaluation de la dérive topologique via distance de Hamming s'effectue en **exactement 1250 cycles de boucle** (40 000 bits / 32) sans aucune instruction de branchement conditionnel (branchless computing), évitant ainsi la pénalité de prédiction de branchement du CPU.

---

## 2. Protocole Réseau (Pont UDP)

Si KinetiX est intégré via le démon réseau `udp_bridge`, le contrat suivant s'applique :

- **Port d'Écoute (Par Défaut)** : UDP `8080`
- **Contrat de Payload** : Les trames entrantes DOIVENT respecter un alignement mémoire strict de **20 octets** (aucun *padding* réseau n'est toléré). 
- **Structure Binaire** (Little-Endian) :
  1. `uint32_t` (4 octets) : Timestamp en millisecondes.
  2. `float` IEEE 754 (4 octets) : Intensité infrarouge (IR).
  3. `float` IEEE 754 (4 octets) : Surface Equivalente Radar (RCS).
  4. `float` IEEE 754 (4 octets) : Ordre IA - Tangage (Pitch).
  5. `float` IEEE 754 (4 octets) : Ordre IA - Lacet (Yaw).

*(Toute trame dont la taille diffère de `sizeof(Kinetix_Telemetry_t)` sera silencieusement rejetée).*

---

## 3. Cycle de Vie et Machine d'État

Le noyau interagit sous forme de machine à états fermée.

### A. Initialisation (`Kinetix_Init`)
Le noyau nécessite le chargement physique d'une matrice HDC (la **Cassette Binaire**) issue de la Forge. Le pointeur de la SRAM et le pointeur de la Cassette (5 Ko) doivent être fournis.

### B. Boucle de Contrôle (`Kinetix_Step`)
À chaque cycle d'horloge de l'avionique (ex: 1000 Hz), l'état renvoyé sera soit `NOMINAL`, soit `VETO_LATCHED`.

### C. Définition du "Latching"
Si la distance topologique HDC dépasse le seuil de rupture (détection d'une anomalie physique ou d'un leurre), le noyau renvoie `KINETIX_STATE_VETO_LATCHED`. 
**ATTENTION :** Ce déclenchement est définitif (*Latching* matériel simulé). L'IA est considérée comme corrompue ou leurrée. Le noyau ne traitera plus de nouvelles données et renverra le VETO en $O(1)$ jusqu'à un *hard reset* de la plateforme.

---

## 4. Procédure de la Forge (Flux de Données)

Pour opérer, le Disjoncteur KinetiX exige une matrice d'alignement nominale spécifique à la plateforme ciblée (la Cassette). L'intégrateur doit procéder comme suit :

1. **Acquisition** : Exécuter un vol simulé ou réel en conditions certifiées saines.
2. **Extraction** : Exporter la télémétrie de ce vol nominal dans un fichier `.csv` pur.
3. **Transmission** : Envoyer le fichier `.csv` à l'équipe Xenisos.
4. **Forge** : Xenisos génère la *Cassette Binaire* chiffrée de 5 Ko et la fournit sous licence.
5. **Déploiement** : Flasher la Cassette binaire dans la mémoire du calculateur pour lier le disjoncteur à son appareil.
