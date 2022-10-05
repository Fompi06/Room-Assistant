void switches(byte mode, byte counter, byte hours, byte mins, byte secs, byte* customChar) {
  lcd.createChar(7, customChar);
  lcd.setCursor(0, 0);
  lcd.write(7);
  switch (mode) {
    case 0:
      lcd.print("Work:   ");
      break;
    case 1:
      lcd.print("Relax: ");
      break;
    case 2:
      lcd.print("Offline:");
      break;
  }
  lcd.print(hours);
  lcd.print(":");
  if (mins < 10) lcd.print("0");
  lcd.print(mins);
  lcd.print(":");
  if (secs < 10) lcd.print("0");
  lcd.print(secs);

  if (counter == 10) {
    lcd.setCursor(0, 1);
    lcd.print("Time to warm up!");
    return;
  }
  // if (counter == 11) {
  //   lcd.setCursor(0, 1);
  //   lcd.print(" Time to relax! ");
  //   return;
  // }
  if (counter == 12) {
    lcd.setCursor(0, 1);
    lcd.print(" Time to work!  ");
    return;
  }

  if (counter == 0) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    lcd.setCursor(0, 1);
    lcd.print("t:");
    lcd.print(temp, 1);
    lcd.write(223);
    lcd.print("C B:");
    lcd.print(hum, 1);
    lcd.print("%");
    return;
  }
  byte customGraph1[] = { B01111, B10000, B10111, B10111, B10111, B10111, B10000, B01111 };
  byte customGraphNone[] = { B01111, B10000, B10000, B10000, B10000, B10000, B10000, B01111 };
  byte customGraph2[] = { B11111, B00000, B11111, B11111, B11111, B11111, B00000, B11111 };
  byte customGraph2None[] = { B11111, B00000, B00000, B00000, B00000, B00000, B00000, B11111 };
  byte customGraph3[] = { 0x1E, 0x01, 0x1D, 0x1D, 0x1D, 0x1D, 0x01, 0x1E };
  byte customGraph3None[] = { B11110, B00001, B00001, B00001, B00001, B00001, B00001, B11110 };
  if (numberDrinks == 0) {
    lcd.createChar(4, customGraphNone);
    lcd.createChar(5, customGraph2None);
    lcd.createChar(6, customGraph3None);
    lcd.setCursor(0, 1);
    lcd.write(4);
    for (byte i = 0; i < 8; i++) lcd.write(5);
    lcd.write(6);
    lcd.print("  0/10");
    return;
  }

  if (numberDrinks > 0 && numberDrinks < 9) {
    uint8_t f = 1;
    lcd.createChar(3, customGraph1);
    lcd.createChar(4, customGraph2);
    lcd.createChar(5, customGraph2None);
    lcd.createChar(6, customGraph3None);
    lcd.setCursor(0, 1);
    lcd.write(3);
    while (f < numberDrinks) {
      lcd.write(4);
      f++;
    }
    while (f < 9) {
      lcd.write(5);
      f++;
    }
    lcd.write(6);
    lcd.print("  ");
    lcd.print(numberDrinks);
    lcd.print("/10");
    return;
  }

  if (numberDrinks == 9) {
    lcd.createChar(4, customGraph1);
    lcd.createChar(5, customGraph2);
    lcd.createChar(6, customGraph3None);
    lcd.setCursor(0, 1);
    lcd.write(4);
    for (byte i = 0; i < 8; i++) lcd.write(5);
    lcd.write(6);
    lcd.print("  9/10");
    return;
  }

  lcd.createChar(4, customGraph1);
  lcd.createChar(5, customGraph2);
  lcd.createChar(6, customGraph3);
  lcd.setCursor(0, 1);
  lcd.write(4);
  for (byte i = 0; i < 8; i++) lcd.write(5);
  lcd.write(6);
  lcd.print(" ");
  lcd.print(numberDrinks);
  lcd.print("/10");
}