'''
    Lewis Higgins, 
    City Football Group & The University of Manchester,
    September 2019

    E: lewis.higgins@postgrad.manchester.ac.uk

    W: https://github.com/hidgjens/ReadTrackingData

    Example for loading and analysing a GamePack.

    tqdm is an optional module used in Football.py which provides progress bars while loading data. (install via python pip)
'''

# import Football.py to use it
import Football as FB

# variables used to locate and load game
data_dir        = "Data/"       #   relative path to data directory
match_id        = 1059714       #   integer MatchID, not "g1059714"
mode_5fps       = True          #   whether to load the 5FPS version

# load the match using the @staticmethod in Match class
loaded_match    = FB.Match.getMatchFromFile(data_dir, match_id, mode_5fps)

# count the number of frames in possession
home_possession = 0
away_possession = 0
total_frames    = 0 # only wish to count alive frames

# match object is iterable
for ball, home_team, away_team in loaded_match:
    '''
        Just to note, an alternative arrangment is to write:
            for frame in match:

            where frame is a tuple = (ball, home_team, away_team)
    '''
    
    # check if ball is alive
    if ball.alive:
        # count the frame
        total_frames += 1

        # check who is in possession
        if ball.owning_team == 'H':
            home_possession += 1
        elif ball.owning_team == 'A':
            away_possession += 1
        else:
            print('Undefined possession')

# compute results
home_fraction   = home_possession/total_frames
away_fraction   = away_possession/total_frames

print("\nHome team possession %.1f%%, Away team possession %.1f%%" % (home_fraction * 100, away_fraction * 100))

# print starting player lineups:
hometeam        = loaded_match.home_frames[0]   # FB.Team objects
awayteam        = loaded_match.away_frames[0]

print("\nInitial team line-ups:\n")
# Team objects are iterable
print("\tHome Team")
for player in hometeam:
    print('\t\t%s' % player)

print("\tAway Team")
for player in awayteam:
    print('\t\t%s' % player)