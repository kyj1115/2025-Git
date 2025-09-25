#define PIN_LED 7

unsigned int toggle = 1;
unsigned int period = 1000;     
unsigned int duty = 0;          
int dir = 1;                    

unsigned int count = 0;         
unsigned int update_rate = 50;  

void setup() {
  pinMode(PIN_LED, OUTPUT);
  set_period(100);  
  set_duty(0);       
}

void loop() {
  
  toggle = toggle_state(toggle);
  digitalWrite(PIN_LED, toggle);
  delayMicroseconds((period * duty) / 100);

  toggle = toggle_state(toggle);
  digitalWrite(PIN_LED, toggle);
  delayMicroseconds((period * (100 - duty)) / 100);

  count++;
  if (count >= update_rate) {
    count = 0;

    duty += dir;

    if (duty >= 100) {
      duty = 100;
      dir = -1;
    } else if (duty <= 0) {
      duty = 0;
      dir = 1;
    }
  }
}

int toggle_state(int toggle) {
  return !toggle;
}

void set_period(int p) {
  if (p >= 100 && p <= 10000) {
    period = p;
  }
}

void set_duty(int d) {
  if (d >= 0 && d <= 100) {
    duty = d;
  }
}
