import threading
import time
import sys
import select
try:
    import RPi.GPIO as GPIO
except ImportError:
    print("RPi.GPIO not available. Using simulation mode.")
    GPIO = None

from .config import TRIGGER_GPIO_PIN

class ManualTrigger:
    def __init__(self, trigger_callback):
        self.trigger_callback = trigger_callback
        self.stop_event = threading.Event()
        self.trigger_thread = None
        self.use_gpio = GPIO is not None

    def start_listening(self):
        """Start the manual trigger listening thread."""
        self.stop_event.clear()
        self.trigger_thread = threading.Thread(target=self._listen_loop)
        self.trigger_thread.start()

    def stop_listening(self):
        """Stop the manual trigger listening thread."""
        self.stop_event.set()
        if self.trigger_thread:
            self.trigger_thread.join()
        if self.use_gpio and GPIO is not None:
            GPIO.cleanup()

    def _listen_loop(self):
        """Main manual trigger listening loop."""
        try:
            if self.use_gpio and GPIO is not None:
                self._listen_gpio()
            else:
                self._listen_keyboard()

        except Exception as e:
            print(f"Error in manual trigger listener: {e}")
        finally:
            if self.use_gpio and GPIO is not None:
                GPIO.cleanup()

    def _listen_gpio(self):
        """Listen for GPIO button press (for real hardware)."""
        try:
            GPIO.setmode(GPIO.BCM)
            GPIO.setup(TRIGGER_GPIO_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

            print("Manual trigger is ready. Press the button to trigger an event.")

            # Use edge detection for the button press
            GPIO.add_event_detect(TRIGGER_GPIO_PIN, GPIO.FALLING, callback=self._gpio_trigger_callback, bouncetime=300)

            while not self.stop_event.is_set():
                time.sleep(0.1)

        except Exception as e:
            print(f"Error in GPIO listener: {e}")

    def _listen_keyboard(self):
        """Listen for keyboard input (for simulation)."""
        print("Manual trigger simulation. Press 't' and Enter to trigger an event.")
        while not self.stop_event.is_set():
            try:
                if select.select([sys.stdin], [], [], 0.1)[0]:
                    key = sys.stdin.readline().strip().lower()
                    if key == 't':
                        print("Manual trigger activated!")
                        self.trigger_callback()
            except KeyboardInterrupt:
                break
            except:
                continue

    def _gpio_trigger_callback(self, channel):
        """Callback for GPIO trigger events."""
        print("Manual trigger activated via GPIO!")
        self.trigger_callback()
