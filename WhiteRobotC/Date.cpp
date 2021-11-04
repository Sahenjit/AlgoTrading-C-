//
// Created by Shankar on 04/11/2021.
//

#include "Date.h"

Date::Date(std::string userinput)
{
    std::stringstream test(userinput);
    std::string segment;
    while(std::getline(test, segment, ' '))
    {
        seglist.push_back(segment); //Spit string at '_' character
    }
}

std::string Date::reformat_date()
{
    int no_of_slashes = 0;
    for(int i=0;i<seglist[0].size();i++)
    {
        if(seglist[0][i] == '/')
        {
            no_of_slashes++;
            continue;
        }
        if(no_of_slashes==0)
        {
            day.push_back(seglist[0][i]);
        }
        else if (no_of_slashes == 1)
        {
            month.push_back(seglist[0][i]);
        } else if (no_of_slashes == 2)
        {
            year.push_back(seglist[0][i]);
        }
    }
    return year + '-' +month + '-' + day + " "+ seglist[1];
}