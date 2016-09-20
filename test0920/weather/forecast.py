#!/usr/bin/env python3.5

""" A small script to download and play weather forecast video from Sky news."""

import subprocess
import os

video_source = "http://news.sky.com/story/952742/uk-weather-the-latest-sky-news-forecast"
destination = "latest-forecast"
screen_resolution = "800 480" 

def download_forecast():
    # Remove old file if it exists
    if os.path.isfile('latest-forecast'):
        os.remove('latest-forecast')

    # Use 'youtube-dl' to capture video from source
    subprocess.run(["youtube-dl", video_source, "-o", destination])

def play_forecast():
    """Plays forecast video using omxplayer"""
    if os.path.isfile('latest-forecast'):
        screen_info = screen_settings() 
        subprocess.run(["omxplayer", "--win", screen_info, destination])
    else:
        print("The file %s could not be found" % (destination))

def screen_settings():
    screen_setting = ""

    if screen_resolution:
        screen_setting = "0 0 %s" % (screen_resolution)

    return screen_setting

if __name__ == "__main__":
    download_forecast()
    play_forecast()

