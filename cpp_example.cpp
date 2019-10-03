/*
    Lewis Higgins, 
    City Football Group & The University of Manchester,
    September 2019

    E: lewis.higgins@postgrad.manchester.ac.uk

    W: https://github.com/hidgjens/ReadTrackingData

    Example for loading and analysing a GamePack.

    To build, please add "FOOTBALL/THIRDPARTY" to your include-dirs via the -I flag:
        g++ cpp_example.cpp -I"FOOTBALL/THIRDPARTY"

    Might also want to consider building a 64-bit binary using the -m64 flag.

*/

// Football.h will include the whole folder
#include "FOOTBALL/Football.h"

int main (int argc, char * argv[])
{
    // variables to locate game
    std::string     DATA_DIR    = "./";
    uint            MATCH_ID    = 919268;
    bool            mode_5fps   = false;         // true for loading the 5fps version

    // Note that everything from the FOOTBALL folder is stored in namespace Football
    // Create match object
    Football::Match ex_match;

    // load game from file
    ex_match.loadFromFile(DATA_DIR, MATCH_ID, mode_5fps);

    // count the number of frames in possession
    uint        home_possession = 0;
    uint        away_possession = 0;
    uint        total_frames    = 0;       // only counting alive frames

    uint        dead_frames     = 0;

    {    

    // create frame object as temporary storage
    Football::Frame _frame;         

    // iterate through match frames
    for (uint i = 0 ; i < ex_match.number_of_frames() ; i++ )
    {
        // store current frame in temporary storage
        _frame = ex_match.get_frame(i);

        // std::cout << "frame get " << _frame.FRAME_ID << " (" << i << ")" << std::endl;

        /* 
            Analysis for this frame.
        */

        // check if ball is alive in this frame
        if (_frame.isAlive())
        {
            // std::cout << "frame get " << _frame.FRAME_ID << " (" << i << ")" << std::endl;

            // increment alive frames counter
            total_frames ++;

            // check who is in possession
            switch (_frame.BALL.get_owningTeam())
            {
            // home team
            case 'H':
                // increment home counter
                home_possession ++;
                break;
            // away team
            case 'A':
                // increment away counter
                away_possession ++;
                break;
            // officials
            case 'O':
                // officials in possession of the ball?
                std::cerr << "Frame " << i << " official possession?" << std::endl;
                // discount this frame
                total_frames --;
                break;
            // undefined
            case 'U':
                // undefined possession - unlikely to occur, but not necessarily an error
                std::cout << "Frame " << i << " undefined possesion" << std::endl;
                // discount this frame
                total_frames --;
            default:
            // default case is none of the above
                std::cerr << "Frame " << i << " Default case on switch" << std::endl;
                // discount this frame
                total_frames --;
                break;
            } /* end of switch */
        } /* endif ball alive */
        else
        {
            dead_frames ++;
            // dead frame
            // std::cout << "DEAD" << std::endl;
        }
    } /* for loop ends */

    } // the extra set of curly braces is limiting the scope of _frame, beyond here it is no longer in scope. Good practice as _frame was temporary storage for the loop and no longer needed
    
    // compute fraction of possession from extracted data
    float home_pos_frac = home_possession / ((float) total_frames); // explicitely casting one of these numbers to float to avoid integer result i.e. 1/2 = 0 vs 1/2.0 = 0.5
    float away_pos_frac = away_possession / ((float) total_frames);

    // print result to console
    printf ("\nHome team possession %4.1f%%, Away team possession %4.1f%%\n", home_pos_frac * 100.0, away_pos_frac * 100.0);
    
    std::cout << "Alive frames: " << total_frames << " Dead frames: " << dead_frames << std::endl;
    // print starting player line-up

    printf ("\nInitial team line-ups:\n");

    // get the first frame
    auto first_frame = ex_match.get_frame(120255);

    // get the Football::Team objects stored in the frame
    auto& initial_home_team = first_frame.HOMETEAM;
    auto& initial_away_team = first_frame.AWAYTEAM;

    printf ("\tHome Team\n");

    // iterate through the players in team
    for (const auto& player : initial_home_team.get_playersInTeam()) // currently, Football::Team is not iterable, but the std::vector\<Football::Player> contained within is
    {
        printf("\t\t%s\n", player.get_summaryString().c_str());
    }

    printf ("\tAway Team\n");
    // iterate through the away players
    for (const auto& player : initial_away_team.get_playersInTeam())
    {
        printf("\t\t%s\n", player.get_summaryString().c_str());
    }


    return EXIT_SUCCESS;
}

/* 
    Goal:
    I want to implement the Football::Match object to be iterable, i.e.

    for (auto frame_ : match)
    {
        //  analyse frame_
    }

    likewise for Football::Team:

    for (auto& player : team)
    {
        //  analyse player
    }
*/