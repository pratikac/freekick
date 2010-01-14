#ifndef __THRESHOLDS__
#define __THRESHOLDS__

//segmentation #defs
// hsv = [ (0,180), (0,256), (0,256) ]
#define BALL_HUE_L		    (170)
#define BALL_HUE_U		    (30)
#define BALL_SAT_L		    (75)
#define BALL_SAT_U		    (150)

#define GOAL_R_HUE_L		(108)
#define GOAL_R_HUE_U		(103)
#define GOAL_R_SAT_L		(210)
#define GOAL_R_SAT_U		(108)

#define GOAL_B_HUE_L		(127)
#define GOAL_B_HUE_U		(133)
#define GOAL_B_SAT_L		(115)
#define GOAL_B_SAT_U		(125)

#define TEAM_B_HUE_L		(103)
#define TEAM_B_HUE_U		(210)
#define TEAM_B_SAT_L		(195)
#define TEAM_B_SAT_U		(195)

#define TEAM_R_HUE_L		(165)
#define TEAM_R_HUE_U		(15)
#define TEAM_R_SAT_L		(90)
#define TEAM_R_SAT_U		(170)

#define TAG_B_HUE_L		    (145)
#define TAG_B_HUE_U		    (165)
#define TAG_B_SAT_L		    (46)
#define TAG_B_SAT_U		    (77)
#define TAG_B_VAL_L		    (46)
#define TAG_B_VAL_U		    (77)

#define TAG_W_HUE_L		    (90)
#define TAG_W_HUE_U		    (160)
#define TAG_W_SAT_L		    (5)
#define TAG_W_SAT_U		    (10)
#define TAG_W_VAL_L		    (46)
#define TAG_W_VAL_U		    (77)

#define VAL_L			    (100)
#define VAL_U			    (250)

// size #defs
#define BOT_AREA_MIN                    (4000)
#define BOT_AREA_MAX                    (10000)
#define BOT_COMPACTNESS_MIN             (0)
#define BOT_COMPACTNESS_MAX             (10)

#define GOAL_AREA_MIN                   (2500)
#define GOAL_AREA_MAX                   (5500)
#define GOAL_COMPACTNESS_MAX            (8)

#define BALL_AREA_MIN                   (150)
#define BALL_AREA_MAX                   (300)
#define BALL_COMPACTNESS_MAX            (9)

#define TAG_CIRCLE_AREA_MIN             (200)
#define TAG_CIRCLE_AREA_MAX             (1000)
#define TAG_CIRCLE_COMPACTNESS_MIN      (5)

#define TAG_RECT_AREA_MIN               (100)
#define TAG_RECT_AREA_MAX               (500)
#define TAG_RECT_COMPACTNESS_MAX        (4)


# endif
