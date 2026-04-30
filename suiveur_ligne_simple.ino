/*
 * Suiveur de ligne - Version simplifiée
 * Ligne NOIRE sur fond BLANC
 *
 * Brochage :
 *   IN1=D10  IN2=D6   -> Moteur GAUCHE
 *   IN3=D5   IN4=D3   -> Moteur DROIT
 *   HC-SR04 TRIG=D11  ECHO=D12
 *   TCRT5000 G=A0     D=A1
 *
 * Logique :
 *   G=blanc  D=blanc  -> avancer
 *   G=noir   D=blanc  -> tourner gauche
 *   G=blanc  D=noir   -> tourner droite
 *   Obstacle < 15cm   -> stop total
 */

// ─── Broches ─────────────────────────────────────────────────────────────────
#define IN1 10
#define IN2  6
#define IN3  5
#define IN4  3
#define TRIG        11
#define ECHO        12
#define TCRT_GAUCHE A0
#define TCRT_DROIT  A1

// ─── Paramètres ──────────────────────────────────────────────────────────────
const int SEUIL_LIGNE      = 5;    // < 5 = noir, > 5 = blanc
const int DIST_OBSTACLE_CM = 15;   // cm -> stop
const int VITESSE_AVANCE   = 70;   // blanc + blanc -> tout droit
const int VITESSE_VIRAGE   = 110;  // rotations
// ─── Moteurs ─────────────────────────────────────────────────────────────────

void moteurGauche(int dir, int vitesse) {
  if      (dir ==  1) { analogWrite(IN1, vitesse); digitalWrite(IN2, LOW);   }
  else if (dir == -1) { digitalWrite(IN1, LOW);    analogWrite(IN2, vitesse);}
  else                { digitalWrite(IN1, LOW);    digitalWrite(IN2, LOW);   }
}

void moteurDroit(int dir, int vitesse) {
  if      (dir ==  1) { analogWrite(IN3, vitesse); digitalWrite(IN4, LOW);   }
  else if (dir == -1) { digitalWrite(IN3, LOW);    analogWrite(IN4, vitesse);}
  else                { digitalWrite(IN3, LOW);    digitalWrite(IN4, LOW);   }
}

void avancer()        { moteurGauche(1, VITESSE_AVANCE); moteurDroit(1, VITESSE_AVANCE); }
void stopper()        { moteurGauche(0, 0);              moteurDroit(0, 0);              }
void tournerGauche()  { moteurGauche(0, 0);              moteurDroit(1, VITESSE_VIRAGE); }
void tournerDroite()  { moteurGauche(1, VITESSE_VIRAGE); moteurDroit(0, 0);              }

// ─── HC-SR04 ─────────────────────────────────────────────────────────────────

long mesurerDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duree = pulseIn(ECHO, HIGH, 30000);
  if (duree == 0) return 999;
  return duree * 0.034 / 2;
}

// ─── Setup ───────────────────────────────────────────────────────────────────

void setup() {
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
  Serial.println("=== Suiveur de ligne pret ===");
}

// ─── Loop ────────────────────────────────────────────────────────────────────

void loop() {
  // Obstacle -> stop total
  if (mesurerDistance() < DIST_OBSTACLE_CM) {
    stopper();
    return;
  }

  // Lecture capteurs
  bool gNoir = analogRead(TCRT_GAUCHE) < SEUIL_LIGNE;
  bool dNoir = analogRead(TCRT_DROIT)  < SEUIL_LIGNE;

  if (gNoir && dNoir)        stopper();       // noir des deux -> stop
  else if (!gNoir && !dNoir) avancer();       // blanc des deux -> tout droit
  else if (gNoir && !dNoir)  tournerGauche(); // noir à gauche -> tourner gauche
  else                       tournerDroite(); // noir à droite -> tourner droite
}