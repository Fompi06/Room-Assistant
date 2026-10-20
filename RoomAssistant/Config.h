#pragma once

// ========== НАСТРОЙКИ (всё время — в минутах) ==========
// 1 — сборка для онлайн-симулятора (Wokwi, Cirkit Designer и т. п.).
// Симуляторы часто не эмулируют сторожевой таймер (watchdog), на котором работает сон,
// и прошивка зависает при старте. В этом режиме сон заменён обычной задержкой.
// Для настоящей платы оставьте 0.
#define SIMULATION 0

#define BTNSOUNDS 1               // 1 — звуки нажатий разрешены (включаются в меню), 0 — всегда выключены

// Профиль «по умолчанию»: время до напоминания и время до повторного напоминания
#define ReminderOnline 60
#define RepeatedReminderOnline 20
#define ReminderRelax 20
#define RepeatedReminderRelax 10
#define ReminderOff 30
#define RepeatedReminderOff 10

#define RandomCheckupMin 10       // проверка присутствия в режиме Work: не раньше чем через N минут...
#define RandomCheckupMax 50       // ...и не позже чем через N минут
#define randomAlertTime 20        // максимальная случайная добавка к таймеру (отключается режимом Strict)
#define reminderDrink 60          // напоминание выпить воды

// Быстрый профиль 1 (красная кнопка) и 2 (жёлтая кнопка): Work / Relax / Offline
#define OnlineTime1 90
#define RelaxTime1 30
#define OffTime1 20
#define OnlineTime2 30
#define RelaxTime2 15
#define OffTime2 10
// ========== КОНЕЦ НАСТРОЕК ==========

#define DHTTYPE DHT22

// ---------- Пины ----------
#define BUZ 5         // зуммер
#define S1 10         // энкодер, S1
#define S2 4          // энкодер, S2
#define KEY A3        // энкодер, кнопка
#define DHTPIN 9      // DHT22, линия данных
#define REDBUT 3      // красная кнопка (INT1)
#define YELLOWBUT 2   // жёлтая кнопка (INT0)
#define RED 8         // светодиод
#define SEN_VCC 11    // питание DHT22 (отключается в режиме Eco)
#define BUZ_VCC 12    // питание зуммера (в схеме не используется)
