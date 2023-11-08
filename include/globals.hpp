#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

//base
#define lf_port 10
#define lt_port 6
#define lb_port 8
#define rf_port 20
#define rt_port 19
#define rb_port 18
#define imu_port 9

//flipper
#define fs_port 5
#define fr_port 4
#define flipperrot_port 15

//cata
#define lc_port 7
#define rc_port 17
#define catarot_port 12

//side rollers
#define lr_port 2
#define rr_port 11

//pid valules
#define base_kp 1.5
#define base_kd 0

#define flipper_targetUp 240 //degrees
#define flipper_targetDown 325 //degrees
#define flipper_kp -5
#define flipper_kd 0
#define flipper_ki 0

#define cata_kp 5
#define cata_kd 0
#define Catadelay 400
#define allowedError 2
#define cata_target 353 //216
#define cata_power 50

#endif