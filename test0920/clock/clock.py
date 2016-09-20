""" A simple kivy clock """

from time import strftime
from kivy.app import App
from kivy.uix.floatlayout import FloatLayout
from kivy.config import Config
from kivy.clock import Clock as KivyClock
from kivy.properties import StringProperty

Config.set('graphics', 'fullscreen', 'auto')
Config.set('graphics', 'show_cursor', 0)

class Clock(FloatLayout):
    current_time = StringProperty()
    current_date = StringProperty()
    current_ampm = StringProperty()

    def update(self, dt):
        self.current_time = strftime('%H:%M')
        self.current_date = strftime('%d %b %Y')


class ClockApp(App):
    def build(self):
        clock = Clock()
        KivyClock.schedule_interval(clock.update, 1.0)
        return clock

if __name__ == '__main__':
    ClockApp().run()
