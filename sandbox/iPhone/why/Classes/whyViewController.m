//
//  whyViewController.m
//  why
//
//  Created by Local Admin on 3/20/09.
//  Copyright Robotics @ Maryland 2009. All rights reserved.
//

#import "whyViewController.h"
#define PORT 9219
#define IP_ADDR "127.0.0.1"

#define CMD_NOTHING     0

#define CMD_TURNLEFT    1
#define CMD_TURNRIGHT   2

#define CMD_ASCEND      3
#define CMD_DESCEND     4

#define CMD_INCSPEED    5
#define CMD_DECSPEED    6

#define CMD_ZEROSPEED   7
#define CMD_EMERGSTOP   8

#define CMD_SETSPEED    9
#define CMD_TSETSPEED 11
#define CMD_INCTSETSPEED 14
#define CMD_DECTSETSPEED 15

#define CMD_ANGLEYAW 10
#define CMD_ANGLEPITCH 12
#define CMD_ANGLEROLL 13

#define PARAM           0

@implementation whyViewController
@synthesize why_button;

@synthesize increase_speed_button;
@synthesize decrease_speed_button;

@synthesize turn_left_button;
@synthesize turn_right_button;

@synthesize ascend_button;
@synthesize descend_button;

@synthesize zero_speed_button;
@synthesize emergency_stop_button;

@synthesize side_step_button;
@synthesize increase_side_step_speed_button;
@synthesize decrease_side_step_speed_button;

@synthesize start_client_button;



struct sockaddr_in host_addr;
int sockfd = 0;
BOOL client_started = NO;

/*
 // The designated initializer. Override to perform setup that is required before the view is loaded.
 - (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
 if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
 // Custom initialization
 }
 return self;
 }
 */

/*
 // Implement loadView to create a view hierarchy programmatically, without using a nib.
 - (void)loadView {
 }
 */


/*
 // Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
 - (void)viewDidLoad {
 [super viewDidLoad];
 }
 */


/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
    [super dealloc];
}

#pragma mark 

/* Triggered by the button being pressed */
-(IBAction) pressed_why_button:(id)sender{
    [self create_client:PORT :(char **)IP_ADDR];
    
    /* Increases the speed */
    if(sender == self.increase_speed_button){
       
        [self send_command:sockfd :CMD_INCSPEED :PARAM];
    }
    
    /* Decrease the speed */
    if(sender == self.decrease_speed_button){
        
        [self send_command:sockfd :CMD_DECSPEED :PARAM];
  
    }
    
    /* Ascend */
    if(sender == self.ascend_button){
        
        [self send_command:sockfd :CMD_ASCEND :PARAM];

    }
    
    /* Descend */
    if(sender == self.descend_button){
        
        [self send_command:sockfd :CMD_DESCEND :PARAM];

    }
    
    /*Turn left */
    if(sender == self.turn_left_button){
        
        [self send_command:sockfd :CMD_TURNLEFT :PARAM];
       
    }
    
    /*Turn Right*/
    if(sender == self.turn_right_button){
        
        [self send_command:sockfd :CMD_TURNRIGHT :PARAM];

    }
    
    /*Zero Speed */
    if(sender == self.zero_speed_button){
        
        [self send_command:sockfd :CMD_ZEROSPEED :PARAM];
      
    }
    
    /* Emergency Stop */
    if(sender == self.emergency_stop_button){
        
        [self send_command:sockfd :CMD_EMERGSTOP :PARAM];
      
    }
    
}

/* Sends the command to the server */
-(void) send_command:(int)socket:(int)command:(int)parameter{
    signed char buf[2];
    buf[0]= command;
    buf[1]= parameter;
    
    if(sendto(socket, buf,2,0, (struct sockaddr *) &host_addr, sizeof(struct sockaddr_in)) == -1){
        NSString *title = [[NSString alloc] initWithFormat:
                           @"Cannot Send Message to the server"];
        UIAlertView *alert_server = [[UIAlertView alloc] initWithTitle:title 
                                                               message:nil
                                                              delegate:nil
                                                     cancelButtonTitle:@"Fuck!!"
                                                     otherButtonTitles:nil];
        [alert_server show];
        [alert_server release];
        [title release];
        
    }
}

/*Tigger the client */
-(void)create_client:(int)argc:(char **)argv{
    
    struct hostent *host;
    /*argv[1];*/
    if((host = gethostbyname(IP_ADDR)) == NULL){
        NSString *title = [[NSString alloc] initWithFormat:
                           @"Cannot get host by name"];
        UIAlertView *alert_server = [[UIAlertView alloc] initWithTitle:title 
                                                               message:nil
                                                              delegate:nil
                                                     cancelButtonTitle:@"Try Again"
                                                     otherButtonTitles:nil];
        [alert_server show];
        [alert_server release];
        [title release];
        
    }
    
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        NSString *title = [[NSString alloc] initWithFormat:
                           @"Cannot create the desired socket"];
        UIAlertView *alert_server = [[UIAlertView alloc] initWithTitle:title 
                                                               message:nil
                                                              delegate:nil
                                                     cancelButtonTitle:@"Try Again"
                                                     otherButtonTitles:nil];
        [alert_server show];
        [alert_server release];
        [title release];
    }
    
    memset((void *) &host_addr, 0, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr = *((struct in_addr *)host->h_addr);
    
    
}
@end
