

#ifndef LASCOMMON_H_INCLUDED
#define LASCOMMON_H_INCLUDED


/**
 * \todo to be documented
 */
typedef struct  {

    double t;
    double x, y, z;
    unsigned short intensity;
    unsigned char cls;
    char scan_angle;
    unsigned char user_data;
    unsigned short retnum;
    unsigned short numret;
    unsigned short scandir;
    unsigned short fedge;
    unsigned short red;
    unsigned short green;
    unsigned short blue;
    long rgpsum;    
    int number_of_point_records;
    int number_of_points_by_return[8];
    int number_of_returns_of_given_pulse[8];
    int classification[32];
    int classification_synthetic;
    int classification_keypoint;
    int classification_withheld;
    LASPointH pmax;
    LASPointH pmin;
} LASPointSummary;

#endif
