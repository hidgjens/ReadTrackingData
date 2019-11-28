#ifndef readtrackingdata_football_roster_teamscores_hpp_
#define readtrackingdata_football_roster_teamscores_hpp_

#include <cstdint>

namespace Football
{

namespace Roster
{

/*!
 *  \brief  Container to store a team's score in a match
 * 
 *  Currently contains:
 *      Goals scored
 *      Expected Goals (xG, total)
 *  
 */
struct TeamScores
{

std::uint16_t   goals_scored;
float           total_xG;

TeamScores()
{
    //  do  nothing
}

TeamScores(
    std::uint16_t   _goals_scored,
    float           _xG_total
)
:   total_xG        (_xG_total)
,   goals_scored    (_goals_scored)
{
    //  do  nothing
}

};


}   /*  namespace Roster    */

}   /*  namespace Football  */

#endif  /*  readtrackingdata_football_roster_teamscores_hpp_    */