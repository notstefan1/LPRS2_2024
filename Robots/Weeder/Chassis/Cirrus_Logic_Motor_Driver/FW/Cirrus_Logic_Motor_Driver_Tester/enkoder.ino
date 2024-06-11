#define BELA A1
#define ZELENA A0

int bela = 0;
int bela_pre = 0;
int counterR = 0;
int counterL = 0;
int angleL = 0;
int angleR = 0;

#define GUARD 50

void setup() {
  Serial.begin(115200);
  pinMode(BELA,INPUT_PULLUP);
  pinMode(ZELENA,INPUT_PULLUP);
}

void loop() {
  bela = digitalRead(BELA);
  
  if(bela != bela_pre && bela == 1){
    if(digitalRead(ZELENA)){
      if(angleL >= GUARD) angleR = 0;
      angleL++;
      if(angleL >= 1000) {
        counterL++;
            Serial.println(counterL);

        angleL = 0;
      }
    }else{
      if(angleR >= GUARD) angleL = 0;
      angleR++;
      if(angleR >= 1000) {
        counterR++;
            Serial.println(counterR);

        angleR = 0;
      }
    }   
  }
  bela_pre = bela;
}