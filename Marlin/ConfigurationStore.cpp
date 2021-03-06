#include "Marlin.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"
#include "ConfigurationStore.h"
//#include "Configuration_prusa.h"

#ifdef MESH_BED_LEVELING
#include "mesh_bed_leveling.h"
#endif

void _EEPROM_writeData(int &pos, uint8_t* value, uint8_t size)
{
    do
    {
        eeprom_write_byte((unsigned char*)pos, *value);
        pos++;
        value++;
    }while(--size);
}
#define EEPROM_WRITE_VAR(pos, value) _EEPROM_writeData(pos, (uint8_t*)&value, sizeof(value))
void _EEPROM_readData(int &pos, uint8_t* value, uint8_t size)
{
    do
    {
        *value = eeprom_read_byte((unsigned char*)pos);
        pos++;
        value++;
    }while(--size);
}
#define EEPROM_READ_VAR(pos, value) _EEPROM_readData(pos, (uint8_t*)&value, sizeof(value))
//======================================================================================




#define EEPROM_OFFSET 100


// IMPORTANT:  Whenever there are changes made to the variables stored in EEPROM
// in the functions below, also increment the version number. This makes sure that
// the default values are used whenever there is a change to the data, to prevent
// wrong data being written to the variables.
// ALSO:  always make sure the variables in the Store and retrieve sections are in the same order.

// EEPROM Extension
// We don't want to invalidate everything when adding variables, so we introduce
// a sub-version to default only new fields when we add at the end.
// Although not bullet proof we use a magic cookie to detect first change.
#define EEPROM_SUBVERSION 3
#define EEPROM_MAGIC 0x3d

#ifdef EEPROM_SETTINGS
void Config_StoreSettings() 
{
  char ver[4]= "000";
  int i=EEPROM_OFFSET;
  EEPROM_WRITE_VAR(i,ver); // invalidate data first 
  EEPROM_WRITE_VAR(i,axis_steps_per_unit);
  EEPROM_WRITE_VAR(i,max_feedrate);  
  EEPROM_WRITE_VAR(i,max_acceleration_units_per_sq_second);
  EEPROM_WRITE_VAR(i,acceleration);
  EEPROM_WRITE_VAR(i,retract_acceleration);
  EEPROM_WRITE_VAR(i,minimumfeedrate);
  EEPROM_WRITE_VAR(i,mintravelfeedrate);
  EEPROM_WRITE_VAR(i,minsegmenttime);
  EEPROM_WRITE_VAR(i,max_xy_jerk);
  EEPROM_WRITE_VAR(i,max_z_jerk);
  EEPROM_WRITE_VAR(i,max_e_jerk);
  EEPROM_WRITE_VAR(i,add_homing);
  #ifndef ULTIPANEL
  int plaPreheatHotendTemp = PLA_PREHEAT_HOTEND_TEMP, plaPreheatHPBTemp = PLA_PREHEAT_HPB_TEMP, plaPreheatFanSpeed = PLA_PREHEAT_FAN_SPEED;
  int absPreheatHotendTemp = ABS_PREHEAT_HOTEND_TEMP, absPreheatHPBTemp = ABS_PREHEAT_HPB_TEMP, absPreheatFanSpeed = ABS_PREHEAT_FAN_SPEED;
 

 
  #endif
  EEPROM_WRITE_VAR(i,plaPreheatHotendTemp);
  EEPROM_WRITE_VAR(i,plaPreheatHPBTemp);
  EEPROM_WRITE_VAR(i,plaPreheatFanSpeed);
  EEPROM_WRITE_VAR(i,absPreheatHotendTemp);
  EEPROM_WRITE_VAR(i,absPreheatHPBTemp);
  EEPROM_WRITE_VAR(i,absPreheatFanSpeed);
 
  EEPROM_WRITE_VAR(i,zprobe_zoffset);
  #ifdef PIDTEMP
    EEPROM_WRITE_VAR(i,Kp);
    EEPROM_WRITE_VAR(i,Ki);
    EEPROM_WRITE_VAR(i,Kd);
  #else
		float dummy = 3000.0f;
    EEPROM_WRITE_VAR(i,dummy);
		dummy = 0.0f;
    EEPROM_WRITE_VAR(i,dummy);
    EEPROM_WRITE_VAR(i,dummy);
  #endif
  #ifndef DOGLCD
    int lcd_contrast = 32;
  #endif
  EEPROM_WRITE_VAR(i,lcd_contrast);
  // Start EEPROM Extension
  uint8_t b = EEPROM_MAGIC;
  EEPROM_WRITE_VAR(i,b);
  b = EEPROM_SUBVERSION;
  EEPROM_WRITE_VAR(i,b);
  // Sub-version 1: motor current
  #ifndef MOTOR_CURRENT_PWM_XY_PIN
    int motor_current_setting[3] = {0, 0, 0};
  #endif
  EEPROM_WRITE_VAR(i,motor_current_setting);
  // Sub-version 2: led brightness
  #if !(defined(LED_PIN) && LED_PIN > -1)
    int ledPwm;
  #endif
  EEPROM_WRITE_VAR(i,ledPwm);
  // Sub-version 3: Bed PID
  #ifdef PIDTEMPBED
    EEPROM_WRITE_VAR(i,bedKp);
    EEPROM_WRITE_VAR(i,bedKi);
    EEPROM_WRITE_VAR(i,bedKd);
  #else
    float dummy = 3000.0f;
    EEPROM_WRITE_VAR(i,dummy);
    dummy = 0.0f;
    EEPROM_WRITE_VAR(i,dummy);
    EEPROM_WRITE_VAR(i,dummy);
  #endif
  
  char ver2[4]=EEPROM_VERSION;
  i=EEPROM_OFFSET;
  EEPROM_WRITE_VAR(i,ver2); // validate data
  SERIAL_ECHO_START;
  SERIAL_ECHOLNPGM("Settings Stored");
}
#endif //EEPROM_SETTINGS


#ifndef DISABLE_M503
void Config_PrintSettings()
{  // Always have this function, even with EEPROM_SETTINGS disabled, the current values will be shown
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Steps per unit:");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M92 X",axis_steps_per_unit[0]);
    SERIAL_ECHOPAIR(" Y",axis_steps_per_unit[1]);
    SERIAL_ECHOPAIR(" Z",axis_steps_per_unit[2]);
    SERIAL_ECHOPAIR(" E",axis_steps_per_unit[3]);
    SERIAL_ECHOLN("");
      
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Maximum feedrates (mm/s):");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M203 X",max_feedrate[0]);
    SERIAL_ECHOPAIR(" Y",max_feedrate[1] ); 
    SERIAL_ECHOPAIR(" Z", max_feedrate[2] ); 
    SERIAL_ECHOPAIR(" E", max_feedrate[3]);
    SERIAL_ECHOLN("");

    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Maximum Acceleration (mm/s2):");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M201 X" ,max_acceleration_units_per_sq_second[0] ); 
    SERIAL_ECHOPAIR(" Y" , max_acceleration_units_per_sq_second[1] ); 
    SERIAL_ECHOPAIR(" Z" ,max_acceleration_units_per_sq_second[2] );
    SERIAL_ECHOPAIR(" E" ,max_acceleration_units_per_sq_second[3]);
    SERIAL_ECHOLN("");
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Acceleration: S=acceleration, T=retract acceleration");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M204 S",acceleration ); 
    SERIAL_ECHOPAIR(" T" ,retract_acceleration);
    SERIAL_ECHOLN("");

    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Advanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M205 S",minimumfeedrate ); 
    SERIAL_ECHOPAIR(" T" ,mintravelfeedrate ); 
    SERIAL_ECHOPAIR(" B" ,minsegmenttime ); 
    SERIAL_ECHOPAIR(" X" ,max_xy_jerk ); 
    SERIAL_ECHOPAIR(" Z" ,max_z_jerk);
    SERIAL_ECHOPAIR(" E" ,max_e_jerk);
    SERIAL_ECHOLN(""); 

    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Home offset (mm):");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M206 X",add_homeing[0] );
    SERIAL_ECHOPAIR(" Y" ,add_homeing[1] );
    SERIAL_ECHOPAIR(" Z" ,add_homeing[2] );
    SERIAL_ECHOLN("");
#ifdef DELTA
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Endstop adjustement (mm):");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M666 X",endstop_adj[0] );
    SERIAL_ECHOPAIR(" Y" ,endstop_adj[1] );
    SERIAL_ECHOPAIR(" Z" ,endstop_adj[2] );
    SERIAL_ECHOLN("");
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Delta settings: L=delta_diagonal_rod, R=delta_radius, S=delta_segments_per_second");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M665 L",delta_diagonal_rod );
    SERIAL_ECHOPAIR(" R" ,delta_radius );
    SERIAL_ECHOPAIR(" S" ,delta_segments_per_second );
    SERIAL_ECHOLN("");
#endif
#ifdef PIDTEMP
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("PID settings:");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("   M301 P",Kp); 
    SERIAL_ECHOPAIR(" I" ,unscalePID_i(Ki)); 
    SERIAL_ECHOPAIR(" D" ,unscalePID_d(Kd));
    SERIAL_ECHOLN(""); 
#endif
    // Sub-version 1
#ifdef MOTOR_CURRENT_PWM_XY_PIN
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Motor current:");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M907 X", (unsigned long)motor_current_setting[0]);
    SERIAL_ECHOPAIR(" Z", (unsigned long)motor_current_setting[1]);
    SERIAL_ECHOPAIR(" E", (unsigned long)motor_current_setting[2]);
    SERIAL_ECHOLN("");
#endif
    // Sub-version 2
#if defined(LED_PIN) && LED_PIN > -1
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Led brightness:");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("  M42 ", (unsigned long)ledPwm);
    SERIAL_ECHOLN("");
#endif
    // Sub-version 3
#ifdef PIDTEMPBED
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Bed PID settings:");
    SERIAL_ECHO_START;
    SERIAL_ECHOPAIR("   M304 P",bedKp);
    SERIAL_ECHOPAIR(" I" ,unscalePID_i(bedKi));
    SERIAL_ECHOPAIR(" D" ,unscalePID_d(bedKd));
    SERIAL_ECHOLN("");
#endif
} 
#endif


#ifdef EEPROM_SETTINGS
void Config_RetrieveSettings()
{
    int i=EEPROM_OFFSET;
    char stored_ver[4];
    char ver[4]=EEPROM_VERSION;
    EEPROM_READ_VAR(i,stored_ver); //read stored version
    //  SERIAL_ECHOLN("Version: [" << ver << "] Stored version: [" << stored_ver << "]");
    if (strncmp(ver,stored_ver,3) == 0)
    {
        // version number match
        EEPROM_READ_VAR(i,axis_steps_per_unit);
        EEPROM_READ_VAR(i,max_feedrate);  
        EEPROM_READ_VAR(i,max_acceleration_units_per_sq_second);
        
        // steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
		reset_acceleration_rates();
        
        EEPROM_READ_VAR(i,acceleration);
        EEPROM_READ_VAR(i,retract_acceleration);
        EEPROM_READ_VAR(i,minimumfeedrate);
        EEPROM_READ_VAR(i,mintravelfeedrate);
        EEPROM_READ_VAR(i,minsegmenttime);
        EEPROM_READ_VAR(i,max_xy_jerk);
        EEPROM_READ_VAR(i,max_z_jerk);
        EEPROM_READ_VAR(i,max_e_jerk);
        EEPROM_READ_VAR(i,add_homing);
        #ifndef ULTIPANEL
        int plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed;
        int absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed;

        #endif
        EEPROM_READ_VAR(i,plaPreheatHotendTemp);
        EEPROM_READ_VAR(i,plaPreheatHPBTemp);
        EEPROM_READ_VAR(i,plaPreheatFanSpeed);
        EEPROM_READ_VAR(i,absPreheatHotendTemp);
        EEPROM_READ_VAR(i,absPreheatHPBTemp);
        EEPROM_READ_VAR(i,absPreheatFanSpeed);
        

        
        EEPROM_READ_VAR(i,zprobe_zoffset);
        #ifndef PIDTEMP
        float Kp,Ki,Kd;
        #endif
        // do not need to scale PID values as the values in EEPROM are already scaled		
        EEPROM_READ_VAR(i,Kp);
        EEPROM_READ_VAR(i,Ki);
        EEPROM_READ_VAR(i,Kd);
        #ifndef DOGLCD
        int lcd_contrast;
        #endif
        EEPROM_READ_VAR(i,lcd_contrast);

        // Get sub-version
        uint8_t sub_version;
        EEPROM_READ_VAR(i,sub_version);
        if (sub_version == EEPROM_MAGIC) {
          // Magic matches, get sub-version
          EEPROM_READ_VAR(i,sub_version);
        } else {
          // Assume first call
          sub_version = 0;
        }
        // Sub-version 1
        if (sub_version < 1) {
          #ifdef MOTOR_CURRENT_PWM_XY_PIN
            int mcs[3] = DEFAULT_PWM_MOTOR_CURRENT;
            for (uint8_t j = 0; j < 3 ; j++)
                motor_current_setting[j] = mcs[j];
          #endif
        } else {
          #ifndef MOTOR_CURRENT_PWM_XY_PIN
            int motor_current_setting[3];
          #endif
          EEPROM_READ_VAR(i,motor_current_setting);
        }
        #ifdef MOTOR_CURRENT_PWM_XY_PIN
          digipot_init();
        #endif
        // Sub-version 2
        if (sub_version < 2) {
          #if defined(LED_PIN) && LED_PIN > -1
            ledPwm=0;
          #endif
        } else {
          #if !(defined(LED_PIN) && LED_PIN > -1)
            int ledPwm;
          #endif
          EEPROM_READ_VAR(i,ledPwm);
        }
        #if defined(LED_PIN) && LED_PIN > -1
          if (ledInit) {
            // Only setup led if we passed the init
            pinMode(LED_PIN, OUTPUT);
            analogWrite(LED_PIN, ledPwm);
          }
        #endif
        // Sub-version 3
        if (sub_version >= 3) {
          #ifndef PIDTEMPBED
            float bedKp,bedKi,bedKd;
          #endif
          // do not need to scale PID values as the values in EEPROM are already scaled
          EEPROM_READ_VAR(i,bedKp);
          EEPROM_READ_VAR(i,bedKi);
          EEPROM_READ_VAR(i,bedKd);
        }
        #ifdef PIDTEMPBED
          if (sub_version < 3 || (bedKp == 3000.0f && bedKi == 0.0f)) {
            // If we have no saved values or if saved values are stubs, use default
            bedKp=DEFAULT_bedKp;
            bedKi=(DEFAULT_bedKi*PID_dT);
            bedKd=(DEFAULT_bedKd/PID_dT);
          }
        #endif

        // Call updatePID (similar to when we have processed M301)
        updatePID();

        SERIAL_ECHO_START;
        SERIAL_ECHOLNPGM("Stored settings retrieved");
    }
    else
    {
        Config_ResetDefault();
    }
    #ifdef EEPROM_CHITCHAT
      Config_PrintSettings();
    #endif
}
#endif

void Config_ResetDefault()
{
    float tmp1[]=DEFAULT_AXIS_STEPS_PER_UNIT;
    float tmp2[]=DEFAULT_MAX_FEEDRATE;
    long tmp3[]=DEFAULT_MAX_ACCELERATION;
    for (short i=0;i<4;i++) 
    {
        axis_steps_per_unit[i]=tmp1[i];  
        max_feedrate[i]=tmp2[i];  
        max_acceleration_units_per_sq_second[i]=tmp3[i];
    }
    
    // steps per sq second need to be updated to agree with the units per sq second
    reset_acceleration_rates();
    
    acceleration=DEFAULT_ACCELERATION;
    retract_acceleration=DEFAULT_RETRACT_ACCELERATION;
    minimumfeedrate=DEFAULT_MINIMUMFEEDRATE;
    minsegmenttime=DEFAULT_MINSEGMENTTIME;       
    mintravelfeedrate=DEFAULT_MINTRAVELFEEDRATE;
    max_xy_jerk=DEFAULT_XYJERK;
    max_z_jerk=DEFAULT_ZJERK;
    max_e_jerk=DEFAULT_EJERK;
    add_homeing[0] = add_homeing[1] = add_homeing[2] = 0;
#ifdef DELTA
    endstop_adj[0] = endstop_adj[1] = endstop_adj[2] = 0;
    delta_radius= DELTA_RADIUS;
    delta_diagonal_rod= DELTA_DIAGONAL_ROD;
    delta_segments_per_second= DELTA_SEGMENTS_PER_SECOND;
    recalc_delta_settings(delta_radius, delta_diagonal_rod);
#endif
#ifdef ULTIPANEL
    plaPreheatHotendTemp = PLA_PREHEAT_HOTEND_TEMP;
    plaPreheatHPBTemp = PLA_PREHEAT_HPB_TEMP;
    plaPreheatFanSpeed = PLA_PREHEAT_FAN_SPEED;
    absPreheatHotendTemp = ABS_PREHEAT_HOTEND_TEMP;
    absPreheatHPBTemp = ABS_PREHEAT_HPB_TEMP;
    absPreheatFanSpeed = ABS_PREHEAT_FAN_SPEED;
#endif
#ifdef ENABLE_AUTO_BED_LEVELING
    zprobe_zoffset = -Z_PROBE_OFFSET_FROM_EXTRUDER;
#endif
#ifdef DOGLCD
    lcd_contrast = DEFAULT_LCD_CONTRAST;
#endif
#ifdef PIDTEMP
    Kp = DEFAULT_Kp;
    Ki = scalePID_i(DEFAULT_Ki);
    Kd = scalePID_d(DEFAULT_Kd);
    
    // call updatePID (similar to when we have processed M301)
    updatePID();
    
#ifdef PID_ADD_EXTRUSION_RATE
    Kc = DEFAULT_Kc;
#endif//PID_ADD_EXTRUSION_RATE
#endif//PIDTEMP

#ifdef MOTOR_CURRENT_PWM_XY_PIN
    int mcs[3] = DEFAULT_PWM_MOTOR_CURRENT;
    for (uint8_t j = 0; j < 3 ; j++)
      motor_current_setting[j] = mcs[j];
    digipot_init();
#endif

#if defined(LED_PIN) && LED_PIN > -1
    ledPwm = 0;
    analogWrite(LED_PIN, 0);
#endif

#ifdef PIDTEMPBED
    bedKp = DEFAULT_bedKp;
    bedKi = scalePID_i(DEFAULT_bedKi);
    bedKd = scalePID_d(DEFAULT_bedKd);
    updatePID();
#endif //PIDTEMPBED

SERIAL_ECHO_START;
SERIAL_ECHOLNPGM("Hardcoded Default Settings Loaded");

}
