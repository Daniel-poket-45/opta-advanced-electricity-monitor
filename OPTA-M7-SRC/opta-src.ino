#include <STM32RTC.h>
#include <cstdint.h>
#include <string.h>

namespace scheduler{
  
    class LogicalTimedThread{
    private:
    unsigned long last_time;
    unsigned long interval;

    public:
    LogicalTimedThread(unsigned long interval){
        this->last_time = millis();
        this->interval = interval;
    }

    unsigned long get_current_time(){
        return millis();
    }

    unsigned long get_last_time(){
        return last_time;
    }

    bool ready(){
        unsigned long now = millis();

        if(now - last_time >= interval){
            last_time = now;
            return true;
        }
        return false;
    }
    };
    }

    namespace chronos{
    const uint8_t JAN = 1;
    const uint8_t FEV = 2;
    const uint8_t MAR = 3;
    const uint8_t APR = 4;
    const uint8_t MAY = 5;
    const uint8_t JUN = 6;
    const uint8_t JUL = 7;
    const uint8_t AUG = 8;
    const uint8_t SEP = 9;
    const uint8_t OCT = 10;
    const uint8_t NOV = 11;
    const uint8_t DEC = 12;

    class Date{
    private:
        STM32RTC& rtc = STM32RTC::getInstance();
    public:
        Date(uint8_t _hour, uint8_t _minute, uint8_t _second, uint8_t _day, uint8_t _month, uint16_t _year){
        set_year(_year);
        set_month(_month);
        set_day(_day);

            set_hour(_hour);
            set_minute(_minute);
            set_second(_second);
        }
        
        bool set_day(uint8_t day){
            //lógica de verificação de fevereiro (caso bissexto segundo STD::CHRONO)
            if(rtc.getMonth() == FEV){
                if((rtc.getYear() % 4 == 0 && rtc.getYear() % 100 != 0) || (rtc.getYear() % 400 == 0)){
                    if(day > 0 && day <= 29){
                        rtc.setDay(day);  
                        log::print("Day changed to: ", rtc.getDay(), log::LOG_LEVEL::WARNING);
                        return true;
                    }else{
                        log::print("Day not changed! The day must be greater than zero and less than 29 for February in leap years - INCORRECT DAY: ", day, log::LOG_LEVEL::ERROR);
                        return false;
                    }
                }else{ //caso não for bissexto
                    if(day > 0 && day <= 28){
                        rtc.setDay(day);    
                        log::print("Day changed to: ", rtc.getDay(), log::LOG_LEVEL::WARNING);
                        return true;
                    }else{
                        log::print("Day not changed! The day must be greater than zero and less than 28 in February - INCORRECT DAY: ", day, log::LOG_LEVEL::ERROR);
                        return false;
                    }
                }
                
            //lógica de verificação de 31 dias
            }else if(rtc.getMonth() == JAN || rtc.getMonth() == MAR || rtc.getMonth() == MAY || rtc.getMonth() == JUL || rtc.getMonth() == AUG || rtc.getMonth() == OCT || rtc.getMonth() == DEC){
                if(day > 0 && day <=31){
                rtc.setDay(day);
                log::print("Day changed to: ", rtc.getDay(), log::LOG_LEVEL::WARNING);
                return true;
                }else{
                log::print("Day not changed! The day must be greater than zero and less than 31 for the months of January, March, May, July, August, October, and December. - INCORRECT DAY: ", day, log::LOG_LEVEL::ERROR);
                return false;
                }
                
            //lógica de verificação de 30 dias
            }else if(rtc.getMonth() == APR || rtc.getMonth() == JUN || rtc.getMonth() == SEP || rtc.getMonth() == NOV){
                if(day > 0 && day <= 30){
                rtc.setDay(day);
                log::print("Day changed to: ", rtc.getDay(), log::LOG_LEVEL::WARNING);
                return true;
                }else{
                log::print("Day not changed! The day must be greater than zero and less than 31 for the months of April, June, September, and November. - INCORRECT DAY: ", day, log::LOG_LEVEL::ERROR);
                return false;
                }
                
            //caso digitou um número inválido
            }else{
                rtc.setDay(1);
                log::print("Set day failed, fallbacking to day 1... - INCORRECT DAY: ", day, log::LOG_LEVEL::ERROR);
                return false;
            }
        }

        bool set_month(uint8_t month){
        if(month > 0 && month <= 12){

            //se for fevereiro o mês a ser setado
            if(month == FEV){
                //se for ano bissexto
                if(((rtc.getYear() + 2000) % 4 == 0 && (rtc.getYear() + 2000) % 100 != 0) || ((rtc.getYear() + 2000) % 400 == 0)){
                    if(rtc.getDay() > 29){
                        rtc.setDay(29);
                        log::print("Set month incongruence, fallbacking to day 29... - Month changed to: ", month, log::LOG_LEVEL::WARNING);
                    }else{
                        log::print("Month changed to: ", month, log::LOG_LEVEL::WARNING);
                    }

                    rtc.setMonth(month);
                    return true;

                }else{ //se não for ano bissexto
                    if(rtc.getDay() > 28){
                            rtc.setDay(28);
                        log::print("Set month incongruence, fallbacking to day 28... - Month changed to: ", month, log::LOG_LEVEL::WARNING);
                    }else{
                        log::print("Month changed to: ", month, log::LOG_LEVEL::WARNING);
                    }

                    rtc.setMonth(month);
                    return true;
                }
            }

            //lógica dos meses com 31 dias a ser setado
            if(month == JAN || month == MAR || month == MAY || month == JUL || month == AUG || month == OCT || month == DEC){
                if(rtc.getDay() == 0 || rtc.getDay() > 31){
                    rtc.setDay(1);

                    log::print("Set month incongruence, fallbacking to day 1... - Month changed to: ", month, log::LOG_LEVEL::WARNING);
                }else{
                    log::print("Month changed to: ", month, log::LOG_LEVEL::WARNING);
                }

                rtc.setMonth(month);
                return true;
            }

            //lógica dos meses com 30 dias a ser setado
            if(month == APR || month == JUN || month == SEP || month == NOV){
                if(rtc.getDay() == 0 || rtc.getDay() > 30){
                    rtc.setDay(1);

                    log::print("Set month incongruence, fallbacking to day 1... - Month changed to: ", month, log::LOG_LEVEL::WARNING);
                }else{
                    log::print("Month changed to: ", month, log::LOG_LEVEL::WARNING);
                }

                rtc.setMonth(month);
                return true;
            }

        //caso digitou um número inválido
        }else{
            rtc.setMonth(1);

            log::print("Set month failed, fallbacking to month 1... The month must be between 1 and 12 - INCORRECT MONTH: ", month, log::LOG_LEVEL::ERROR);

            return false;
        }
        }

        bool set_year(uint16_t year){
            if(year == 0){
            log::print("God bless the America! Jesus, have mercy on us... ", "[...]", log::LOG_LEVEL::CRITICAL);
            rtc.setYear(year);
            return true;
            }

            // Se a data atual for 29 de fevereiro,
            if(rtc.getMonth() == FEV && rtc.getDay() == 29){
                // Verifica se o NOVO ano é bissexto
                if(!((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))){
                    rtc.setDay(28);

                    log::print("Year changed with date incongruence, fallbacking day 29 to day 28... New year: ",year,log::LOG_LEVEL::WARNING);
                }
            }
            
            rtc.setYear((year - 2000));
            return true;
        }
        
        bool set_hour(uint8_t hour){
            if(hour >= 0 && hour <= 23){
            log::print("Hour changed to: ", hour, log::LOG_LEVEL::WARNING);
            rtc.setHours(hour);
            return true;
            }else{
            log::print("Set hour failed, fallbacking to hour 0... The hour must be between 0 and 23 - INCORRECT HOUR: ", hour, log::LOG_LEVEL::ERROR);
            rtc.setHours(00);
            return false;
            }
        }

        bool set_minute(uint8_t minute){
            if(minute >= 0 && minute <= 59){
            log::print("Minute changed to: ", minute, log::LOG_LEVEL::WARNING);
            rtc.setMinutes(minute);
            return true;
            }else{
            log::print("Set minute failed, fallbacking to minute 00... The minute must be between 00 and 59 - INCORRECT MINUTE: ", minute, log::LOG_LEVEL::ERROR);
            rtc.setMinutes(00);
            return false;
            }
        }

        bool set_second(uint8_t second){
            if(second >= 0 && second <= 59){
            log::print("Second changed to: ", second, log::LOG_LEVEL::WARNING);
            rtc.setSeconds(second);
            return true;
            }else{
            log::print("Set second failed, fallbacking to second 00... The second must be between 00 and 59 - INCORRECT SECOND: ", second, log::LOG_LEVEL::ERROR);
            rtc.setSeconds(00);
            return false;
            }
        }

        uint8_t get_day(){return rtc.getDay();}
        uint8_t get_month(){return rtc.getMonth();}
        uint16_t get_year(){return (rtc.getYear() + 2000);}
        uint8_t get_hour(){return rtc.getHours();}
        uint8_t get_minute(){return rtc.getMinutes();}
        uint8_t get_second(){return rtc.getSeconds();}

        String get_iso_data(){
            char buffer[25];
            snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d", get_year(), get_month(), get_day(), get_hour(), get_minute(), get_second());
            return String(buffer);    
        }

        String get_brazil_data(){
            char buffer[25];
            snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d", get_day(), get_month(), get_year(), get_hour(), get_minute(), get_second());
            return String(buffer);
        }

        String get_eua_data(){
            char buffer[25];
            snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d", get_month(), get_day(), get_year(), get_hour(), get_minute(), get_second());
            return String(buffer);
        }

        String get_compact_data(){
            char buffer[20];
        snprintf(buffer, sizeof(buffer), "%04d%02d%02d_%02d%02d%02d", get_year(), get_month(), get_day(), get_hour(), get_minute(), get_second());
        return String(buffer);
        }

        bool sync_with_ntp_server(){
        //TODO
        }
    };
}

//TODO
namespace shell{
    String USER = "serial";
    String HOSTNAME = "OPTA";
    char PERMISSION = '#'; //# = Administrador e $ = usuário padrão
    String shell_string = USER + "@" + HOSTNAME + "~" + String(PERMISSION) + ">> ";
  
    void update_shell(){
      shell_string = USER + "@" + HOSTNAME + "~" + String(PERMISSION) + ">> ";
    }
  
    void change_user(String new_user){
    USER = new_user;
        update_shell();
    }
  
    void change_hostname(String new_hostname){
    HOSTNAME = new_hostname;
        update_shell();
    }
  
    void change_permission(char new_permission){
    PERMISSION = new_permission;
        update_shell();
    }
  
    
}

namespace log{
    //níveis de debug disponíveis
    enum class LOG_LEVEL : uint8_t{
    DEBUG = 4,
        INFO = 3,
        WARNING = 2,
        ERROR = 1,
        CRITICAL = 0
    };
  
  LOG_LEVEL global_level = LOG_LEVEL::ERROR;
  
    //implementação emergencial pra não bugar o print de log
    void print(String content, String result, LOG_LEVEL level);
  
    //trocar level de debug
    void change_level(LOG_LEVEL new_level){
        String log_prompt = "";
        switch(new_level){
          case LOG_LEVEL::DEBUG:
              log_prompt = "DEBUG";
          break;
          case LOG_LEVEL::INFO:
              log_prompt = "INFO";
          break;
          case LOG_LEVEL::WARNING:
              log_prompt = "WARNING";
          break;
          case LOG_LEVEL::ERROR:
              log_prompt = "ERROR";
          break;
          case LOG_LEVEL::CRITICAL:
              log_prompt = "CRITICAL";
          break;
          default:
              log_prompt = "LOG SYSTEM HAD A BAD ARGUMENT!!!";
          break;
        }
          global_level = new_level;
      
          log::print("System global log settings changed", log_prompt, LOG_LEVEL::WARNING);
    }
  
    //print inteligente dos logs
    void print(String content, String result, LOG_LEVEL level){
      String current_time = "FAZER TEMPO DEPOIS";
      String log_prompt = "";
      uint8_t log_num = 0;
      switch(level){
        case LOG_LEVEL::DEBUG:
          log_prompt = "DEBUG";
          uint8_t log_num = 4;
        break;
        case LOG_LEVEL::INFO:
          log_prompt = "INFO";
          uint8_t log_num = 3;
        break;
        case LOG_LEVEL::WARNING:
          log_prompt = "WARNING";
          uint8_t log_num = 2;
        break;
        case LOG_LEVEL::ERROR:
          log_prompt = "ERROR";
          uint8_t log_num = 1;
        break;
        case LOG_LEVEL::CRITICAL:
          log_prompt = "CRITICAL";
          uint8_t log_num = 0;
        break;
        default:
          log_prompt = "LOG SYSTEM HAD A BAD ARGUMENT!!!";
          uint8_t log_num = 0;
        break;
      }
      
      //verificação de nível de LOG global
      if(log_num >= ){
        //TODO
      }
      
      Serial.print('[');
      Serial.print(current_time);
      Serial.print(']');
      Serial.print(" ");
      Serial.print('[');
      Serial.print(log_prompt);
      Serial.print('] ');
      Serial.print(content);
      Serial.print(" : ");
      Serial.println(result);
    }
}

//classe base de sensores
class modbusSensor{
public:
    //construtor
    //função inicializadora de objeto
    //função de definição de endereço modbus
    //destrutor

    sensor.getVoltagePhase1(MODBUS_ADDRESS).toFloat();

    sensor.getVoltagePhase2(MODBUS_ADDRESS).toFloat();

    sensor.getVoltagePhase3(MODBUS_ADDRESS).toFloat();


protected:
    //objeto de sensor
    Finder7M sensor;
    //variável nome
    String sensor_name;
    //variável localização
    String sensor_location;
    //variável endereço modbus
    constexpr uint8_t MODBUS_ADDRESS;
}

scheduler::LogicalTimedThread imp(250);

void setup(){
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
    if(imp.ready()){
        Serial.println("impresso a cada 1s");
        (digitalRead(LED_BUILTIN) == HIGH) ? digitalWrite(LED_BUILTIN, LOW) : digitalWrite(LED_BUILTIN, HIGH);
    }
}
