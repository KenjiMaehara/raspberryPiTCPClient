import kivy
kivy.require('1.9.0')

from kivy.app import App
from kivy.uix.button import Label,Button
from kivy.config import Config
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.textinput import TextInput
from kivy.clock import Clock
from datetime import datetime

class RootWidget(BoxLayout):
    def __init__(self, **kwargs):
        super(RootWidget, self).__init__(padding=30, orientation='vertical')

 
        self.label02=Label(text="Home Security")
        self.add_widget(self.label02)
        #Clock.schedule_interval(self.TimerCallback02, 1.0)
        self.label02.font_size='80sp'


        self.label_time=Label(text="Time Display")
        self.add_widget(self.label_time)
        Clock.schedule_interval(self.TimerCallback, 1.0)


        #self.textinput = TextInput(text='Hello world', multiline=False)
        #self.add_widget(self.textinput)
        #Clock.schedule_interval(self.TimerCallback, 1.0)


    #def Label_status(self,dt):
        #time=datetime.now().strftime("%Y/%m/%d %H:%M:%S")
        #self.textinput.text=time
        #self.label02.text=time


    def TimerCallback(self,dt):
        time=datetime.now().strftime("%Y/%m/%d %H:%M:%S")
        #self.textinput.text=time
        self.label_time.text=time
        self.label_time.font_size='60sp'


        

class TestApp(App):
    def build(self):
        self.root = RootWidget()
        self.title = 'Text Input Sample'
        return self.root

TestApp().run()


#class HelloApp(App):

#    def build(self):
#        return Label(text='Hello World')


#if __name__ == '__main__':
#    HelloApp().run()

