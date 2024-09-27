from microbit import *
import music
import power
import time

def show(image, pixel_brightness):
    for x in range(5):
        for y in range(5):
            if image.get_pixel(x, y) > 0:
                image.set_pixel(x, y, pixel_brightness)
    display.show(image)

class State:
    sound_on_image = Image('00099:'
                           '99909:'
                           '90909:'
                           '99909:'
                           '00099')
    sound_off_image = Image('90009:'
                            '09090:'
                            '00900:'
                            '09090:'
                            '90009')
    
    def __init__(self, app):
        self.app = app

    def onEnter(self):
        pass

    def update(self):
        if pin_logo.is_touched():
            time_entered_ms = time.ticks_ms()
            show(self.sound_on_image if self.app.sound_on else self.sound_off_image, self.app.pixel_brightness)
            sleep(300)
            while (time.ticks_diff(time.ticks_ms(), time_entered_ms) < 2000 and
                   not button_a.was_pressed() and not button_b.was_pressed()):
                if pin_logo.is_touched():
                    self.app.sound_on = not self.app.sound_on
                    show(self.sound_on_image if self.app.sound_on else self.sound_off_image, self.app.pixel_brightness)
                    sleep(300)
                    time_entered_ms = time.ticks_ms()
            display.clear()

class ShowNumPomodoros(State):
    pomodoro_images = [
        Image('0'), Image('1'), Image('2'), Image('3'), Image('4'),
        Image('5'), Image('6'), Image('7'), Image('8'), Image('9'),
        Image('05050:'
              '55505:'
              '05505:'
              '05505:'
              '05050'),  # 10
        Image('05005:'
              '55055:'
              '05005:'
              '05005:'
              '05005'),  # 11
        Image('05050:'
              '55505:'
              '05005:'
              '05050:'
              '05555')   # 12
    ]
    
    def update(self):
        super().update()
        if button_a.was_pressed():
            self.app.setState(self.app.achievingPomodoro)
            return

        if button_b.was_pressed():
            display.clear()
            power.deep_sleep(wake_on=(button_a, button_b))
            button_a.was_pressed()
            button_b.was_pressed()
        
        numPomodoros = self.app.numPomodoros
        if numPomodoros < len(self.pomodoro_images):
            image = self.pomodoro_images[numPomodoros]
        else:
            image = Image('00000:'
                          '05050:'
                          '50505:'
                          '05050:'
                          '00000')
        show(image, self.app.pixel_brightness)

class AchievingPomodoro(State):
    def __init__(self, app):
        super().__init__(app)
        self.pomodoro_ms = 1000 * 60 * 25
        self.time_entered_ms = 0
        self.time_paused_ms = None

    def onEnter(self):
        display.clear()
        self.time_entered_ms = time.ticks_ms()

    def update(self):
        super().update()
        if button_b.was_pressed():
            self.app.setState(self.app.showNumPomodoros)
            return

        if button_a.was_pressed():
            if self.time_paused_ms:
                paused_duration = time.ticks_diff(time.ticks_ms(), self.time_paused_ms)
                self.time_entered_ms = time.ticks_add(self.time_entered_ms, paused_duration)
                self.time_paused_ms = None
            else:
                self.time_paused_ms = time.ticks_ms()

        if self.time_paused_ms:
            return

        time_elapsed_ms = time.ticks_diff(time.ticks_ms(), self.time_entered_ms)
        if time_elapsed_ms >= self.pomodoro_ms:
            self.app.numPomodoros += 1

            if self.app.sound_on:
                speaker.on()
                music.play(music.BA_DING)
                speaker.off()

            self.app.setState(self.app.showNumPomodoros)
            return

        led_ms = self.pomodoro_ms // 25
        lit_leds = time_elapsed_ms // led_ms
        for led in range(lit_leds):
            x, y = led % 5, led // 5
            display.set_pixel(x, y, self.app.pixel_brightness)

        x, y = lit_leds % 5, lit_leds // 5
        brightness = self.app.pixel_brightness if time_elapsed_ms % 1000 < 500 else 0
        display.set_pixel(x, y, brightness)

class PomodoroApp:
    def __init__(self):
        self.numPomodoros = 0
        self.currentState = None
        self.pixel_brightness = 9
        self.sound_on = True

        self.showNumPomodoros = ShowNumPomodoros(self)
        self.achievingPomodoro = AchievingPomodoro(self)
        self.setState(self.showNumPomodoros)

    def setState(self, state):
        if state:
            state.onEnter()
        self.currentState = state

    def run(self):
        set_volume(127)
        while self.currentState:
            self.pixel_brightness = scale(display.read_light_level(), from_=(0, 255), to=(6, 9))
            self.currentState.update()
            sleep(40)

app = PomodoroApp()
app.run()
