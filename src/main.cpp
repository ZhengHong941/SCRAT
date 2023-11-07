#include "main.h"
#include "pros/adi.h"
#include "pros/adi.hpp"
#include "pros/misc.h"
#include "pros/motors.h"

double prevErrorLeft = 0;
double prevErrorRight = 0;
double encdleft = 0;
double encdright = 0;
double errorLeft = 0;
double errorRight = 0;
double LEFTTARGET = 0;
double RIGHTTARGET = 0;

void pidvalues(double targleft, double targright){
  LEFTTARGET = targleft;
  RIGHTTARGET = targright;

  errorLeft = targleft;
  errorRight = targright;
}

void pidmove() {
    using namespace pros;
    //Motor lf_wheel (lf_port);
    //Motor rf_wheel (rf_port);
    Motor lf_base(lf_port, pros::E_MOTOR_GEARSET_06, true, pros::E_MOTOR_ENCODER_DEGREES);
	Motor lt_base(lt_port, pros::E_MOTOR_GEARSET_06, true, pros::E_MOTOR_ENCODER_DEGREES);
	Motor lb_base(lb_port, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
	Motor rf_base(rf_port, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
	Motor rt_base(rt_port, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
	Motor rb_base(rb_port, pros::E_MOTOR_GEARSET_06, true, pros::E_MOTOR_ENCODER_DEGREES);
    //25 units = 1cm
    //and fabs(errorRight) >= 50
    while (true){
        if (fabs(errorLeft) >= 50 ){
            while (fabs(errorLeft) >= 50 ){
                encdleft = lf_base.get_position();
                encdright = rf_base.get_position();

                errorLeft = LEFTTARGET - encdleft;
                errorRight = RIGHTTARGET - encdright;

                double deltaErrorLeft = errorLeft - prevErrorLeft;
                double deltaErrorRight = errorRight - prevErrorRight;

                double powerL = base_kp * (errorLeft/25) + base_kd * deltaErrorLeft;
                double powerR = base_kp * (errorRight/25) + base_kd * deltaErrorRight;

                lf_base.move(powerL);
                lt_base.move(powerL);
                lb_base.move(powerL);
                rf_base.move(powerR);
                rt_base.move(powerR);
                rb_base.move(powerR);

    
                //printf("encdleft: %f encdright:%f errorleft:%f errorright:%f deltaerrleft:%f deltaerrright:%f \n",\
                encdleft, encdright, errorLeft, errorRight, deltaErrorLeft, deltaErrorRight);

                prevErrorLeft = errorLeft;
                prevErrorRight = errorRight;
                pros::delay(2);
            }
            // lf_base.brake();
            // lt_base.brake();
            // lb_base.brake();
            // rf_base.brake();
            // rt_base.brake();
            // rb_base.brake();
            //printf("stopped");
        }
        pros::delay(2);
    }
}

bool shoot = false;
float cata_error;
float prev_cata_error;
float cata_d;
uint32_t timestamp;
int correctingPow;

void cata_pid(){
    using namespace pros;
    pros::Motor lc(lc_port);
    pros::Motor rc(rc_port);
    pros::Rotation catarot(catarot_port);
    while(true){
        int currentPos = catarot.get_position() / 100;
        cata_error = cata_target - currentPos;
        cata_d = cata_error - prev_cata_error;
        prev_cata_error = cata_error;
        correctingPow = cata_error * cata_kp + cata_d * cata_kd + cata_power;
        //printf("CorrectingPow: %i \n", correctingPow);
        if(shoot){
            lc.move(30);
            rc.move(30);
            pros::delay(500);
            shoot = false;
            pros::delay(Catadelay);
            
        }
        else if (cata_error > allowedError && catarot.get_position() > cata_target) {
            lc.move(correctingPow);
            rc.move(correctingPow);
        }
        else {
            lc.move(0);
            rc.move(0);
        }
    }
}


bool IntakeTargetPosUp;
int RollerPow;
float flipper_error;
float prev_flipper_error;
float flipper_d;
float total_flipper_error;

void flipper_pid() {
    using namespace pros;
    pros::Motor fs(fs_port);
    pros::Motor fr(fr_port);
    pros::Rotation flipperrot(flipperrot_port);
    
    // IntakeTargetPosUp = true; //move to up position    
    // IntakeTargetPosUp = false; //move to down position

    // RollerPow = 127;//roller outtake    
    // RollerPow = -127; //roller intake    
    //RollerPow = 0; //roller stop
    while(true){
        int currentPos = flipperrot.get_position() / 100;
        //PID loop to get the arm to the target position
        //calculates TargetOmegaA, and ActualOmegaFS will be changed according to the PID loop in order to reach the target encoder value given by flipper_target
        
        if(IntakeTargetPosUp)
            flipper_error = currentPos - flipper_targetUp;
        else
            flipper_error = currentPos - flipper_targetDown;
        
        flipper_d = flipper_error - prev_flipper_error;
        prev_flipper_error = flipper_error;
        total_flipper_error += flipper_error;
        
        fs.move(flipper_error * flipper_kp + total_flipper_error * flipper_ki + flipper_d * flipper_kd);
        fr.move(RollerPow);
    }
}


void initialize() {
    //controller
    pros::Controller master(CONTROLLER_MASTER);

	//base
    pros::Motor lf_base(lf_port, pros::E_MOTOR_GEARSET_06, true, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor lt_base(lt_port, pros::E_MOTOR_GEARSET_06, true, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor lb_base(lb_port, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor rf_base(rf_port, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor rt_base(rt_port, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor rb_base(rb_port, pros::E_MOTOR_GEARSET_06, true, pros::E_MOTOR_ENCODER_DEGREES);

    //flipper
    pros::Motor fs(fs_port, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor fr(fr_port, pros::E_MOTOR_GEARSET_06, true, pros::E_MOTOR_ENCODER_DEGREES);
    pros::Rotation flipperrot(flipperrot_port);
    
    //cata
    pros::Motor lc(lc_port, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor rc(rc_port, pros::E_MOTOR_GEARSET_36, false, pros::E_MOTOR_ENCODER_DEGREES);
    // lc.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    // rc.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    pros::Rotation catarot(catarot_port);

    //side rollers
    pros::Motor lr (lr_port, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
    pros::Motor rr (lr_port, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

    //pid tasks
    pros::Task hardcode(pidmove);
    pros::Task flipper(flipper_pid);
    pros::Task cata(cata_pid);
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

void opcontrol() {

    // pros::Controller master(CONTROLLER_MASTER);

	// //base motors
	// pros::Motor lf_base(lf_port);
	// pros::Motor lt_base(lt_port);
	// pros::Motor lb_base(lb_port);
	// pros::Motor rf_base(rf_port);
	// pros::Motor rt_base(rt_port);
	// pros::Motor rb_base(rb_port);

	// //drive mode control
	// bool tankdrive = true;

    // //flipper motors
    // pros::Motor fs(fs_port);
    // pros::Motor fr(fr_port);
    // pros::Rotation flipperrot(flipperrot_port);


    // bool IntakeTargetPosUp = true;
    // int RollerPow = 0;
    // float flipper_error;
    // float prev_flipper_error;
    // float flipper_d;
    // float total_flipper_error;

    // //cata motors
    // pros::Motor lc(lc_port);
    // pros::Motor rc(rc_port);
    // pros::Rotation catarot(catarot_port);

    // int cata_error;
    // int prev_cata_error;
    // int cata_d;
    // uint32_t timestamp;
    // int correctingPow;

    //side rollers motor
    pros::Motor lr(lr_port);
    pros::Motor rr(rr_port);
    
    //auton code ==================================

    //resetting of flipper arm and cata position
    //not for actual auton code
    IntakeTargetPosUp = true;
    shoot = false;
    pros::delay(5000);

    //fast shooting when Gupta hasn't reach the goal
    for (int i = 1; i < 6; i++){
    IntakeTargetPosUp = true;
    shoot = true;
    pros::delay(100);
    IntakeTargetPosUp = false;
    pros::delay(500);
    RollerPow = 80;
    pros::delay(100);
    RollerPow = 120;
    pros::delay(100);
    RollerPow = 0;
    pros::delay(50);
    IntakeTargetPosUp = true;
    pros::delay(700);
    }
    //accurate shooting when Gupta is catching
    for (int i = 1; i < 16; i++){
    IntakeTargetPosUp = true;
    shoot = true;
    pros::delay(100);
    IntakeTargetPosUp = false;
    pros::delay(500);
    RollerPow = 80;
    pros::delay(100);
    RollerPow = 120;
    pros::delay(100);
    RollerPow = 0;
    pros::delay(50);
    IntakeTargetPosUp = true;
    pros::delay(1000);
    }
    

    //movement code
    int a_l = 1000;
	int a_r = 1000;
	int b_l = 0;
	int b_r = 980;
	int c_l = 2650;
	int c_r = 2300;
	int d_l = 1000;
	int d_r = -1000;
	int e_l = -2300;
	int e_r = -1750;
	
    //flipper arm must be up when moving
    IntakeTargetPosUp = true;
	
	pidvalues(a_l, a_r);
	pros::delay(800);
	pidvalues(a_l+b_l, a_r+b_r);
	pros::delay(400);
	pidvalues(a_l+b_l+c_l, a_r+b_r+c_r);
	lr.move(-1000);
    rr.move(-1000);
	pros::delay(1450);
	pidvalues(a_l+b_l+c_l+d_l, a_r+b_r+c_r+d_r);
	pros::delay(700);
	pidvalues(a_l+b_l+c_l+d_l+e_l, a_r+b_r+c_r+d_r+e_r);
	pros::delay(900);




	// while(true){
    //     //base control
    //     double left, right;
    //     if(master.get_digital_new_press(DIGITAL_Y)) tankdrive = !tankdrive;
    //     if(tankdrive) {
    //         left = master.get_analog(ANALOG_LEFT_Y);
    //         right = master.get_analog(ANALOG_RIGHT_Y);
    //     } 
                
    //     else {
    //         double power =  master.get_analog(ANALOG_LEFT_Y);
    //         double turn = master.get_analog(ANALOG_RIGHT_X);
    //         left = power + turn;
    //         right = power - turn;
    //     }

    //     lf_base.move(left);
    //     lt_base.move(left);
    //     lb_base.move(left);
    //     rf_base.move(right);
    //     rt_base.move(right);
    //     rb_base.move(right);

    //     //flipper control

    //     //update target speeds for I and update target position for flipper
    //     if(master.get_digital_new_press(DIGITAL_X))
    //         IntakeTargetPosUp = true; //move to up position
    //     else if(master.get_digital_new_press(DIGITAL_B))
    //         IntakeTargetPosUp = false; //move to down position

    //     if(master.get_digital(DIGITAL_DOWN))
    //         RollerPow = 127;//roller outtake
    //     else if(master.get_digital(DIGITAL_UP))
    //         RollerPow = -127; //roller intake
    //     else
    //         RollerPow = 0; //roller stop

    //     int currentPos = flipperrot.get_position() / 100;
    //     //PID loop to get the arm to the target position
    //     //calculates TargetOmegaA, and ActualOmegaFS will be changed according to the PID loop in order to reach the target encoder value given by flipper_target
        
    //     if(IntakeTargetPosUp)
    //         flipper_error = currentPos - flipper_targetUp;
    //     else
    //         flipper_error = currentPos - flipper_targetDown;

    //     prev_flipper_error = flipper_error;
    //     total_flipper_error += flipper_error;

    //     fs.move(-flipper_error * flipper_kp + total_flipper_error * flipper_ki + prev_flipper_error * flipper_kd);
    //     fr.move(RollerPow);

        
    //     //updating values of these global variables
    //     cata_error = cata_target - catarot.get_position()/100;
    //     cata_d = cata_error - prev_cata_error;
    //     correctingPow = cata_error * cata_kp + cata_d * cata_kd + cata_power;
        
    //     // latest cata control
    //     if(master.get_digital(DIGITAL_L2)){
    //         lc.move(30);
    //         rc.move(30);
    //         timestamp = pros::millis();
    //     }
        
    //     else if(pros::millis() - timestamp > Catadelay){
    //         //delay is time taken for catapult arm to fully fire
    //         //timestamp records the moment from which the catapult began spinning and the slip gear slips
    //         //after some delay, the catapult has fired and the slip gear can then begin to rewind
    //         if(catarot.get_position() > cata_target //if we are still undershooting
    //         && abs(cata_error) > allowedError){ //magnitude of catapult error is greater than allowed error
    //             lc.move(correctingPow);
    //             rc.move(correctingPow);
    //         }

    //         else{
    //             lc.move(0);
    //             rc.move(0);
    //         }
    //     }
        
    //     //cata debugging
    //     // printf("Position: %i \n", catarot.get_position()/100);
    //     // printf("Error: %i \n", cata_error);
    //     // printf("CorrectingPow: %i \n", correctingPow);
    //     // printf("Current: %i \n", lc.get_current_draw());

    //     //side rollers control
    //     lr.move(100 * (master.get_digital(DIGITAL_R2) - master.get_digital(DIGITAL_R1)));
    //     rr.move(100 * (master.get_digital(DIGITAL_R2) - master.get_digital(DIGITAL_R1)));


    //     pros::delay(5);
	// }
}