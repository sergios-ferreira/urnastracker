/*
   USO DA TECNOLOGIA RFID NA IDENTIFICAÇÃO, ARMAZENAMENTO, 
             CONTROLE E MOVIMENTAÇÃO DE URNAS ELETRÔNICAS.             
*/

// --- Bibliotecas ---
#include <LiquidCrystal.h>   
#include <SoftwareSerial.h>

SoftwareSerial RFID(2, 3); // RX e TX              
LiquidCrystal lcd(5, 6, 7, 8, 9, 10, 11);  //RS R/W ENABLE D4 D5 D6 D7
//PIN PLACA  4,5,6 - 11,12,13,14// RS RW E D4 D5 D6 D7

// --- Variáveis Globais ---
int dados = 0;
int ok    = -1;
int LED   = 13;
int count = 0;

/* Códigos das Etiquetas RFID - Dados já catalogados */ 
int tag1[14] = {2,48,53,48,48,66,68,52,55,50,69,68,49,3};
int tag2[14] = {2,48,54,48,48,48,67,51,57,49,70,50,67,3};
int tag3[14] = {2,48,53,48,48,67,55,65,49,51,49,53,50,3};
/* Vetor para comparação com as etiquetas anteriores */
int newtag[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
int lasttag[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 

// --- Protótipo das Funções ---

int freeRam(void);
boolean compararTag(int aa[14], int bb[14]);
void checarMinhasTags();
void lerTags();
void imprimeLCD(int numeroUrna);


//----------  CONFIGURAÇÕES INICIAIS  ----------- //  
void setup()
{
  RFID.begin(9600);    
  Serial.begin(9600);  
  lcd.begin(16,2);
  
  while (!Serial) { /* Espera a porta serial conectar, usado apenas quando coloca-se o cabo USB */ }
  
  Serial.println(F("\t..: INICIALIZANDO RFID SOFTWARE :.."));
  Serial.print(F("\tCompilado em:  "));          Serial.print(__TIME__);  Serial.print(F(", "));      Serial.println(__DATE__);
  Serial.print(F("\tMemoria Dinamica Livre: ")); Serial.print(freeRam()); Serial.println(F(" bytes"));   
  Serial.println(F(""));
  Serial.println(F("Inicio RFID"));
  Serial.println(F("Inicio Serial"));
  Serial.println(F("Inicio LCD"));
  Serial.println(F("============="));
    
  pinMode(LED, OUTPUT); // LED 
  

  lcd.clear();            //Limpa LCD
  lcd.setCursor(0,0);    
  lcd.print("[ Urnas Tracker]");   
  lcd.setCursor(0,1);    
  lcd.print("[     v1.0     ]");   
}

void loop()
{
  lerTags();
}

//---------- PROCEDIMENTOS E FUNÇÕES ----------- //  

/* Exibir memória RAM disponível no Setup */
int freeRam(void) {
  extern int  __bss_end, *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  } 
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
}

boolean compararTag(int aa[14], int bb[14])
{
  boolean ff = false;
  int fg = 0;
  for (int cc = 0 ; cc < 14 ; cc++)
  {
    if (aa[cc] == bb[cc])
    {
      fg++;
    }
  }
  if (fg == 14)
  {
    ff = true;
  }
  return ff;
}

void checarMinhasTags() // compara cada tag com a tag que acabou de ler
{
  ok = 0; 
  // OK == 1 .:. temos uma correspondência. 
  // OK == 0 .:. zero é uma leitura, mas nenhuma correspondência.
  // OK ==-1 .:. não é houve nenhuma tentativa de leitura.

  if (compararTag(newtag, tag1) == true)
  {
    ok++; imprimeLCD(1);
  }
  else if (compararTag(newtag, tag2) == true)
  {
    ok++; imprimeLCD(2);
  }
  else if (compararTag(newtag, tag3) == true)
  {
    ok++; imprimeLCD(3);
  }
    
    // Copiar a tag atual como última tag utilizada
    for (int z = 0 ; z < 14 ; z++) 
    {
      lasttag[z] =newtag[z];
    }
}

void lerTags()
{
  ok = -1;

  if (RFID.available() > 0) 
  {    
    delay(100); // Não modificar - Necessário para leitura do buffer da porta serial.

    for (int i = 0 ; i < 14 ; i++) // leitura de uma TAG
    {
      dados = RFID.read();
      newtag[i] = dados;   
      //Serial.print(dados); //Debug
    }
    //Serial.println(F(""));
    RFID.flush(); // Parar multiplas leituras

   if( compararTag(lasttag, newtag) == false) {    
    checarMinhasTags();
   } else {
    lcd.clear();            //Limpa LCD
    lcd.setCursor(0,0);    
    lcd.print("[ Urnas Tracker]");   
    lcd.setCursor(0,1);    
    lcd.print("Contador:");      
    lcd.setCursor(10,1); 
    lcd.print(count);   

    Serial.print(F("Contagem de Urnas  : ")); Serial.println(count);
    Serial.println(F("============="));
   }
    
  }

  /*if (ok > 0) // Quanto é encontrado...
  {
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);    
  }
  else*/ if (ok == 0) // Não encontrado...
  {
    ok = -1;
  }  
}
//--------------------------------------------------------------------------
void imprimeLCD(int numeroUrna)
{
    char urna[4],zona[4], secao[5], palete[4];
    
    if(numeroUrna==1)
    {
      strcpy(urna, "001");strcpy(zona, "111");strcpy(secao, "0001");strcpy(palete, "001");
    } else if(numeroUrna==2) {
      strcpy(urna, "002");strcpy(zona, "222");strcpy(secao, "0002");strcpy(palete, "002");
    } else if(numeroUrna==3) {
      strcpy(urna, "003");strcpy(zona, "333");strcpy(secao, "0003");strcpy(palete, "003");
    } else {          
      Serial.println(F("[URNA ZZZ] - Nao identificada"));
    }    
    Serial.print(F("Urna  : ")); Serial.println(urna);
    Serial.print(F("Zona  : ")); Serial.println(zona);
    Serial.print(F("Secao : ")); Serial.println(secao);    
    Serial.print(F("Palete: ")); Serial.println(palete);       
    Serial.println(F("============="));
    
    lcd.clear();            //Limpa LCD
    lcd.setCursor(0,0);     //posiciona cursor na coluna 1 linha 1
    lcd.print("ID :");        
    lcd.setCursor(4, 0);
    lcd.print(urna);
    lcd.setCursor(9, 0);
    lcd.print("ZON:"); 
    lcd.setCursor(13, 0);
    lcd.print(zona); 
    
    lcd.setCursor(0, 1);
    lcd.print("SES:");
    lcd.setCursor(4, 1);
    lcd.print(secao);
    lcd.setCursor(9, 1);
    lcd.print("PLT:");
    lcd.print(palete);
    
    digitalWrite(LED, HIGH);
    delay(8000);
    digitalWrite(LED, LOW);   
    count++;
    lcd.clear();            //Limpa LCD
}
