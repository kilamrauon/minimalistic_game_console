/*
 * Missile command ATTINY85
 * just a minimalistic game console with:
 * an Attiny85
 * a 5 button keyboard from keyes : ref : Keyer-AD-key - KB45037
 * a 8x7 led segment max7219
 * a buzzer (optional), just for fun and because I had one pin left on the Attiny
 * 
 * How to play
 * turn on the console
 * for 2 seconds , it displays the high score
 * 
 * the game begins straight
 * SW1 : go left
 * SW2 and SW3 : go center
 * SW4 : go right
 * SW5 : shoot
 * 
 * 1 to 3 aliens are going from top to bottom
 * if it reaches the bottom , you lose 1 shield (4 shields at the beginning)
 * the shields are represented by 4 dashes at the bottom, if you lose the 4 shields, 
 * you die when the next alien reaches the bottom 
 * 
 * aliens speed is increasing during the game and when you reach some levels, 2 to 3 aliens are going down.
 * 
 * when you lose, the score is displayed for 2 seconds and then switches back and forth 
 * to the high score.
 * The high score is saved to the eeprom
 * 
 * 
 * To play again you need to push a button for a few seconds.
 * 
 * 
 * Kilam Rauon
 */

/*
    8 Digits LED Seven Segment Display Module based on MAX7219 using Arduino IDE

    updated by Ahmad Shamshiri for Robojax
    On Monday Sep 16, 2019 in Ajax, Ontario, Canada
    You can get this code from Robojax.com

    Original Libary source: https://github.com/ozhantr/DigitLedDisplay

    Watch video instruction for this video: https://youtu.be/R5ste5UHmQk
    At the moments, it doesn't display decimal points. It needs a little work to make it work

  You can get the wiring diagram from my Arduino Course at Udemy.com
  Learn Arduino step by step with all library, codes, wiring diagram all in one place
  visit my course now http://robojax.com/L/?id=62

  If you found this tutorial helpful, please support me so I can continue creating
  content like this. You can support me on Patreon http://robojax.com/L/?id=63
  or make donation using PayPal http://robojax.com/L/?id=64

   Code is available at http://robojax.com/learn/arduino

   This code is "AS IS" without warranty or liability. Free to be used as long as you keep this note intact.
   This code has been download from Robojax.com
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see .


*/
#include "DigitLedDisplay.h"
#include "EEPROM.h" //must be in the source directory, not included in the Attiny85 library

/* Arduino Pin to Display Pin
   7 to DIN,
   6 to CS,
   5 to CLK */
#define DIN PB1
#define CS PB3
#define CLK PB4
#define b A1
DigitLedDisplay ld = DigitLedDisplay(DIN, CS, CLK);


int tg = B11000000, tm = B10000001, td = B10001000, tir = B10000000; // sprite pour tir droit, milieu, gauche

int sg = B01000000, sm = B00000001, sd = B00001000; // sprite pour alien ou vaisseau droit, milieu, gauche sans tir
int sprite[] = { sg, sm, sd}; // positionne le sprite sur le x

int vie[] = {0, B00000010, B00000110, B00010110, B00110110}; //nb vie à l'affichage
int xj = 1; // x du joueur 0 (gauche),1 droite ou 2 gauche
int yj = 8; // y du joueur , part du bas
int tir_ok = 0; //on tire?
int nb_vie = 4; // 5 vies
int temps_deplacement = 100; //100 millis pour le deplacement
unsigned long  temps_joueur;
unsigned long temps_tir;
int nb_aliens =1 ;
//const aliens_max =2;
int xa1, ya1,xa2,ya2,xa3,ya3; //coordonnées des aliens
unsigned long  ta1,ta2,ta3; //temps deplacement alien x
int alien1,alien2,alien3;
int vitesse_alien1 = 500; //1/2 seconde pour déplacer les aliens au depart
int vitesse_alien2 = 500;
int vitesse_alien3 = 500;
//int alien[][][][];//alien numero a, temps_aliens, x,y en cours

int score = 0;
int niveau = 0;
int best;

const int buzzer = PB0;
bool buzzerState = LOW;
bool dd = 0;//est-ce qu'on a dejà déplacé l'alien 2


void touche()
{
  for(int i=0;i<10;i++)
   {
   digitalWrite(buzzer,HIGH);
  delay(20);
  
  digitalWrite(buzzer,LOW);
   delay(5);
  }
}

void setup() {

  /* Set the brightness min:1, max:15 */
  ld.setBright(6);

  /* Set the digit count */
  ld.setDigitLimit(8);
  pinMode(b, INPUT);

  pinMode(buzzer, OUTPUT);
  best =  EEPROM.get(0, best);
    ld.clear();
      ld.printDigit(best);
      delay(2000);
}

void loop() {



  //delay(500);
  ld.clear();
  int bouton = analogRead (b); // read the value from the sensor
 //pour debugger et connaitre les plage de dlecture pour les boutons 
// ld.printDigit(bouton);
// digitalWrite(buzzer, HIGH);


 // on deplace le joueur toutes les temps_deplacement ms
  if (millis() - temps_joueur > temps_deplacement )
  {
      
    //appui bouton gauche
    if (bouton < 10)
    {
      //ld.write(5, vie [0]|tg); //aller à gauche
      xj--;
      if (xj < 0) xj = 0;
    }

    //appui sur le bouton droit
    if (bouton > 100 && bouton < 200)
    {
      //ld.write(5, vie [1]|td); //aller à droite
      xj++;
      if (xj > 2) xj = 2;
    }

    //millieu

    if (bouton > 10 && bouton < 100)
    {
      //ld.write(5, vie[2]|tm);//aller au milieu
      xj = 1;
    }


    // tir
    if (bouton > 300 && bouton < 400)
    {
      //ld.write(5,vie[3] |tir);//bouton tir
      tir_ok = tir;

    }
    temps_joueur = millis();
  }

  //on a tiré?
  if (tir_ok && millis() - temps_tir >100)
{
  yj--; //on lance le tir
  //le tir arrive en haut 
  if (yj <= 0) {
    tir_ok = 0;
    yj = 8;
  }
  temps_tir = millis();
 }
  
  ld.write (8, vie[nb_vie] | sprite[xj] * (yj == 8) | tir_ok); // affiche la ligne 8  avec le nombre de vies, s'il y a un tir et eventuellement le joueur
  if (yj <8) ld.write (yj, sprite[xj]|tir); //on affiche le tir 

  //alien
  
 // on lance un alien 1?
 if (rand()%10 > 5 && alien1 == 0) 
 {
  alien1 = 1;
  ta1 = 0;
  xa1 = rand()%3;
  ya1 = 0;
 }

//on touche l'alien ?

   if (ya1 == yj && xa1 == xj && alien1 == 1)
   {
    score = score+ya1;
    tir_ok = 0;
      alien1 = 0;
  ya1 = 0;
  yj = 8;

  //buzzer
 
  digitalWrite(buzzer,HIGH);
  delay(100);
  
  digitalWrite(buzzer,LOW);
   }
   
//on déplace l'alien
 if (alien1 && (millis() - ta1 > vitesse_alien1))
  {
      ya1++;
//on arrive à la base?
if (  ya1 > 7)
 { nb_vie--;
   alien1 = 0;
  ya1 = 0;
  touche();
  }
 ta1 = millis();
  }
   //on affiche l'alien
    ld.write( ya1, sprite[xa1]);
// fin alien 1


// on lance un alien 2?
if (nb_aliens >=2)
{
 if (rand()%10 > 8 && alien2 == 0) 
 {
  alien2 = 1;
  ta2 = 0;
  xa2 = rand()%3;
  ya2 = 0;
 }

//on touche l'alien ?

   if (ya2 == yj && xa2 == xj && alien2 == 1)
   {
    score = score+ya1;
    tir_ok = 0;
      alien2 = 0;
  ya2 = 0;
  yj = 8;
dd =0;
  //buzzer
 
  digitalWrite(buzzer,HIGH);
  delay(100);
  
  digitalWrite(buzzer,LOW);
   }
   
//on déplace l'alien
 if (alien2 && (millis() - ta2 > vitesse_alien2))
  {
      ya2++;
      // on deplace l'alien à gauche ou droite
if (rand()%1000 > 950 /*&& !dd */ )
{
  if  (rand()%100 > 50 ) 
  {
    xa2++;
    if (xa2>2) xa2=2;
  }
  else {
    xa2--;
    if (xa2<0) xa2=0;
  }
  dd = 1;
}

//on arrive à la base?
if (  ya2 > 7)
 { nb_vie--;
   alien2 = 0;
  ya2 = 0;
  dd =0;
  touche();
  }
 ta2 = millis();
  }
   //on affiche l'alien
    ld.write( ya2, sprite[xa2]);
}
// fin alien 2

    // on lance un alien 3?
if (nb_aliens ==3)
{
 if (rand()%1000 > 995 && alien3 == 0) 
 {
  alien3 = 1;
  ta3 = 0;
  xa3 = rand()%3;
  ya3 = 0;
 }

//on touche l'alien ?

   if (ya3 == yj && xa3 == xj && alien3 == 1)
   {
    score = score+ya1;
    tir_ok = 0;
      alien3 = 0;
  ya3 = 0;
  yj = 8;

  //buzzer
 
  digitalWrite(buzzer,HIGH);
  delay(100);
  
  digitalWrite(buzzer,LOW);
   }
   
//on déplace l'alien
 if (alien3 && (millis() - ta3 > vitesse_alien3))
  {
      ya3++;
//on arrive à la base?
if (  ya3 > 7)
 { nb_vie--;
   alien3 = 0;
  ya3 = 0;
  touche();
   
 }
 ta3 = millis();
  }
   //on affiche l'alien
    ld.write( ya3, sprite[xa3]);
}
// fin alien 3
  //plus de vie

  

//augmentation du niveau
if (score > niveau*10) 
{
  niveau++;
  vitesse_alien1 = vitesse_alien1 - 10;
  if (vitesse_alien1 <= 200) vitesse_alien1 = 200;

  if (nb_aliens >=2)
  {
    vitesse_alien2 = vitesse_alien2 - 5;
  if (vitesse_alien2 <= 200) vitesse_alien2 = 200;
  }

  if (nb_aliens == 3)
  {
    vitesse_alien3 = vitesse_alien3 - 5;
  if (vitesse_alien3 <= 200) vitesse_alien3 = 200;
  }
}

if(score >50) nb_aliens=2;
if(score >300) nb_aliens=3;
  
  if(nb_vie <0)
  {
      ld.clear();
      ld.printDigit(score);
      delay(2000);
      nb_vie = 4;
      yj = 8;
      xj = 1;
      tir_ok = 0;
      
    
  niveau = 0;
 temps_deplacement = 100; //100 millis pour le deplacement
temps_tir = 0;
temps_joueur = 0;
 nb_aliens = 1 ;
 ta1 = 0;
 alien1 = 0;
  ya1 = 0;
  ta2 = 0;
 alien2 = 0;
  ya2 = 0;
  ta3 = 0;
 alien3 = 0;
  ya3 = 0;

  dd =0;


vitesse_alien1 = 500; //1/2 seconde pour déplacer les aliens au depart
vitesse_alien2 = 500;
vitesse_alien3 = 500;
if (score > best) 
{
  best=score;
  EEPROM.put(0, best);
}
      // adc_key_in = analogRead (0); // read the value from the sensor
      bouton = analogRead (b);
     //on attend un appui sur le bouton de tir pour recommencer
     while( bouton > 800)
     {
      bouton = analogRead (b);
     delay(500);
       ld.clear();
     ld.printDigit(score);
     delay(500);
       ld.clear();
     ld.printDigit(best);
     }
     score = 0;
  }
 //  pour info sur les différents affichages possibles 
  /* for (int i=0; i<255;i++)
    {
    for (int j = 0; j<9; j++)
    {
      ld.write(j,i+j);
      delay(40);
    }
    delay(40);
    }*/


  /* Prints data to the display */
  /*ld.printDigit(12345678);
    delay(3000);
    ld.clear();

    ld.printDigit(22222222);
    delay(500);
    ld.clear();

    ld.printDigit(44444444);
    delay(500);
    ld.clear();

    for (int i = 0; i < 100; i++) {
    ld.printDigit(i);

    /* Start From Digit 4 */
  /*   ld.printDigit(i, 4);
     delay(50);


    }

    for (int i = 0; i <= 10; i++) {
     /* Display off */
  /*   ld.off();
     delay(150);

     /* Display on */
  /*   ld.on();
     delay(150);
    }

    /* Clear all display value */
  /* ld.clear();
    delay(500);

    for (long i = 0; i < 100; i++) {
     ld.printDigit(i);
     delay(25);
    }*/

  /* for (int i = 0; i <= 20; i++) {
     /* Select Digit 5 and write B01100011 */
  /*  ld.write(1, B11000000);
    delay(200);

    /* Select Digit 5 and write B00011101 */
  /*  ld.write(1, B11000000);
     delay(200);
        ld.write(2, B10001000);
     delay(200);
        ld.write(3, B10000001);
     delay(200);
        ld.write(5, B01001000);
     delay(200);
        ld.write(6, B00000100);
     delay(200);
        ld.write(7, B00000010); delay(200);
           ld.write(8, B00001001);
     delay(200);*/
  // }

  /* Clear all display value */
  //  ld.clear();
  // delay(500);

  /*ld.printDigit(2019, 3);
    delay(4000);*/

}// loop ends
