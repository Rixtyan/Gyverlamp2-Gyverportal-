#include <GyverPortal.h>
#include <GyverHUB.h>
#define GH_NO_MQTT

char* pwName = "Gyverlamp2";
char* pwOnOffCol = "GP_GREEN";
byte pwEffect = CUR_PRES.effect;
byte pwPalette = CUR_PRES.palette;


String HueToHex(byte color) {
  //static String pwColorSld = "#000080";
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
  String s;
  s += "#";
  s += String(rgb, HEX);
  DEBUG("Color: ");
  DEBUGLN(s);
  return s;
  //        char* tmp = s.c_str()
  //        byte len = s.length() + 1;

  //       pwColorSld = "";
  //       pwColorSld = "#";
  //        s.toCharArray(pwColorSld, len);
  //     pwColorSld = tmp;

}


void build() {

  GP.BUILD_BEGIN(GP_DARK);
  //  GP.TITLE("GyverPortal");
  GP.UI_BEGIN(pwName, "/,/mode,/mqtt,/dawn,/palette,/config,/update", "Управление,Режим,MQTT,Рассвет,Палитра,Конфиг,Обновление");
  GP.GRID_RESPONSIVE(350);
  GP.ICON_SUPPORT();


  String effects[] = {"Перлин", "Цвет", "Смена", "Градиент", "Частицы", "Костер", "Огонь", "Конфетти", "Матрица", "Часы", "Погода", ""};
  String paletts[] = {"Своя", "Тепло", "Пламя", "Рыжее пламя", "Красное пламя", "Пастель пламя", "Желтое пламя", "Зеленое пламя", "Синее пламя", "Лава", "Вечеринка", "Радуга", "Павлин", "Облака", "Океан", "Лес", "Закат", "Полиция", "Пепси", "Теплая лава", "Холодная лава", "Горячая лава", "Розовая лава", "Уютный", "Киберпанк", "Девчачая", "Рождество", "Кислота", "Синий дым", "Жвачка", "Леопард", "Аврора", ""};

  String effectCur[cfg.presetAmount];
  FOR_i(0, cfg.presetAmount) {
    effectCur[i] += i + 1;
    effectCur[i] += " ";
    byte val = preset[i].effect - 1;
    effectCur[i] += effects[val];
    effectCur[i] += " ";
    if (val < 9) effectCur[i] += paletts[preset[i].palette - 1];
  }

  GP.UPDATE(effectCur[CUR_PRES.effect]);
  //  GP.UPDATE(pwEffect);
  //  GP.UPDATE(pwPalette);
  //  GP.UPDATE(pwOnOffCol); GP.ICON("power-off")

  if (ui.uri("/")) {
    GP.FORM_BEGIN("/");
    GP.UPDATE("pwOnOff,pwBright,pwEffc");
    M_GRID(
      M_BLOCK_TAB(
        pwName,
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

      );
      M_BLOCK_TAB(
        "Настройка яркости",
        M_BOX(GP.LABEL("Переопределить яркость"); GP.SWITCH("pwBrEffOn", CUR_PRES.fadeBright););
        M_BOX(GP_RIGHT, GP.SLIDER("pwBrEff", CUR_PRES.bright, 0, 255, 1, 0, GP_GREEN, CUR_PRES.fadeBright ? 1 : 0););
        M_BOX(GP.LABEL("Скорость"); GP.SLIDER("pwSpeed", CUR_PRES.speed, 0, 255););
        M_BOX(GP.LABEL("Масштаб"); GP.SLIDER("pwSacle", CUR_PRES.scale, 0, 255););
        M_BOX(GP.LABEL("Цвет из палитры"); GP.SWITCH("pwPalOn", CUR_PRES.fromPal););
        M_BOX(GP.LABEL("Цвет"); GP.SLIDER("pwColor", CUR_PRES.color, 0, 255););
        M_BOX(GP.LABEL("Из центра"); GP.SWITCH("pwCentr", CUR_PRES.fromCenter, GP_GREEN, CUR_PRES.effect = 3 ? 1 : 0););
        M_BOX(GP.SUBMIT("Сохранить"););
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
        M_BOX(GP.LABEL("test"); GP.SWITCH("dawnOn0", dawn.state[0]); GP.TIME("test"););
        M_BOX(GP.LABEL("Пн."); GP.SWITCH("dawnOn0", dawn.state[0]); GP.SPINNER("dawnH0", dawn.hour[0]); GP.SPINNER("dawnM0", dawn.minute[0]););
        M_BOX(GP.LABEL("Вт."); GP.SWITCH("dawnOn1", dawn.state[1]); GP.SPINNER("dawnH1", dawn.hour[1]); GP.SPINNER("dawnM1", dawn.minute[1]););
        M_BOX(GP.LABEL("Ср."); GP.SWITCH("dawnOn2", dawn.state[2]); GP.SPINNER("dawnH2", dawn.hour[2]); GP.SPINNER("dawnM2", dawn.minute[2]););
        M_BOX(GP.LABEL("Чт."); GP.SWITCH("dawnOn3", dawn.state[3]); GP.SPINNER("dawnH3", dawn.hour[3]); GP.SPINNER("dawnM3", dawn.minute[3]););
        M_BOX(GP.LABEL("Пт."); GP.SWITCH("dawnOn4", dawn.state[4]); GP.SPINNER("dawnH4", dawn.hour[4]); GP.SPINNER("dawnM4", dawn.minute[4]););
        M_BOX(GP.LABEL("Сб."); GP.SWITCH("dawnOn5", dawn.state[5]); GP.SPINNER("dawnH5", dawn.hour[5]); GP.SPINNER("dawnM5", dawn.minute[5]););
        M_BOX(GP.LABEL("Вс."); GP.SWITCH("dawnOn6", dawn.state[6]); GP.SPINNER("dawnH6", dawn.hour[6]); GP.SPINNER("dawnM6", dawn.minute[6]););
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
//    GP.UPDATE("pwName,pwCur,pwLength,pwWidth,pwMatrix,pwGroup,pwRole,clockOn,pwMode,pwGMT,pwCity,localOn,pwssid,pwps");
//    GP.RELOAD("rel");

//FOR_i(0, pal.size) {
//    GPcolor color[i](pal.strip[2*i-1], pal.strip[2*i], pal.strip[2*i+1]);
//  }
//  char* colName;
  GPcolor color1(255, 0, 0);
  
    M_GRID(
      M_BLOCK_TAB(
        "Кастом палитра",
        M_BOX(GP.LABEL("Количество цветов"); GP.SLIDER("spdPid", pal.size, 1, 16););
        M_BOX(
//            FOR_i(0, pal.size) {
//              iota(i, colName[i], DEC);
          GP.COLOR("c1", color1);
          GP.COLOR("c2", color1);
          GP.COLOR("c3", color1);
//            }
          );
        M_BOX(GP.SUBMIT("Сохранить"););
      );
    );
    GP.FORM_END();
  } else if (ui.uri("/config")) {
    GP.FORM_BEGIN("/config");
    GP.UPDATE("pwName,pwCur,pwLength,pwWidth,pwMatrix,pwGroup,pwRole,clockOn,pwMode,pwGMT,pwCity,localOn,pwssid,pwps");
    GP.RELOAD("rel");
    M_GRID(
      M_BLOCK_TAB(
        "Конфигурация",
        M_BOX(GP.LABEL("Имя лампы"); GP.TEXT("pwName", "Gyverlamp2", pwName););
        M_BOX(GP.LABEL("Макс. ток (мА)"); GP.NUMBER("pwCur", "0", cfg.maxCur * 1000););
        M_BOX(GP.LABEL("Длина"); GP.NUMBER("pwLength", "", cfg.length););
        M_BOX(GP.LABEL("Ширина"); GP.NUMBER("pwWidth", "", cfg.width););
        M_BOX(GP.LABEL("Тип матрицы"); GP.SELECT("pwMatrix", "LB_R, LB_U, LT_R, LT_D, RT_L, RT_D, RB_L, RB_U", cfg.matrix - 1););
        M_BOX(GP.LABEL("Группа"); GP.NUMBER("pwGroup", "", cfg.group););
        M_BOX(GP.LABEL("Роль"); GP.SELECT("pwRole", "Ведущий, Ведомый", cfg.role););
        M_BOX(GP.LABEL("Часы в выключенном режиме"); GP.SWITCH("clockOn", cfg.clock););
        M_BOX(GP.LABEL("Режим АЦП"); GP.SELECT("pwMode", "Нет, Яркость, Музыка, Яркость и Музыка", cfg.adcMode - 1, 0, 0, 0););
        M_BOX(GP.HR(););
        M_BOX(GP.LABEL("Часовой пояс"); GP.NUMBER("pwGMT", "0", cfg.GMT - 13););
        M_BOX(GP.LABEL("Номер города"); GP.NUMBER("pwCity", "0", cfg.cityID););
        M_BOX(GP.HR(););
        M_BOX(GP.LABEL("Подключаться к локальной сети"); GP.SWITCH("localOn", cfg.WiFimode););
        M_BOX(GP.LABEL("Имя сети"); GP.TEXT("pwssid", "SSID", cfg.ssid););
        M_BOX(GP.LABEL("Пароль"); GP.PASS("pwps", "Password", cfg.pass););
        M_BOX(GP.SUBMIT("Сохранить"););
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
  DEBUGLN("App builded");
  device.addTitle("GyverLamp2");
}

void action_app() {
  //  Serial.println(device.clickName() + ':' + device.getString());
  DEBUGLN("Click on Device!");
}

void action() {
  //    DEBUGLN("Click!");
  //    if (ui.form()) {
  //          if (ui.form("/")) {
  //  if (p.form("/settings")) {      // кнопка нажата
  //    p.copyStr("lg", cfg.ssid);  // копируем себе
  //    p.copyStr("ps", cfg.pass);
  //    cfg.WiFimode = 1;
  //    EE_updCfgRst();             // сохраняем
  //    WiFi.softAPdisconnect();        // отключаем AP
  //  }


  if (ui.click("pwOnOff")) {
    controlHandler(!cfg.state);
    //      pwOnOffCol = cfg.state ? "#008000" : "#800000";
    if (cfg.state) {
      pwOnOffCol = "GP_GREEN";
    } else {
      pwOnOffCol = "GP_RED";
    }
  }
  if (ui.click("pwBright")) {
    cfg.bright = mapFF(ui.getInt("pwBright"), 0, 255);
    DEBUGLN(ui.getInt("pwBright"));
  }

  if (ui.click("pwMode")) {
    cfg.adcMode = ui.getInt("pwMode") + 1;
    DEBUGLN(ui.getInt("pwMode"));
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


  if (ui.click("pwEff")) {
    DEBUGLN(ui.getInt("pwEff"));
    CUR_PRES.effect = ui.getInt("pwEff") + 1;
    setPreset(cfg.curPreset);
    pwEffect = CUR_PRES.effect;
    //  if (ui.update("rel")) ui.answer(1);
  }

  if (ui.click("pwPal")) {
    CUR_PRES.palette = ui.getInt("pwEff") + 1;
    setPreset(cfg.curPreset);
    pwPalette = CUR_PRES.palette;
  }

  if (ui.click("clockOn")) {
    cfg.clock = ui.getInt("clockOn");
    DEBUGLN(ui.getInt("clockOn"));
  }

  if (ui.form("/mode")) {      // кнопка нажата
    //      ui.copyStr("pwssid", cfg.ssid);  // копируем себе
    //      ui.copyStr("pwps", cfg.pass);
    CUR_PRES.fadeBright = ui.getInt("pwBrEffOn");
    CUR_PRES.bright = ui.getInt("pwBrEff");
    CUR_PRES.speed = ui.getInt("pwSpeed");
    CUR_PRES.scale = ui.getInt("pwScale");
    CUR_PRES.fromPal = ui.getInt("pwPalOn");
    CUR_PRES.color = ui.getInt("pwColor");
    CUR_PRES.advMode = ui.getInt("pwAdv" + 1);
    CUR_PRES.soundReact = ui.getInt("pwSreact") + 1;
    CUR_PRES.min = ui.getInt("pwRmin");
    CUR_PRES.max = ui.getInt("pwRmax");
    CUR_PRES.fromCenter = ui.getInt("pwCentr");
    HueToHex(CUR_PRES.color);
    DEBUGLN("Mode saved");
  }


  if (ui.form("/mqtt")) {      // кнопка нажата
    //      ui.copyStr("pwssid", cfg.ssid);  // копируем себе
    //      ui.copyStr("pwps", cfg.pass);
    //      cfg.WiFimode = ui.getInt("localOff");
    DEBUGLN("MQTT saved");
  }


  if (ui.form("/dawn")) {      // кнопка нажата
    DEBUGLN("Dawn saved");
  }



  if (ui.form("/config")) {      // кнопка нажата
    ui.copyStr("pwName", pwName);
    ui.copyStr("pwssid", cfg.ssid);  // копируем себе
    ui.copyStr("pwps", cfg.pass);
    cfg.WiFimode = ui.getInt("localOn");
    DEBUG("SSID: ");
    DEBUG(cfg.ssid);
    DEBUGLN(" saved");
  }

  if (ui.update()) {
    //    if (cfg.state) {
    //      pwOnOffCol = "#008000";
    //    } else {
    //      pwOnOffCol = "#800000";
    //    }
    //      char* pwOnOffCol = "#00EE00";
    pwEffect = CUR_PRES.effect;
    pwPalette = CUR_PRES.palette;
  }
}
