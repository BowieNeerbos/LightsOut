#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>     /* srand, rand */
#include <time.h>



using namespace std;

//klassen=======================================================================

class puzzel
{
public:
    puzzel(int hoogteWaarde, int breedteWaarde, char aanKeuze,
    char uitKeuze, int percentKeuze, bool torusKeuze, int penKeuze);

    puzzel();
    int hoogte, breedte;
    char aankarakter, uitkarakter;//de symbolen voor de lampen
    bool torus; //verbindt de linker- met de rechterkant vd puzzel; een ring
    bool gegenereerd; //houdt bij op welke manier de puzzel is gegenereerd
    int percentage; // het percentage lampen dat aangaat bij maakRandom
    int pen; //het gedrag van de pen bij tekenen (0,1,2)
    int moeilijkheid; //hoe veel zetten worden gedaan bij genereer

    void maakSchoon();
    void drukAf();
    void maakWereld();
    void maakRandom(); //gegenereerd = false
    void genereer(); //gegenereerd = true
    void volg();
    void laatsteRij();
    void losop();
    void zet(int, int);
    void eenlamp(int, int);
    bool check();
    bool zetofniet(int, int);
    bool cursor[26][26];
private:
    char deWereld[26][26];//max 26, zodat we in ieder geval niet voorbij z gaan
    bool oplossing[26][26];
    void binnenDeGrenzen();
};

//functies======================================================================

void infoblok(){
    cout << "Dit is LightsOut. Een puzzelspel ontworpen door "
    <<"Tiger Electronics in 1995" << endl
    << "gemaakt met de GNU GCC Compiler" << endl << "11/11/2022" << endl
    << "Bowie van Neerbos." << endl;
}//infoblok

// Pseudo-random-getal tussen 0 en 999: aangepast van dictaat.
long randomgetal ( ) {
    static long getal = rand()%100; //rand() zodat niet elke run hetzelfde doet
    getal = ( 221 * getal + 1 ) % 10000;
    return getal;
} // randomgetal

//returnt de eerste niet-enter. bij herhaald aanroepen geeft hij
//opeenvolgende losse chars uit de invoer
char eerste(){
    char invoer = '\n';
    while (invoer == '\n'){
        invoer = cin.get();
    }//while
    return invoer;
}//eerste

//geeft van een invoer (bijv "/n 6fgdh73sjiod30) een getal tussen minimum
//en maximum.
int leesGetal(int minimum, int maximum){
    int getal = 0;
    char invoer = eerste(); //skipt de enters
    while (invoer != '\n'){
        if ('0' <= invoer && invoer <= '9'){
            getal = 10 * getal + (invoer - '0');
            if (getal > maximum){getal = maximum;}
        }//if
        invoer = cin.get();
    }//while
    if (getal < minimum){getal = minimum;}
    return getal;
}//leesGetal

//checkt of de gekozen karakters voldoen (geen 0 of 1, en ongelijk aan elkaar)
void checkKarakter(char &aan, char &uit){
    if (aan == uit || aan == '1' || aan == '0' || uit == '1' || uit == '0'){
        aan = 'x'; uit = '.';
        cout << "Ongeldige invoer. Standaardkarakters worden gebruikt" << endl;
    }//if
}//checkKarakter

//doet eigenlijk niks, een zogenaamde easter egg
void lebowski(){
    cout << "Well, that's just, like, your opinion, man..." << endl;
}//lebowski

//klapt een enkele lamp om in de puzzel
void toggle(char &lamp){
    if (lamp == 'x'){lamp = 'o';}
    else if (lamp == 'o'){lamp = 'x';}
}//toggle

//klapt een enkele waarde om in de oplossing
void booltoggle(bool &lamp){
    if (lamp){lamp = false;}
    else {lamp = true;}
}//booltoggle

//lost de puzzel op mbv de oplossing, mits beschikbaar
void losop(puzzel &mijnpuzzel){
    int i, j;
    for (i=1; i < mijnpuzzel.hoogte; i++){
        for (j=1; j < mijnpuzzel.breedte; j++){
            if (mijnpuzzel.zetofniet(i,j)){//is op deze positie een zet gedaan?
                mijnpuzzel.zet(i, j);//draai deze dan terug
                mijnpuzzel.drukAf();
                cout << endl;
            }//if
        }//for
    }//for
}//losop

//het tekenen
void tekenen(puzzel &mijnpuzzel){
    int i = 1; int j = 1;
    mijnpuzzel.cursor[i][j]=true; mijnpuzzel.drukAf();
    char richting;
    bool stoppen = false;
    while(!stoppen){
        cout << "Beweeg de cursor met wasd of toets t om te stoppen. "
        << "Afhankelijk van de pen doe je lichten aan of uit." << endl;
        if (richting != 't' && richting != 'T'){
            mijnpuzzel.cursor[i][j] = false;
            richting = eerste();
            if ((richting == 's' || richting == 'S')
            && i != mijnpuzzel.hoogte - 1){i++;}
            if ((richting == 'w' || richting == 'W')
            && i != 1){i--;}
            if ((richting == 'd' || richting == 'D')
            && j != mijnpuzzel.breedte - 1){j++;}
            if ((richting == 'a' || richting == 'A')
            && j != 1){j--;}
            mijnpuzzel.cursor[i][j]=true; mijnpuzzel.eenlamp(i, j);
            mijnpuzzel.drukAf();
        }
        else{stoppen = true;}//t getypt, terug naar menu
    }
    mijnpuzzel.cursor[i][j]=false; // haalt de cursor weer van de puzzel
}

//doe zetten tot de puzzel is opgelost, of stop eerder met Z
bool puzzelen(puzzel &mijnpuzzel){
    int letter, getal;
    static int zetten = 0;
    bool stoppen = false;
    while (!stoppen){
        bool foutje = false; //voert geen zet uit als er een invoerfout is
        cout << "Selecteer een lamp, of typ \"Z\" om terug te gaan." << endl;
        letter = eerste() - 'a' + 1; //geeft de kolom
        if (letter == ('Z' - 'a' + 1)||letter == ('z' - 'a' + 1))
            {stoppen = true; return false;}
        if (letter >= mijnpuzzel.breedte || letter <= 0){
            letter = letter + 'a' - 'A';
            if (letter >= mijnpuzzel.breedte || letter <= 0){
                cout << "Ongeldige invoer. Voer bestaande coordinaten in. "
                << " Deze staan om de puzzel heen. Bijv: A3 of d5 (of k18?)."
                << endl;
                foutje = true;
                }//if
            }//if
        if (!foutje){
            getal = leesGetal(1, mijnpuzzel.hoogte - 1);
            char kolom = letter + 'A' - 1;
            cout << "gekozen zet: " << kolom << getal << endl ;
            mijnpuzzel.zet(getal, letter);
            mijnpuzzel.drukAf();
            zetten++; cout << "zet " << zetten << endl;
        }//if
        if (mijnpuzzel.check()){
            cout << "Gefeliciteerd, u heeft de puzzel opgelost" << endl;
            return true;
        }//if
    }//while
    return false;
}//puzzelen

//het puzzel submenu
void puzzelMenu(puzzel &mijnpuzzel){
    mijnpuzzel.drukAf();
    bool stoppen = false;
    while(!stoppen){// stopt pas als de gebruiker dit wil, of wanneer opgelost
        cout << "Kies een van de volgende opties:" << endl
        << "[T]erug, [P]uzzelen, [L]os op, [C]hasing en [V]olg." << endl;
        char keuze; cin >> keuze;
        switch (keuze){
            case 'p': case 'P':
                //gaat naar een submenu waar het puzzelen plaatsvindt
                if (puzzelen(mijnpuzzel)){stoppen = true;}
                break;
            case 'l': case 'L':
                if (!mijnpuzzel.gegenereerd){
                    cout << "Dit werkt alleen voor puzzels die met de optie "
                    << "\"genereer\" zijn gemaakt uit het tekenmenu." << endl
                    << "Probeer optie \"C\". " << endl; break;
                }//if
                losop(mijnpuzzel); stoppen = true; break;
            case 'v': case 'V':
                mijnpuzzel.volg(); break;
            case 'c': case 'C':
                if (mijnpuzzel.hoogte != 6 || mijnpuzzel.breedte != 6
                || mijnpuzzel.torus){
                    cout << "Dit kan alleen voor een standaard 5x5 puzzel!"
                    << endl;
                    break;
                    }//if
                mijnpuzzel.volg(); mijnpuzzel.laatsteRij(); mijnpuzzel.volg();
                stoppen = true; break;
            case 't': case 'T':
                stoppen = true; break;
            default:
                cout << "ongeldige keuze \n" << keuze << endl; break;
        }//switch
    }//while
}//puzzelMenu

//het submenu waar een selectie eigenschappen van de puzzel kan worden aangepast
void parameterMenu(puzzel &mijnpuzzel){
    bool stoppen=false;
    while(!stoppen){//stopt pas als de gebruiker klaar is met aanpassen
        cout << "Kies een van de volgende opties:" << endl
        << "[T]erug, [D]imensies, [K]arakters, t[O]rus,"
        << " pe[R]centage en [P]en." << endl;
        char keuze; cin >> keuze;
        switch(keuze){
            case 'K': case 'k':
                cout << "Kies het karakter voor een brandende lamp"
                << " (geen 0 of 1):" << endl;
                mijnpuzzel.aankarakter = eerste();
                cout << "Kies een ander karakter voor een niet-brandende lamp"
                << " (geen 0 of 1):"<< endl;
                mijnpuzzel.uitkarakter = eerste();
                checkKarakter(mijnpuzzel.aankarakter, mijnpuzzel.uitkarakter);
                break;
            case 'D': case 'd':
                cout << "Kies de hoogte van het speelveld:"
                << " (3 - 20):" << endl;
                mijnpuzzel.hoogte = leesGetal(3, 20)+1;
                cout << "Kies de breedte van het speelveld"
                << " (3 - 20):"<< endl;
                mijnpuzzel.breedte = leesGetal(3, 20)+1; break;
            case 'R': case 'r':
                cout << "Zet het percentage lampen dat aangaat bij een random"
                << " speelveld (0-100):" << endl;
                mijnpuzzel.percentage = leesGetal(0,100); break;
            case 'O': case 'o':
                if (mijnpuzzel.torus){
                mijnpuzzel.torus = false;
                cout << "Torusmode uit" << endl;
                }
                else {
                mijnpuzzel.torus = true;
                cout << "Torusmode aan" << endl;
                }
                break;
            case 'P': case 'p':
                mijnpuzzel.pen++;
                if (mijnpuzzel.pen == 3){mijnpuzzel.pen = 0;}
                cout << "Penstand is nu " << mijnpuzzel.pen << endl;
                break;
            case 'T': case 't':
                stoppen = true; break;
            default:
                cout << "ongeldige keuze \n" << keuze << endl; break;
        }//switch
    }//while
    mijnpuzzel.maakWereld();//maakt een nieuwe wereld met de parameters
    mijnpuzzel.maakRandom();
}//parameterMenu

//het menu waar de gebruiker (zelf of automatisch) een nieuwe puzzel kan maken
void tekenMenu(puzzel &mijnpuzzel){
    bool stoppen = false;
    int moeilijk = 1;
    while (!stoppen){
        cout << "Kies een van de volgende opties:" << endl
        << "[T]erug, [R]andom, [G]enereer, t[O]ggle en [S]choon." << endl;
        char keuze; cin >> keuze;
        switch (keuze){
            case 't': case 'T':
                stoppen = true; cout << "stoppen \n"; break;
            case 'g': case 'G':
                cout << "kies moeilijkheidsgraad (een getal >= 1)" << endl;
                moeilijk = leesGetal(1,999);
                mijnpuzzel.moeilijkheid = moeilijk;
                mijnpuzzel.maakSchoon();
                mijnpuzzel.genereer();
                mijnpuzzel.drukAf();
                mijnpuzzel.gegenereerd = true;
                break;
            case 'r': case 'R':
                mijnpuzzel.maakRandom();//maakSchoon is hier niet nodig
                mijnpuzzel.drukAf();
                mijnpuzzel.gegenereerd = false;
                break;
            case 'o': case 'O':{
                tekenen(mijnpuzzel);
                break;
                }
            case 's': case 'S':
                mijnpuzzel.maakSchoon(); break;
            default:
                cout << "ongeldige keuze \n" << keuze << endl; break;
        }//switch
    }//while
}

//het hoofdmenu
void menu(puzzel &mijnpuzzel){
    bool stoppen = false;
    while (!stoppen){
        cout << "Kies een van de volgende opties:" << endl
        << "[T]ekenen, [P]uzzelen, para[M]eters en [S]toppen." << endl;
        char keuze; keuze = cin.get();
        switch (keuze){
            case 'm': case 'M':
                cout << "parameters \n"; parameterMenu(mijnpuzzel); break;
            case 'p': case 'P':
                cout << "puzzel \n"; puzzelMenu(mijnpuzzel); break;
            case 't': case 'T':
                cout << "tekenen \n"; tekenMenu(mijnpuzzel); break;
            case 's': case 'S':
                cout << "stoppen \n"; stoppen = true; break;
            case 'l': case 'L':
                lebowski(); break;
            default:
                cout << "ongeldige keuze \n" << keuze << endl; break;
        }//switch
    }//while
}//menu

//main==========================================================================

int main(){
    infoblok();
    srand(time(0)); //dit voorkomt dat rand() bij elke run hetzelfde geeft
    puzzel mijnpuzzel;
    mijnpuzzel.maakWereld();
    mijnpuzzel.maakRandom();
    menu(mijnpuzzel);
    cout << "Bedankt voor het spelen en tot ziens!";
    return 0;
}//main

//constructors==================================================================

//de constructor met gebruikersopties
puzzel::puzzel(int hoogteWaarde, int breedteWaarde, char aanKeuze,
char uitKeuze, int percentKeuze, bool torusKeuze, int penKeuze):
hoogte(hoogteWaarde), breedte(breedteWaarde), aankarakter(aanKeuze),
uitkarakter(uitKeuze), percentage(percentKeuze), torus(torusKeuze),
pen(penKeuze)
{
    binnenDeGrenzen();
}

//de standaard constructor
puzzel::puzzel(): hoogte(6), breedte(6), aankarakter('x'), uitkarakter('.'),
percentage(30), torus(false), gegenereerd(false), pen(0)
{}

//memberfuncties================================================================

//puzzel moet tussen 3x3 (4 vakjes en coordinaten) en 26x26 zitten
void puzzel::binnenDeGrenzen()
{
    if ((hoogte < 3) || (hoogte > 26) || (breedte < 3) || (breedte > 26)){
        cout << "Gekozen dimensies vallen niet binnen de grenzen" << endl;
        exit(1);
    }//if
}//puzzel::binnenDeGrenzen

void puzzel::drukAf()
{
    int i, j;//nog iets met cursor?
    for ( i = 0; i < hoogte; i++ ) {//skip de randen, i=1
        for ( j = 0; j < breedte; j++ ) {//idem
            if ( deWereld[i][j] == 'o' && cursor[i][j])
                cout << " " << '0';
            else if ( deWereld[i][j] == 'x' && cursor[i][j] )
                cout << " " << '1';
            else if ( deWereld[i][j] == 'x')
                cout << " " << aankarakter; // <== later "aankarakter"
            else if ( deWereld[i][j] == 'o' )
                cout << " " << uitkarakter;
            else if ( (deWereld[i][j] - '0') >= 10
            && (deWereld[i][j] - '0') < 20 && j == 0)
                cout << "1" << deWereld[i][j] - '0' - 10;
            else if ( (deWereld[i][j] - '0') >= 20
            && (deWereld[i][j] - '0') < 27 && j == 0)
                cout << "2" << deWereld[i][j] - '0' - 20;
            else
                cout << " " << deWereld[i][j];
        }//for j
        cout << endl;
    }//for i
}//puzzel::drukaf

//Maakt de wereld/puzzel, inclusief de coordinaten
void puzzel::maakWereld()
{
    int i, j;
    for ( i = 0; i < hoogte; i++ ) {
        for ( j = 0; j < breedte; j++ ) {
            if (i == 0 && j == 0){
                deWereld[i][j] = ' '; //de hoek is leeg
            }
            else if (i == 0){
                char letter = j + 'A' - 1;//letter as
                deWereld[i][j] = letter;
            }
            else if (j == 0){
                char getal = i + '1' - 1;//getallen as
                deWereld[i][j] = getal;
            }
            else{
                deWereld[i][j] = 'o';//lampen (uit)
            }
            oplossing[i][j] = false;
            cursor[i][j] = false;
        }//for
    }//for
}//puzzel::maakWereld

//Zet alle lampen uit. Zelfde effect als maakWereld, maar iets sneller.
void puzzel::maakSchoon()
{
    int i, j;
    for ( i = 1; i < hoogte; i++ ) {//skip de randen, i=1
        for ( j = 1; j < breedte; j++ ) {//idem
            deWereld[i][j] = 'o';
            oplossing[i][j] = false;
        }//for
    }//for
}//puzzel::maakSchoon

// Zet "random" een gedeelte van de lampen aan
void puzzel::maakRandom()
{
    int percent = percentage;
    int i, j;
    for ( i = 1; i < hoogte; i++ ) {//skip de coordinaatassen, i=1
        for ( j = 1; j < breedte; j++ ) {//idem
            int TrueFalse = (rand() * randomgetal())%100;//* zie verslag
            if (TrueFalse <= percent){
                deWereld[i][j] = 'x';
            }
            else {deWereld[i][j] = 'o';}
        }
    }
    gegenereerd = false;
}//puzzel::maakRandom

//genereer een puzzel op basis van zetten, ipv individuele lampen
void puzzel::genereer()
{
    int stap, i, j;
    for (stap=1; stap <= moeilijkheid; stap++){
        i = (rand() * randomgetal())%(hoogte-1) + 1;//* zie verslag
        j = (rand() * randomgetal())%(breedte-1) + 1;//*
        zet(i, j);
    }//for
    gegenereerd = true;
}//puzzel::genereer

//algoritme dat alle rijen behalve de onderste uit zet
void puzzel::volg()
{
    int i, j;
    for (i = 1; i < (hoogte-1); i++){
        for ( j = 1; j < breedte; j++ ) {
            if (deWereld[i][j]=='x'){
                zet(i+1,j);
                drukAf();
            }//if
        }//for
    }//for
}//puzzel::volg

//lost, icm volg, de puzzel op dmv "light chasing". alleen voor 5x5 puzzels!
void puzzel::laatsteRij()
{
    if(deWereld[5][1] == 'x' && deWereld[5][2] == 'x' && deWereld[5][3] == 'x'
    && deWereld[5][4] == 'o' && deWereld[5][5] == 'o'){
        zet(1,2); drukAf();}
    else if(deWereld[5][1] == 'x' && deWereld[5][2] == 'x'
    && deWereld[5][3] == 'o' && deWereld[5][4] == 'x' && deWereld[5][5] == 'x'){
        zet(1,3); drukAf();}
    else if(deWereld[5][1] == 'x' && deWereld[5][2] == 'o'
    && deWereld[5][3] == 'x' && deWereld[5][4] == 'x' && deWereld[5][5] == 'o'){
        zet(1,5); drukAf();}
    else if(deWereld[5][1] == 'x' && deWereld[5][2] == 'o'
    && deWereld[5][3] == 'o' && deWereld[5][4] == 'o' && deWereld[5][5] == 'x'){
        zet(1,2); drukAf(); zet(1,1); drukAf();}
    else if(deWereld[5][1] == 'o' && deWereld[5][2] == 'x'
    && deWereld[5][3] == 'x' && deWereld[5][4] == 'o' && deWereld[5][5] == 'x'){
        zet(1,1); drukAf();}
    else if(deWereld[5][1] == 'o' && deWereld[5][2] == 'x'
    && deWereld[5][3] == 'o' && deWereld[5][4] == 'x' && deWereld[5][5] == 'o'){
        zet(1,1); drukAf(); zet(1,4); drukAf();}
    else if(deWereld[5][1] == 'o' && deWereld[5][2] == 'o'
    && deWereld[5][3] == 'x' && deWereld[5][4] == 'x' && deWereld[5][5] == 'x'){
        zet(1,4); drukAf();}
    else if(deWereld[5][1] == 'o' && deWereld[5][2] == 'o'
    && deWereld[5][3] == 'o' && deWereld[5][4] == 'o' && deWereld[5][5] == 'o'){
        cout << "reeds opgelost!" << endl;}
    else {cout << "onoplosbaar" << endl;}
}//puzzel::laatsteRij

//checkt of de puzzel is opgelost
bool puzzel::check()
{
    int i, j;
    for ( i = 1; i < hoogte; i++ ) {//skip de randen, i=1
        for ( j = 1; j < breedte; j++ ) {//idem
            if (deWereld[i][j] == 'x'){return false;} //brandende lamp gevonden
        }//for
    }//for
    return true;
}//puzzel::check

//voert een zet uit, en klapt geselecteerd vakje in een pluspatroon om
void puzzel::zet(int i, int j)
{
    booltoggle(oplossing[i][j]);//houdt de zetten bij in oplossing
    toggle(deWereld[i][j]);
    toggle(deWereld[i+1][j]);
    toggle(deWereld[i-1][j]);
    if( !torus){
        toggle(deWereld[i][j+1]);
        toggle(deWereld[i][j-1]);
    }//if
    else{//buitenste kolommen hebben effect op overkant in torusmode
        if (j == 1){
            toggle(deWereld[i][j+1]);
            toggle(deWereld[i][breedte - 1]);
        }
        else if (j == (breedte - 1) ){
            toggle(deWereld[i][j-1]);
            toggle(deWereld[i][1]);
        }
        else {
            toggle(deWereld[i][j-1]);
            toggle(deWereld[i][j+1]);
        }
    }//else
}//puzzel::zet

//verandert de lampen bij het tekenen
void puzzel::eenlamp(int i, int j)
{
    if (pen == 0){toggle(deWereld[i][j]);}
    if (pen == 1){deWereld[i][j] = 'x';}
    if (pen == 2){deWereld[i][j] = 'o';}
    gegenereerd = false;
}//puzzel::eenlamp

// wordt gebruikt bij het genereren van de oplossing. omdat deze private is
bool puzzel::zetofniet(int i, int j){
    return oplossing[i][j];
}//puzzel::zetofniet
