#ifndef readtrackingdata_football_roster_teamroster_hpp_
#define readtrackingdata_football_roster_teamroster_hpp_

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

#include "FOOTBALL/Roster/PlayerRoster.hpp"
#include "FOOTBALL/Roster/TeamScores.hpp"

namespace Football
{

namespace Roster
{

/*!
 *  \brief  Class to store and manage team data provided in the metadata file.
 */
class TeamRoster
{

protected:

std::vector<PlayerSummary>  players_in_this_team;
std::uint16_t               team_id;
std::string                 team_name;
TeamScores                  team_score;

public:

TeamRoster()
{

}

TeamRoster(
    std::uint16_t       _team_id,
    std::string         _team_name  =   ""
)
:   team_id     (_team_id)
,   team_name   (_team_name)
{

}

TeamRoster(
    std::uint16_t               _team_id,
    std::string                 _team_name,
    TeamScores                  _team_scores,
    std::vector<PlayerSummary>  _players
)
:   team_id                 (_team_id)
,   team_name               (_team_name)
,   team_score              (_team_scores)
,   players_in_this_team    (_players)
{

}

TeamRoster(
    const nlohmann::json &  _json_obj
)
{
    from_json ( _json_obj);
}
    
void    from_json (const nlohmann::json &  _json_obj)
{
    std::string     temp_string;
    //  team id 
    if (_json_obj.find("optaID") != _json_obj.end())
    {
        _json_obj.at("optaID").get_to(temp_string);
        team_id =   to_uint32_ref(temp_string);
    }
    else
    {
        std::clog   <<  "[Info] No Opta ID" << std::endl;
    }

    //  name
    if (_json_obj.find("name") != _json_obj.end())
    {
        _json_obj.at("name").get_to(team_name);
    }
    else
    {
        std::clog   <<  "[Info] No team name" << std::endl;
    }

    //  goals
    if (_json_obj.find("goals") != _json_obj.end())
    {
        _json_obj.at("goals").get_to(temp_string);
        team_score.goals_scored =   std::stoul(temp_string);
    }
    else
    {
        std::clog   << "[Info] No goals" << std::endl;
    }

    //  xG
    if (_json_obj.find("xG") != _json_obj.end())
    {
        _json_obj.at("xG").get_to(team_score.total_xG);
        // team_score.total_xG =   std::stof(temp_string);
    }
    else
    {
        std::clog   << "[Info] No xG" << std::endl;
    }

    //  players
    if (_json_obj.find("players") != _json_obj.end())
    {
        const auto & _players_obj   =   _json_obj.at("players");

        for (const auto & _p : _players_obj)
        {
            players_in_this_team.emplace_back(_p);
        }
    }
    else
    {
        std::clog   <<  "[Error] No players in metadata json" << std::endl;
        std::cerr   <<  "[Error] No players in metadata json" << std::endl;
    }
}

//  getters and setters

std::size_t     get_number_of_players   ()  const
{
    return players_in_this_team.size();
}

void    set_player_summary  (std::uint32_t    _index, const PlayerSummary&    _plyr_sum)
{
    if (_index > get_number_of_players())
    {
        players_in_this_team[_index]    =   _plyr_sum;
    }
    else
    {
        throw std::out_of_range("Index " + std::to_string(_index) + " is out of range (size_t: " + std::to_string(get_number_of_players()) + ")");
    }
}

const PlayerSummary&    get_player_summary  (std::uint32_t    _index)     const
{
    if (_index > get_number_of_players())
    {
        return  players_in_this_team[_index];
    }
    else
    {
        throw std::out_of_range("Index " + std::to_string(_index) + " is out of range (size_t: " + std::to_string(get_number_of_players()) + ")");
    }
}

PlayerSummary&          get_player_summary  (std::uint32_t    _index)
{
    if (_index > get_number_of_players())
    {
        return  players_in_this_team[_index];
    }
    else
    {
        throw std::out_of_range("Index " + std::to_string(_index) + " is out of range (size_t: " + std::to_string(get_number_of_players()) + ")");
    }
}

const PlayerSummary *   find_player_summary (std::uint32_t  _player_id, std::uint16_t _shirt_number)
{
    for (const auto & plyr : players_in_this_team)
    {
        if (plyr.player_id == _player_id || plyr.shirt_number == _shirt_number)
        {
            return &plyr;
        }
    }

    //  if this part is reached, the player wasn't found
    return nullptr;
}

void                    set_team_id (std::uint16_t  _team_id)
{
    team_id =   _team_id;
}

std::uint16_t           get_team_id ()  const
{
    return team_id;
}

void                    set_team_name   (const std::string & _team_name)
{
    team_name   =   _team_name;
}
const std::string &     get_team_name   ()  const
{
    return team_name;
}

const TeamScores &      get_team_scores ()  const
{
    return team_score;
}

};

}   /*  namespace Roster    */

}   /*  namespace Football  */

#endif /* readtrackingdata_football_roster_teamroster_hpp_ */