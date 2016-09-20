from kivy.app import App
from kivy.uix.widget import Widget
from kivy.config import Config
from kivy.lang import Builder

Clock_screen = Builder.load_file("../clock/clock.kv")



Config.set('graphics', 'fullscreen', 0)

class Menu(Widget):
    def weather_btn(self):
        print("The weather is awesome!")

    def clock_btn(self):
        print("The time is ... about time you got a watch!")
        return Clock_screen

    def alarm_btn(self):
        print("Wake up at 8am? Are you kidding?")

    def calendar_btn(self):
        print("I don't know what day it is...")

    def timer_btn(self):
        print("Lets boil some eggs!")

    def preferences_btn(self):
        print("Lets change something!")

    def desktop_btn(self):
        print("We should get out of here...")

class MenuApp(App):
    def build(self):
        menu = Menu()
        return menu

if __name__ == '__main__':
    MenuApp().run()

