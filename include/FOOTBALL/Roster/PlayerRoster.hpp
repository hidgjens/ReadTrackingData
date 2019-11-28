#ifndef readtrackingdata_football_roster_playerroster_hpp_
#define readtrackingdata_football_roster_playerroster_hpp_

#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>        /*  Third-Party JSON lib        */



namespace   Football
{

namespace   Roster
{

class TeamRoster;

inline std::uint32_t    to_uint32_ref   (std::string s)
/*
 Functions as to_uint, but ignores the first character in string.
 Useful for references, which are given as e.g. "p39440" -> 39440
 */
{
    return (std::uint32_t) std::stoull(s.substr(1));
}

/*!
 *  \brief  Assign an enum to each role on the pitch
 */
enum    PlayerPositions
{
    NON_APPLICABLE,             /*!<    For use with PlayerSummary::sub_position if the player is not a sub or if no info is given at all   */
    SUBSTITUTE,                 /*!<    This player started the game as a substitute                                                        */
    GOALKEEPER,                 /*!<    This player played as a goalkeeper                                                                  */
    DEFENDER,                   /*!<    This player played as a defender                                                                    */
    MIDFIELDER,                 /*!<    This player played as a midfielder                                                                  */
    STRIKER,                    /*!<    This player played as a striker                                                                     */
    FORWARD
};

/*!
 *  \brief  Object to store information about players' roles within the team.
 * 
 *  Stores players 
 * 
 */
class PlayerSummary
{

friend class TeamRoster;

protected:
std::uint16_t       shirt_number;
std::uint32_t       player_id;
PlayerPositions     starting_position;
PlayerPositions     sub_position;


public:

//  Default constructable
PlayerSummary()
{
    //  Do nothing
}

//  Parameterised Constructors
PlayerSummary(
    std::uint16_t   _shirt_num,
    std::uint32_t   _plyr_id
)
:   shirt_number        (_shirt_num)
,   player_id           (_plyr_id)
,   starting_position   (NON_APPLICABLE)
,   sub_position        (NON_APPLICABLE)
{
    
}

PlayerSummary(
    std::uint16_t   _shirt_num,
    std::uint32_t   _plyr_id,
    PlayerPositions _start_pos
)
:   shirt_number        (_shirt_num)
,   player_id           (_plyr_id)
,   starting_position   (_start_pos)
,   sub_position        (NON_APPLICABLE)
{

}

PlayerSummary(
    std::uint16_t   _shirt_num,
    std::uint32_t   _plyr_id,
    PlayerPositions _start_pos,
    PlayerPositions _sub_pos
)
:   shirt_number        (_shirt_num)
,   player_id           (_plyr_id)
,   starting_position   (_start_pos)
,   sub_position        (_sub_pos)
{

}

PlayerSummary(
    const nlohmann::json & _json_obj
)
{
    from_json(_json_obj);
}

/*  Construct from nlohmann::json   */

void    from_json(const nlohmann::json & _json_obj)
{
    std::string temp_string;

    //  optaId
    if (_json_obj.find("optaID") != _json_obj.end())
    {
        ////////////////////////////////////////
        _json_obj.at("optaID").get_to(temp_string);
        player_id   =   to_uint32_ref(temp_string);
    }
    else
    {
        std::clog   <<  "[Error] No optaID for player." << std::endl;
    }

    //  shirt
    if (_json_obj.find("shirt") != _json_obj.end())
    {
        _json_obj.at("shirt").get_to(temp_string);
        shirt_number    =   std::stoul(temp_string);
    }
    else
    {
        std::clog   <<  "[Error] No shirt number for player." << std::endl;
    }

    //  position
    if (_json_obj.find("position") != _json_obj.end())
    {
        _json_obj.at("position").get_to(temp_string);

        //  work out which position this is
        if (temp_string ==  "Goalkeeper")
        {
            starting_position   =   GOALKEEPER;
        }
        else if (temp_string == "Defender")
        {
            starting_position   =   DEFENDER;
        }
        else if (temp_string == "Midfielder")
        {
            starting_position   =   MIDFIELDER;
        }
        else if (temp_string == "Striker")
        {
            starting_position   =   STRIKER;
        }
        else if (temp_string == "Forward")
        {
            starting_position   =   FORWARD;
        }
        else if (temp_string == "Substitute")
        {
            starting_position   =   SUBSTITUTE;
        }
        else
        {
            starting_position   =   NON_APPLICABLE;
            std::clog   <<  "[INFO] Starting position given was not recognised -> '" << temp_string << "'" << std::endl;
        }
        

    }
    else
    {
        starting_position   =   NON_APPLICABLE;
        std::clog   <<  "[INFO] Starting position not given." << std::endl;
    }

    //  subposition
    if (_json_obj.find("subPosition") != _json_obj.end())
    {
        _json_obj.at("subPosition").get_to(temp_string);

        //  work out which position this is
        if (temp_string ==  "Goalkeeper")
        {
            sub_position    =   GOALKEEPER;
        }
        else if (temp_string == "Defender")
        {
            sub_position    =   DEFENDER;
        }
        else if (temp_string == "Midfielder")
        {
            sub_position    =   MIDFIELDER;
        }
        else if (temp_string == "Striker")
        {
            sub_position    =   STRIKER;
        }
        else if (temp_string == "Forward")
        {
            sub_position    =   FORWARD;
        }
        else if (temp_string == "Substitute")
        {
            sub_position    =   SUBSTITUTE;
        }
        else
        {
            sub_position    =   NON_APPLICABLE;
            std::clog   <<  "[INFO] Subposition given was not recognised -> '" << temp_string << "'" << std::endl;
        }
        

    }
    else
    {
        sub_position    =   NON_APPLICABLE;
        if (starting_position == SUBSTITUTE)
        {
            std::clog   <<  "[INFO] Sub position not given." << std::endl;
        }
    }
}

/*  Getters and Setters     */

void            set_shirt_num       (std::uint16_t _shirt_num)
{
    shirt_number    =   _shirt_num;
}
std::uint16_t   get_shirt_num       ()  const
{
    return shirt_number;
}

void            set_player_id       (std::uint32_t _plyr_id)
{
    player_id       =   _plyr_id;
}
std::uint32_t   get_player_id       ()  const
{
    return player_id;
}

void            set_start_position  (PlayerPositions _start_pos)
{
    starting_position   =   _start_pos;
}
PlayerPositions get_start_position  ()  const
{
    return starting_position;
}

void            set_sub_position    (PlayerPositions _sub_pos)
{
    sub_position   =   _sub_pos;
}
PlayerPositions get_sub_position    ()  const
{
    return sub_position;
}

/*!
 *  \brief  Check if player starts in a given position (\a pos)
 *  \param  pos     The position to compare with this player's position
 * 
 *  \returns        Boolean, whether the player's position matches the one given
 */
bool            starts_in_position(PlayerPositions  pos)    const
{
    return (starting_position == pos);
}

/*!
 *  \brief  Check if player is subbed into a given position (\a pos)
 *  \param  pos     The position to compare with this player's position
 * 
 *  \returns        Boolean, whether the player's position matches the one given
 */
bool            subbed_into_position(PlayerPositions    pos)    const
{
    return (sub_position == pos);
}

/*!
 *  \brief  Check if player starting position information is given
 * 
 *  \returns        Boolean, whether the player's position is not equal to non_applicable
 */
bool            starting_information_is_given()     const
{
    return  (starting_position != NON_APPLICABLE);
}

/*!
 *  \brief  Check if player starting position information is given
 * 
 *  \returns        Boolean, whether the player's position is not equal to non_applicable
 */
bool            subposition_information_is_given()     const
{
    return  !((starting_position == SUBSTITUTE) && (sub_position == NON_APPLICABLE));
}

bool            is_goalkeeper() const
{
    return ((starting_position == GOALKEEPER) || (sub_position == GOALKEEPER));
}
bool            is_not_goalkeeper() const
{
    return !(is_goalkeeper());
}

bool            is_defender() const
{
    return ((starting_position == DEFENDER) || (sub_position == DEFENDER));
}
bool            is_not_defender() const
{
    return !(is_defender());
}

bool            is_midfielder() const
{
    return ((starting_position == MIDFIELDER) || (sub_position == MIDFIELDER));
}
bool            is_not_midfielder() const
{
    return !(is_midfielder());
}

bool            is_striker() const
{
    return ((starting_position == STRIKER) || (sub_position == STRIKER) || is_forward());
}
bool            is_not_striker() const
{
    return !(is_striker());
}

bool            is_forward() const
{
    return ((starting_position == FORWARD) || (sub_position == FORWARD));
}
bool            is_not_forward() const
{
    return !(is_forward());
}

bool            is_substitute() const
{
    return (starting_position == SUBSTITUTE);
}
bool            is_not_substitute() const
{
    return !(is_substitute());
}


};

}   /*  namespace Roster    */

}   /*  namespace Football  */

#endif /* readtrackingdata_football_roster_playerroster_hpp_ */