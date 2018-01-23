#ifndef LRUPOLICY_H
#define LRUPOLICY_H

#include "Policy.h"
#include <ctime>

class TimePolicy : public Policy {

private:
	long int minimumAge;

public:

	//Takes minimum age for file to be migrated in seconds
	TimePolicy::TimePolicy(long int minAge)
	{
		minimumAge = minAge;
	}

	//Takes minimum age for file to be migrated in years, months, days, hours, minutes, seconds format
	TimePolicy::TimePolicy(long int years, long int months, long int days, long int hours, long int minutes, long int seconds)
	{
		//convert years, months, etc. to seconds
		years = years * 31536000;
		months = months * 2592000;
		days = days * 86400;
		hours = hours * 3600;
		minutes = minutes * 60;

		//tally up the total
		minimumAge = seconds + minutes + hours + days + months + years;
	}


	bool isFileKept(FileData dataObject) 
	{
		//if last access is before current time minus minimum age of files to be migrated
		if (dataObject.lastAccessed <= (time(NULL) - minimumAge))
		{
			//migrate the file
			return false;
		}
		else
		{
			//keep the file
			return true;
		}

	}
};
#endif
