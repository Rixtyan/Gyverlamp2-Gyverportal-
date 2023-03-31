#include <GyverPortal.h>
#include <GyverHUB.h>

char* pwOnOffCol = "GP_GREEN";
char* pwColSld = "GP_GREEN";
byte pwEffect = CUR_PRES.effect;
byte pwPalette = CUR_PRES.palette;
bool EEfirstFlag = true;
GPcolor color[16];

// тестовая функция по определению цвета по HUE для изменения цвета слайдера (работает, но веб форма не принимает)
char* HueToHex(byte color) {
  byte _r;
  byte _g;
  byte _b;
  uint8_t shift;
  if (color > 170) {
    shift = (color - 170) * 3;
    _r = shift;
    _g = 0;
    _b = 255 - shift;
  } else if (color > 85) {
    shift = (color - 85) * 3;
    _r = 0;
    _g = 255 - shift;
    _b = shift;
  } else {
    shift = color * 3;
    _r = 255 - shift;
    _g = shift;
    _b = 0;
  }

  int rgb = 0;
  rgb = ((int)_r << 16) | ((int)_g << 8 ) | (int)_b;
  //  String s;
  //  s += "#";
  //  s += String(rgb, HEX);
  //  DEBUG("Color: ");
  //  DEBUGLN(s);
  char* str;
  ltoa(rgb, str, HEX);
  //  return String(rgb, HEX);
  //  str = "#" + str;
  return str;
}


void build() {

  GP.BUILD_BEGIN(GP_DARK);


   
  GP.UI_BEGIN(cfg.name, "/,/mode,/mqtt,/dawn,/palette,/config,/update", "Управление,Режим,MQTT,Рассвет,Палитра,Конфиг,Обновление");
  GP.GRID_RESPONSIVE(700);
  GP.ICON_SUPPORT();


  String effects[] = {"Перлин", "Цвет", "Смена", "Градиент", "Частицы", "Костер", "Огонь", "Конфетти", "Матрица", "Часы", "Погода", ""};
  String paletts[] = {"Своя", "Тепло", "Пламя", "Рыжее пламя", "Красное пламя", "Пастель пламя", "Желтое пламя", "Зеленое пламя", "Синее пламя", "Лава", "Вечеринка", "Радуга", "Павлин", "Облака", "Океан", "Лес", "Закат", "Полиция", "Пепси", "Теплая лава", "Холодная лава", "Горячая лава", "Розовая лава", "Уютный", "Киберпанк", "Девчачая", "Рождество", "Кислота", "Синий дым", "Жвачка", "Леопард", "Аврора", ""};
  String days[] = {"Пн.", "Вт.", "Ср.", "Чт.", "Пт.", "Сб.", "Вс.", ""};

  // Полное наименование эффекта на странице
  String effectCur[cfg.presetAmount];
  FOR_i(0, cfg.presetAmount + 1) {
    if (i < cfg.presetAmount) {
    effectCur[i] += i + 1;
    effectCur[i] += " ";
    byte val = preset[i].effect - 1;
    effectCur[i] += effects[val];
    effectCur[i] += " ";
    if (val < 9) effectCur[i] += paletts[preset[i].palette - 1];
    } else {
      effectCur[i] = "";
    }
  }

  GP.UPDATE(effectCur[CUR_PRES.effect]);

   if (EEfirstFlag) {   // попытка сделать кнопку-ссылку на вайфай при первом старте, но не работает на мобилке
    GP.FORM_BEGIN("/");
    GP.BUTTON_LINK("/config", "Настроить WI-FI"); 
    GP.FORM_END();
   }




  if (ui.uri("/")) {
    GP.FORM_BEGIN("/");

    GP.UPDATE("pwOnOff,pwBright,pwEffc");
    M_GRID(
      M_BLOCK_TAB(
        cfg.name,
        M_BOX(GP.BUTTON("pwOnOff", GP.ICON("power-off"), "", pwOnOffCol, "", 0, 1); GP.BUTTON("pwClock", GP.ICON("clock-o"), "", "#969619"););
        M_BOX(GP.LABEL("Яркость"); GP.SLIDER("pwBright", cfg.bright, 0, 255););
      );
    if (cfg.state) {
    M_BLOCK_TAB(
      "Смена режима",
      M_BOX(GP.BUTTON("bkw", "◄", "", "", "", 0, 1); GP.BUTTON("frw", "►", "", "", "", 0, 1););
      M_BOX(GP.LABEL("Текущий эффект"); GP.SELECT("pwEffc", effectCur, pwEffect, 0, 0, 1););
    );
    }
    );
    GP.FORM_END();
  } else if (ui.uri("/mode")) {
    GP.FORM_BEGIN("/mode");
    GP.UPDATE("pwEff,pwBright,pwEffc,pwPal,pwBrEffOn,pwBrEff,pwSpeed,pwSacle,pwPalOn,pwColor,pwAdv,pwAdv2,pwMin,pwMax,pwCentr");
    GP.RELOAD("rel");
    M_GRID(
      M_BLOCK_TAB(
        "Настройка режима",
        M_BOX(GP.LABEL("Текущий эффект"); GP.SELECT("pwEffc", effectCur, pwEffect););
        M_BOX(GP.BUTTON("pwPlus", GP.ICON("plus")); GP.BUTTON("pwMinus", GP.ICON("minus")););
        M_BOX(GP.LABEL("Эффект"); GP.SELECT("pwEff", effects, pwEffect - 1););
        M_BOX(GP.LABEL("Палитра"); GP.SELECT("pwPal", paletts, pwPalette - 1););
        M_BOX(GP.SUBMIT("Сохранить"););
      );
      M_BLOCK_TAB(
        "Настройка яркости",
        M_BOX(GP.LABEL("Переопределить яркость"); GP.SWITCH("pwBrEffOn", CUR_PRES.fadeBright););
        M_BOX(GP.LABEL("Яркость"); GP.SLIDER("pwBrEff", CUR_PRES.bright, 0, 255, 1, 0, GP_GREEN);); //, CUR_PRES.fadeBright = 1 ? 1 : 0););
        M_BOX(GP.LABEL("Скорость"); GP.SLIDER("pwSpeed", CUR_PRES.speed, 0, 255););
        M_BOX(GP.LABEL("Масштаб"); GP.SLIDER("pwScale", CUR_PRES.scale, 0, 255););
        M_BOX(GP.LABEL("Цвет из палитры"); GP.SWITCH("pwPalOn", CUR_PRES.fromPal););
        M_BOX(GP.LABEL("Цвет"); GP.SLIDER("pwColor", CUR_PRES.color, 0, 255, 1, 0, GP_GREEN);); //, CUR_PRES.fromPal = 1 ? 1 : 0););
        M_BOX(GP.LABEL("Из центра"); GP.SWITCH("pwCentr", CUR_PRES.fromCenter, GP_GREEN);); //, CUR_PRES.effect = 3 ? 1 : 0);); // отключение элементов работает, но потом их не включить. Блок if ломает всю структуру
      );
    if (cfg.adcMode - 1 > 0) {
    M_BLOCK_TAB(
      "Дополнительные параметры",
      M_BOX(GP.LABEL("Дополнительно"); GP.SELECT("pwAdv", "Нет,Громкость,Низкие,Высокие,Часы", CUR_PRES.advMode - 1););
      M_BOX(GP.LABEL("Реакция на звук"); GP.SELECT("pwAdv2", "Яркость,Масштаб,Длина", CUR_PRES.soundReact - 1););
      M_BOX(GP.LABEL("Минимум"); GP.SLIDER("pwMin", CUR_PRES.min, 0, 255););
      M_BOX(GP.LABEL("Максимум"); GP_RIGHT; GP.SLIDER("pwMax", CUR_PRES.max, 0, 255););
    if (CUR_PRES.advMode - 1 > 0) {
      M_BOX(GP.LABEL("Реакция на звук"); GP.SELECT("pwAdv2", "Яркость,Масштаб,Длина", CUR_PRES.soundReact - 1););
        M_BOX(GP.LABEL("Минимум"); GP.SLIDER("pwMin", CUR_PRES.min, 0, 255););
        M_BOX(GP.LABEL("Максимум"); GP_RIGHT; GP.SLIDER("pwMax", CUR_PRES.max, 0, 255););
      }
      );
    }
    );
    GP.FORM_END();
  } else if (ui.uri("/mqtt")) {
    GP.FORM_BEGIN("/mqtt");
    GP.UPDATE("pwMQTT,pwMQTTid,pwMQTThost,pwMQTTport,pwMQTTlogin,pwMQTTps");
    GP.RELOAD("rel");
    M_GRID(
      M_BLOCK_TAB(
        "MQTT",
        M_BOX(GP.LABEL("MQTT"); GP_RIGHT; GP.SWITCH("pwMQTT", cfg.mqtt););
        M_BOX(GP.LABEL("ID"); GP.TEXT("pwMQTTid", "ID", cfg.mqttID););
        M_BOX(GP.LABEL("Сервер"); GP.TEXT("pwMQTThost", "Host", cfg.mqttHost););
        M_BOX(GP.LABEL("Порт"); GP.NUMBER("pwMQTTport", "0", cfg.mqttPort););
        M_BOX(GP.LABEL("Логин"); GP.TEXT("pwMQTTlogin", "Login", cfg.mqttLogin););
        M_BOX(GP.LABEL("Пароль"); GP.PASS("pwMQTTps", "Password", cfg.mqttPass););
        M_BOX(GP.SUBMIT("Сохранить"););
      );
    );

  } else if (ui.uri("/dawn")) {
    GP.FORM_BEGIN("/dawn");
    GP.UPDATE("pwMQTT,pwMQTTid,pwMQTThost,pwMQTTport,pwMQTTlogin,pwMQTTps");
    //GP.RELOAD("rel");
    M_GRID(
      M_BLOCK_TAB(
        "Рассвет",
    FOR_i(0, 6) {
      M_BOX(GP.LABEL(days[i]); GP.SWITCH("dawnOn" + i, dawn.state[i]); GP.TIME("pwTime" + i););
    }

    M_BOX(GP.LABEL("Максимальная яркость"); GP.SLIDER("spdPid", dawn.bright););
    M_BOX(GP.LABEL("Минут до рассвета"); GP.SPINNER("spdPid", dawn.time, 5, 60, 5););
    M_BOX(GP.LABEL("Минут после рассвета"); GP.SPINNER("spdPid", dawn.post, 5, 60, 5););
    M_BOX(GP.SUBMIT("Сохранить"););
    M_BOX(GP.BUTTON("bkwPid", "◄"); GP.BUTTON("frwPid", "►"););
    GP.SPINNER("test");
      );
    );

    
  } else if (ui.uri("/palette")) {
    GP.FORM_BEGIN("/palette");

    FOR_i(0, pal.size) {
      color[i] = (pal.strip[i * 3 - 2], pal.strip[i * 3 - 1], pal.strip[i * 3]);
    }
    M_GRID(
      M_BLOCK_TAB(
        "Кастом палитра",
        M_BOX(GP.LABEL("Количество цветов"); GP.SLIDER("pwPalSize", pal.size, 1, 16););
        M_BOX(
    FOR_i(0, pal.size) {
      GP.COLOR("c" + i, color[i]);
    }
        );
    M_BOX(GP.SUBMIT("Сохранить"););
      );
    );
    GP.FORM_END();
  } else if (ui.uri("/config")) {
    GP.FORM_BEGIN("/config");
    GP.UPDATE("pwName,pwCur,pwLength,pwWidth,pwMatrix,pwGroup,pwRole,clockOn,pwMode,pwGMT,pwCity,pwlocalOn,pwssid,pwps");
    GP.RELOAD("rel");
    M_GRID(
      M_BLOCK_TAB(
        "Конфигурация",
        M_BOX(GP.LABEL("Имя лампы"); GP.TEXT("pwName", "Gyverlamp2", cfg.name););
        M_BOX(GP.LABEL("Макс. ток (мА)"); GP.NUMBER("pwCur", "0", cfg.maxCur * 1000););
        M_BOX(GP.LABEL("Длина"); GP.NUMBER("pwLength", "", cfg.length););
        M_BOX(GP.LABEL("Ширина"); GP.NUMBER("pwWidth", "", cfg.width););
        M_BOX(GP.LABEL("Тип матрицы"); GP.SELECT("pwMatrix", "Лента,Матрица Зигзаг,Матрица спираль", cfg.deviceType - 1););
        M_BOX(GP.LABEL("Точка подключения"); GP.SELECT("pwMatrixDot", "LB_R, LB_U, LT_R, LT_D, RT_L, RT_D, RB_L, RB_U", cfg.matrix - 1););
        M_BOX(GP.LABEL("Группа"); GP.NUMBER("pwGroup", "", cfg.group););
        M_BOX(GP.LABEL("Роль"); GP.SELECT("pwRole", "Мастер, Раб", cfg.role););
        M_BOX(GP.LABEL("Часы в выключенном режиме"); GP.SWITCH("clockOn", cfg.clock););
        M_BOX(GP.LABEL("Режим АЦП"); GP.SELECT("pwMode", "Нет, Яркость, Музыка, Яркость и Музыка", cfg.adcMode - 1, 0, 0, 0););
        M_BOX(GP.HR(););
        M_BOX(GP.LABEL("Часовой пояс"); GP.NUMBER("pwGMT", "0", cfg.GMT - 13););
        M_BOX(GP.LABEL("Номер города"); GP.NUMBER("pwCity", "0", cfg.cityID););
        M_BOX(GP.HR(););
        M_BOX(GP.LABEL("Локальный режим"); GP.SWITCH("pwlocalOn", cfg.WiFimode););
        M_BOX(GP.LABEL("Имя сети"); GP.TEXT("pwssid", "SSID", cfg.ssid););
        M_BOX(GP.LABEL("Пароль"); GP.PASS("pwpasswd", "Password", cfg.pass););
        M_BOX(GP.LABEL("Сервер времени"); GP.TEXT("pwNTP", "NTP", cfg.NTPserver););
        M_BOX(GP.SUBMIT("Сохранить"); GP.BUTTON("pwReset", GP.ICON("repeat") + "Перезагрузка"););
      );
    );
    GP.FORM_END();
  } else if (ui.uri("/update")) {
    GP.FORM_BEGIN("/update");
    GP.OTA_FIRMWARE();
    GP.OTA_FILESYSTEM();
    GP.FORM_END();
    GP.UI_END();
    GP.BUILD_END();

  }
}


void build_app() {
  DEBUGLN("App builded!!");
}

void action_app() {
  DEBUGLN("Click on Device!");
}

void action() {
  if (ui.click("pwOnOff")) {
    controlHandler(!cfg.state);
    //      pwOnOffCol = cfg.state ? "#008000" : "#800000"; // такой вариант не работает, ругается компилятор
    if (cfg.state) {
      pwOnOffCol = "GP_GREEN";
    } else {
      pwOnOffCol = "GP_RED";
    }
  }
  if (ui.click("pwBright")) {
    cfg.bright = ui.getInt("pwBright");
//    DEBUGLN(ui.getInt("pwBright"));
  }

  if (ui.click("pwMode")) {
    cfg.adcMode = ui.getInt("pwMode") + 1;
//    DEBUGLN(ui.getInt("pwMode"));
  }
  if (ui.click("bkw")) {
    changePreset(-1);
    pwEffect = CUR_PRES.effect;
  }
  if (ui.click("frw")) {
    changePreset(1);
    pwEffect = CUR_PRES.effect;
  }
  if (ui.click("pwEffc")) {
    setPreset(ui.getInt("pwEffc"));
    pwEffect = CUR_PRES.effect;
  }

  //Настройка режима

  if (ui.click("pwPlus")) {
    cfg.presetAmount = cfg.presetAmount + 1;
    changePreset(1);
    pwEffect = CUR_PRES.effect;
  }

  if (ui.click("pwMinus")) {
    cfg.presetAmount = cfg.presetAmount - 1;
    changePreset(-1);
    pwEffect = CUR_PRES.effect;
  }


  if (ui.click("pwEff")) {
    //    DEBUGLN(ui.getInt("pwEff"));
    CUR_PRES.effect = ui.getInt("pwEff") + 1;
    setPreset(cfg.curPreset);
    pwEffect = CUR_PRES.effect;
  }

  if (ui.click("pwPal")) {
    CUR_PRES.palette = ui.getInt("pwPal") + 1;
    setPreset(cfg.curPreset);
    pwPalette = CUR_PRES.palette;
  }

  if (ui.click("clockOn")) {
    cfg.clock = ui.getInt("clockOn");
    //    DEBUGLN(ui.getInt("clockOn"));
  }

  if (ui.click("pwColor")) {
    pwColSld = HueToHex(CUR_PRES.color);
    CUR_PRES.color = ui.getInt("pwColor");
    DEBUGLN(pwColSld);
  }

  if (ui.click("pwBrEffOn")) CUR_PRES.fadeBright = ui.getInt("pwBrEffOn");
  if (ui.click("pwBrEff")) CUR_PRES.bright = ui.getInt("pwBrEff");
  if (ui.click("pwSpeed")) CUR_PRES.speed = ui.getInt("pwSpeed");
  if (ui.click("pwScale")) CUR_PRES.scale = ui.getInt("pwScale");
  if (ui.click("pwPalOn")) CUR_PRES.fromPal = ui.getInt("pwPalOn");
  if (ui.click("pwAdv")) CUR_PRES.advMode = ui.getInt("pwAdv") + 1;
  if (ui.click("pwSreact")) CUR_PRES.soundReact = ui.getInt("pwSreact") + 1;
  if (ui.click("pwRmin")) CUR_PRES.min = ui.getInt("pwRmin");
  if (ui.click("pwRmax")) CUR_PRES.max = ui.getInt("pwRmax");
  if (ui.click("pwCentr")) CUR_PRES.fromCenter = ui.getInt("pwCentr");





  if (ui.form("/mode")) {      // кнопка нажата
    EE_updCfg();
    DEBUGLN("<--- Mode saved");
  }


  if (ui.form("/mqtt")) {      // кнопка нажата
    DEBUGLN(ntp.timeString());
//    DEBUGLN(ui.encode(ui.getSystemTime()));
    DEBUGLN("MQTT saved");
  }


  if (ui.form("/dawn")) {      // кнопка нажата
    DEBUGLN("Dawn saved");
  }


  if (ui.click("pwPalSize")) pal.size = ui.getInt("pwPalSize");


  if (ui.form("/palette")) {      // кнопка нажата
    pal.size = ui.getInt("pwPalSize");
    FOR_i(0, pal.size) {
      pal.strip[i * 3 - 2] = color[i].r;
      pal.strip[i * 3 - 1] = color[i].g;
      pal.strip[i * 3] = color[i].b;
    }
    DEBUGLN("Palette saved");
  }


  // Настройка конфигурации

  if (ui.click("pwlocalOn")) cfg.WiFimode = !cfg.WiFimode;


  if (ui.form("/config")) {      // кнопка нажата

    
    ui.copyStr("pwName", cfg.name);
    cfg.maxCur = ui.getInt("pwCur") / 1000;
    cfg.length = ui.getInt("pwLength");
    cfg.width = ui.getInt("pwWidth");
    cfg.deviceType = ui.getInt("pwMatrix") + 1;
    cfg.matrix = ui.getInt("pwMatrixDot") + 1;
    cfg.group = ui.getInt("pwGroup");
    cfg.role = ui.getInt("pwRole");
    cfg.adcMode = ui.getInt("pwMode") + 1;
    cfg.GMT = ui.getInt("pwGMT") + 13;
    cfg.cityID = ui.getInt("pwCity");
    ui.copyStr("pwssid", cfg.ssid);
    ui.copyStr("pwpasswd", cfg.pass);
    ui.copyStr("pwNTP", cfg.NTPserver);

    EE_updCfg();
  }
  if (ui.click("pwReset")) EE_updCfgRst();

  // попытка обновить цвета
  if (ui.update()) {
    if (cfg.state) {
      pwOnOffCol = "GP_GREEN";
    } else {
      pwOnOffCol = "GP_RED";
    }
    pwEffect = CUR_PRES.effect;
    pwPalette = CUR_PRES.palette;

  }
}
