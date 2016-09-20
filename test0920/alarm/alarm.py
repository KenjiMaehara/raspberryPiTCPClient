""" A crude alarm clock for the raspberry pi """

import datetime
import time

class AlarmManager:

    def __init__(self):
        self.alarms = [] # TODO import alarms from persistence storage

    def check_alarms(self):
        for alarm in self.alarms:
            alarm.check()

class Alarm:

    def __init__(self, hour=0, minute=0):
        self.hour = hour
        self.minute = minute
        self.on = True
        
    def check(self):
        time = datetime.datetime.now().time()
        if alarm.on == True and self.hour == time.hour and self.minute == time.minute:
           print("Beep Beep Beep!") 

    def toggle_alarm(self):
        if self.on == True:
            self.on == False
        else:
            self.on == True

class AlarmApp:
    pass

if __name__ == "__main__":
    alarm_manager = AlarmManager()
    alarm_time = datetime.datetime.now() + datetime.timedelta(minutes=1)
    alarm_time = alarm_time.time().replace(second=0, microsecond=0)
    alarm = Alarm(alarm_time.hour, alarm_time.minute)
    alarm_manager.alarms.append(alarm)

    while True:
        alarm_manager.check_alarms()
        time.sleep(1)

