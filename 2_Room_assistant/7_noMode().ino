byte noMode() {
  lcd.clear();
  while(1) {
    
dht1.temp = dht.readTemperature();
dht1.hum = dht.readHumidity();
// Serial.println(dht1.hum);
// Serial.println(dht1.temp);
lcd.setCursor(0, 1);
lcd.print("t:");
lcd.print(dht1.temp, 1);
lcd.print("°C В:");
lcd.print(dht1.hum, 1);
lcd.print("%");
  }
  return 1;
}