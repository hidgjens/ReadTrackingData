import sys
from os import makedirs
from os.path import exists
from shutil import copyfile
import typing
import json

import msgpack

# incase TQDM isn't installed
try:
    from tqdm import tqdm
except:
    def tqdm(thing, *args, **kwargs):
        return thing

class Ball:
    x_pos               :   int     =   0
    y_pos               :   int     =   0
    z_pos               :   int     =   0
    alive               :   bool    =   False
    owning_team         :   str     =   "U"
    owning_player_id    :   int     =   0
    frame_id            :   int     =   0

    def check_kwargs(self, key : str, kwargs : typing.Dict):
        if key in kwargs.keys():
            self.__setattr__(key, kwargs[key])

    def __init__ (self, **kwargs):
        self.check_kwargs("x_pos", kwargs)
        self.check_kwargs("y_pos", kwargs)
        self.check_kwargs("z_pos", kwargs)
        self.check_kwargs("alive", kwargs)
        self.check_kwargs("owning_team", kwargs)
        self.check_kwargs("owning_player_id", kwargs)
        self.check_kwargs("frame_id", kwargs)

    def __str__ (self):
        return "Frame {0}, ball alive: {4}, pos: ({1}, {2}, {3})".format(self.frame_id, self.x_pos, self.y_pos, self.z_pos, self.alive)

    def print_pos(self):
        print("({},{})".format(self.x_pos, self.y_pos))

    def get_json_line(self):
        alive_str = "true" if self.alive else "false"
        return "[{},{},{},{},{},{},{}]".format(
            self.frame_id,
            self.x_pos,
            self.y_pos,
            self.z_pos,
            alive_str,
            self.owning_team,
            self.owning_player_id
        )

    def get_json_list(self):
        return [
            self.frame_id,
            self.x_pos,
            self.y_pos,
            self.z_pos,
            self.alive,
            self.owning_team,
            self.owning_player_id
        ]

    def get_json_dict(self):
        return {
            'FRAMEID'   :   self.frame_id,
            'XPOS'      :   self.x_pos,
            'YPOS'      :   self.y_pos,
            'ZPOS'      :   self.z_pos,
            'ALIVE'     :   self.alive,
            'TEAM'      :   self.owning_team,
            'PLAYERID'  :   self.owning_player_id
        }

    @staticmethod
    def getBallFramesFromFile(file_path: str):
        with open(file_path, 'rb') as ball_file:
            ball_data = msgpack.unpack(ball_file)

        ball_frames : typing.List[Ball] = []

        for ball_line in tqdm(ball_data, desc = "Reading"):
            ball_frames.append(
                Ball(
                    frame_id            = ball_line[0],
                    x_pos               = ball_line[1],
                    y_pos               = ball_line[2],
                    z_pos               = ball_line[3],
                    alive               = ball_line[4],
                    owning_team         = chr(ball_line[5]),
                    owning_player_id    = ball_line[6]
                    )
                )


        return ball_frames

class Player:
    x_pos               :   int     =   0
    y_pos               :   int     =   0
    team                :   str     =   'U'
    player_id           :   int     =   0
    shirt_num           :   int     =   0
    ball_owned          :   bool    =   False

    def check_kwargs(self, key : str, kwargs : typing.Dict):
        if key in kwargs.keys():
            self.__setattr__(key, kwargs[key])

    def __init__ (self, **kwargs):
        self.check_kwargs("x_pos", kwargs)
        self.check_kwargs("y_pos", kwargs)
        self.check_kwargs("team", kwargs)
        self.check_kwargs("player_id", kwargs)
        self.check_kwargs("shirt_num", kwargs)
        self.check_kwargs("ball_owned", kwargs)

    def __str__ (self):
        return "OptaID: {0}, Shirt: {3}, ({1},{2})".format(self.player_id, self.x_pos, self.y_pos, self.shirt_num)

    def print_pos(self):
        print("({},{})".format(self.x_pos, self.y_pos))

    def get_json_list(self):
        return [
            self.team,
            self.player_id,
            self.shirt_num,
            self.x_pos,
            self.y_pos,
            self.ball_owned
        ]

    def get_json_dict(self):
        return {
            'TEAM'      :   self.team,
            'PLAYERID'  :   self.player_id,
            'SHIRT'     :   self.shirt_num,
            'XPOS'      :   self.x_pos,
            'YPOS'      :   self.y_pos,
            'BALL'      :   self.ball_owned
        }

    @staticmethod
    def get_player_from_msgpk_segment(msgpk_segment):
        return Player(
            team            = chr(msgpk_segment[0]),
            player_id       = msgpk_segment[1],
            shirt_num       = msgpk_segment[2],
            x_pos           = msgpk_segment[3],
            y_pos           = msgpk_segment[4],
            ball_owned      = msgpk_segment[5]
        )

class TeamIterator:
    def __init__(self, team):
        self._team = team
        self._index = 0

    def __next__(self):
        if (self._index < self._team.player_count()):
            result = self._team.players_in_team[self._index]
            self._index += 1
            return result
        else:
            raise StopIteration

class Team:
    frame_id            :   int                     =   0
    ball_owned          :   bool                    =   False
    players_in_team     :   typing.List[Player]     =   []

    def check_kwargs(self, key : str, kwargs : typing.Dict):
        if key in kwargs.keys():
            self.__setattr__(key, kwargs[key])

    def __init__ (self, **kwargs):
        self.check_kwargs("frame_id", kwargs)
        self.check_kwargs("ball_owned", kwargs)
        self.check_kwargs("players_in_team", kwargs)

    def __str__ (self):
        return "Frame {0}, Players: {1}".format(self.frame_id, [p.__str__() for p in self.players_in_team])

    def __iter__(self):
        return TeamIterator(self)

    def player_count(self):
        return len(self.players_in_team)

    def get_json_list(self):
        return [
            self.frame_id,
            self.ball_owned,
            [p.get_json_list() for p in self.players_in_team]
        ]

    def get_json_dict(self):
        return {
            'FRAMEID'   :   self.frame_id,
            'BALL'      :   self.ball_owned,
            'PLAYERS'   :   [p.get_json_dict() for p in self.players_in_team]
        }

    @staticmethod
    def getTeamFromFile(file_path: str):
        with open(file_path, 'rb') as team_file:
            team_data = msgpack.unpack(team_file)

        team_frames : typing.List[Team] = []

        for team_line in tqdm(team_data, desc = "Reading"):
            team_frames.append(
                Team(
                    frame_id            = team_line[0],
                    ball_owned          = team_line[1],
                    players_in_team     = [Player.get_player_from_msgpk_segment(p) for p in team_line[2]]
                )
            )

        return team_frames


class Period:
    period_id           :   int                     =   0
    start_frame         :   int                     =   0
    end_frame           :   int                     =   0

    def check_kwargs(self, key : str, kwargs : typing.Dict):
        if key in kwargs.keys():
            self.__setattr__(key, kwargs[key])

    def __init__(self, **kwargs):
        self.check_kwargs("period_id", kwargs)
        self.check_kwargs("start_frame", kwargs)
        self.check_kwargs("end_frame", kwargs)

    def __str__(self):
        return ("PeriodID: {0}, [{1},{2}]".format(self.period_id, self.start_frame, self.end_frame))
    
    @staticmethod
    def getPeriodFromJSONObj(json_obj):
        # get period ID
        keys = list(json_obj.keys())

        period_id = None

        if len(keys) == 1:
            period_id = int(keys[0])
        else:
            # shouldn't be necessary but lets find the key
            for key in keys:
                if key in "012345": # should only be a number between 1-5
                    period_id = int(key)

        return Period(
            period_id   =   period_id,
            start_frame =   json_obj[str(period_id)][0],
            end_frame   =   json_obj[str(period_id)][1]
        )

class Metadata:
    match_id            :   int                     =   0
    date                :   str                     =   ""
    fps                 :   float                   =   0.0
    pitch_dims          :   typing.List[float]      =   [0.0, 0.0]
    periods             :   typing.List[Period]     =   []
    opta_f7             :   bool                    =   False
    opta_f24            :   bool                    =   False
    tracking_provider   :   str                     =   ""

    def check_kwargs(self, key : str, kwargs : typing.Dict):
        if key in kwargs.keys():
            self.__setattr__(key, kwargs[key])

    def __init__(self, **kwargs):
        self.check_kwargs("match_id", kwargs)
        self.check_kwargs("date", kwargs)
        self.check_kwargs("fps", kwargs) 
        self.check_kwargs("pitch_dims", kwargs) 
        self.check_kwargs("periods", kwargs) 
        self.check_kwargs("opta_f7", kwargs) 
        self.check_kwargs("opta_f24", kwargs) 
        self.check_kwargs("tracking_provider", kwargs) 

    def __str__(self):
        return (
            "Metadata:\nMatchID: {0}\nDate: {1}\nFPS: {2}\nPitchDims: {3}\nPeriods: {4}\nOptaF7: {5}\nOptaF24: {6}\nTrackingProvider: {7}\n".format(
                self.match_id, self.date, self.fps, self.pitch_dims, [p.__str__() for p in self.periods], self.opta_f7, self.opta_f24, self.tracking_provider
            )
        )

    @staticmethod
    def getMetadataFromFile(file_path: str):
        with open(file_path, 'r') as mdata_file:
            mdata = json.load(mdata_file)

        mdata = mdata[0] # quick fix, remove this line post fix

        return Metadata(
            match_id            =   mdata['MATCHID'],
            date                =   mdata['DATE'],
            fps                 =   mdata['FPS'],
            pitch_dims          =   mdata['PITCH_DIMS'],
            periods             =   [Period.getPeriodFromJSONObj(p) for p in mdata['PERIODS']],
            opta_f7             =   mdata['OPTA_F7'],
            opta_f24            =   mdata['OPTA_F24'],
            tracking_provider   =   mdata['TRACKING_PROVIDER']
        ) 

class MatchIterator:
    def __init__(self, match):
        # Match obj reference
        self._match = match
        self._index = 0

    def __next__(self):
        if (self._index < self._match.frame_count()):
            result = (self._match.ball_frames[self._index], self._match.home_frames[self._index], self._match.away_frames[self._index])
            self._index += 1
            return result
        else:
            raise StopIteration


class Match:
    match_id            :   int                     =   0
    ball_frames         :   typing.List[Ball]       =   []
    home_frames         :   typing.List[Team]       =   []
    away_frames         :   typing.List[Team]       =   [] 
    official_frames     :   typing.List[Team]       =   []
    metadata            :   Metadata                =   Metadata()

    def check_kwargs(self, key : str, kwargs : typing.Dict):
        if key in kwargs.keys():
            self.__setattr__(key, kwargs[key])

    def __init__(self, **kwargs):
        self.check_kwargs("match_id", kwargs)
        self.check_kwargs("ball_frames", kwargs)
        self.check_kwargs("home_frames", kwargs) 
        self.check_kwargs("away_frames", kwargs) 
        self.check_kwargs("official_frames", kwargs) 
        self.check_kwargs("metadata", kwargs)

    def __str__(self):
        return "Match {0} has {1} frames".format(self.match_id, len(self.ball_frames))

    def __iter__(self):
        return MatchIterator(self)

    def frame_count(self):
        if (len(self.ball_frames) == len(self.home_frames)) and (len(self.ball_frames) == len(self.away_frames)):
            return len(self.ball_frames)
        else:
            print("Uneven list lengths in match")
            if (len(self.ball_frames) < len(self.home_frames)) and (len(self.ball_frames) < len(self.away_frames)):
                return len(self.ball_frames)
            else:
                if (len(self.home_frames) < len(self.away_frames)):
                    return len(self.home_frames)
                else:
                    return len(self.away_frames)

    @staticmethod
    def getMatchFromFile(tracking_data_dir: str, match_id: int, fps5: bool = True):
        # used to target the 5fps subdir if fps5 is true
        subdir = ""     
        if fps5:
            subdir = "5fps/"

        # check that path is formatted correctly  
        if tracking_data_dir[-1] != "/":
            tracking_data_dir += "/"

        base_input_file_path    = tracking_data_dir + "{0}/{1}{0}".format(match_id, subdir)

        BALL_INPUT_PATH         = "{0}.BALL.msgpack".format(base_input_file_path)

        HOME_INPUT_PATH         = "{0}.HOME.msgpack".format(base_input_file_path)

        AWAY_INPUT_PATH         = "{0}.AWAY.msgpack".format(base_input_file_path)

        METADATA_INPUT_PATH     = "{0}.METADATA.json".format(base_input_file_path)

        return Match (
            match_id        = match_id,
            ball_frames     = Ball.getBallFramesFromFile(BALL_INPUT_PATH),
            home_frames     = Team.getTeamFromFile(HOME_INPUT_PATH),
            away_frames     = Team.getTeamFromFile(AWAY_INPUT_PATH),
            metadata        = Metadata.getMetadataFromFile(METADATA_INPUT_PATH)
        )

# ball

def load_msgpk_to_ball_frames(file_path: str):
    # with open(file_path, 'rb') as ball_file:
    #     ball_data = msgpack.unpack(ball_file)

    # ball_frames : typing.List[Ball] = []

    # for ball_line in tqdm(ball_data, desc = "Reading"):
    #     ball_frames.append(
    #         Ball(
    #             frame_id            = ball_line[0],
    #             x_pos               = ball_line[1],
    #             y_pos               = ball_line[2],
    #             z_pos               = ball_line[3],
    #             alive               = ball_line[4],
    #             owning_team         = chr(ball_line[5]),
    #             owning_player_id    = ball_line[6]
    #             )
    #         )


    # return ball_frames
    return Ball.getBallFramesFromFile(file_path)

def save_ball_frames_to_efficient_json(ball_frames : typing.List[Ball], output_path: str, json_spacing: int = None):
    with open(output_path, 'w') as ball_file:
        json.dump([b.get_json_list() for b in tqdm(ball_frames, desc = "Writing")], ball_file, indent = json_spacing)

def save_ball_frames_to_readable_json(ball_frames: typing.List[Ball], output_path: str, json_spacing: int = None):
    with open(output_path, 'w') as ball_file:
        json.dump([b.get_json_dict() for b in tqdm(ball_frames, desc = "Writing")], ball_file, indent = json_spacing)


def convert_ball_msgpk_to_efficient_json(input_path : str, output_path: str, json_spacing: int = None):
    ball_frames = load_msgpk_to_ball_frames(input_path)
    save_ball_frames_to_efficient_json(ball_frames, output_path, json_spacing)

def convert_ball_msgpk_to_readable_json(input_path : str, output_path: str, json_spacing: int = None):
    ball_frames = load_msgpk_to_ball_frames(input_path)
    save_ball_frames_to_readable_json(ball_frames, output_path, json_spacing)

# team

def load_msgpk_to_team_frames(file_path: str):
    # with open(file_path, 'rb') as team_file:
    #     team_data = msgpack.unpack(team_file)

    # team_frames : typing.List[Team] = []

    # def get_player_from_msgpk_segment(msgpk_segment):
    #     return Player(
    #         team            = chr(msgpk_segment[0]),
    #         player_id       = msgpk_segment[1],
    #         shirt_num       = msgpk_segment[2],
    #         x_pos           = msgpk_segment[3],
    #         y_pos           = msgpk_segment[4],
    #         ball_owned      = msgpk_segment[5]
    #     )

    # for team_line in tqdm(team_data, desc = "Reading"):
    #     team_frames.append(
    #         Team(
    #             frame_id            = team_line[0],
    #             ball_owned          = team_line[1],
    #             players_in_team     = [get_player_from_msgpk_segment(p) for p in team_line[2]]
    #         )
    #     )

    # return team_frames
    return Team.getTeamFromFile(file_path)

def save_team_frames_to_efficient_json(team_frames: typing.List[Team], output_path: str, json_spacing: int = None):
    with open(output_path, 'w') as team_file:
        json.dump([t.get_json_list() for t in tqdm(team_frames, desc = "Writing")], team_file, indent = json_spacing)

def save_team_frames_to_readable_json(team_frames: typing.List[Team], output_path: str, json_spacing: int = None):
    with open(output_path, 'w') as team_file:
        json.dump([t.get_json_dict() for t in tqdm(team_frames, desc = "Writing")], team_file, indent = json_spacing)

def convert_team_msgpk_to_efficient_json(input_path: str, output_path: str, json_spacing: int = None):
    team_frames = load_msgpk_to_team_frames(input_path)
    save_team_frames_to_efficient_json(team_frames, output_path, json_spacing)

def convert_team_msgpk_to_readable_json(input_path: str, output_path: str, json_spacing: int = None):
    team_frames = load_msgpk_to_team_frames(input_path)
    save_team_frames_to_readable_json(team_frames, output_path, json_spacing)
    

# metadata

def move_metadata(input_path: str, output_path: str):
    copyfile(input_path, output_path)

# match

def make_folder(directory: str):
    if not exists(directory):
        makedirs(directory)

def convert_match_msgpk_to_efficient_json(tracking_data_folder: str, match_id: int, output_path: str = 'JSON/', fps5 : bool = False ):
    '''
    Converts a MessagePack Gamepack to a JSON Gamepack. Returns a list of filepaths.

    @param      tracking_data_folder        - path to find gamepacks
    @param      match_id                    - integer optaMatchId of match
    @param      output_path     ['JSON/']   - path to create JSON gamepack
    @param      fps5            [false]     - convert the data in the 5fps subfolder

    @returns    List of file paths:
                    [0]                     - path to ball json file
                    [1]                     - path to hometeam json file
                    [2]                     - path to awayteam json file
                    [3]                     - path to metadata json file

    Searches tracking_data_folder for a folder matching match_id. Then searches this folder for '[match_id].BALL.msgpack', '[match_id].HOME.msgpack', '[match_id].AWAY.msgpack', and '[match_id].METADATA.msgpack'. If fps5 is True then these files are located in the '5fps/' subdirectory instead. The file is unpacked with msgpack.unpack() and converted to a JSON-like, Python-native object - this is then written to a JSON file using json.dump().
    An efficient JSON does not include keys. The structure of the JSON must be known by the user.
    Example:    [0, 47, -17, 32, false, 72, 2464660512]
    '''

    if tracking_data_folder[-1] != "/":
        tracking_data_folder += "/"

    if output_path[-1] != "/":
        output_path += "/"

    subdir = "" # keep blank if not 5fps
    if fps5:
        subdir = "5fps/"

    make_folder(output_path + str(match_id) + "/" + subdir)

    base_input_file_path    = tracking_data_folder + "{0}/{1}{0}".format(match_id, subdir)
    base_output_file_path   = output_path + "{0}/{1}{0}".format(match_id, subdir)

    BALL_INPUT_PATH         = "{0}.BALL.msgpack".format(base_input_file_path)
    BALL_OUTPUT_PATH        = "{0}.BALL.json".format(base_output_file_path)

    HOME_INPUT_PATH         = "{0}.HOME.msgpack".format(base_input_file_path)
    HOME_OUTPUT_PATH        = "{0}.HOME.json".format(base_output_file_path)

    AWAY_INPUT_PATH         = "{0}.AWAY.msgpack".format(base_input_file_path)
    AWAY_OUTPUT_PATH        = "{0}.AWAY.json".format(base_output_file_path)

    print("Ball")
    convert_ball_msgpk_to_efficient_json(BALL_INPUT_PATH, BALL_OUTPUT_PATH)
    
    print("Home Team")
    convert_team_msgpk_to_efficient_json(HOME_INPUT_PATH, HOME_OUTPUT_PATH)
    
    print("Away Team")
    convert_team_msgpk_to_efficient_json(AWAY_INPUT_PATH, AWAY_OUTPUT_PATH)

    METADATA_INPUT_PATH     = "{0}.METADATA.json".format(base_input_file_path)
    METADATA_OUTPUT_PATH    = "{0}.METADATA.json".format(base_output_file_path)

    move_metadata(METADATA_INPUT_PATH, METADATA_OUTPUT_PATH)

    return [
        BALL_OUTPUT_PATH,
        HOME_OUTPUT_PATH,
        AWAY_OUTPUT_PATH,
        METADATA_OUTPUT_PATH
    ]

def convert_match_msgpk_to_readable_json(tracking_data_folder: str, match_id: int, output_path: str = 'JSON/', fps5 : bool = False, json_spacing: int = 0):
    '''
    Converts a MessagePack Gamepack to a JSON Gamepack. Returns a list of filepaths.

    @param      tracking_data_folder        - path to find gamepacks
    @param      match_id                    - integer optaMatchId of match
    @param      output_path     ['JSON/']   - path to create JSON gamepack
    @param      fps5            [false]     - convert the data in the 5fps subfolder

    @returns    List of file paths:
                    [0]                     - path to ball json file
                    [1]                     - path to hometeam json file
                    [2]                     - path to awayteam json file
                    [3]                     - path to metadata json file

    Searches tracking_data_folder for a folder matching match_id. Then searches this folder for '[match_id].BALL.msgpack', '[match_id].HOME.msgpack', '[match_id].AWAY.msgpack', and '[match_id].METADATA.msgpack'. If fps5 is True then these files are located in the '5fps/' subdirectory instead. The file is unpacked with msgpack.unpack() and converted to a JSON-like, Python-native object - this is then written to a JSON file using json.dump().
    An efficient JSON does not include keys. The structure of the JSON must be known by the user.
    Example:    [0, 47, -17, 32, false, 72, 2464660512]
    '''

    if tracking_data_folder[-1] != "/":
        tracking_data_folder += "/"

    if output_path[-1] != "/":
        output_path += "/"

    subdir = "" # keep blank if not 5fps
    if fps5:
        subdir = "5fps/"

    make_folder(output_path + str(match_id) + "/" + subdir)

    base_input_file_path    = tracking_data_folder + "{0}/{1}{0}".format(match_id, subdir)
    base_output_file_path   = output_path + "{0}/{1}{0}".format(match_id, subdir)

    BALL_INPUT_PATH         = "{0}.BALL.msgpack".format(base_input_file_path)
    BALL_OUTPUT_PATH        = "{0}.BALL.json".format(base_output_file_path)

    HOME_INPUT_PATH         = "{0}.HOME.msgpack".format(base_input_file_path)
    HOME_OUTPUT_PATH        = "{0}.HOME.json".format(base_output_file_path)

    AWAY_INPUT_PATH         = "{0}.AWAY.msgpack".format(base_input_file_path)
    AWAY_OUTPUT_PATH        = "{0}.AWAY.json".format(base_output_file_path)

    print("Ball")
    convert_ball_msgpk_to_readable_json(BALL_INPUT_PATH, BALL_OUTPUT_PATH)
    
    print("Home Team")
    convert_team_msgpk_to_readable_json(HOME_INPUT_PATH, HOME_OUTPUT_PATH)
    
    print("Away Team")
    convert_team_msgpk_to_readable_json(AWAY_INPUT_PATH, AWAY_OUTPUT_PATH)

    METADATA_INPUT_PATH     = "{0}.METADATA.json".format(base_input_file_path)
    METADATA_OUTPUT_PATH    = "{0}.METADATA.json".format(base_output_file_path)

    move_metadata(METADATA_INPUT_PATH, METADATA_OUTPUT_PATH)

    return [
        BALL_OUTPUT_PATH,
        HOME_OUTPUT_PATH,
        AWAY_OUTPUT_PATH,
        METADATA_OUTPUT_PATH
    ]

if __name__ == '__main__':
    # match_id            = 1059743
    # tracking_data_dir   = "TRACKING/DVMS_TEST/"
    # output_path         = "JSON/"

    # make_folder(output_path)

    # convert_match_msgpk_to_efficient_json(tracking_data_dir, match_id, output_path, False)

    # print(Metadata.getMetadataFromFile("1059714.METADATA.json"))
    m = Match.getMatchFromFile("./", 1059714, True)

    for ball, home, away in m:
        for player in home:
            print(player)
    
