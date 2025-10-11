
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0    
#define INTERVAL 25        
#define PULSE_DURATION 10  
#define _DIST_MIN 100      
#define _DIST_MAX 300      

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.5 

#define N 3

unsigned long last_sampling_time = 0;
float dist_prev = _DIST_MAX;
float dist_ema  = _DIST_MAX;      
float med_buf[N];             
int   med_idx = 0;
int   med_cnt = 0;

void setup() {
  pinMode(PIN_LED,  OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  
  Serial.begin(57600);

  dist_ema = _DIST_MAX;
}

void loop() {
  

  float dist_raw, dist_filtered, dist_median;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX) || (dist_raw < _DIST_MIN)) {
    dist_filtered = dist_prev;
  } else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  }

  median_push(dist_filtered);
  dist_median = median_current();
  
  dist_ema = _EMA_ALPHA * dist_median + (1 - _EMA_ALPHA) * dist_ema;

  Serial.print("Min:");     Serial.print(_DIST_MIN);
  Serial.print(",raw:");    Serial.print(dist_raw);
  Serial.print(",ema:");    Serial.print(dist_ema);
  Serial.print(",median:"); Serial.print(dist_median);
  Serial.print(",Max:");    Serial.print(_DIST_MAX);
  Serial.println("");

  if ((dist_median < _DIST_MIN) || (dist_median > _DIST_MAX))
    digitalWrite(PIN_LED, 1); // LED OFF
  else
    digitalWrite(PIN_LED, 0);  // LED ON

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}

void median_push(float v) {
  med_buf[med_idx] = v;
  med_idx = (med_idx + 1) % N;
  if (med_cnt < N) med_cnt++;
}

float median_current() {
  int m = med_cnt;
  if (m == 0) return 0.0;

  float t[N];
  for (int i = 0; i < m; i++) t[i] = med_buf[i];

  for (int i = 1; i < m; i++) {
    float key = t[i];
    int j = i - 1;
    while (j >= 0 && t[j] > key) {
      t[j + 1] = t[j];
      j--;
    }
    t[j + 1] = key;
  }

  if (m % 2 == 1) return t[m / 2];
  else return 0.5f * (t[m / 2 - 1] + t[m / 2]);
}
