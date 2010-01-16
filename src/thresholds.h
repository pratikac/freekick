#ifndef __THRESHOLDS__
#define __THRESHOLDS__

//segmentation #defs
// hsv = [ (0,180), (0,256), (0,256) ]
#define BALL_HUE_L		    (175)
#define BALL_HUE_U		    (20)
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

#define TEAM_B_HUE_L		(80)
#define TEAM_B_HUE_U		(95)
#define TEAM_B_SAT_L		(36)
#define TEAM_B_SAT_U		(64)

#define TEAM_R_HUE_L		(160)
#define TEAM_R_HUE_U		(20)
#define TEAM_R_SAT_L		(100)
#define TEAM_R_SAT_U		(130)

#define TAG_B_HUE_L		    (140)
#define TAG_B_HUE_U		    (170)
#define TAG_B_SAT_L		    (50)
#define TAG_B_SAT_U		    (80)
#define TAG_B_VAL_L		    (100)
#define TAG_B_VAL_U		    (145)

#define TAG_W_HUE_L		    (10)
#define TAG_W_HUE_U		    (170)
#define TAG_W_SAT_L		    (0)
#define TAG_W_SAT_U		    (10)
#define TAG_W_VAL_L		    (250)
#define TAG_W_VAL_U		    (255)

#define VAL_L			    (100)
#define VAL_U			    (255)

// size #defs
#define BOT_AREA_MIN                    (2000)
#define BOT_AREA_MAX                    (10000)
#define BOT_COMPACTNESS_MIN             (0)
#define BOT_COMPACTNESS_MAX             (10)

#define GOAL_AREA_MIN                   (2500)
#define GOAL_AREA_MAX                   (5500)
#define GOAL_COMPACTNESS_MAX            (8)

#define BALL_AREA_MIN                   (275)
#define BALL_AREA_MAX                   (375)
#define BALL_COMPACTNESS_MIN            (9)

#define TAG_CIRCLE_AREA_MIN             (300)
#define TAG_CIRCLE_AREA_MAX             (650)
#define TAG_CIRCLE_COMPACTNESS_MIN      (5)

#define TAG_RECT_AREA_MIN               (750)
#define TAG_RECT_AREA_MAX               (1200)
#define TAG_RECT_COMPACTNESS_MAX        (4)


# endif
