/*!
 *  \file Player.hpp
 *
 *  \author Lewis Higgins
 *  \date October 2019
 *
 *  \brief Defines a Class used to represent a player.
 */

#ifndef readtrackingdata_football_player_hpp_
#define readtrackingdata_football_player_hpp_

#include <cstdint>
#include <iostream>

#include <msgpack.hpp>

#include "FOOTBALL/PitchObject.hpp"
#include "FOOTBALL/Roster/PlayerRoster.hpp"

namespace Football 
{

class Player: public PitchObject 
{
    friend class Match;
    friend class Team;
    friend inline bool operator==(const Player& lhs, const Player& rhs);

    protected:
    // inherited: int OBJECT_POS_X
    // inherited: int OBJECT_POS_Y
    std::uint8_t    PLAYER_SHIRT_NUM;

    // FROM TRACAB integration
    char            TEAM;       // H/A/O (official)
    std::uint32_t   PLAYER_ID;
    bool            BALL_OWNED      = false;
    // double          SPEED;      // metres per second
    std::array<std::int16_t, 2>     VELOCITY;   //  centimetres per second

    const Roster::PlayerSummary *   PLAYER_SUMMARY;

    public:    
    MSGPACK_DEFINE(PLAYER_ID, PLAYER_SHIRT_NUM, OBJECT_POS_X, OBJECT_POS_Y, BALL_OWNED);

    static Player createRandomPlayer(const std::uint16_t sn = 1) 
    {
        Player rtrn_player;
        rtrn_player.OBJECT_POS_X = rand();
        rtrn_player.OBJECT_POS_Y = rand();
        rtrn_player.PLAYER_SHIRT_NUM = rand();
        rtrn_player.TEAM = rand();
        rtrn_player.PLAYER_ID = rand();
        // rtrn_player.SPEED = ((double) rand() / RAND_MAX);

        return(rtrn_player);
    }
    
    Player(std::int16_t x = 0.0, std::int16_t y = 0.0, std::uint8_t sn = 1) 
    :   PLAYER_SHIRT_NUM(sn)
    ,   VELOCITY({0, 0}) 
    {
        OBJECT_POS_X = x ; OBJECT_POS_Y = y;
    }
    Player(std::pair<std::int16_t, std::int16_t> p, std::uint8_t sn = 1) 
    :   PLAYER_SHIRT_NUM(sn)
    ,   VELOCITY({0, 0})
    {
        OBJECT_POS_X = p.first ; OBJECT_POS_Y = p.second;
    }

    // setters and getters

    /// get player position as a 2D array
    std::array<std::int16_t, 2> get_pos() const
    {
        return std::array<std::int16_t, 2>({OBJECT_POS_X, OBJECT_POS_Y});
    }
    /// set player position using 2D array
    void            set_pos(const std::array<std::int16_t, 2> & _pos)
    {
        OBJECT_POS_X = _pos[0];
        OBJECT_POS_Y = _pos[1];
    }

    /// get the player's shirt number
    std::uint8_t    get_shirtNumber() const 
    { 
        return PLAYER_SHIRT_NUM; 
    }
    /// set the player's shirt number
    void            set_shirtNumber(const std::uint8_t _sn) 
    { 
        PLAYER_SHIRT_NUM = _sn; 
    }

    /// get character representing player's current team. Key: 'H' - Home, 'A' - Away, 'O' - Official, 'U' - Undefined
    char            get_team() const
    {
        return TEAM;
    }
    /// set character representing player's current team. Key: 'H' - Home, 'A' - Away, 'O' - Official, 'U' - Undefined
    void            set_team(const char _team)
    {
        TEAM = _team;
    }

    /// get optaPlayerId
    std::uint32_t   get_playerId() const
    {
        return PLAYER_ID;
    }
    /// set optaPlayerId
    void            set_playerId(const std::uint32_t _player_id)
    {
        PLAYER_ID = _player_id;
    }

    /// check if this player is specifically in possession of the ball
    bool            get_ballOwned() const
    {
        return BALL_OWNED;
    }
    /// check if this player is specifically in possession of the ball
    bool            ownsBall()      const
    {
        return get_ballOwned();
    }
    /// set if this player is specifically in possession of the ball
    void            set_ballOwned(const bool _ball_owned) 
    {
        BALL_OWNED = _ball_owned;
    }

    /// get string with player summary
    std::string     get_summaryString() const
    {
        return std::string("PlayerID: ") + std::to_string(PLAYER_ID) + "\tShirt: " + std::to_string(PLAYER_SHIRT_NUM) + "\t(" + std::to_string(OBJECT_POS_X) + "," + std::to_string(OBJECT_POS_Y) + ")";
    }

    /// sends some player details to the provided ostream
    virtual void print(std::ostream& os) const 
    {
        os << "Player " << PLAYER_SHIRT_NUM << ": (" << OBJECT_POS_X << "," << OBJECT_POS_Y << ")";
    }

    void            set_player_summary(const Roster::PlayerSummary * _player_sum)
    {
        PLAYER_SUMMARY  =   _player_sum;
    }

    const Roster::PlayerSummary *   get_player_summary()    const
    {
        return PLAYER_SUMMARY;
    }

    /*!
     *  \brief  Check if player starts in a given position (\a pos)
     *  \param  pos     The position to compare with this player's position
     * 
     *  \returns        Boolean, whether the player's position matches the one given
     */
    bool            starts_in_position(Roster::PlayerPositions  pos)    const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->starts_in_position(pos);
        }
    }

    /*!
    *  \brief  Check if player is subbed into a given position (\a pos)
    *  \param  pos     The position to compare with this player's position
    * 
    *  \returns        Boolean, whether the player's position matches the one given
    */
    bool            subbed_into_position(Roster::PlayerPositions    pos)    const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->subbed_into_position(pos);
        }
    }

    /*!
    *  \brief  Check if player starting position information is given
    * 
    *  \returns        Boolean, whether the player's position is not equal to non_applicable
    */
    bool            starting_information_is_given()     const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->starting_information_is_given();
        }
    }

    /*!
    *  \brief  Check if player starting position information is given
    * 
    *  \returns        Boolean, whether the player's position is not equal to non_applicable
    */
    bool            subposition_information_is_given()     const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->subposition_information_is_given();
        }
    }

    bool            is_goalkeeper() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_goalkeeper();
        }
    }
    bool            is_not_goalkeeper() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_not_goalkeeper();
        }
    }

    bool            is_defender() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_defender();
        }
    }
    bool            is_not_defender() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_not_defender();
        }
    }

    bool            is_midfielder() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_midfielder();
        }
    }
    bool            is_not_midfielder() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_not_midfielder();
        }
    }

    bool            is_striker() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_striker();
        }
    }
    bool            is_not_striker() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_not_striker();
        }   
    }

    bool            is_forward() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_forward();
        }   
    }
    bool            is_not_forward() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_not_forward();
        }   
    }

    bool            is_substitute() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_substitute();
        }   
    }
    bool            is_not_substitute() const
    {
        if (PLAYER_SUMMARY == nullptr)
        {
            std::cerr << "Player Summary not given." << std::endl;
            return false;
        }
        else
        {
            return PLAYER_SUMMARY->is_not_substitute();
        }   

    }
    /*!
     *  @brief  Get the player's velocity. This is calculated during the loading of a match (and can be skipped, which if it was means this function will just return [0,0]).
     * 
     *  Units of centimetres per second.
     */
    std::array<std::int16_t, 2>     get_player_velocity () const
    {
        return VELOCITY;
    }
    std::int16_t    get_velocity_x_component () const
    {
        return VELOCITY[0];
    }
    std::int16_t    get_velocity_y_component () const
    {
        return VELOCITY[1];
    }
};

inline bool operator==(const Player& lhs, const Player& rhs)
{
    return (
        lhs.OBJECT_POS_X        == rhs.OBJECT_POS_X
        &&
        lhs.OBJECT_POS_Y        == rhs.OBJECT_POS_Y
        &&
        lhs.PLAYER_ID           == rhs.PLAYER_ID
        &&
        lhs.PLAYER_SHIRT_NUM    == rhs.PLAYER_SHIRT_NUM
        &&
        lhs.TEAM                == rhs.TEAM
        &&
        lhs.BALL_OWNED          == rhs.BALL_OWNED
    );
}
inline bool operator!=(const Player& lhs, const Player& rhs)
{
    return !(lhs == rhs);
}

} /* namespace Football */

#endif /* trackingdatalib_football_player_hpp_ */