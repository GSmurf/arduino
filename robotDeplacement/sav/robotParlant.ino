/* Source : http://goo.gl/nx4tsw
 * Importation des bibliothèques
 */
#include <Wire.h>		//Permet à Arduino et Rotoshield de communiquer 
#include <snootor.h>	//Permet de manipuler le Rotoshield
#include <WaveHC.h>		//Permet de manipuler le Wave Shield
#include <WaveUtil.h>	//Permet de manipuler le Wave Shield
 
/*
 * Déclarations des variables globales
 * 
 * Les déclarations sont faites en dehors de setup() et loop(),
 * afin de pouvoir être utilisées à la fois dans les deux fonctions.
 * Il faut être dans setup() ou loop() pour appeler du code (obligation)
 */
 
//Définition du mode par défaut. 0 pour Stationnaire / 1 pour Sentinelle
bool mode = 0;

//Dossier de la racine à utiliser pour les effets sonores (8 caractères)
//define soundTheme "claptrap"
//define soundTheme "p_turret"
#define soundTheme "sw_r2d2"

//Défini les dossiers des différentes émotions des effets sonores
#define opening "opening"
#define sentinel_start "sent_str"
#define sentinel_running "sent_run"
#define stationnary_start "stat_str"
#define stationnary_running "stat_run"
#define turning "turning"

//Pins utilités par les capteurs
#define IRSensorRightPin 14
#define IRSensorLeftPin 15
#define noiseRandomPin 3

//Instanciation de deux objets moteurs
SnootorDC Motor_1;
SnootorDC Motor_2;

//Définition des vitesses indépendantes des moteurs pour le parallélisme
#define leftFastSpeed 255
#define rightFastSpeed 253
#define leftSlowSpeed 155
#define rightSlowSpeed 153

//Définition d'une variable mémorisant le sens des virages
int turnDirection = 0;	//0 : réinitialisé / 1 : Gauche / 2 : Droite

//Déclaration des variables de temps
unsigned long timeA;
unsigned long timeB;
unsigned long timeSinceTheLastSound = 0;

//Variable aléatoire, en secondes
long randomCount = random(10) * 1000;

//Définition d'une variable mémorisant le random
int lastRandom = 0;

//Instanciation des objets nécessaires au Wave Shield
SdReader card;	//Contient l'information de la carte SD
FatVolume vol;	//Contient l'information de la partition sur la carte
FatReader root;	//Contient l'information du dossier root du volume
WaveHC wave;	//Seul objet WaveHC car il ne joue qu'un son à la fois

//Déclaration de variables utiles pour naviguer sur la carte SD 
uint8_t dirLevel;	//Niveau d'indentation des noms de fichiers/dossiers
dir_t dirBuf;		//Buffer pour les lectures de dossier

//Déclaration de la variable qui contient le fichier wav à jouer
FatReader file;

//Défini une macro pour insérer des messages d'erreur dans la mémoire flash
#define error(msg) error_P(PSTR(msg))


/*
 * Définition des fonctions basiques
 * 
 * Les fonctions facilitent la lecture du code, elles regroupent
 * des instructions répétitives sous un nom explicite.
 */

void moveForward()
{
	Motor_1.run(FORWARD);
	Motor_2.run(FORWARD);
	
	delay(500);
}

void turnLeftForward()
{
	Motor_1.run(RELEASE);
	Motor_2.run(FORWARD);
	
	delay(500);
}

void turnRightForward()
{
	Motor_1.run(FORWARD);
	Motor_2.run(RELEASE);
	
	delay(500);
}

void moveBackward()
{
	Motor_1.run(BACKWARD);
	Motor_2.run(BACKWARD);
	
	delay(500);
}

void turnLeftBackward()
{
	Motor_1.run(RELEASE);
	Motor_2.run(BACKWARD);
	
	delay(500);
}


void turnRightBackward()
{
	Motor_1.run(BACKWARD);
	Motor_2.run(RELEASE);
	
	delay(500);
}

void releaseMotors()
{
	Motor_1.run(RELEASE);
	Motor_2.run(RELEASE);
}


/*
 * Définition de fonctions plus complexes
 * 
 * Il s'agit de fonctions qui sont presque des programmes à part entière
 * et qui auraient pu être placées dans loop(). Leur lecture peut être
 * difficile mais elles améliorent encore la compréhension de loop().
 */

void stationaryMode()
{
	//Si aucun son n'est joué et que cela fait plus de x secondes
	if(!wave.isplaying && ((millis() - timeSinceTheLastSound) > randomCount))
	{
		playOneRandomFileFromDirectory(stationnary_running);
		timeSinceTheLastSound = millis();
		
		randomCount = random(15) * 1000;
	}
	
	//Si les deux capteurs sont activés pendant 5s: Mode Sentinelle
	if(digitalRead(IRSensorLeftPin) == LOW && digitalRead(IRSensorRightPin) == LOW)
	{
		timeA = millis();
		timeB = millis();
		
		while(digitalRead(IRSensorLeftPin) == LOW && (timeB-timeA) < 5050)
		{
			timeB = millis();
		}
		
		if((timeB-timeA) > 5000)
		{
			//Initialise la vitesse par défaut utilisée par les moteurs
			Motor_1.setSpeed(leftFastSpeed);
			Motor_2.setSpeed(rightFastSpeed);
			
			//éventuellement émettre un son
			//avancer pendant 1s pour laisser à l'utilisateur le temps de retirer sa main
			
			Serial.println("Switching to sentinelMode()");
			mode = 1;
			
			if(!wave.isplaying)
			{
				playOneRandomFileFromDirectory(sentinel_start);
				timeSinceTheLastSound = millis();
			}
			
			return;
		}
	}
}

void sentinelMode()
{
	//Donne des détails sur le statut courant des moteurs
	//quand MOTOR_DEBUG est défini dans snootor_common.h
	SC.dump();
	
	//Si seul le capteur gauche est déclenché
	if(digitalRead(IRSensorLeftPin) == LOW)
	{
		Serial.println("Turn right forward");
		
		while(digitalRead(IRSensorLeftPin) == LOW || digitalRead(IRSensorRightPin) == LOW)
		{
			turnRightForward();
			
			//Si aucun son n'est joué et que cela fait plus de x secondes
			if(!wave.isplaying && ((millis() - timeSinceTheLastSound) > randomCount))
			{
				playOneRandomFileFromDirectory(turning);
				timeSinceTheLastSound = millis();
				
				randomCount = random(3) * 1000;
			}
		}
	}
	
	//Si seul le capteur droit est déclenché
	else if(digitalRead(IRSensorRightPin) == LOW)
	{
		Serial.println("Turn left forward");
		
		while(digitalRead(IRSensorLeftPin) == LOW || digitalRead(IRSensorRightPin) == LOW)
		{
			turnLeftForward();
			
			//Si aucun son n'est joué et que cela fait plus de x secondes
			if(!wave.isplaying && ((millis() - timeSinceTheLastSound) > randomCount))
			{
				playOneRandomFileFromDirectory(turning);
				timeSinceTheLastSound = millis();
				
				randomCount = random(3) * 1000;
			}
		}
	}
	
	//Si aucun capteur n'est activé, les deux moteurs s'activent
	else if(digitalRead(IRSensorLeftPin) == HIGH && digitalRead(IRSensorRightPin) == HIGH)
	{
		Serial.println("Move forward");
		
		while(digitalRead(IRSensorLeftPin) == HIGH && digitalRead(IRSensorRightPin) == HIGH)
		{
			moveForward();
			
			//Si aucun son n'est joué et que cela fait plus de x secondes
			if(!wave.isplaying && ((millis() - timeSinceTheLastSound) > randomCount))
			{
				playOneRandomFileFromDirectory(sentinel_running);
				timeSinceTheLastSound = millis();
				
				randomCount = random(10) * 1000;
			}
		}
	}
	
	//On réinitialise les moteurs avant le prochain cycle
	releaseMotors();
}


/*
 * Définition des fonctions relatives au WaveShield
 */

//Affiche un message d'erreur et arrête le programme
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}

//Affiche un message d'erreur et arrête le programme en cas d'erreur I/O
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}

//Lit un fichier au hasard dans un dossier donné
void playOneRandomFileFromDirectory(char folderGiven[])
{
	FatReader folder;	//Dossier de lecture, sera réinitialisé à chaque appel
	
	//Ouvre le dossier du thème qui est à la racine
	if (!folder.open(root, soundTheme))
	{
		error("open the theme folder has failed");
	}
	
	//Ouvre le sous dossier spécifié
	if (!folder.open(folder, folderGiven))
	{
		error("open the given folder has failed");
	}
	
	dir_t dirBuf;
	int folderSize = 0;
	//Parcours le dossier une première fois pour compter les éléments
	while (folder.readDir(dirBuf) > 0)
	{
		folderSize ++;
	}
	
	//Réinitialise le curseur de parcours du dossier
	folder.rewind();
	
	//Choisi un nombre aléatoire qui désigne le fichier à jouer
	int randomFile = random(1, folderSize+1);	//random exclue la valeur max
	
	//Évite la répétition successive du même son
	while (randomFile == lastRandom)
	{
		randomFile = random(1, folderSize+1);
	}
	lastRandom = randomFile;
	
	int selected = 1;	//Il y a forcément au moins 1 fichier dans le random
	//Recherche dans le dossier le nième fichier sélectionné
	while (folder.readDir(dirBuf) > 0)
	{
		if (selected == randomFile)
		{
			/*
			 * Le point de l'extension n'existe pas dans le format 8.3,
			 * ensuite, en C un string doit se terminer par un "null" :
			 * il faut dont réserver la place pour 2 caractères de plus
			 */
			int fileNameLength = 2;
			
			/*
			 * Converti le nom du fichier stocké dans dirBuf.name
			 * du format 8.3 (bourrage des 8 premiers caractères avec
			 * des espaces si le nom est plus court que 8 symboles, puis
			 * 3 caractères pour l'extension, le point n'est pas stocké)
			 */
			for (int i = 0; i < 11; i++) //11 car un nom de fichier au format 8.3 a 11 caractères
			{
				if (dirBuf.name[i] == ' ')
					continue;
				else
					fileNameLength++;
			}
			
			//Recompose le nom dans un tableau de taille fileNameLength
			char fileName[fileNameLength];
			int letters = 0;	//Le tableau fileName est plus petit que dirBuf.name
			for (int i = 0; i < 11; i++)
			{
				if (dirBuf.name[i] == ' ')
					continue;
				if (i == 8)
				{
					fileName[letters] = '.';
					letters++;
				}
				fileName[letters] = dirBuf.name[i];
				letters++;
			}
			
			fileName[fileNameLength-1] = NULL;
			
			if (!file.open(folder, fileName))
			{
				error("open the file has failed");
			}
			
			break;
		}
		
		selected++;
	}
		
	if (!wave.create(file))
	{
		error("the file is not a valid wav");
	}
	Serial.println("play");
	wave.play();
}


/*
 * setup(): Fonction appelée automatiquement au début du programme
 * 
 * Tout est initialisé ici car elle n'est appelé qu'une seule fois.
 */
void setup()
{
	//Initialisation d'une liaison série pour avoir un retour en console
	//Baudrate 9600: identique à la valeur initialisée par la connexion
	Serial.begin(9600);
	
	Serial.println("Program started");
	
	//Initialisation des pins des capteurs en mode "écoute"
	pinMode(IRSensorLeftPin,INPUT);
	pinMode(IRSensorRightPin,INPUT);
	
	//Initialise la graine de random() (bruit aléatoire sur pin analog)
	randomSeed(analogRead(noiseRandomPin));
	
	//Initialisation d'une communication I²C/TWI avec le Rotoshield
	Wire.begin();
	
	//Instanciation des moteurs sur leur bornier correspondant
	Motor_1.init(3);	//Gauche
	Motor_2.init(4);	//Droite
		
	//Initialise la carte SD et affiche une erreur si cela se passe mal
	if (!card.init()) {			//lit avec 8Mhz spi (défaut, plus rapide)
	//if (!card.init(true)) {	//lit avec 4Mhz spi si 8Mhz ne marche pas
		error("Card init. failed");
	}

	//Active l'optimisation en lecture (désactiver en cas de timeout/pb)
	card.partialBlockRead(true);

	//Recherche une partition FAT
	uint8_t part;
	for (part = 0; part < 5; part++) {	//Il y a jusqu'à 5 slots à lire
	if (vol.init(card, part)) 
	  break;							//Si une partition a été trouvée
	}
	if (part == 5) {
		error("No valid FAT partition");//Sinon cela affiche une erreur
	}

	//Imprime dans la console ce qui a été trouvé
	putstring("Using partition ");
	Serial.print(part, DEC);
	putstring(", type is FAT");
	Serial.println(vol.fatType(), DEC);	//FAT16 ou FAT32

	//Essaye d'ouvrir le dossier root
	if (!root.openRoot(vol)) {
		error("Can't open root dir");	//Quelque chose s'est mal passé
	}

	//Si tout s'est correctement déroulé
	//putstring_nl("Files found (* = fragmented):");
	//Affiche le contenu de tous les dossiers de la carte SD
	//root.ls(LS_R | LS_FLAG_FRAGMENTED);
	
	//Premier test audio
	playOneRandomFileFromDirectory(opening);
	
	Serial.print("Ready in ");
	Serial.print(millis());
	Serial.println("ms");
	
	//Défini le Mode qui va être utilisé
	while(digitalRead(IRSensorLeftPin) == HIGH || digitalRead(IRSensorRightPin) == HIGH)
	{
		
		//Capteur droit activé le premier 3s : mode Stationnaire
		if(digitalRead(IRSensorRightPin) == LOW)
		{
			timeA = millis();
			timeB = millis();
			
			while(digitalRead(IRSensorRightPin) == LOW && (timeB-timeA) < 3050)
			{
				timeB = millis();
			}
			
			if((timeB-timeA) > 3000)
			{
				//Initialise la vitesse par défaut utilisée par les moteurs
				Motor_1.setSpeed(leftSlowSpeed);
				Motor_2.setSpeed(rightSlowSpeed);
				
				Serial.println("Switching to stationaryMode()");
				mode = 0;
				
				if(!wave.isplaying)
				{
					playOneRandomFileFromDirectory(stationnary_start);
					timeSinceTheLastSound = millis();
				}
				
				break;
			}
		}
		//Capteur gauche activé le premier 3s: mode Sentinelle
		if(digitalRead(IRSensorLeftPin) == LOW)
		{
			timeA = millis();
			timeB = millis();
			
			while(digitalRead(IRSensorLeftPin) == LOW && (timeB-timeA) < 3050)
			{
				timeB = millis();
			}
			
			if((timeB-timeA) > 3000)
			{
				//Initialise la vitesse par défaut utilisée par les moteurs
				Motor_1.setSpeed(leftFastSpeed);
				Motor_2.setSpeed(rightFastSpeed);
				
				//éventuellement émettre un son
				//avancer pendant 1s pour laisser à l'utilisateur le temps de retirer sa main
				
				Serial.println("Switching to sentinelMode()");
				mode = 1;
				
				if(!wave.isplaying)
				{
					playOneRandomFileFromDirectory(sentinel_start);
					timeSinceTheLastSound = millis();
				}
				
				break;
			}
		}
	}
}


/*
 * loop(): Fonction appelée après setup(), en boucle, à l'infini
 */
void loop()
{
	//Mode Stationnaire actif
	if(mode == 0)
	{
		stationaryMode();
	}
	//Mode Sentinelle actif
	if(mode == 1)
	{
		sentinelMode();
	}
}
