//
//  whyViewController.h
//  why
//
//  Created by Local Admin on 3/20/09.
//  Copyright Robotics @ Maryland 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>


@interface whyViewController : UIViewController {
    IBOutlet UIButton *why_button;
    
    IBOutlet UIButton *increase_speed_button;
    IBOutlet UIButton *decrease_speed_button;
    
    IBOutlet UIButton *turn_left_button;
    IBOutlet UIButton *turn_right_button;
    
    IBOutlet UIButton *ascend_button;
    IBOutlet UIButton *descend_button;
    
    IBOutlet UIButton *zero_speed_button;
    IBOutlet UIButton *emergency_stop_button;
    
    IBOutlet UIButton *side_step_button;
    IBOutlet UIButton *increase_side_step_speed_button;
    IBOutlet UIButton *decrease_side_step_speed_button;
    
    IBOutlet UIButton *start_client_button;
}
@property (nonatomic, retain) UIButton *why_button;

@property (nonatomic, retain) UIButton *increase_speed_button;
@property (nonatomic, retain) UIButton *decrease_speed_button;

@property (nonatomic, retain) UIButton *turn_left_button;
@property (nonatomic, retain) UIButton *turn_right_button;

@property (nonatomic, retain) UIButton *ascend_button;
@property (nonatomic, retain) UIButton *descend_button;

@property (nonatomic, retain) UIButton *zero_speed_button;
@property (nonatomic, retain) UIButton *emergency_stop_button;

@property (nonatomic, retain) UIButton *side_step_button;
@property (nonatomic, retain) UIButton *increase_side_step_speed_button;
@property (nonatomic, retain) UIButton *decrease_side_step_speed_button;

@property (nonatomic, retain) UIButton *start_client_button;

-(IBAction) pressed_why_button:(id)sender;
-(void) send_command:(int)socket:(int)command:(int)parameter;
-(void) create_client:(int)argc:(char **)argv;
@end

