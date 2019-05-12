#define RACE_NAME        "race"

#define RACE_IOC_ATTACH  _IOR('R', 0, int)
#define RACE_IOC_DETACH  _IOW('R', 1, int)
#define RACE_IOC_QUERY   _IOW('R', 2, int)
#define RACE_IOC_LIST    _IO('R', 3)
